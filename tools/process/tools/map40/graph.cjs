// Grafo semântico do Roadcraft 4.0, programa inteiro, gerado por AST e pelo verificador de tipos do próprio projeto.
// Saída: records/map40/graph.json com
//   - unidades: toda declaração de topo e todo membro de classe de src/ (arquivo, linhas, tipo, exportada, comentário);
//   - arestas: chamada, reading/escrita e referência de tipo entre unidades, resolvidas pelo verificador (não por texto);
//   - canais dinâmicos: workers (new Worker / ?worker), mensagens (postMessage/onmessage/discriminantes), eventos do DOM,
//     timers, ganchos do three.js, chaves de texto t('...'), chaves de localStorage, recursos (?url, fetch, JSON);
//   - estado: campos do documento (src/world/doc.ts) e de estados da simulação lidos e escritos por cada unidade;
//   - testes: cada spec de tests/ e as unidades de src/ que ele importa, com os títulos de describe/it.
// Só lê o 4.0. Uso: node tools/map40/graph.cjs [--raiz <4.0>] [--saida <graph.json>]
'use strict';
const fs = require('fs');
const path = require('path');

const ARGS = process.argv.slice(2);
const opt = (nome, padrao) => { const i = ARGS.indexOf(nome); return i >= 0 ? ARGS[i + 1] : padrao; };
const RAIZ = path.resolve(opt('--raiz', 'C:/Users/Jonathan/Documents/Labs/roadcraft-4.0-main'));
const SAIDA = path.resolve(opt('--saida', path.join(__dirname, '..', '..', 'records', 'map40', 'graph.json')));
const ts = require(path.join(RAIZ, 'node_modules', 'typescript'));
const norm = (f) => path.resolve(f).split(path.sep).join('/');
const SRC = norm(path.join(RAIZ, 'src')) + '/';
const TESTS = norm(path.join(RAIZ, 'tests')) + '/';
const rel = (f) => norm(f).slice(norm(RAIZ).length + 1);
const emSrc = (f) => norm(f).startsWith(SRC);
const emTests = (f) => norm(f).startsWith(TESTS);

const t0 = Date.now();
const cfg = ts.parseJsonConfigFileContent(ts.readConfigFile(path.join(RAIZ, 'tsconfig.json'), ts.sys.readFile).config, ts.sys, RAIZ);
const program = ts.createProgram({ rootNames: cfg.fileNames, options: cfg.options });
const checker = program.getTypeChecker();
console.error(`programa: ${cfg.fileNames.length} arquivos em ${((Date.now() - t0) / 1000).toFixed(1)} s`);

// ---- unidades ------------------------------------------------------------------------------------------------
const unidades = new Map(); // chave -> registro
const chaveDe = new Map(); // nó -> chave
function nomeDe(n) {
  if (ts.isConstructorDeclaration(n)) return 'constructor';
  if (n.name && ts.isIdentifier(n.name)) return n.name.text;
  if (n.name && (ts.isStringLiteral(n.name) || ts.isNumericLiteral(n.name))) return n.name.text;
  if (ts.isVariableStatement(n)) return n.declarationList.declarations.map((d) => d.name.getText()).join(',');
  if (ts.isExportAssignment(n)) return 'default';
  return ts.SyntaxKind[n.kind];
}
function tipoDe(n) {
  if (ts.isFunctionDeclaration(n)) return 'function';
  if (ts.isClassDeclaration(n)) return 'class';
  if (ts.isInterfaceDeclaration(n)) return 'interface';
  if (ts.isTypeAliasDeclaration(n)) return 'type';
  if (ts.isEnumDeclaration(n)) return 'enum';
  if (ts.isVariableStatement(n)) {
    const d = n.declarationList.declarations[0];
    return d && d.initializer && (ts.isArrowFunction(d.initializer) || ts.isFunctionExpression(d.initializer)) ? 'function' : 'variable';
  }
  if (ts.isMethodDeclaration(n) || ts.isConstructorDeclaration(n) || ts.isGetAccessor(n) || ts.isSetAccessor(n)) return 'method';
  if (ts.isPropertyDeclaration(n)) return 'field';
  if (ts.isModuleDeclaration(n)) return 'namespace';
  return 'statement';
}
function comentario(n, sf) {
  const faixas = ts.getLeadingCommentRanges(sf.text, n.getFullStart()) || [];
  const ultimo = faixas[faixas.length - 1];
  if (!ultimo) return '';
  return sf.text.slice(ultimo.pos, ultimo.end).replace(/^\/\*\*?|\*\/$/g, '').replace(/^\s*\*\s?/gm, '').replace(/^\/\/\s?/gm, '').trim().slice(0, 240);
}
const exportada = (n) => !!(ts.canHaveModifiers(n) && (ts.getModifiers(n) || []).some((m) => m.kind === ts.SyntaxKind.ExportKeyword));

function registrar(n, sf, pai) {
  const ini = sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1;
  const fim = sf.getLineAndCharacterOfPosition(n.end).line + 1;
  const nome = nomeDe(n);
  const chave = `${rel(sf.fileName)}#${pai ? pai + '.' : ''}${nome}@${ini}`;
  chaveDe.set(n, chave);
  unidades.set(chave, { chave, arquivo: rel(sf.fileName), nome: pai ? `${pai}.${nome}` : nome, tipo: tipoDe(n), ini, fim, linhas: fim - ini + 1,
    exportada: pai ? exportada(n) || true : exportada(n), doc: comentario(n, sf), pai: pai || null });
  return chave;
}
for (const sf of program.getSourceFiles()) {
  if (!emSrc(sf.fileName) || sf.isDeclarationFile) continue;
  for (const st of sf.statements) {
    if (ts.isImportDeclaration(st) || ts.isExportDeclaration(st)) continue;
    const chave = registrar(st, sf, null);
    if (ts.isClassDeclaration(st)) {
      for (const m of st.members) if (ts.isClassElement(m) && !ts.isSemicolonClassElement(m)) registrar(m, sf, unidades.get(chave).nome);
    }
  }
}
console.error(`unidades: ${unidades.size}`);

// A unidade que contém um nó: o membro de classe, a declaração de topo.
function unidadeDe(n) {
  for (let p = n; p && !ts.isSourceFile(p); p = p.parent) {
    const k = chaveDe.get(p);
    if (k) return k;
  }
  return null;
}
function unidadeDaDeclaracao(d) {
  // membro de classe → o membro; senão a declaração de topo que o contém
  for (let p = d; p && !ts.isSourceFile(p); p = p.parent) {
    if (chaveDe.has(p)) return chaveDe.get(p);
  }
  return null;
}
function declaracoes(sim) {
  if (!sim) return [];
  if (sim.flags & ts.SymbolFlags.Alias) { try { sim = checker.getAliasedSymbol(sim); } catch { return []; } }
  return sim.declarations || [];
}

// ---- arestas e canais --------------------------------------------------------------------------------------
const arestas = new Map(); // "de\tpara\ttipo" -> contagem
const canais = [];        // {tipo, de, ...}
const estado = new Map(); // "unidade\tcampo\tmodo" -> contagem
const ESTADO_ARQUIVOS = new Set(['src/world/doc.ts', 'src/sim/host/protocol.ts', 'src/sim/crossings/state.ts', 'src/sim/intersections/claims.ts',
  'src/sim/vehicles/state.ts', 'src/sim/world.ts']);
const HOOKS_THREE = new Set(['onBeforeRender', 'onAfterRender', 'onBeforeCompile', 'onBeforeShadow', 'onAfterShadow', 'customDepthMaterial']);
const TIMERS = new Set(['setTimeout', 'setInterval', 'requestAnimationFrame', 'requestIdleCallback', 'queueMicrotask']);
function aresta(de, para, tipo) {
  if (!de || !para || de === para) return;
  const k = `${de}\t${para}\t${tipo}`;
  arestas.set(k, (arestas.get(k) || 0) + 1);
}
function tipoDeUso(id) {
  const p = id.parent;
  if (!p) return 'read';
  if ((ts.isCallExpression(p) || ts.isNewExpression(p)) && p.expression === id) return 'call';
  if (ts.isPropertyAccessExpression(p) && p.name === id) {
    const pp = p.parent;
    if ((ts.isCallExpression(pp) || ts.isNewExpression(pp)) && pp.expression === p) return 'call';
    if (ts.isBinaryExpression(pp) && pp.left === p && pp.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && pp.operatorToken.kind <= ts.SyntaxKind.LastAssignment) return 'write';
    return 'read';
  }
  if (ts.isTypeReferenceNode(p) || ts.isExpressionWithTypeArguments(p) || ts.isHeritageClause(p) || ts.isTypeQueryNode(p)) return 'typeRef';
  if (ts.isBinaryExpression(p) && p.left === id && p.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && p.operatorToken.kind <= ts.SyntaxKind.LastAssignment) return 'write';
  return 'read';
}
function literal(n) { return n && ts.isStringLiteralLike(n) ? n.text : (n && ts.isTemplateExpression(n) ? n.head.text + '…' : null); }

function resolverModulo(spec, de) {
  const limpo = spec.replace(/\?.*$/, '');
  const r = ts.resolveModuleName(limpo, de, cfg.options, ts.sys).resolvedModule;
  if (r) return rel(r.resolvedFileName);
  const tent = path.resolve(path.dirname(de), limpo);
  for (const ext of ['', '.ts', '.tsx', '.js', '.mjs']) if (fs.existsSync(tent + ext) && fs.statSync(tent + ext).isFile()) return rel(tent + ext);
  return limpo;
}

for (const sf of program.getSourceFiles()) {
  if (!emSrc(sf.fileName) || sf.isDeclarationFile) continue;
  const arquivo = rel(sf.fileName);
  const visitar = (n) => {
    if (ts.isIdentifier(n) || ts.isPrivateIdentifier(n)) {
      const de = unidadeDe(n);
      let sim = checker.getSymbolAtLocation(n);
      if (n.parent && ts.isShorthandPropertyAssignment(n.parent)) sim = checker.getShorthandAssignmentValueSymbol(n.parent) || sim;
      for (const d of declaracoes(sim)) {
        const f = d.getSourceFile();
        if (!emSrc(f.fileName)) continue;
        const uso = tipoDeUso(n);
        // estado: propriedade declarada num arquivo de documento/estado
        if ((ts.isPropertySignature(d) || ts.isPropertyDeclaration(d)) && ESTADO_ARQUIVOS.has(rel(f.fileName)) && de) {
          const dono = d.parent && d.parent.name ? d.parent.name.getText() : '?';
          const k = `${de}\t${dono}.${d.name.getText()}\t${uso === 'write' ? 'escreve' : 'le'}`;
          estado.set(k, (estado.get(k) || 0) + 1);
        }
        const para = unidadeDaDeclaracao(d);
        if (para && de && !(f === sf && d.getStart() >= 0 && chaveDe.get(d) === de)) aresta(de, para, uso);
      }
    }
    if (ts.isCallExpression(n)) {
      const de = unidadeDe(n);
      const txt = n.expression.getText(sf);
      const a0 = n.arguments[0];
      if (/^t$/.test(txt) || /\.t$/.test(txt) || txt === 'tr') { const k = literal(a0); if (k) canais.push({ tipo: 'texto', de, chave: k, arquivo }); }
      if (/addEventListener$/.test(txt) || /\.on$/.test(txt)) { const k = literal(a0); if (k) canais.push({ tipo: 'evento', de, nome: k, arquivo, modo: 'ouve' }); }
      if (/dispatchEvent$/.test(txt)) canais.push({ tipo: 'evento', de, nome: a0 ? a0.getText(sf).slice(0, 60) : '?', arquivo, modo: 'emite' });
      if (/postMessage$/.test(txt) && a0 && ts.isObjectLiteralExpression(a0)) {
        const disc = a0.properties.find((p) => ts.isPropertyAssignment(p) && /^(type|kind|op|cmd|tag)$/.test(p.name.getText()));
        canais.push({ tipo: 'mensagem', de, discriminante: disc ? literal(disc.initializer) || disc.initializer.getText(sf).slice(0, 40) : '?', arquivo, modo: 'envia' });
      } else if (/postMessage$/.test(txt)) canais.push({ tipo: 'mensagem', de, discriminante: a0 ? a0.getText(sf).slice(0, 40) : '?', arquivo, modo: 'envia' });
      if (TIMERS.has(txt)) canais.push({ tipo: 'timer', de, nome: txt, arquivo });
      if (/localStorage\.(get|set|remove)Item$/.test(txt)) { const k = literal(a0); if (k) canais.push({ tipo: 'localStorage', de, chave: k, arquivo, modo: txt.endsWith('getItem') ? 'le' : 'escreve' }); }
      if (txt === 'fetch') { const k = literal(a0); canais.push({ tipo: 'recurso', de, url: k || a0.getText(sf).slice(0, 80), arquivo, modo: 'fetch' }); }
      if (n.expression.kind === ts.SyntaxKind.ImportKeyword && a0) { const k = literal(a0); if (k) canais.push({ tipo: 'moduloDinamico', de, modulo: resolverModulo(k, sf.fileName), arquivo }); }
    }
    if (ts.isNewExpression(n) && /Worker$/.test(n.expression.getText(sf))) {
      const url = n.arguments && n.arguments[0];
      let alvo = '?';
      if (url && ts.isNewExpression(url) && url.arguments && url.arguments[0]) alvo = resolverModulo(literal(url.arguments[0]) || '?', sf.fileName);
      canais.push({ tipo: 'worker', de: unidadeDe(n), modulo: alvo, arquivo });
    }
    if (ts.isImportDeclaration(n) && ts.isStringLiteral(n.moduleSpecifier)) {
      const spec = n.moduleSpecifier.text;
      if (/\?(worker|sharedworker)/.test(spec)) canais.push({ tipo: 'worker', de: null, modulo: resolverModulo(spec, sf.fileName), arquivo });
      else if (/\?url|\?raw|\.(json|glsl|png|jpg|ktx2|gltf|glb|bin|csv|wasm)$/.test(spec)) canais.push({ tipo: 'recurso', de: null, url: resolverModulo(spec, sf.fileName), arquivo, modo: 'import' });
      else if (!spec.startsWith('.') && !spec.startsWith('@/') && !spec.startsWith('@core') && !spec.startsWith('@world') && !spec.startsWith('@sim') && !spec.startsWith('@render') && !spec.startsWith('@view') && !spec.startsWith('@editor') && !spec.startsWith('@ui') && !spec.startsWith('@people')) canais.push({ tipo: 'biblioteca', de: null, modulo: spec, arquivo });
    }
    if (ts.isPropertyAccessExpression(n) && HOOKS_THREE.has(n.name.text) && n.parent && ts.isBinaryExpression(n.parent) && n.parent.left === n) canais.push({ tipo: 'ganchoThree', de: unidadeDe(n), nome: n.name.text, arquivo });
    if (ts.isCaseClause(n) && n.expression && ts.isStringLiteralLike(n.expression)) {
      // discriminante de mensagem recebida: `switch (msg.type) { case 'x': ...`
      const sw = n.parent && n.parent.parent;
      const alvo = sw && ts.isSwitchStatement(sw) ? sw.expression.getText(sf) : '';
      if (/\.(type|kind|op|cmd|tag)$/.test(alvo)) canais.push({ tipo: 'mensagem', de: unidadeDe(n), discriminante: n.expression.text, arquivo, modo: 'recebe', sobre: alvo.slice(0, 40) });
    }
    ts.forEachChild(n, visitar);
  };
  ts.forEachChild(sf, visitar);
}
console.error(`arestas: ${arestas.size}, canais: ${canais.length}, estado: ${estado.size} (${((Date.now() - t0) / 1000).toFixed(1)} s)`);

// ---- testes --------------------------------------------------------------------------------------------------
const testes = [];
for (const sf of program.getSourceFiles()) {
  if (!emTests(sf.fileName) || sf.isDeclarationFile || !/\.spec\.ts$/.test(sf.fileName)) continue;
  const importa = new Set(), titulos = [];
  const visitar = (n) => {
    if (ts.isImportDeclaration(n) && ts.isStringLiteral(n.moduleSpecifier)) { const m = resolverModulo(n.moduleSpecifier.text, sf.fileName); if (m.startsWith('src/')) importa.add(m); }
    if (ts.isCallExpression(n) && /^(describe|it|test)$/.test(n.expression.getText(sf)) && n.arguments[0]) { const k = literal(n.arguments[0]); if (k) titulos.push(k); }
    ts.forEachChild(n, visitar);
  };
  ts.forEachChild(sf, visitar);
  testes.push({ arquivo: rel(sf.fileName), importa: [...importa], titulos: titulos.slice(0, 60) });
}

// ---- imports entre arquivos (para o fecho por arquivo) ----------------------------------------------------
const importacoes = [];
for (const sf of program.getSourceFiles()) {
  if (!emSrc(sf.fileName) || sf.isDeclarationFile) continue;
  for (const st of sf.statements) {
    if ((ts.isImportDeclaration(st) || ts.isExportDeclaration(st)) && st.moduleSpecifier && ts.isStringLiteral(st.moduleSpecifier)) {
      const m = resolverModulo(st.moduleSpecifier.text, sf.fileName);
      if (m.startsWith('src/')) importacoes.push([rel(sf.fileName), m]);
    }
  }
}

const saida = {
  gerado: new Date().toISOString(), raiz: norm(RAIZ), commit: (() => { try { return require('child_process').execSync('git rev-parse HEAD', { cwd: RAIZ, encoding: 'utf8' }).trim(); } catch { return null; } })(),
  arquivos: [...new Set([...unidades.values()].map((u) => u.arquivo))].sort(),
  unidades: [...unidades.values()],
  arestas: [...arestas].map(([k, n]) => { const [de, para, tipo] = k.split('\t'); return { de, para, tipo, n }; }),
  estado: [...estado].map(([k, n]) => { const [unidade, campo, modo] = k.split('\t'); return { unidade, campo, modo, n }; }),
  canais, testes, importacoes,
};
fs.mkdirSync(path.dirname(SAIDA), { recursive: true });
fs.writeFileSync(SAIDA, JSON.stringify(saida));
console.error(`grafo: ${saida.arquivos.length} arquivos, ${saida.unidades.length} unidades, ${saida.arestas.length} arestas, ${canais.length} canais, ${estado.size} acessos a estado, ${testes.length} specs -> ${SAIDA} (${((Date.now() - t0) / 1000).toFixed(1)} s)`);
