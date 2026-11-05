// The code of src/core/perfBudget.ts that is the feature's (the rest of the file is shared).

// src/core/perfBudget.ts:29-37 function stepBudget: the feature's own
export function stepBudget(name: string): { readonly key: string; readonly budget: Budget } | null {
  const bare = name.startsWith('hitch:') ? name.slice(6) : name;
  const whole = STEP_BUDGETS[bare];
  if (whole) return { key: bare, budget: whole };
  const space = bare.indexOf(' ');
  const key = space < 0 ? bare : bare.slice(0, space);
  const budget = STEP_BUDGETS[key];
  return budget ? { key, budget } : null;
}
