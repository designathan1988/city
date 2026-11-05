// The code of src/core/commands.ts that is the feature's (the rest of the file is shared).

// src/core/commands.ts:43-46 method CommandRegistry.register: the feature's own
  register(command: Command<Ctx>): void {
    if (this.commands.has(command.id)) throw new Error(`Command ${command.id} registered twice`);
    this.commands.set(command.id, command);
  }

// src/core/commands.ts:48-51 method CommandRegistry.bind: the feature's own
  bind(rule: KeyRule<Ctx>): void {
    if (!this.commands.has(rule.command)) throw new Error(`Key ${String(rule.code ?? rule.key ?? 'any')} bound to an unknown command ${rule.command}`);
    this.keys.push(rule);
  }
