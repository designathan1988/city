// An import through a path alias a tsconfig above the folder read declares (TypeScript looks for it up the folders).
import { tool } from "@lib/tool";

export function use() { return tool(); }
