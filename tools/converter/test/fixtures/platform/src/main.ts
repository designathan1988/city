import { draw, Scene } from 'fakegl';
import { plant } from './feature/lamps';
import { drain, later } from './shared/queue';

const scene = new Scene();
plant(scene);
later(() => draw(scene));
drain();
import { patchRenderer, useRenderer } from './shared/patch';
patchRenderer();
void useRenderer();
import { later2 } from './shared/queue';
later2({ run: () => 0 });
import { measure } from './shared/shapes';
void measure({ area: 0, grow() {} });
import { startEngine, watchEngines } from './shared/patch';
watchEngines();
startEngine();
