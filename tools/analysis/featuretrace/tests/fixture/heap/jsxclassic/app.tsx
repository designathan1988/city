// JSX through the classic transform whose factory its tsconfig names (log/trace-core-plan.md, Task 6).
export class Apple {}
export function h(tag: (props: unknown) => unknown, props: unknown) { return tag(props); }
export function Item(props: { thing: Apple }) { return props.thing; }
export const made = <Item thing={new Apple()} />;
export function viaFactory() { return made; }
