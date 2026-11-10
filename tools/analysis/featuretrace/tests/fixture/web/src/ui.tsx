// A function component used as a JSX tag.
export function Panel(props: { title: string }) {
	return props.title;
}
export const view = <Panel title="x" />;
