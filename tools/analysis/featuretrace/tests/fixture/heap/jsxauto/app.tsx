// JSX through the automatic transform its tsconfig chooses (log/trace-core-plan.md, Task 6): the runtime's function
// calls the component back; the children are the prop the runtime's declarations name.
export class Apple {}
let kids: unknown;
export function List(props: { label: string; kids?: unknown }) { kids = props.kids; return props.label; }
export const view = <List label="a">{new Apple()}</List>;
export function shown() { return kids; }
