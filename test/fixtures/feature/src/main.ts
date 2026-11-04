// Fixture: the program's root (the page's script). It plugs the feature in and uses shared code of its own.
import { Ferry } from './feature/ferry';
import { clamp } from './shared/math';
import { rows } from './shared/table';

const ferry = new Ferry();
ferry.sail(clamp(rows(), 0, 10));
