// The code of src/editor/roads/economy.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/economy.ts:11-13 slice of src/editor/roads/economy.ts#roadsBefore (shared): the feature's statements; "…" marks code left out
export function roadsBefore(doc: RoadDoc): RoadsBefore {
  return { value: roadValue(doc), balance: doc.economy.balance };
}

// src/editor/roads/economy.ts:16-22 slice of src/editor/roads/economy.ts#settleRoadEdit (shared): the feature's statements; "…" marks code left out
export function settleRoadEdit(before: RoadsBefore, after: RoadDoc, apply = true): RoadCharge {
  const charge = chargeFor(before.value, roadValue(after), before.balance);
  if (apply && charge.affordable && charge.amount !== 0) {
    after.setBalance(charge.balance, charge.amount > 0 ? 'road paid' : 'demolition refunded');
  }
  return charge;
}
