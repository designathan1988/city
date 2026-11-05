// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:502-507 slice of src/ui/v2/shell.ts#mountShell.syncMoney (shared): the feature's statements; "…" marks code left out
  const syncMoney = (value: number): void => {
    // … 2 lines
    moneyText.data = formatMoney(value);
    money.title = balanceTip();
  };
