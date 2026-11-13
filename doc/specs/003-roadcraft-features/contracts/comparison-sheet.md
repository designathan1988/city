# Contract: a feature's comparison sheet

File: `doc/specs/003-roadcraft-features/sheets/<area>-<feature>.md`. Written before the feature's code; the item's commit holds it.
Each area has an index, `sheets/<area>.md`: every unpaired inventory row of the area, the feature sheet it belongs to or why it is
justified.

```markdown
# <area>: <feature>
- State: drafted | read | designed | built | paired | justified (why)
- Inventory rows: <reference source> ... | <city source> ...

## Reference (read for behaviour; nothing copied)
- Trace: `trace.py --feature <output> --root <files>` (its outputs and roots)
- Read: <path>, what it does
- Outputs, conditions, cases, values (each file:line)
- State it reads and writes; what sets it off
- Cost (`--cost`): loops, growth, per-frame or per-change; faults never to bring (the constitution, principle II, by name)

## This game
- The same, from its own trace and reading, or "absent"

## Differences
- One line each: the reference does X (file:line), this game does Y or nothing (file:line)

## Design here
- State in CityState, command, simulation step, render read, data keys (DataTables), owner, budgets, threads
- Context7: library id, question, answer, for each API called

## Proof
- Test scene(s) and what each checks; photo plan and the claim of each shot
```
