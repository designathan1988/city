---
name: no-unwatched-processes
description: "never launch a process in the background unwatched, with a long deadline, or without live output of what it is doing"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-11T19:17:52.222Z
---

Never send a process to the background without monitoring it; never leave one running with time out of control; never launch one that does not show, in real time, what it is doing. Said by the user on 2026-11-10, in capitals, three times, after a measurement (camera_pan on the metropolis, hidden, 1500 s deadline a run, launched with run_in_background and no monitor) sat hung for 40 minutes in a wait with no deadline inside the probe and I only looked when the user told me it had hung.

**Why:** a hung run burns the user's time and machine while nothing is learnt; I ended my turn "waiting for the notification", so nobody was watching.

**How to apply:** run things in the foreground with a short deadline sized to the work (seconds to a few minutes), and print the log's progress lines as part of the same command. If something truly needs longer, split it into steps that each report, or watch its log with a monitor that streams lines; check it at once if no line arrives when one is due. Before launching a probe or test for the first time on a new scene, read its waits: a wait with no deadline is fixed first. Never end a turn with an unwatched process alive. The one long-lived process the owner ordered (2026-11-11), the live game instance, is watched by a mechanism, not by hope: its supervisor's heartbeat on the telemetry restarts it from a checkpoint when it hangs or crashes, and `live.py status` reports it (log/live-instance-design.md). Related: [[fast-direct-checks]], [[one-godot-at-a-time]].
