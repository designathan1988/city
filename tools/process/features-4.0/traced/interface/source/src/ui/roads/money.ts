// The code of src/ui/roads/money.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/money.ts:5-8 slice of src/ui/roads/money.ts#formatMoney (shared): the feature's statements; "…" marks code left out
export function formatMoney(value: number): string {
  // … 1 lines
  return t('economy.money', { value: formatDecimal(Math.round(value), 0, language()) });
}

// src/ui/roads/money.ts:11-13 slice of src/ui/roads/money.ts#formatCost (shared): the feature's statements; "…" marks code left out
export function formatCost(cost: number): string {
  return cost < 0 ? t('economy.refund', { value: formatMoney(-cost) }) : formatMoney(cost);
}

// src/ui/roads/money.ts:16-18 function balanceTip: the feature's own
export function balanceTip(): string {
  return t('economy.balance', { share: Math.round(ROAD_TUNING.economy.demolitionRefund * 100) });
}
