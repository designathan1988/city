export function createCounter(step: number) {
  let total = 0;
  function add(): number {
    total += step;
    return total;
  }
  const reset = (): void => {
    total = 0;
  };
  function unused(): number {
    return 99;
  }
  return {
    add,
    reset,
    read() {
      return total;
    },
  };
}
