// The entry: imports through a path alias, a barrel chain, a package, and the host's callbacks.
import * as THREE from "three";
import { bus, announce } from "@app/events";
import { Actor, Hero } from "./barrel";
import type { Ready } from "./types";

const helpers = {
	greet(who: Actor) { return who.measure(); },
};

export class Game {
	actors: Actor[] = [];
	renderer = new THREE.WebGLRenderer();

	start(): void {
		const first = new Actor("a");
		this.actors.push(first);
		this.actors.forEach((a) => a.measure());
		for (const part of first) { part.measure(); }
		this.renderer.setAnimationLoop((t: number) => this.frame(t));
		bus.addEventListener("ready", () => this.onReady());
		window.setTimeout(() => announce(), 0);
		const speed = first.pace;
		helpers.greet(first);
		const run = this.frame.bind(this);
		run.call(this, 1);
		legacyHelper(speed);
		this.later().then((value) => value.measure());
	}

	frame(t: number): void {
		const h = new Hero("h");
		h.measure();
	}

	onReady(): void {}

	async later(): Promise<Actor> {
		return new Actor("later");
	}
}

new Game().start();
