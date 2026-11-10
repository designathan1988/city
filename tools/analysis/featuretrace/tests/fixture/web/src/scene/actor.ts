// A class with a getter, a field written through `this`, an iterator and a base the code does not define.
import { Object3D } from "three";
export class Actor extends Object3D {
	speed: number = 2;
	constructor(public name: string) {
		super();
		this.tag = name;
	}
	get pace(): number { return this.measure(); }
	measure(): number { return this.speed; }
	*[Symbol.iterator]() { yield this; }
	onBeforeRender(): void { this.measure(); }
}
