"""Export the prepared source corrections; apply them only to a compatible isolated Git worktree.

This transports reviewed changes. It never discovers/fixes arbitrary source by regex, executes the game,
runs checks, builds, commits, merges or pushes. Audit closure is a packaging guard, not proof of correctness.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
from pathlib import Path, PurePosixPath
import subprocess
import tempfile
import zipfile


FORMAT = 1
MATRIX = "log/performance-causes.json"


def git(root: Path, *args: str, optional: bool = False) -> bytes | None:
    result = subprocess.run(["git", "-c", "core.fsmonitor=false", "-C", str(root), *args], capture_output=True)
    if result.returncode:
        if optional:
            return None
        raise ValueError(result.stderr.decode(errors="replace").strip())
    return result.stdout


def digest(data: bytes | None) -> str | None:
    return hashlib.sha256(data).hexdigest() if data is not None else None


def source_path(root: Path, relative: str) -> Path:
    name = PurePosixPath(relative)
    if not name.parts or name.as_posix() != relative or name.is_absolute() or any((p in (".", "..") or p.casefold() == ".git") or ":" in p or "\\" in p for p in name.parts):
        raise ValueError(f"Unsafe archive path: {relative!r}")
    reserved = {"CON", "PRN", "AUX", "NUL", "CONIN$", "CONOUT$",
                *(f"{prefix}{i}" for prefix in ("COM", "LPT") for i in "123456789¹²³")}
    if any(part.endswith((".", " ")) or part.split(".", 1)[0].rstrip(" ").upper() in reserved or
           any(ord(c) < 32 or c in '<>"|?*' for c in part) for part in name.parts):
        raise ValueError(f"Unsafe Windows archive path: {relative!r}")
    path = root.joinpath(*name.parts)
    if not path.resolve().is_relative_to(root):
        raise ValueError(f"Path leaves the worktree: {relative}")
    for part in (path, *path.parents):
        if part == root:
            break
        if part.is_symlink() or (hasattr(part, "is_junction") and part.is_junction()):
            raise ValueError(f"Symlink in source path: {relative}")
    return path


def common_dir(root: Path) -> Path:
    value = Path(git(root, "rev-parse", "--git-common-dir").decode().strip())
    return (root / value).resolve() if not value.is_absolute() else value.resolve()


def guarded_worktree(root: Path, protected: Path, expected_common: Path) -> None:
    if root == protected or root.is_relative_to(protected) or protected.is_relative_to(root):
        raise ValueError("The protected original checkout cannot be a source or destination.")
    if not (root / ".git").is_file():
        raise ValueError("An additional Git worktree with a .git pointer file is required.")
    top = Path(git(root, "rev-parse", "--show-toplevel").decode().strip()).resolve()
    branch = git(root, "branch", "--show-current").decode().strip()
    if top != root or common_dir(root) != expected_common or not branch.startswith("codex/"):
        raise ValueError("Wrong worktree root, repository or branch; a codex/ branch is required.")


def export(root: Path, output: Path) -> None:
    matrix = json.loads((root / MATRIX).read_text(encoding="utf-8-sig"))
    protected = Path(matrix["protected_checkout"]).resolve()
    repository = common_dir(root)
    guarded_worktree(root, protected, repository)
    if output.is_relative_to(protected):
        raise ValueError("The bundle cannot be written into the protected original checkout.")
    if output.suffix.lower() != ".zip":
        raise ValueError("Output must be a .zip artifact, never a source file.")
    occurrences = matrix["occurrences"]
    ids = [row["id"] for row in occurrences]
    if len(ids) != 92 or set(ids) != {f"C{i:02}" for i in range(1, 93)}:
        raise ValueError("The complete 92-occurrence matrix is required.")
    unfinished = [row["id"] for row in occurrences if row["status"] != "implemented" or row["evidence"]["static_verdict"] != "source_reviewed"]
    if unfinished:
        raise ValueError("Source corrections are incomplete; no complete-fix bundle produced: " + ", ".join(unfinished))
    base = matrix["baseline"]
    if not re.fullmatch(r"[0-9a-f]{40}|[0-9a-f]{64}", base):
        raise ValueError("A full audited commit hash is required.")
    if git(root, "rev-parse", "HEAD").decode().strip() != base:
        raise ValueError("Export requires the audited base HEAD; unrelated commits cannot enter this bundle.")
    tracked = git(root, "diff", "--no-ext-diff", "--no-textconv", "--name-only", "--no-renames", "-z", base).split(b"\0")
    added = git(root, "ls-files", "--others", "--exclude-standard", "-z").split(b"\0")
    files, payloads = [], {}
    for raw in sorted(set(tracked + added)):
        if not raw:
            continue
        relative = raw.decode("utf-8")
        path = source_path(root, relative)
        if path == output:
            continue
        if path.suffix.lower() in (".zip", ".7z", ".exe", ".dll", ".so", ".pck"):
            raise ValueError(f"Unexpected non-source artifact among the changes: {relative}")
        before = git(root, "show", f"{base}:{relative}", optional=True)
        after = path.read_bytes() if path.is_file() else None
        if before == after:
            continue
        after_hash = digest(after)
        files.append({"path": relative, "before": digest(before), "after": after_hash})
        if after is not None:
            payloads[after_hash] = after
    if not files:
        raise ValueError("No prepared source changes to export.")
    if output.exists():
        with zipfile.ZipFile(output) as previous:
            old = json.loads(previous.read("manifest.json"))
        if old.get("format") != FORMAT or old.get("baseline") != base or old.get("repository_common_dir") != str(repository):
            raise ValueError("Existing output is not a bundle from this source baseline/repository.")
    manifest = {"format": FORMAT, "baseline": base, "repository_common_dir": str(repository),
                "protected_checkout": str(protected), "runtime_verification": "not_executed",
                "occurrences": ids, "files": files}
    output.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=output.name + ".", dir=output.parent)
    os.close(handle)
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED) as archive:
            archive.writestr("manifest.json", json.dumps(manifest, ensure_ascii=False, indent=2))
            for key, value in payloads.items():
                archive.writestr("payload/" + key, value)
        os.replace(temporary, output)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    print(f"Exported {len(files)} prepared source changes to {output}. No executable validation performed.")


def atomic_write(path: Path, data: bytes | None) -> None:
    if data is None:
        path.unlink(missing_ok=True)
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=".performance-fix-", dir=path.parent)
    try:
        with os.fdopen(handle, "wb") as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def apply(root: Path, bundle: Path, dry_run: bool) -> None:
    with zipfile.ZipFile(bundle) as archive:
        manifest = json.loads(archive.read("manifest.json"))
        if manifest.get("format") != FORMAT:
            raise ValueError("Unsupported bundle format.")
        guarded_worktree(root, Path(manifest["protected_checkout"]).resolve(), Path(manifest["repository_common_dir"]).resolve())
        entries = manifest["files"]
        if not entries or len({row["path"].casefold() for row in entries}) != len(entries):
            raise ValueError("Empty or duplicate destination paths.")
        pending, allowed = [], set()
        for row in entries:
            relative = row["path"]
            path = source_path(root, relative)
            allowed.add(relative)
            if path.exists() and not path.is_file():
                raise ValueError(f"Destination is not a regular source file: {relative}")
            before = path.read_bytes() if path.is_file() else None
            current = digest(before)
            if current == row["after"]:
                continue
            if current != row["before"]:
                raise ValueError(f"Conflict; destination has unrelated changes: {relative}")
            after = archive.read("payload/" + row["after"]) if row["after"] is not None else None
            if digest(after) != row["after"]:
                raise ValueError(f"Corrupt payload: {relative}")
            pending.append((relative, before, after))
    if not pending:
        print("Every bundle file already matches; no source changed. This is not executable validation.")
        return
    if git(root, "rev-parse", "HEAD").decode().strip() != manifest["baseline"]:
        raise ValueError("Destination HEAD is not the audited base.")
    for record in git(root, "status", "--porcelain=v1", "--no-renames", "-z", "--untracked-files=all").split(b"\0"):
        if record:
            relative = record[3:].decode("utf-8")
            if relative not in allowed and source_path(root, relative) != bundle:
                raise ValueError(f"Unrelated destination change: {relative}")
    print(f"Preflight passed for {len(pending)} source changes; {len(entries) - len(pending)} already match.")
    if dry_run:
        print("Read-only preflight; no source changed.")
        return
    written = []
    try:
        for relative, before, after in pending:
            path = source_path(root, relative)  # resolve again immediately before each mutation
            current = path.read_bytes() if path.is_file() else None
            if current != before:
                raise ValueError(f"Destination changed after preflight: {relative}")
            atomic_write(path, after)
            written.append((relative, before, after))
    except BaseException:
        for relative, before, after in reversed(written):
            path = source_path(root, relative)
            current = path.read_bytes() if path.is_file() else None
            if current == after:
                atomic_write(path, before)
            else:
                print(f"Concurrent edit preserved during rollback; inspect {path}")
        raise
    print(f"Applied {len(written)} source changes in {root}. No build, test, commit, merge or push executed.")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    build = commands.add_parser("export", help="Package the completed source corrections from the approved worktree")
    build.add_argument("--worktree", type=Path, required=True)
    build.add_argument("--output", type=Path, required=True)
    install = commands.add_parser("apply", help="Preflight and apply a prepared bundle to another isolated worktree")
    install.add_argument("--worktree", type=Path, required=True)
    install.add_argument("--bundle", type=Path, required=True)
    install.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    try:
        if args.command == "export":
            export(args.worktree.resolve(), args.output.resolve())
        else:
            apply(args.worktree.resolve(), args.bundle.resolve(), args.dry_run)
    except (OSError, ValueError, KeyError, zipfile.BadZipFile) as error:
        parser.exit(1, f"Refused: {error}\n")


if __name__ == "__main__":
    main()
