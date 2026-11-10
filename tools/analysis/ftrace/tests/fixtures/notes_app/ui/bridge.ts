// Bridge to the native side: forwards a named call and its arguments.
import { log_line } from "./log";

export function invoke(name: string, ...args: unknown[]): unknown {
  log_line("invoke " + name);
  return native_module.call_native(name, args);
}
