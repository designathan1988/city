"""A git repository read at one commit: files, blobs, history and exports. Git is called directly, never through a shell."""
import fnmatch
import io
import os
import pickle
import subprocess
import tarfile
import threading

from .core import inside, own_prefixes, program_root, text_of

GIT = ("git", "-c", "core.quotepath=off", "-c", "core.autocrlf=false")
GIT_TIMEOUT_S = 120  # one git listing; a git that hangs is an error, not a wait (the stream readers run under the task's deadline)
_IGNORED = {}


def toplevel(path):
    out = subprocess.run((*GIT, "-C", path, "rev-parse", "--show-toplevel"), capture_output=True, check=True, timeout=GIT_TIMEOUT_S)
    return os.path.normpath(out.stdout.decode("utf8").strip())


class Repo:
    """Every read goes to the commit, never to the working tree: what another session left uncommitted is not analysed."""

    def __init__(self, path, rev="HEAD"):
        self.root = toplevel(path)
        self.head = self.git("rev-parse", rev).decode().strip()
        self._cat = None
        self._asking = threading.Lock()
        self._texts = {}

    def translate(self, rel):
        """A path as this reader calls it: the same, unless a copy renamed it."""
        return rel

    def flag(self, word):
        return word

    def profile(self, text):
        return text

    def content_id(self, blob):
        """What identifies the content a reader gets for a blob."""
        return blob

    def spec(self):
        """How another process reads the same contents."""
        return ("git", self.root)

    def export_key(self):
        return self.head

    def overlay(self, target):
        pass

    def text(self, blob):
        """A blob as text, or None for what is not text; read once."""
        if blob not in self._texts:
            self._texts[blob] = text_of(self.blob(blob))
        return self._texts[blob]

    def stream(self, blobs):
        """The texts of a list of blobs, in order, through one stream that keeps nothing: each ask of a pipe waits for its answer, a list does not."""
        cat = subprocess.Popen((*GIT, "-C", self.root, "cat-file", "--batch"), stdin=subprocess.PIPE, stdout=subprocess.PIPE)

        def feed():
            cat.stdin.write("".join(b + "\n" for b in blobs).encode())
            cat.stdin.close()

        writer = threading.Thread(target=feed)
        writer.start()
        for _ in blobs:
            head = cat.stdout.readline().split()
            if len(head) < 3:
                yield None
                continue
            text = text_of(cat.stdout.read(int(head[2])))
            cat.stdout.read(1)
            yield text
        writer.join()
        cat.wait()

    def prefetch(self, blobs):
        """Texts wanted soon, all in one go."""
        todo = sorted({b for b in blobs if b not in self._texts})
        self._texts.update(zip(todo, self.stream(todo)))

    def git(self, *args, check=True):
        out = subprocess.run((*GIT, "-C", self.root, *args), capture_output=True, check=False, timeout=GIT_TIMEOUT_S)
        if check and out.returncode != 0:
            raise RuntimeError(f"git {args[0]}: {out.stderr.decode('utf8', 'replace')[:300]}")
        return out.stdout

    def files(self, rev=None):
        """path -> blob id of every tracked file of a commit (submodules left out)."""
        raw = self.git("ls-tree", "-r", "-z", "--full-tree", rev or self.head)
        out = {}
        for entry in raw.split(b"\0"):
            if entry:
                meta, path = entry.split(b"\t", 1)
                _, kind, blob = meta.split()
                if kind == b"blob":
                    out[path.decode("utf8", "surrogateescape")] = blob.decode()
        return out

    def blob(self, blob):
        """The bytes of a blob, through one long-lived `git cat-file`: a question and its answer are one step, or two threads read each other's."""
        with self._asking:
            if self._cat is None:
                self._cat = subprocess.Popen((*GIT, "-C", self.root, "cat-file", "--batch"), stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            self._cat.stdin.write(blob.encode() + b"\n")
            self._cat.stdin.flush()
            head = self._cat.stdout.readline().split()
            if len(head) < 3:
                return b""
            data = self._cat.stdout.read(int(head[2]))
            self._cat.stdout.read(1)
            return data

    def close(self):
        if self._cat is not None:
            self._cat.stdin.close()
            self._cat.wait()
            self._cat = None

    def history(self):
        """Commits oldest first, merges left out: (id, [(status, path, new blob, old blob)])."""
        raw = self.git("log", "--reverse", "--topo-order", "--no-merges", "--no-renames", "--raw", "--abbrev=40", "-z",
                       "--format=%x01%H", self.head)
        out = []
        for chunk in raw.split(b"\x01")[1:]:
            fields = chunk.split(b"\0")
            sha = fields[0].split(b"\n")[0].decode()
            changes, i = [], 0
            fields[0] = fields[0].partition(b"\n")[2]
            while i + 1 < len(fields):
                meta = fields[i].strip().split()
                if len(meta) >= 5 and meta[0].startswith(b":"):
                    changes.append((meta[4].decode()[:1], fields[i + 1].decode("utf8", "surrogateescape"), meta[3].decode(), meta[2].decode()))
                    i += 2
                else:
                    i += 1
            out.append((sha, changes))
        return out

    def export(self, dest, paths=None, rev=None):
        """The files of a commit on disk; a name the system refuses is left out and reported."""
        args = ["archive", "--format=tar", rev or self.head]
        if paths:
            args += ["--", *paths]
        refused = []
        own = own_prefixes(self.root)
        with tarfile.open(fileobj=io.BytesIO(self.git(*args))) as tar:
            for member in tar:
                if member.isfile() and not (own and member.name.startswith(own)):
                    target = os.path.join(dest, member.name)
                    try:
                        os.makedirs(os.path.dirname(target), exist_ok=True)
                        with open(target, "wb") as f:
                            f.write(tar.extractfile(member).read())
                    except OSError:
                        refused.append(member.name)
        return refused

    def on_disk(self, pattern):
        """Ignored files on disk matching a name: what a build left, found by what it is. One listing a process, and never of the program's own output."""
        if self.root not in _IGNORED:
            own = inside(os.path.join(program_root(), "dist", "arch"), self.root)
            skip = (f":(exclude){own}",) if own else ()
            raw = self.git("ls-files", "-z", "--others", "--ignored", "--exclude-standard", "--", ".", *skip, check=False)
            _IGNORED[self.root] = [p.decode("utf8", "surrogateescape") for p in raw.split(b"\0") if p]
        return sorted(p for p in _IGNORED[self.root] if fnmatch.fnmatch(p, pattern) and os.path.isfile(os.path.join(self.root, p)))


_READERS = {}


def reader(spec):
    """Blobs in another process: its own stream to git, and the map when the text is renamed."""
    if spec not in _READERS:
        base = Repo(spec[1])
        if spec[0] == "renamed":
            with open(spec[2], "rb") as f:
                renamer = pickle.load(f)

            def read(blob):
                data = base.blob(blob)
                text = text_of(data)
                return data if text is None else renamer.text(text).encode("utf8")
            _READERS[spec] = read
        else:
            _READERS[spec] = base.blob
    return _READERS[spec]
