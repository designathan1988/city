# Contract: the block's close checks the method

`python tools/tasks/cli.py block close` (tools/tasks/block.py, the check in tools/tasks/block_method.py) refuses, naming each gap and
the line to add, while `doc/work/<block>.md`:

- is missing;
- has no `## Read` section with at least one `a-b of N` line whose b equals N (a file read to its last line);
- has no `## Research` section with a `Context7` line and a source on the internet (`http://` or `https://`) when the block changed
  code under `native/`, `project/` or `tools/` (git's changed and new files since the commit the block opened on);
- has no `## Skills` section naming `city-method` and `verification-before-completion`, and `systematic-debugging` when its
  `## Checks` record a failure;
- has, in `## Checks`, a failing run (a line with `FAIL`) whose line does not say its cause (the word `cause`): a run again without a
  cause read is trial and error.

It refuses for nothing else. The block's state records `head`, the commit it opened on. Its tests: tools/tasks/tests/test_block.py.
