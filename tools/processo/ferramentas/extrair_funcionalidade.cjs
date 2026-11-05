// Extrai o código completo de uma funcionalidade de um projeto TypeScript, fechando sobre tudo o que ela usa, a partir
// de uma ou mais entradas, e junta num só Markdown: a árvore de uso e o código de cada declaração tocada.
// Como fecha (o verificador de tipos do próprio projeto, não busca de texto):
// - todo identificador dentro de uma declaração coletada é resolvido ao seu símbolo e a declaração dele entra
//   (funções chamadas ou passadas como valor, métodos, construtores, campos, classes, constantes, enums, tipos);
// - método ou campo de interface, ou método abstrato: entram todas as implementações e, se o campo é uma função,
//   todo lugar que atribui um valor a ele (callbacks registrados);
// - workers e imports dinâmicos (new Worker(new URL('./x', import.meta.url)), import X from './x?worker',
//   import('./x')): entra o arquivo inteiro do módulo, com o tratamento das mensagens;
// - só o que está em src/ (o resto é biblioteca); --excluir corta pastas que você não quer.
// Só lê o projeto; escreve apenas o arquivo de saída.
//
// Uso:
//   node extrair_funcionalidade.cjs --raiz <projeto> --entrada <arquivo.ts>[:<nome>] [--entrada ...]
//        [--excluir src/render] [--saida <arquivo.md>]
// <nome>: função, Classe.metodo, Classe ou constante; sem <nome>, o arquivo inteiro é a entrada.
'use strict';
const fs = require('fs');
const path = require('path');

function args(argv) {
	const o = { entradas: [], excluir: [] };
	for (let i = 2; i < argv.length; i++) {
		const a = argv[i];
		if (a === '--raiz') o.raiz = argv[++i];
		else if (a === '--entrada') o.entradas.push(argv[++i]);
		else if (a === '--excluir') o.excluir.push(argv[++i]);
		else if (a === '--saida') o.saida = argv[++i];
		else throw new Error('argumento desconhecido: ' + a);
	}
	if (!o.raiz || !o.entradas.length || !o.saida) throw new Error('faltam --raiz, --entrada ou --saida');
	return o;
}

const opt = args(process.argv);
const raiz = path.resolve(opt.raiz);
const ts = require(path.join(raiz, 'node_modules', 'typescript'));
const norm = (f) => path.resolve(f).split(path.sep).join('/');
const src = norm(path.join(raiz, 'src')) + '/';
const fora = opt.excluir.map((e) => norm(path.resolve(raiz, e)) + '/');
const dentro = (f) => {
	const n = norm(f);
	return n.startsWith(src) && !fora.some((e) => n.startsWith(e));
};
const rel = (f) => path.relative(raiz, f).split(path.sep).join('/');

const cfg = ts.parseJsonConfigFileContent(ts.readConfigFile(path.join(raiz, 'tsconfig.json'), ts.sys.readFile).config, ts.sys, raiz);
const host = {
	getScriptFileNames: () => cfg.fileNames,
	getScriptVersion: () => '0',
	getScriptSnapshot: (f) => (fs.existsSync(f) ? ts.ScriptSnapshot.fromString(fs.readFileSync(f, 'utf8')) : undefined),
	getCurrentDirectory: () => raiz,
	getCompilationSettings: () => cfg.options,
	getDefaultLibFileName: (o) => ts.getDefaultLibFilePath(o),
	fileExists: ts.sys.fileExists,
	readFile: ts.sys.readFile,
	readDirectory: ts.sys.readDirectory,
	directoryExists: ts.sys.directoryExists,
	getDirectories: ts.sys.getDirectories,
};
const ls = ts.createLanguageService(host, ts.createDocumentRegistry());
const program = ls.getProgram();
const checker = program.getTypeChecker();
const fonte = (f) => program.getSourceFile(f) || program.getSourceFile(norm(f));

// A unidade que se imprime para uma declaração: o membro de classe, a função, a instrução de módulo (const, enum,
// tipo, interface, classe sem membros soltos) ou, dentro de uma função, a própria função que a contém.
function unidade(no) {
	for (let p = no; p && !ts.isSourceFile(p); p = p.parent) {
		const pai = p.parent;
		if (ts.isClassElement(p) && pai && ts.isClassLike(pai)) return p;
		if (ts.isFunctionDeclaration(p) && pai && (ts.isSourceFile(pai) || ts.isModuleBlock(pai))) return p;
		if (pai && (ts.isSourceFile(pai) || ts.isModuleBlock(pai))) {
			if (ts.isClassDeclaration(p)) return cabecaDeClasse(p);
			return p;
		}
		if (ts.isTypeElement(p) && pai && ts.isInterfaceDeclaration(pai)) return pai;
	}
	return null;
}
// Uma classe como unidade: só a cabeça (nome, herança) e os campos; os métodos entram um a um, quando usados.
const cabecas = new Map();
function cabecaDeClasse(c) {
	if (!cabecas.has(c)) cabecas.set(c, { classe: c });
	return cabecas.get(c);
}

const unidades = new Map(); // chave -> { no, arquivo, nome, de }
const usa = new Map(); // chave -> Set(chave)
const fila = [];
const chave = (u) => (u.classe ? 'C' : 'N') + norm((u.classe || u).getSourceFile().fileName) + '@' + (u.classe || u).getStart();
const nomeDe = (u) => {
	const n = u.classe || u;
	const nm = n.name && n.name.getText ? n.name.getText() : null;
	if (u.classe) return 'class ' + nm;
	if (ts.isConstructorDeclaration(n)) return (n.parent.name ? n.parent.name.getText() : '?') + '.constructor';
	if (ts.isClassElement(n) && n.parent && n.parent.name) return n.parent.name.getText() + '.' + (nm || '?');
	if (ts.isVariableStatement(n)) return n.declarationList.declarations.map((d) => d.name.getText()).join(', ');
	return nm || ts.SyntaxKind[n.kind];
};
function adicionar(u, de) {
	if (!u) return;
	const sf = (u.classe || u).getSourceFile();
	if (!dentro(sf.fileName)) return;
	const k = chave(u);
	if (de && de !== k) {
		if (!usa.has(de)) usa.set(de, new Set());
		usa.get(de).add(k);
	}
	if (unidades.has(k)) return;
	unidades.set(k, { u, arquivo: sf.fileName, nome: nomeDe(u) });
	fila.push(k);
}

function declaracoesDe(sim) {
	if (!sim) return [];
	if (sim.flags & ts.SymbolFlags.Alias) {
		try { sim = checker.getAliasedSymbol(sim); } catch { return []; }
	}
	return sim.declarations || [];
}

// Implementações de um membro de interface ou abstrato, e quem atribui a um campo de função (uma vez por membro; as
// arestas de uso repetem para cada quem usa).
const implCache = new Map();
function implementacoes(decl, de) {
	const nome = decl.name;
	if (!nome) return;
	if (implCache.has(decl)) {
		for (const k of implCache.get(decl)) if (unidades.has(k) && de !== k) (usa.get(de) || usa.set(de, new Set()).get(de)).add(k);
		return;
	}
	const antes = new Set(unidades.keys());
	implCache.set(decl, []);
	implementacoesDe(decl, de);
	implCache.set(decl, [...unidades.keys()].filter((k) => !antes.has(k)));
}
function implementacoesDe(decl, de) {
	const nome = decl.name;
	const sf = decl.getSourceFile();
	for (const im of ls.getImplementationAtPosition(sf.fileName, nome.getStart()) || []) {
		const isf = fonte(im.fileName);
		if (!isf || !dentro(im.fileName)) continue;
		const no = acharNo(isf, im.textSpan.start);
		adicionar(no && unidade(no), de);
	}
	const t = checker.getTypeAtLocation(decl);
	if (t && t.getCallSignatures().length) {
		for (const r of ls.getReferencesAtPosition(sf.fileName, nome.getStart()) || []) {
			if (!r.isWriteAccess || !dentro(r.fileName)) continue;
			const rsf = fonte(r.fileName);
			const no = acharNo(rsf, r.textSpan.start);
			adicionar(no && unidade(no), de);
		}
	}
}
function acharNo(sf, pos) {
	let achado = null;
	const v = (n) => {
		if (pos >= n.getStart(sf) && pos < n.end) {
			achado = n;
			ts.forEachChild(n, v);
		}
	};
	ts.forEachChild(sf, v);
	return achado;
}

// Módulo carregado por worker ou import dinâmico: o arquivo inteiro entra.
function moduloInteiro(especificador, deArquivo, de) {
	const limpo = especificador.replace(/\?.*$/, '');
	const r = ts.resolveModuleName(limpo, deArquivo, cfg.options, ts.sys).resolvedModule;
	let alvo = r && r.resolvedFileName;
	if (!alvo) {
		const tentativa = path.resolve(path.dirname(deArquivo), limpo);
		for (const ext of ['', '.ts', '.tsx', '.js']) if (fs.existsSync(tentativa + ext) && fs.statSync(tentativa + ext).isFile()) { alvo = tentativa + ext; break; }
	}
	const sf = alvo && fonte(alvo);
	if (!sf) return;
	for (const st of sf.statements) {
		if (ts.isImportDeclaration(st) || ts.isExportDeclaration(st)) continue;
		adicionar(ts.isClassDeclaration(st) ? cabecaDeClasse(st) : st, de);
		if (ts.isClassDeclaration(st)) for (const m of st.members) adicionar(m, de);
	}
}
const ehWorker = (n) => ts.isNewExpression(n) && /Worker$/.test(n.expression.getText());

function percorrer(k) {
	const { u } = unidades.get(k);
	const raizNo = u.classe || u;
	const sf = raizNo.getSourceFile();
	const visitar = (n) => {
		// numa cabeça de classe, os métodos não entram por si: só a herança e os campos
		if (u.classe && n.parent === u.classe && (ts.isMethodDeclaration(n) || ts.isGetAccessor(n) || ts.isSetAccessor(n))) return;
		if (ts.isIdentifier(n) || ts.isPrivateIdentifier(n)) {
			let sim = checker.getSymbolAtLocation(n);
			if (n.parent && ts.isShorthandPropertyAssignment(n.parent)) sim = checker.getShorthandAssignmentValueSymbol(n.parent) || sim;
			for (const d of declaracoesDe(sim)) {
				if (!dentro(d.getSourceFile().fileName)) continue;
				if (d.getSourceFile() === sf && d.getStart() >= raizNo.getStart() && d.end <= raizNo.end) continue; // local
				adicionar(unidade(d), k);
				const abstrato = ts.canHaveModifiers(d) && (ts.getModifiers(d) || []).some((m) => m.kind === ts.SyntaxKind.AbstractKeyword);
				const campoDeFuncao = ts.isPropertySignature(d) && checker.getTypeAtLocation(d).getCallSignatures().length > 0;
				if (ts.isMethodSignature(d) || campoDeFuncao || abstrato) implementacoes(d, k);
			}
		}
		if (ehWorker(n) || (ts.isCallExpression(n) && n.expression.kind === ts.SyntaxKind.ImportKeyword)) {
			const visitarStr = (m) => {
				if (ts.isStringLiteralLike(m)) moduloInteiro(m.text, sf.fileName, k);
				ts.forEachChild(m, visitarStr);
			};
			ts.forEachChild(n, visitarStr);
		}
		if (ts.isImportDeclaration(n) && ts.isStringLiteral(n.moduleSpecifier) && /\?worker|\?sharedworker/.test(n.moduleSpecifier.text)) {
			moduloInteiro(n.moduleSpecifier.text, sf.fileName, k);
		}
		ts.forEachChild(n, visitar);
	};
	if (u.classe) {
		visitar(u.classe.name || u.classe);
		for (const h of u.classe.heritageClauses || []) visitar(h);
		for (const m of u.classe.members) if (ts.isPropertyDeclaration(m) || ts.isConstructorDeclaration(m)) adicionar(m, k);
	} else {
		visitar(u);
	}
}

// Entradas.
function entrada(spec) {
	const i = spec.lastIndexOf(':');
	const temNome = i > 2; // não confundir com C:
	const arquivo = norm(path.resolve(raiz, temNome ? spec.slice(0, i) : spec));
	const sf = fonte(arquivo);
	if (!sf) throw new Error('arquivo fora do programa: ' + arquivo);
	if (!temNome) {
		moduloInteiro(arquivo, arquivo, null);
		return;
	}
	const nome = spec.slice(i + 1);
	const [classe, membro] = nome.includes('.') ? nome.split('.') : [null, nome];
	let achou = null;
	const v = (n) => {
		if (achou) return;
		const nm = n.name && ts.isIdentifier(n.name) ? n.name.text : null;
		if (!classe && nm === membro && (ts.isFunctionDeclaration(n) || ts.isVariableDeclaration(n) || ts.isClassDeclaration(n) ||
			ts.isEnumDeclaration(n) || ts.isInterfaceDeclaration(n) || ts.isTypeAliasDeclaration(n))) achou = n;
		if (classe && ts.isClassDeclaration(n) && nm === classe) {
			for (const m of n.members) if (m.name && ts.isIdentifier(m.name) && m.name.text === membro) achou = m;
		}
		ts.forEachChild(n, v);
	};
	v(sf);
	if (!achou) throw new Error('não achei ' + nome + ' em ' + rel(arquivo));
	if (ts.isClassDeclaration(achou)) {
		adicionar(cabecaDeClasse(achou), null);
		for (const m of achou.members) adicionar(m, null);
	} else {
		adicionar(unidade(achou), null);
	}
}

const t0 = Date.now();
const raizes = [];
for (const e of opt.entradas) {
	const antes = unidades.size;
	entrada(e);
	raizes.push(...[...unidades.keys()].slice(antes));
}
while (fila.length) percorrer(fila.shift());

// Saída.
const sfDe = (k) => (unidades.get(k).u.classe || unidades.get(k).u).getSourceFile();
const linha = (k) => sfDe(k).getLineAndCharacterOfPosition((unidades.get(k).u.classe || unidades.get(k).u).getStart()).line + 1;
const rotulo = (k) => `${unidades.get(k).nome} (${rel(unidades.get(k).arquivo)}:${linha(k)})`;
const ehFuncao = (k) => {
	const u = unidades.get(k).u;
	if (u.classe) return false;
	if (ts.isFunctionDeclaration(u) || ts.isMethodDeclaration(u) || ts.isConstructorDeclaration(u) || ts.isGetAccessor(u) || ts.isSetAccessor(u)) return true;
	if (ts.isVariableStatement(u)) return u.declarationList.declarations.some((d) => d.initializer && (ts.isArrowFunction(d.initializer) || ts.isFunctionExpression(d.initializer)));
	if (ts.isPropertyDeclaration(u)) return !!(u.initializer && (ts.isArrowFunction(u.initializer) || ts.isFunctionExpression(u.initializer)));
	return false;
};
const arquivos = new Map();
for (const [k, v] of unidades) {
	const f = norm(v.arquivo);
	if (!arquivos.has(f)) arquivos.set(f, []);
	arquivos.get(f).push(k);
}
const out = [];
const nFun = [...unidades.keys()].filter(ehFuncao).length;
out.push(`# Funcionalidade: ${opt.entradas.join(', ')}`, '');
out.push(`${unidades.size} declarações (${nFun} funções e métodos) em ${arquivos.size} arquivos, fechadas sobre tudo o que usam dentro de src/${fora.length ? ' (excluídas: ' + opt.excluir.join(', ') + ')' : ''}. ${((Date.now() - t0) / 1000).toFixed(1)} s.`, '');
out.push('## Árvore de uso (funções e métodos)', '', '```');
const impressos = new Set();
const arvore = (k, prof, caminho) => {
	const rep = impressos.has(k);
	out.push('  '.repeat(prof) + rotulo(k) + (rep ? '  (já listado acima)' : ''));
	if (rep) return;
	impressos.add(k);
	for (const f of [...(usa.get(k) || [])].filter(ehFuncao)) if (!caminho.has(f)) arvore(f, prof + 1, new Set([...caminho, f]));
};
for (const r of raizes) if (ehFuncao(r) || raizes.length < 20) arvore(r, 0, new Set([r]));
out.push('```', '');
out.push('## Código, por arquivo', '');
for (const [f, ks] of [...arquivos.entries()].sort()) {
	const sf = fonte(f);
	const spans = ks.map((k) => {
		const u = unidades.get(k).u;
		if (u.classe) {
			const c = u.classe;
			const ini = c.getStart(sf);
			const fimCabeca = c.members.length ? c.members.pos : c.end;
			return { ini, fim: fimCabeca, txt: sf.text.slice(ini, fimCabeca).trimEnd() + '\n  // (membros usados listados abaixo)\n}', k };
		}
		return { ini: u.getStart(sf), fim: u.end, txt: sf.text.slice(u.getFullStart(), u.end).replace(/^\s*\n/, ''), k };
	}).sort((a, b) => a.ini - b.ini);
	out.push(`### ${rel(f)}`, '', '```ts');
	let ate = -1;
	for (const s of spans) {
		if (s.ini < ate && !unidades.get(s.k).u.classe) continue; // aninhada numa já impressa
		ate = Math.max(ate, s.fim);
		out.push(`// linha ${sf.getLineAndCharacterOfPosition(s.ini).line + 1}`, s.txt, '');
	}
	out.push('```', '');
}
fs.mkdirSync(path.dirname(path.resolve(opt.saida)), { recursive: true });
fs.writeFileSync(opt.saida, out.join('\n'), 'utf8');
console.log(`ok: ${unidades.size} declarações (${nFun} funções), ${arquivos.size} arquivos em ${((Date.now() - t0) / 1000).toFixed(1)} s -> ${opt.saida}`);
