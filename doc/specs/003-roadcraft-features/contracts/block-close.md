# Contract: the block's close checks the method

`python tools/tasks/cli.py block close` (tools/tasks/block.py) refuses, naming each gap, while `doc/work/<block>.md`:

- has no `## Read` section with at least one `path: a-b of N` line whose b equals N (a file read to its last line);
- has no `## Research` section with a `Context7` line (library id, question, answer) when the block changed code under `native/`,
  `project/` or `tools/` (from git's changed files);
- has no `## Skills` section naming `city-method`, and `verification-before-completion`; and `systematic-debugging` when its `##
  Checks` record a failure;
- has an item whose `## Checks` record more than one failing run of the same check with no cause written between them (trial
  and error).

It never refuses for anything else; its refusal is printed with the line to add. Its tests: tools/tasks/tests/test_block.py.
