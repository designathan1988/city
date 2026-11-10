"""Command line of the trace-fix skill: the on-disk memory of one problem's reading, planning and fixing.

One trace per problem, in dist/trace/<slug>/ledger.json: the request, the files on the flow in the order the code
runs, the line ranges read with what each chunk does, the problems found, the causes they are grouped under and
the fixes. It survives compaction and new sessions (`resume`, always a short answer) and it can fail: `read`
refuses a chunk the Read tool did not return, `gate read` a flow not read in whole, `gate plan` a problem without
a cause or a cause without a plan, `fix` anything before those gates, `gate close` an open problem.
What each command does: commands.py. The changes a trace accepts: operations.py. State and paths: store.py.
Proof of reading and notes kept by content: proof.py. Output: views.py.
Usage: python ledger.py <command> -h
"""
import sys

sys.dont_write_bytecode = True  # the skill's files are swapped in place: no stale bytecode beside them

import argparse  # noqa: E402

import commands  # noqa: E402


def main():
    for stream in (sys.stdout, sys.stderr):
        stream.reconfigure(encoding="utf-8", errors="replace")
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("open", help="start the trace of one problem")
    p.add_argument("slug")
    p.add_argument("--request", required=True, help="the user's request, word for word")
    p.add_argument("--reuse", action="store_true",
                   help="let files unchanged since another trace read them be carried over (`carry`) instead of read")
    p.set_defaults(fn=commands.cmd_open)
    p = sub.add_parser("switch", help="make another trace the active one")
    p.add_argument("slug")
    p.set_defaults(fn=commands.cmd_switch)
    sub.add_parser("list", help="every trace and its phase").set_defaults(fn=commands.cmd_list)
    p = sub.add_parser("flow", help="put files (a file, a folder or a glob) on the flow, in the order the code runs")
    p.add_argument("paths", nargs="+")
    p.add_argument("--why", required=True, help="who calls it or which data it reads or writes on this flow")
    p.add_argument("--after", help="a file already on the flow: the new ones run right after it")
    p.set_defaults(fn=commands.cmd_flow)
    sub.add_parser("next", help="the next batch of chunks to read, in parallel").set_defaults(fn=commands.cmd_next)
    p = sub.add_parser("read", help="record one chunk that was read in whole")
    p.add_argument("path")
    p.add_argument("start", type=int)
    p.add_argument("end", type=int)
    p.add_argument("--saw", required=True, help="what the chunk does and what it calls (the memory of the reading)")
    p.set_defaults(fn=commands.cmd_read)
    p = sub.add_parser("record", help="record a batch (flow, reads, problems, note) from a JSON file, whole or not at all")
    p.add_argument("file", help="the JSON file, or - for stdin")
    p.set_defaults(fn=commands.cmd_record)
    p = sub.add_parser("carry", help="count a file as read from another trace's notes (only in an `open --reuse` trace)")
    p.add_argument("path")
    p.add_argument("--saw", required=True, help="what of those notes matters for this trace's symptom")
    p.set_defaults(fn=commands.cmd_carry)
    p = sub.add_parser("problem", help="record a problem where it was read")
    p.add_argument("where", help="<file>:<line>")
    p.add_argument("--what", required=True)
    p.add_argument("--cause", required=True, help="the design decision that produces it, as seen there")
    p.add_argument("--symptom", help="what the player sees because of it")
    p.add_argument("--of", help="the cause (C<n>) it belongs to, when already known")
    p.set_defaults(fn=commands.cmd_problem)
    p = sub.add_parser("cause", help="the design decisions the problems are grouped under")
    cause = p.add_subparsers(dest="cause_cmd", required=True)
    q = cause.add_parser("add", help="a cause: the decision, its single owner, the correction planned")
    q.add_argument("--what", required=True, help="the design decision that produces the problems")
    q.add_argument("--owner", required=True, help="the one file or module that owns the correction")
    q.add_argument("--plan", required=True, help="the correction, and why the symptoms cannot return another way")
    q.add_argument("--problems", nargs="*", help="P<n> ... that come from it")
    q.set_defaults(fn=commands.cmd_cause_add)
    q = cause.add_parser("link", help="put problems under a cause")
    q.add_argument("id")
    q.add_argument("problems", nargs="+")
    q.set_defaults(fn=commands.cmd_cause_link)
    q = cause.add_parser("set", help="correct a cause's text")
    q.add_argument("id")
    q.add_argument("--what")
    q.add_argument("--owner")
    q.add_argument("--plan")
    q.set_defaults(fn=commands.cmd_cause_set)
    p = sub.add_parser("fix", help="mark a cause fixed: closes its problems (only after `gate read` and `gate plan`)")
    p.add_argument("id", help="C<n>")
    p.add_argument("--how", required=True, help="what changed and why the symptoms cannot return another way")
    p.add_argument("--commit")
    p.set_defaults(fn=commands.cmd_fix)
    p = sub.add_parser("drop", help="a recorded item that the code proved not to be a problem")
    p.add_argument("id")
    p.add_argument("--why", required=True)
    p.set_defaults(fn=commands.cmd_drop)
    p = sub.add_parser("note", help="the next action, for whoever resumes")
    p.add_argument("text")
    p.set_defaults(fn=commands.cmd_note)
    sub.add_parser("status", help="where the trace is, in a short answer").set_defaults(fn=commands.cmd_status)
    p = sub.add_parser("resume", help="the same, to continue after a compaction or in a new session")
    p.add_argument("--hook", action="store_true", help="called by a session hook: silent when there is nothing open")
    p.set_defaults(fn=commands.cmd_resume)
    p = sub.add_parser("notes", help="what was read in a file or a folder of the flow")
    p.add_argument("where")
    p.add_argument("--from", dest="start", type=int, default=1, help="continue at this file of the listing")
    p.add_argument("--prior", action="store_true", help="also what another trace read of each file, if unchanged")
    p.set_defaults(fn=commands.cmd_notes)
    p = sub.add_parser("problems", help="the problems in full")
    p.add_argument("--all", action="store_true", help="fixed and dropped ones too")
    p.add_argument("--from", dest="start", type=int, default=1, help="continue at this problem of the listing")
    p.set_defaults(fn=commands.cmd_problems)
    p = sub.add_parser("gate", help="read: the flow is read in whole; plan: every problem has a planned cause; "
                                    "close: every problem is fixed")
    p.add_argument("which", choices=["read", "plan", "close"])
    p.set_defaults(fn=commands.cmd_gate)
    for name in ("export", "report"):
        sub.add_parser(name, help="write report.md (causes and problems) and notes.md (every chunk) in the trace's "
                                  "folder, to read with the Read tool").set_defaults(fn=commands.cmd_export)

    args = ap.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
