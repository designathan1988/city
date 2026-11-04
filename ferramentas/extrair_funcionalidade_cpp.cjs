// Extrai o código completo de uma funcionalidade do núcleo C++ (native/src), fechando sobre tudo o que ela usa, a
// partir de uma ou mais entradas, pelo índice do clangd (o mesmo compilador e as mesmas flags do build:
// compile_commands.json), e junta num só Markdown: a árvore de uso e o código de cada declaração tocada.
// Como fecha:
// - cada nome dentro de uma declaração coletada que o clangd classifica como função, método, tipo, enum, campo,
//   variável de namespace ou de classe, ou macro (os locais e parâmetros ficam de fora) é resolvido à sua definição e a
//   declaração dela entra: chamadas diretas, funções passadas como valor, tipos, constantes, macros;
// - método virtual: entram todas as sobrescritas (textDocument/implementation);
// - só native/src (godot-cpp e a biblioteca padrão são bibliotecas); --excluir corta pastas.
// Só lê o projeto; o índice do clangd fica numa pasta temporária.
//
// Uso:
//   node extrair_funcionalidade_cpp.cjs --raiz <repo> --build <pasta com compile_commands.json>
//        --entrada <arquivo>:<Nome|Classe::metodo> [--entrada ...] [--excluir native/src/render] --saida <arquivo.md>
'use strict';
const fs = require('fs');
const os = require('os');
const path = require('path');
const { spawn } = require('child_process');

function args(argv) {
	const o = { entradas: [], excluir: [], clangd: process.env.CLANGD || 'clangd' };
	for (let i = 2; i < argv.length; i++) {
		const a = argv[i];
		if (a === '--raiz') o.raiz = argv[++i];
		else if (a === '--build') o.build = argv[++i];
		else if (a === '--entrada') o.entradas.push(argv[++i]);
		else if (a === '--excluir') o.excluir.push(argv[++i]);
		else if (a === '--clangd') o.clangd = argv[++i];
		else if (a === '--saida') o.saida = argv[++i];
		else throw new Error('argumento desconhecido: ' + a);
	}
	if (!o.raiz || !o.build || !o.entradas.length || !o.saida) throw new Error('faltam --raiz, --build, --entrada ou --saida');
	return o;
}
const opt = args(process.argv);
const raiz = path.resolve(opt.raiz);
const norm = (f) => path.resolve(f).split(path.sep).join('/').replace(/^([a-z]):/, (m, d) => d.toUpperCase() + ':');
const srcDir = norm(path.join(raiz, 'native', 'src')) + '/';
const fora = opt.excluir.map((e) => norm(path.resolve(raiz, e)) + '/');
const dentro = (f) => f.startsWith(srcDir) && !fora.some((e) => f.startsWith(e));
const rel = (f) => path.relative(raiz, f).split(path.sep).join('/');
const uriDe = (f) => 'file:///' + norm(f).replace(/:/, '%3A');
const arquivoDe = (u) => norm(decodeURIComponent(u.replace(/^file:\/\/\//, '')));

// Só as unidades de native/src no compile_commands (godot-cpp fora do índice: é biblioteca).
const cc = JSON.parse(fs.readFileSync(path.join(opt.build, 'compile_commands.json'), 'utf8'));
const proprio = cc.filter((e) => e.command && dentro(norm(path.resolve(e.directory, e.file))));
// Pasta fixa por repositório: o índice do clangd (.cache/clangd) fica nela e é reaproveitado na próxima extração.
const tmp = path.join(os.tmpdir(), 'extrator-cpp-' + Buffer.from(raiz).toString('hex').slice(-16));
fs.mkdirSync(tmp, { recursive: true });
fs.writeFileSync(path.join(tmp, 'compile_commands.json'), JSON.stringify(proprio));

// Cliente LSP mínimo sobre stdio.
const cd = spawn(opt.clangd, [`--compile-commands-dir=${tmp}`, '--background-index', '-j=4', '--log=error', '--header-insertion=never'], { stdio: ['pipe', 'pipe', 'ignore'] });
let buf = Buffer.alloc(0), seq = 0;
const pend = new Map();
const aoNotificar = [];
cd.stdout.on('data', (d) => {
	buf = Buffer.concat([buf, d]);
	for (;;) {
		const fimCab = buf.indexOf('\r\n\r\n');
		if (fimCab < 0) return;
		const m = /Content-Length: (\d+)/i.exec(buf.slice(0, fimCab).toString());
		const n = Number(m[1]);
		if (buf.length < fimCab + 4 + n) return;
		const msg = JSON.parse(buf.slice(fimCab + 4, fimCab + 4 + n).toString('utf8'));
		buf = buf.slice(fimCab + 4 + n);
		if (msg.id !== undefined && msg.method) enviar({ jsonrpc: '2.0', id: msg.id, result: null }); // pedidos do servidor
		else if (msg.id !== undefined && pend.has(msg.id)) { pend.get(msg.id)(msg); pend.delete(msg.id); }
		else if (msg.method) for (const f of aoNotificar) f(msg);
	}
});
function enviar(o) {
	const s = Buffer.from(JSON.stringify(o), 'utf8');
	cd.stdin.write(`Content-Length: ${s.length}\r\n\r\n`);
	cd.stdin.write(s);
}
const pedir = (method, params) => new Promise((res) => {
	const id = ++seq;
	pend.set(id, (m) => res(m.result));
	enviar({ jsonrpc: '2.0', id, method, params });
});
const notificar = (method, params) => enviar({ jsonrpc: '2.0', method, params });

const textos = new Map(), abertos = new Set(), simbolos = new Map(), tokens = new Map();
const texto = (f) => { if (!textos.has(f)) textos.set(f, fs.readFileSync(f, 'utf8')); return textos.get(f); };
const linhas = (f) => texto(f).split('\n');
function abrir(f) {
	if (abertos.has(f)) return;
	abertos.add(f);
	const ext = path.extname(f);
	notificar('textDocument/didOpen', { textDocument: { uri: uriDe(f), languageId: ext === '.c' ? 'c' : 'cpp', version: 1, text: texto(f) } });
}
let legenda;
async function simbolosDe(f) {
	if (!simbolos.has(f)) {
		abrir(f);
		const lista = [];
		const plano = (ss, pai) => { for (const s of ss || []) { lista.push({ ...s, pai }); plano(s.children, s); } };
		plano(await pedir('textDocument/documentSymbol', { textDocument: { uri: uriDe(f) } }), null);
		simbolos.set(f, lista);
	}
	return simbolos.get(f);
}
async function tokensDe(f) {
	if (!tokens.has(f)) {
		abrir(f);
		const r = await pedir('textDocument/semanticTokens/full', { textDocument: { uri: uriDe(f) } });
		const d = (r && r.data) || [];
		const lista = [];
		let ln = 0, col = 0;
		for (let i = 0; i < d.length; i += 5) {
			ln += d[i];
			col = d[i] ? d[i + 1] : col + d[i + 1];
			const mods = [];
			legenda.tokenModifiers.forEach((m, b) => { if (d[i + 4] & (1 << b)) mods.push(m); });
			lista.push({ ln, col, len: d[i + 2], tipo: legenda.tokenTypes[d[i + 3]], mods });
		}
		tokens.set(f, lista);
	}
	return tokens.get(f);
}

// Tipos de símbolo do LSP que viram unidade (função, método, construtor, classe, struct, enum, variável, campo, tipo).
const SK = { File: 1, Module: 2, Namespace: 3, Package: 4, Class: 5, Method: 6, Property: 7, Field: 8, Constructor: 9, Enum: 10, Interface: 11, Function: 12, Variable: 13, Constant: 14, Struct: 23, EnumMember: 22, TypeParameter: 26 };
const UNIDADE = new Set([SK.Class, SK.Method, SK.Property, SK.Field, SK.Constructor, SK.Enum, SK.Interface, SK.Function, SK.Variable, SK.Constant, SK.Struct]);
const contem = (r, p) => (p.line > r.start.line || (p.line === r.start.line && p.character >= r.start.character)) &&
	(p.line < r.end.line || (p.line === r.end.line && p.character <= r.end.character));
// A unidade de uma posição: o símbolo mais externo de função ou método que a contém; senão o tipo (classe, struct,
// enum) mais externo; senão a variável ou constante de namespace.
async function unidadeEm(f, p) {
	const ss = (await simbolosDe(f)).filter((s) => UNIDADE.has(s.kind) && contem(s.range, p));
	if (!ss.length) return macroEm(f, p);
	const funcs = ss.filter((s) => [SK.Function, SK.Method, SK.Constructor].includes(s.kind));
	const tiposExt = ss.filter((s) => [SK.Class, SK.Struct, SK.Enum, SK.Interface].includes(s.kind));
	const larg = (s) => (s.range.end.line - s.range.start.line) * 1e6 + (s.range.end.character - s.range.start.character);
	const escolher = (l) => l.sort((a, b) => larg(b) - larg(a))[0];
	const s = funcs.length ? escolher(funcs) : tiposExt.length ? escolher(tiposExt) : escolher(ss);
	const nome = [s.pai && s.pai.kind !== SK.Namespace ? s.pai.name : null, s.name].filter(Boolean).join('::');
	return { f, ini: s.range.start, fim: s.range.end, nome, tipo: s.kind };
}
// Uma macro: o #define e as linhas continuadas com barra invertida.
function macroEm(f, p) {
	const ls = linhas(f);
	let i = p.line;
	if (!/^\s*#\s*define\b/.test(ls[i] || '')) return null;
	let j = i;
	while (/\\\s*$/.test(ls[j]) && j + 1 < ls.length) j++;
	const nome = (/#\s*define\s+(\w+)/.exec(ls[i]) || [])[1] || 'macro';
	return { f, ini: { line: i, character: 0 }, fim: { line: j, character: ls[j].length }, nome: '#define ' + nome, tipo: 0 };
}
const chave = (u) => `${u.f}@${u.ini.line}:${u.ini.character}`;

const unidades = new Map(), usa = new Map(), fila = [];
function adicionar(u, de) {
	if (!u || !dentro(u.f)) return null;
	const k = chave(u);
	if (de && de !== k) { if (!usa.has(de)) usa.set(de, new Set()); usa.get(de).add(k); }
	if (!unidades.has(k)) { unidades.set(k, u); fila.push(k); }
	return k;
}

// Tipos de token cujo nome vale seguir (os locais e parâmetros ficam de fora).
const SEGUIR = new Set(['function', 'method', 'class', 'enum', 'enumMember', 'type', 'struct', 'interface', 'typeParameter', 'concept', 'namespace', 'macro', 'property', 'variable']);
const implVisto = new Map();
async function percorrer(k) {
	const u = unidades.get(k);
	const toks = (await tokensDe(u.f)).filter((t) => contem({ start: u.ini, end: u.fim }, { line: t.ln, character: t.col }));
	const seguir = toks.filter((t) => SEGUIR.has(t.tipo) && t.tipo !== 'namespace' && !t.mods.includes('functionScope') && !t.mods.includes('declaration') && !(t.tipo === 'variable' && t.mods.includes('functionScope')));
	const defs = await Promise.all(seguir.map((t) => pedir('textDocument/definition', { textDocument: { uri: uriDe(u.f) }, position: { line: t.ln, character: t.col } }).then((r) => ({ t, r }))));
	for (const { t, r } of defs) {
		for (const loc of [].concat(r || [])) {
			const uri = loc.uri || loc.targetUri;
			const rg = loc.range || loc.targetSelectionRange;
			if (!uri || !rg) continue;
			const f = arquivoDe(uri);
			if (!dentro(f)) continue;
			const alvo = await unidadeEm(f, rg.start);
			const kk = adicionar(alvo, k);
			if (kk && t.tipo === 'method' && t.mods.includes('virtual')) await sobrescritas(f, rg.start, k);
		}
	}
}
async function sobrescritas(f, p, de) {
	const kk = `${f}@${p.line}:${p.character}`;
	if (!implVisto.has(kk)) {
		abrir(f);
		const r = await pedir('textDocument/implementation', { textDocument: { uri: uriDe(f) }, position: p });
		const lista = [];
		for (const loc of [].concat(r || [])) {
			const g = arquivoDe(loc.uri || loc.targetUri);
			if (dentro(g)) lista.push(await unidadeEm(g, (loc.range || loc.targetSelectionRange).start));
		}
		implVisto.set(kk, lista);
	}
	for (const u of implVisto.get(kk)) adicionar(u, de);
}

async function entrada(spec) {
	const m = /^(.*?\.(?:cpp|cc|cxx|h|hpp|hh|inl)):(.+)$/.exec(spec);
	if (!m) throw new Error('entrada deve ser <arquivo .cpp/.h>:<Nome|Classe::metodo>: ' + spec);
	const f = norm(path.resolve(raiz, m[1]));
	const nome = m[2];
	const partes = nome.split('::');
	const ss = await simbolosDe(f);
	const s = ss.find((x) => x.name === partes[partes.length - 1] && (partes.length === 1 || (x.pai && x.pai.name === partes[partes.length - 2])) && UNIDADE.has(x.kind))
		|| ss.find((x) => x.name === nome);
	if (!s) throw new Error(`não achei ${nome} em ${rel(f)}`);
	const ks = [adicionar({ f, ini: s.range.start, fim: s.range.end, nome, tipo: s.kind }, null)];
	if ([SK.Class, SK.Struct].includes(s.kind)) {
		// a classe inteira: os métodos definidos fora dela (.cpp) também entram
		for (const t of (await tokensDe(f)).filter((t) => contem(s.range, { line: t.ln, character: t.col }) && t.tipo === 'method' && t.mods.includes('declaration'))) {
			const r = await pedir('textDocument/definition', { textDocument: { uri: uriDe(f) }, position: { line: t.ln, character: t.col } });
			for (const loc of [].concat(r || [])) {
				const g = arquivoDe(loc.uri || loc.targetUri);
				if (dentro(g)) ks.push(adicionar(await unidadeEm(g, (loc.range || loc.targetSelectionRange).start), ks[0]));
			}
		}
	}
	return ks.filter(Boolean);
}

(async () => {
	const t0 = Date.now();
	const ini = await pedir('initialize', {
		processId: process.pid, rootUri: uriDe(raiz),
		capabilities: {
			window: { workDoneProgress: true },
			textDocument: { documentSymbol: { hierarchicalDocumentSymbolSupport: true }, semanticTokens: { requests: { full: true }, tokenTypes: [], tokenModifiers: [], formats: ['relative'] }, definition: { linkSupport: true }, implementation: { linkSupport: true } },
		},
	});
	legenda = ini.capabilities.semanticTokensProvider.legend;
	notificar('initialized', {});
	// espera o índice em segundo plano terminar (a resolução entre arquivos vem dele)
	await new Promise((res) => {
		let ativo = false;
		const fim = setTimeout(res, 20 * 60 * 1000);
		aoNotificar.push((m) => {
			if (m.method !== '$/progress' || !/backgroundIndex/i.test(String(m.params.token))) return;
			if (m.params.value.kind === 'begin') ativo = true;
			if (m.params.value.kind === 'end') { clearTimeout(fim); res(); }
		});
		setTimeout(() => { if (!ativo) { clearTimeout(fim); res(); } }, 15000); // índice já pronto no cache
	});
	const tIndice = (Date.now() - t0) / 1000;
	const raizes = [];
	for (const e of opt.entradas) raizes.push(...(await entrada(e)));
	while (fila.length) await percorrer(fila.shift());

	const ehFuncao = (u) => [SK.Function, SK.Method, SK.Constructor].includes(u.tipo);
	const rotulo = (k) => { const u = unidades.get(k); return `${u.nome} (${rel(u.f)}:${u.ini.line + 1})`; };
	const arquivos = new Map();
	for (const [k, u] of unidades) { if (!arquivos.has(u.f)) arquivos.set(u.f, []); arquivos.get(u.f).push(k); }
	const nFun = [...unidades.values()].filter(ehFuncao).length;
	const out = [`# Funcionalidade (C++): ${opt.entradas.join(', ')}`, '',
		`${unidades.size} declarações (${nFun} funções e métodos) em ${arquivos.size} arquivos de native/src, fechadas sobre tudo o que usam${fora.length ? ' (excluídas: ' + opt.excluir.join(', ') + ')' : ''}. Índice ${tIndice.toFixed(0)} s, extração ${((Date.now() - t0) / 1000 - tIndice).toFixed(0)} s.`, '',
		'## Árvore de uso (funções e métodos)', '', '```'];
	const impressos = new Set();
	const arvore = (k, prof, caminho) => {
		const rep = impressos.has(k);
		out.push('  '.repeat(prof) + rotulo(k) + (rep ? '  (já listado acima)' : ''));
		if (rep) return;
		impressos.add(k);
		for (const f of [...(usa.get(k) || [])].filter((x) => ehFuncao(unidades.get(x)))) if (!caminho.has(f)) arvore(f, prof + 1, new Set([...caminho, f]));
	};
	for (const r of raizes) arvore(r, 0, new Set([r]));
	out.push('```', '', '## Código, por arquivo', '');
	for (const [f, ks] of [...arquivos.entries()].sort()) {
		const ls = linhas(f);
		const spans = ks.map((k) => unidades.get(k)).sort((a, b) => a.ini.line - b.ini.line || a.ini.character - b.ini.character);
		out.push(`### ${rel(f)}`, '', '```cpp');
		let ate = -1;
		for (const u of spans) {
			if (u.ini.line <= ate) continue; // aninhada numa já impressa
			// o comentário logo acima vai junto
			let a = u.ini.line;
			while (a > 0 && /^\s*\/\//.test(ls[a - 1]) && a - 1 > ate) a--;
			out.push(`// linha ${u.ini.line + 1}`, ...ls.slice(a, u.fim.line + 1), '');
			ate = u.fim.line;
		}
		out.push('```', '');
	}
	fs.mkdirSync(path.dirname(path.resolve(opt.saida)), { recursive: true });
	fs.writeFileSync(opt.saida, out.join('\n'), 'utf8');
	console.log(`ok: ${unidades.size} declarações (${nFun} funções), ${arquivos.size} arquivos; índice ${tIndice.toFixed(0)} s, extração ${((Date.now() - t0) / 1000 - tIndice).toFixed(0)} s -> ${opt.saida}`);
	await pedir('shutdown', null);
	notificar('exit', null);
	cd.kill();
})().catch((e) => { console.error(e); cd.kill(); process.exit(1); });
