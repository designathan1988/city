"""Rebuilds the oracle copy (tools/oracle-4.0) from the valid Roadcraft 4.0 reference: every file the reference's commit holds,
as committed, except its run artefacts (reports/, 8.4 GB) and its session data (.claude/); our vite.config.ts stays, and a
file an earlier copy brought that the reference no longer has is removed. The reference is the converter machine file's
"source" (tools/converter/configs/roadcraft-machine.json); the commit copied is written to source.json. It only copies files:
no install, no server. Use: python tools/oracle-4.0/rebuild.py
"""
import json
import os
import subprocess
import sys
import tarfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(HERE, "..", "converter", "scripts"))
from machine_config import machine  # noqa: E402

# Ours, never the reference's: the oracle's Vite config, this script and the record of what was copied.
OURS = {"vite.config.ts", "rebuild.py", "source.json"}
# What the reference holds that is not the game: its run artefacts and its sessions' data.
EXCLUDED = ["reports", ".claude"]
RECORD = "source.json"
# A git command of the rebuild (a listing, the archive of the reference's commit); past this its process is stopped.
GIT_TIMEOUT_S = 1800


def git(cwd, *args):
    return subprocess.run(["git", "-C", cwd, *args], capture_output=True, text=True, encoding="utf8", check=True, timeout=GIT_TIMEOUT_S).stdout


def main():
    source = machine()["source"]
    commit = git(source, "rev-parse", "HEAD").strip()
    pathspec = ["--", ".", *[f":(exclude){e}" for e in EXCLUDED]]
    archive = subprocess.Popen(["git", "-C", source, "archive", "--format=tar", commit, *pathspec], stdout=subprocess.PIPE)
    copied, written = set(), 0
    try:
        with tarfile.open(fileobj=archive.stdout, mode="r|") as tar:
            for member in tar:
                if not member.isfile() or member.name in OURS:
                    continue
                data = tar.extractfile(member).read()
                target = os.path.join(HERE, *member.name.split("/"))
                copied.add(member.name)
                if os.path.isfile(target):
                    with open(target, "rb") as f:
                        if f.read() == data:
                            continue
                os.makedirs(os.path.dirname(target), exist_ok=True)
                with open(target, "wb") as f:
                    f.write(data)
                written += 1
        if archive.wait(timeout=GIT_TIMEOUT_S) != 0:
            raise SystemExit(f"git archive of {source} at {commit} failed ({archive.returncode})")
    finally:
        if archive.poll() is None:
            archive.kill()
    if not copied:
        raise SystemExit(f"the reference {source} at {commit} gave no file")
    # what an earlier copy brought (the repository tracks it here) and the reference no longer holds
    prefix = os.path.relpath(HERE, REPO).replace("\\", "/") + "/"
    tracked = [f[len(prefix):] for f in git(REPO, "ls-files", "-z", "--", prefix).split("\0") if f]
    removed = sorted(f for f in tracked if f not in copied and f not in OURS)
    for f in removed:
        target = os.path.join(HERE, *f.split("/"))
        if os.path.isfile(target):
            os.remove(target)
        parent = os.path.dirname(target)
        while parent != HERE and os.path.isdir(parent) and not os.listdir(parent):
            os.rmdir(parent)
            parent = os.path.dirname(parent)
    with open(os.path.join(HERE, RECORD), "w", encoding="utf8", newline="\n") as f:
        json.dump({"_doc": "The Roadcraft 4.0 commit this copy holds (tools/oracle-4.0/rebuild.py); every file but ours is that commit's.",
                   "source": source, "commit": commit, "excluded": EXCLUDED, "ours": sorted(OURS)}, f, indent=1)
        f.write("\n")
    print(f"oracle-4.0: {len(copied)} files of {source} at {commit}, {written} written, {len(removed)} removed")
    for f in removed:
        print(f"  removed {f}")


if __name__ == "__main__":
    main()
