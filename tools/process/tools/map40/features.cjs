// Inventário funcional do Roadcraft 4.0, decidido pelo grafo (records/map40/graph.json), sem lista escrita à mão.
// Sementes (pontos de entrada do jogador), achadas no próprio código:
//   - cada ferramenta do editor (src/editor/*.ts) e cada painel da interface (src/ui/**) é uma funcionalidade;
//   - cada worker (*.worker.ts), com quem o cria;
//   - cada subsistema da simulação, do mundo, das pessoas e da vista (pasta ou prefixo do nome do arquivo);
//   - no render, os arquivos agrupados pela pasta ou pelo prefixo do nome (road*, terrain*, citizen*...).
// A raiz do programa (main.ts, *Wiring.ts, frameLoop.ts), o núcleo (src/core), os jobs e os laboratórios não são
// funcionalidades: são a ligação e a infraestrutura, listadas à parte. Para cada semente: fecho alcançável pelas
// arestas do grafo; "próprio" é o que só ela alcança (dominância); o resto é compartilhado, com dono principal pela
// menor distância. A ligação de cada funcionalidade é o trecho da raiz que chama as unidades dela.
// Saída: records/map40/features.json, index.md, specifications/<id>.md e a infraestrutura em infrastructure.md.
// Uso: node tools/map40/features.cjs [--grafo <graph.json>] [--saida <pasta>] [--ports <pasta ports>]
'use strict';
const fs = require('fs');
const path = require('path');

const ARGS = process.argv.slice(2);
const opt = (nome, padrao) => { const i = ARGS.indexOf(nome); return i >= 0 ? ARGS[i + 1] : padrao; };
const RAIZ = path.resolve(__dirname, '..', '..');
const GRAFO = path.resolve(opt('--grafo', path.join(RAIZ, 'records', 'map40', 'graph.json')));
const SAIDA = path.resolve(opt('--saida', path.join(RAIZ, 'records', 'map40')));
const PORTS = path.resolve(opt('--ports', path.join(RAIZ, 'ports')));
const g = JSON.parse(fs.readFileSync(GRAFO, 'utf8'));

const unidade = new Map(g.unidades.map((u) => [u.chave, u]));
const porArquivo = new Map();
for (const u of g.unidades) { if (!porArquivo.has(u.arquivo)) porArquivo.set(u.arquivo, []); porArquivo.get(u.arquivo).push(u.chave); }
const sai = new Map();
for (const a of g.arestas) { if (!sai.has(a.de)) sai.set(a.de, []); sai.get(a.de).push(a); }
const LAB = /^src\/(lab|peopleLab)\//;
const RAIZ_PROGRAMA = /^src\/(main\.ts|\w+Wiring\.ts|frameLoop\.ts|globals\.d\.ts|desktop\w*\.ts|steamAchievements\.ts|fieldReport\.ts)$/;
const INFRA = /^src\/(core|jobs)\//;

// ---- sementes --------------------------------------------------------------------------------------------------
const base = (f) => path.basename(f, '.ts');
const prefixo = (nome) => { const m = /^([a-z]+)/.exec(nome); return m ? m[1] : nome; };
const sementes = new Map();
function semente(id, grupo, arquivo, entrada) {
  if (!sementes.has(id)) sementes.set(id, { id, grupo, arquivos: new Set(), entrada });
  sementes.get(id).arquivos.add(arquivo);
}
for (const f of g.arquivos) {
  if (LAB.test(f) || RAIZ_PROGRAMA.test(f) || INFRA.test(f)) continue;
  const nome = base(f);
  const partes = f.split('/');
  if (/\.worker$/.test(nome)) { semente(`worker:${nome.replace(/\.worker$/, '')}`, 'worker', f, 'worker'); continue; }
  if (partes[1] === 'editor') { semente(`editor:${partes.length > 3 ? partes[2] + '-' : ''}${nome}`, 'editor', f, 'ferramenta'); continue; }
  if (partes[1] === 'ui') { semente(`ui:${partes.length > 3 ? partes[2] : nome}`, 'interface', f, 'painel'); continue; }
  if (/^(sim|world|people|view|render)$/.test(partes[1])) {
    semente(partes.length > 3 ? `${partes[1]}:${partes[2]}` : `${partes[1]}:${prefixo(nome)}`, partes[1], f, partes[1] === 'render' ? 'desenho' : 'subsistema');
    continue;
  }
  semente(`src:${prefixo(nome)}`, 'raiz', f, 'ligação');
}
for (const c of g.canais) if (c.tipo === 'worker' && c.modulo && c.modulo.startsWith('src/')) {
  const id = `worker:${base(c.modulo).replace(/\.worker$/, '')}`;
  if (sementes.has(id) && !RAIZ_PROGRAMA.test(c.arquivo)) sementes.get(id).arquivos.add(c.arquivo);
}
console.error(`sementes: ${sementes.size}`);

// ---- fecho e dominância ----------------------------------------------------------------------------------------
function distancias(arquivos) {
  const d = new Map();
  const fila = [];
  for (const f of arquivos) for (const k of porArquivo.get(f) || []) { d.set(k, 0); fila.push(k); }
  for (let i = 0; i < fila.length; i++) {
    const k = fila[i], dk = d.get(k);
    for (const a of sai.get(k) || []) if (!d.has(a.para)) { d.set(a.para, dk + 1); fila.push(a.para); }
  }
  return d;
}
const dist = new Map();
for (const s of sementes.values()) dist.set(s.id, distancias(s.arquivos));
const quantos = new Map();
for (const d of dist.values()) for (const k of d.keys()) quantos.set(k, (quantos.get(k) || 0) + 1);
const donoDe = new Map(); // unidade -> semente mais próxima (dono principal)
for (const k of unidade.keys()) {
  let melhor = null, md = Infinity;
  for (const [id, d] of dist) { const v = d.get(k); if (v !== undefined && (v < md || (v === md && sementes.get(id).arquivos.has(unidade.get(k).arquivo)))) { md = v; melhor = id; } }
  if (melhor) donoDe.set(k, melhor);
}

// ---- canais, estado e testes -----------------------------------------------------------------------------------
const canaisDe = new Map();
for (const c of g.canais) { const k = c.de || `${c.arquivo}#*`; if (!canaisDe.has(k)) canaisDe.set(k, []); canaisDe.get(k).push(c); }
const estadoDe = new Map();
for (const e of g.estado) { if (!estadoDe.has(e.unidade)) estadoDe.set(e.unidade, []); estadoDe.get(e.unidade).push(e); }
const testesDe = new Map();
for (const t of g.testes) for (const f of t.importa) { if (!testesDe.has(f)) testesDe.set(f, []); testesDe.get(f).push(t); }
const entra = new Map(); // unidade -> [{de, tipo, n}]
for (const a of g.arestas) { if (!entra.has(a.para)) entra.set(a.para, []); entra.get(a.para).push(a); }

// ---- coberturas dos portes: estado no jogo por arquivo do 4.0 --------------------------------------------------
const ESTADOS = ['portado em parte', 'não portado (pendência)', 'de outra funcionalidade', 'não se aplica', 'já existia', 'não portado', 'a portar', 'outro porte', 'outra sessão', 'em parte', 'parcial', 'pendente', 'portado'];
const coberturaJogo = new Map();
if (fs.existsSync(PORTS)) {
  for (const porte of fs.readdirSync(PORTS)) {
    const cob = path.join(PORTS, porte, 'coverage.md');
    if (!fs.existsSync(cob)) continue;
    let arquivoAtual = null;
    for (const linha of fs.readFileSync(cob, 'utf8').split(/\r?\n/)) {
      const mh = /^##\s+`?(src\/[^\s`(]+)/.exec(linha);
      if (mh) arquivoAtual = mh[1];
      const celulas = linha.startsWith('|') ? linha.split('|').map((c) => c.trim()) : null;
      const texto = linha.toLowerCase();
      const marq = /`?(src\/[\w./-]+\.ts)`?/.exec(linha);
      const arquivo = marq ? marq[1] : arquivoAtual;
      if (!arquivo) continue;
      const estado = ESTADOS.find((e) => (celulas ? celulas.some((c) => c.toLowerCase().startsWith(e)) : texto.includes(`: ${e}`) || texto.includes(`→ ${e}`) || texto.includes(` ${e} `)));
      if (!estado) continue;
      if (!coberturaJogo.has(arquivo)) coberturaJogo.set(arquivo, {});
      const porPorte = coberturaJogo.get(arquivo);
      if (!porPorte[porte]) porPorte[porte] = {};
      porPorte[porte][estado] = (porPorte[porte][estado] || 0) + 1;
    }
  }
}

// ---- montagem ----------------------------------------------------------------------------------------------------
const lista = (xs, n = 30) => xs.length ? xs.slice(0, n).map((x) => `\`${x}\``).join(', ') + (xs.length > n ? ` … (+${xs.length - n})` : '') : '-';
const idArq = (id) => id.replace(/[^\w.-]+/g, '_');
const funcionalidades = [];
for (const s of sementes.values()) {
  const r = dist.get(s.id);
  const proprias = [...r.keys()].filter((k) => quantos.get(k) === 1 && !LAB.test(unidade.get(k).arquivo));
  const donas = [...r.keys()].filter((k) => quantos.get(k) > 1 && donoDe.get(k) === s.id);
  const compartilhadas = [...r.keys()].filter((k) => quantos.get(k) > 1 && donoDe.get(k) !== s.id);
  const arquivos = new Set(proprias.map((k) => unidade.get(k).arquivo));
  const linhas = proprias.reduce((n, k) => n + unidade.get(k).linhas, 0);
  const textos = new Set(), eventos = new Set(), mensagens = new Set(), recursos = new Set(), workers = new Set(), ganchos = new Set(), storage = new Set(), bibliotecas = new Set();
  const estado = { le: new Set(), escreve: new Set() };
  let timers = 0;
  for (const k of [...proprias, ...donas]) {
    for (const c of canaisDe.get(k) || []) {
      if (c.tipo === 'texto') textos.add(c.chave);
      else if (c.tipo === 'evento') eventos.add(`${c.modo}:${c.nome}`);
      else if (c.tipo === 'mensagem') mensagens.add(`${c.modo}:${c.discriminante}`);
      else if (c.tipo === 'recurso') recursos.add(c.url);
      else if (c.tipo === 'worker') workers.add(c.modulo);
      else if (c.tipo === 'ganchoThree') ganchos.add(c.nome);
      else if (c.tipo === 'localStorage') storage.add(`${c.modo}:${c.chave}`);
      else if (c.tipo === 'timer') timers++;
    }
    for (const e of estadoDe.get(k) || []) estado[e.modo === 'escreve' ? 'escreve' : 'le'].add(e.campo);
  }
  for (const f of [...s.arquivos, ...arquivos]) for (const c of canaisDe.get(`${f}#*`) || []) {
    if (c.tipo === 'recurso') recursos.add(c.url);
    else if (c.tipo === 'worker') workers.add(c.modulo);
    else if (c.tipo === 'biblioteca') bibliotecas.add(c.modulo);
  }
  const testes = new Set();
  for (const f of [...s.arquivos, ...arquivos]) for (const t of testesDe.get(f) || []) testes.add(t.arquivo);
  // interações: para quem as unidades próprias apontam (dono principal da unidade alvo), e quem aponta para elas
  const usa = new Map(), usadaPor = new Map(), ligacao = new Map();
  for (const k of proprias) {
    for (const a of sai.get(k) || []) { const dono = donoDe.get(a.para); if (dono && dono !== s.id) usa.set(dono, (usa.get(dono) || 0) + a.n); }
    for (const a of entra.get(k) || []) {
      const u = unidade.get(a.de);
      if (!u) continue;
      if (RAIZ_PROGRAMA.test(u.arquivo)) ligacao.set(a.de, (ligacao.get(a.de) || 0) + a.n);
      else { const dono = donoDe.get(a.de); if (dono && dono !== s.id) usadaPor.set(dono, (usadaPor.get(dono) || 0) + a.n); }
    }
  }
  const estadoJogo = {};
  for (const f of [...s.arquivos, ...arquivos]) for (const [porte, est] of Object.entries(coberturaJogo.get(f) || {})) { if (!estadoJogo[porte]) estadoJogo[porte] = {}; for (const [e, n] of Object.entries(est)) estadoJogo[porte][e] = (estadoJogo[porte][e] || 0) + n; }
  const ordenar = (m) => [...m].sort((a, b) => b[1] - a[1]).slice(0, 12).map(([id, n]) => ({ id, arestas: n }));
  funcionalidades.push({ id: s.id, grupo: s.grupo, entrada: s.entrada, sementes: [...s.arquivos].sort(), arquivos_proprios: [...arquivos].sort(), unidades_proprias: proprias.length, linhas_proprias: linhas,
    unidades_alcancadas: r.size, compartilhadas_que_usa: compartilhadas.length, compartilhadas_de_que_e_dona: donas.length,
    textos: [...textos].sort(), eventos: [...eventos].sort(), mensagens: [...mensagens].sort(), recursos: [...recursos].sort(), workers: [...workers].sort(), ganchos_three: [...ganchos].sort(), local_storage: [...storage].sort(), bibliotecas: [...bibliotecas].sort(), timers,
    estado_le: [...estado.le].sort(), estado_escreve: [...estado.escreve].sort(), testes: [...testes].sort(),
    usa: ordenar(usa), usada_por: ordenar(usadaPor), ligacao: ordenar(ligacao).map((l) => ({ unidade: l.id, arestas: l.arestas })), estado_no_jogo: estadoJogo,
    proprias: proprias.map((k) => { const u = unidade.get(k); return { nome: u.nome, tipo: u.tipo, arquivo: u.arquivo, ini: u.ini, fim: u.fim, doc: u.doc }; }).sort((a, b) => a.arquivo.localeCompare(b.arquivo) || a.ini - b.ini) });
}
funcionalidades.sort((a, b) => a.grupo.localeCompare(b.grupo) || b.linhas_proprias - a.linhas_proprias);

// ---- infraestrutura e ligação ---------------------------------------------------------------------------------
const todas = [...unidade.keys()].filter((k) => !LAB.test(unidade.get(k).arquivo));
const alcancadas = new Set(todas.filter((k) => quantos.has(k)));
const semDono = todas.filter((k) => !quantos.has(k));
const infraPorArquivo = new Map();
for (const k of semDono) { const u = unidade.get(k); if (!infraPorArquivo.has(u.arquivo)) infraPorArquivo.set(u.arquivo, []); infraPorArquivo.get(u.arquivo).push(u); }
const compartilhadasPorArquivo = new Map(); // arquivos cujas unidades são alcançadas por 2+ sementes
for (const k of todas) if ((quantos.get(k) || 0) > 1) { const u = unidade.get(k); if (!compartilhadasPorArquivo.has(u.arquivo)) compartilhadasPorArquivo.set(u.arquivo, new Set()); for (const [id, d] of dist) if (d.has(k)) compartilhadasPorArquivo.get(u.arquivo).add(id); }

// ---- saída --------------------------------------------------------------------------------------------------------
fs.mkdirSync(path.join(SAIDA, 'specifications'), { recursive: true });
fs.writeFileSync(path.join(SAIDA, 'features.json'), JSON.stringify({ gerado: new Date().toISOString(), grafo: { gerado: g.gerado, commit: g.commit, unidades: g.unidades.length, arestas: g.arestas.length },
  funcionalidades: funcionalidades.map((f) => ({ ...f, proprias: undefined })), infraestrutura: [...infraPorArquivo.keys()].sort() }, null, 1));
for (const f of funcionalidades) {
  const linhas = [`# ${f.id}`, '', `Grupo: ${f.grupo}. Entrada: ${f.entrada}. Gerado de \`records/map40/graph.json\` (commit do 4.0 ${g.commit ? g.commit.slice(0, 12) : '?'}) por \`tools/map40/features.cjs\`: cada declaração própria abaixo tem arquivo e linhas no 4.0.`, '',
    `- Sementes: ${lista(f.sementes)}`, `- Unidades próprias (só esta funcionalidade alcança): ${f.unidades_proprias} em ${f.arquivos_proprios.length} arquivo(s), ${f.linhas_proprias} linhas`,
    `- Alcance total: ${f.unidades_alcancadas} unidades; usa ${f.compartilhadas_que_usa} compartilhadas de outras; é dona principal de ${f.compartilhadas_de_que_e_dona} compartilhadas`, '',
    '## O que o jogador vê e faz', `- Textos da interface (\`t()\`): ${lista(f.textos, 60)}`, `- Eventos de entrada: ${lista(f.eventos)}`, `- Chaves de localStorage: ${lista(f.local_storage)}`, '',
    '## Onde o programa a liga (raiz: main.ts e wiring)', `- ${f.ligacao.length ? f.ligacao.map((l) => `\`${l.unidade}\` (${l.arestas})`).join(', ') : 'nenhuma chamada direta da raiz: chega por outra funcionalidade'}`, '',
    '## Entradas, saídas e interações', `- Mensagens de worker (envia/recebe): ${lista(f.mensagens, 40)}`, `- Workers: ${lista(f.workers)}`, `- Ganchos do three.js: ${lista(f.ganchos_three)}`, `- Timers: ${f.timers}`,
    `- Usa: ${f.usa.length ? f.usa.map((i) => `\`${i.id}\` (${i.arestas})`).join(', ') : '-'}`, `- É usada por: ${f.usada_por.length ? f.usada_por.map((i) => `\`${i.id}\` (${i.arestas})`).join(', ') : '-'}`, '',
    '## Estado (campos do documento/estado da simulação)', `- Lê: ${lista(f.estado_le, 60)}`, `- Escreve: ${lista(f.estado_escreve, 60)}`, '',
    '## Dados e recursos', `- Recursos (JSON, modelos, texturas, fetch): ${lista(f.recursos, 40)}`, `- Bibliotecas externas: ${lista(f.bibliotecas)}`, '',
    '## Testes do 4.0 que a exercitam', `- ${f.testes.length ? f.testes.map((t) => `\`${t}\``).join(', ') : 'nenhum spec importa os arquivos desta funcionalidade'}`, '',
    '## Estado no jogo (lido de ports/*/coverage.md)', ...(Object.keys(f.estado_no_jogo).length ? Object.entries(f.estado_no_jogo).map(([p, e]) => `- porte \`${p}\`: ${Object.entries(e).map(([k, n]) => `${k} ${n}`).join(', ')}`) : ['- nenhuma cobertura de porte cita os arquivos desta funcionalidade']), '',
    '## Código próprio (linhas — declaração — comentário)'];
  let ultimo = null;
  for (const u of f.proprias) {
    if (u.arquivo !== ultimo) { linhas.push('', `### ${u.arquivo}`); ultimo = u.arquivo; }
    linhas.push(`- ${u.ini}-${u.fim} \`${u.nome}\` (${u.tipo})${u.doc ? ': ' + u.doc.replace(/\s+/g, ' ') : ''}`);
  }
  fs.writeFileSync(path.join(SAIDA, 'specifications', `${idArq(f.id)}.md`), linhas.join('\n') + '\n');
}
const totalLinhas = funcionalidades.reduce((n, f) => n + f.linhas_proprias, 0);
const indice = ['# Inventário funcional do Roadcraft 4.0', '', `Gerado em ${new Date().toISOString()} por \`tools/map40/features.cjs\` sobre \`graph.json\` (${g.unidades.length} unidades, ${g.arestas.length} arestas, ${g.canais.length} canais dinâmicos, ${g.testes.length} specs; commit do 4.0 ${g.commit ? g.commit.slice(0, 12) : '?'}). Sementes achadas no código: ${sementes.size}. Unidades fora dos laboratórios: ${todas.length}; alcançadas por alguma funcionalidade: ${alcancadas.size} (${(alcancadas.size / todas.length * 100).toFixed(1)} %); só da raiz e da infraestrutura: ${semDono.length} em ${infraPorArquivo.size} arquivo(s) (\`infrastructure.md\`). Linhas próprias somadas: ${totalLinhas}.`, '',
  '| Funcionalidade | Grupo | Próprias | Linhas | Alcance | Textos | Msgs | Workers | Testes | Estado no jogo |', '|---|---|---|---|---|---|---|---|---|---|'];
for (const f of funcionalidades) {
  const ej = Object.entries(f.estado_no_jogo).map(([p, e]) => `${p}: ${Object.entries(e).map(([k, n]) => `${k} ${n}`).join(', ')}`).join('; ') || '-';
  indice.push(`| [${f.id}](specifications/${idArq(f.id)}.md) | ${f.grupo} | ${f.unidades_proprias} | ${f.linhas_proprias} | ${f.unidades_alcancadas} | ${f.textos.length} | ${f.mensagens.length} | ${f.workers.length} | ${f.testes.length} | ${ej.slice(0, 160)} |`);
}
fs.writeFileSync(path.join(SAIDA, 'index.md'), indice.join('\n') + '\n');
const infra = ['# Raiz e infraestrutura do 4.0', '', 'Unidades que nenhuma funcionalidade do jogador alcança: a raiz do programa (ligação), o núcleo e os jobs. Cada uma com o que ela faz, para quem porta a arquitetura e não a funcionalidade.', ''];
for (const [arq, us] of [...infraPorArquivo].sort()) {
  infra.push(`## ${arq}`, '');
  for (const u of us.sort((a, b) => a.ini - b.ini)) infra.push(`- ${u.ini}-${u.fim} \`${u.nome}\` (${u.tipo})${u.doc ? ': ' + u.doc.replace(/\s+/g, ' ') : ''}`);
  infra.push('');
}
infra.push('## Arquivos compartilhados por mais de uma funcionalidade', '');
for (const [arq, ids] of [...compartilhadasPorArquivo].sort((a, b) => b[1].size - a[1].size)) infra.push(`- \`${arq}\`: ${ids.size} funcionalidades (${[...ids].slice(0, 8).join(', ')}${ids.size > 8 ? ', …' : ''})`);
fs.writeFileSync(path.join(SAIDA, 'infrastructure.md'), infra.join('\n') + '\n');
console.error(`funcionalidades: ${funcionalidades.length}; alcançadas ${alcancadas.size}/${todas.length}; infraestrutura ${semDono.length} unidades em ${infraPorArquivo.size} arquivos -> ${SAIDA}`);
