// The code of src/ui/roads/money.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/money.ts:16-18 function balanceTip: the feature's own
export function balanceTip(): string {
  return t('economy.balance', { share: Math.round(ROAD_TUNING.economy.demolitionRefund * 100) });
}
