import { pack } from "%shelf/box";
import { seal } from "lib/tape";

export function ship() { return seal(pack(1)); }
