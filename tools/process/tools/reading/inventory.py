"""Inventário de leitura: prova auditável do que foi lido, gerada pelo programa, não declarada pelo modelo.

Uso:
  inventory.py hook                      hook PostToolUse (Read, Bash, PowerShell, WebFetch): registra o que a chamada leu
  inventory.py importar <transcricao.jsonl>   registra toda leitura de uma transcrição de sessão (prova retroativa)
  inventory.py registrar <arquivo> <a>-<b> [--nota ...]     anotação manual de linhas lidas
  inventory.py url <url> <a>-<b> --total <n> [--nota ...]   anotação manual de caracteres lidos
  inventory.py concluir <fonte> <bloco> "<conclusão>" ["<decisão>"]   conclusão técnica ligada a uma decisão
  inventory.py cobertura [--md <saida.md>]   relatório: blocos totais e lidos, cobertura, desatualizados, obrigatórios
  inventory.py verificar                 sai com 1 se há fonte obrigatória incompleta, desatualizada ou ausente
  inventory.py cruzar                    arquivos citados nos documentos de regra que não estão no inventário
  inventory.py contexto                  hook SessionStart: estado persistido e cobertura, como additionalContext
  inventory.py checkpoint                hook Stop/PreCompact: grava o estado do turno (git, sessão, último texto)
O inventário vive em records/reading/ e é commitado: a prova sobrevive a sessões e compactações.
"""
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import leitura_lib as L  # noqa: E402


def _entrada_hook():
    try:
        return json.load(sys.stdin)
    except ValueError:
        return {}


def _dentro_do_projeto(cwd):
    return L.no_projeto(cwd or os.getcwd())


def cmd_hook():
    dados = _entrada_hook()
    if not dados or not _dentro_do_projeto(dados.get("cwd")):
        return 0
    nome = dados.get("tool_name", "")
    leituras = L.leituras_da_ferramenta(nome, dados.get("tool_input"), dados.get("tool_response"), dados.get("cwd"))
    if not leituras:
        return 0
    inv = L.inventario()
    sessao = dados.get("session_id", "")
    comando = (dados.get("tool_input") or {}).get("command", "")
    for item in leituras:
        if item[0] == "arquivo":
            L.registrar_arquivo(inv, item[1], item[2], item[3], f"hook:{nome}", sessao=sessao, comando=comando)
        else:
            L.registrar_url(inv, item[1], item[2], item[3], item[4], f"hook:{nome}", sessao=sessao)
    L.gravar(L.INVENTARIO, inv)
    return 0


def cmd_importar(caminho):
    """Lê uma transcrição (.jsonl) e registra cada Read, Bash/PowerShell e WebFetch com o seu resultado."""
    inv = L.inventario()
    usos = {}
    n = 0
    with open(caminho, encoding="utf8", errors="replace") as f:
        for linha in f:
            try:
                it = json.loads(linha)
            except ValueError:
                continue
            msg = it.get("message") or {}
            conteudo = msg.get("content")
            if not isinstance(conteudo, list):
                continue
            if it.get("type") == "assistant":
                for b in conteudo:
                    if isinstance(b, dict) and b.get("type") == "tool_use":
                        usos[b.get("id")] = (b.get("name", ""), b.get("input") or {}, it.get("cwd") or it.get("sessionId"))
            elif it.get("type") == "user":
                for b in conteudo:
                    if not (isinstance(b, dict) and b.get("type") == "tool_result"):
                        continue
                    uso = usos.pop(b.get("tool_use_id"), None)
                    if not uso or b.get("is_error"):
                        continue
                    nome, entrada, _ = uso
                    resposta = b.get("content")
                    if isinstance(resposta, list):
                        resposta = " ".join(x.get("text", "") for x in resposta if isinstance(x, dict))
                    cwd = it.get("cwd") or L.RAIZ
                    for item in L.leituras_da_ferramenta(nome, entrada, resposta, cwd):
                        sessao = it.get("sessionId", "")
                        if item[0] == "arquivo":
                            if L.registrar_arquivo(inv, item[1], item[2], item[3], f"transcricao:{nome}", sessao=sessao,
                                                   comando=entrada.get("command", "")):
                                n += 1
                        else:
                            L.registrar_url(inv, item[1], item[2], item[3], item[4], f"transcricao:{nome}", sessao=sessao)
                            n += 1
    L.gravar(L.INVENTARIO, inv)
    print(f"importadas {n} leituras de {os.path.basename(caminho)}")
    return 0


def _faixa(texto):
    m = re.match(r"^(\d+)-(\d+)$", texto)
    if not m:
        print("faixa no formato a-b", file=sys.stderr)
        sys.exit(2)
    return int(m.group(1)), int(m.group(2))


def cmd_registrar(args):
    inv = L.inventario()
    a, b = _faixa(args[1])
    nota = args[args.index("--nota") + 1] if "--nota" in args else ""
    chave = L.registrar_arquivo(inv, args[0], a, b, "manual", nota=nota)
    L.gravar(L.INVENTARIO, inv)
    print("registrado" if chave else "arquivo inexistente ou não é texto")
    return 0 if chave else 1


def cmd_url(args):
    inv = L.inventario()
    a, b = _faixa(args[1])
    total = int(args[args.index("--total") + 1]) if "--total" in args else None
    nota = args[args.index("--nota") + 1] if "--nota" in args else ""
    L.registrar_url(inv, args[0], a, b, total, "manual", nota=nota)
    L.gravar(L.INVENTARIO, inv)
    print("registrado")
    return 0


def cmd_concluir(args):
    inv = L.inventario()
    chave = args[0] if args[0].startswith("http") else L.normalizar(args[0])
    f = inv["sources"].get(chave)
    if f is None:
        print("fonte não está no inventário; leia-a antes", file=sys.stderr)
        return 1
    f["conclusoes"].append({"bloco": args[1], "quando": L.agora(), "conclusao": args[2],
                            "decisao": args[3] if len(args) > 3 else ""})
    L.gravar(L.INVENTARIO, inv)
    print("conclusão registrada")
    return 0


def _linhas_relatorio(inv):
    obrig = dict(L.expandir_obrigatorios(L.obrigatorios()))
    linhas, pendencias = [], []
    chaves = sorted(set(inv["sources"]) | set(obrig), key=lambda k: (k not in obrig, L.relativo(k)))
    for chave in chaves:
        f = inv["sources"].get(chave)
        if f is None:
            tipo = "url" if chave.startswith("http") else "arquivo"
            cob = {"total": 0, "blocos": 0, "lidos": [], "cobertura": 0.0, "desatualizado": False, "ausente": False,
                   "unidade": "chars" if tipo == "url" else "linhas", "faltam": []}
            if tipo == "arquivo" and os.path.isfile(chave):
                cob["total"], _ = L.contar_linhas(chave)
                cob["blocos"] = L.blocos_de(cob["total"], inv.get("bloco_linhas", L.BLOCO_LINHAS))
                cob["faltam"] = list(range(1, cob["blocos"] + 1))
            conclusoes = 0
        else:
            cob = L.cobertura_fonte(inv, chave, f)
            conclusoes = len(f.get("conclusoes", []))
        estado = []
        if chave in obrig:
            estado.append("obrigatória")
            if cob["cobertura"] < 0.999 or cob["desatualizado"] or cob["ausente"]:
                pendencias.append((chave, cob))
        if cob["desatualizado"]:
            estado.append("DESATUALIZADA")
        if cob["ausente"]:
            estado.append("AUSENTE")
        linhas.append({"fonte": chave, "nome": L.relativo(chave) if not chave.startswith("http") else chave, "cob": cob,
                       "estado": estado, "conclusoes": conclusoes})
    return linhas, pendencias


def cmd_cobertura(args):
    inv = L.inventario()
    linhas, pendencias = _linhas_relatorio(inv)
    saida = ["# Cobertura de leitura", "",
             f"Gerado por `tools/reading/inventory.py cobertura` em {L.agora()}. Bloco = {inv.get('bloco_linhas', L.BLOCO_LINHAS)} "
             f"linhas (arquivos) ou {inv.get('bloco_chars', L.BLOCO_CHARS)} caracteres (páginas). Um bloco só conta como lido quando a união "
             "dos intervalos registrados pelo hook, pela importação da transcrição ou por anotação manual o cobre inteiro.", "",
             f"Fontes: {len(linhas)}. Obrigatórias pendentes: {len(pendencias)}.", "",
             "| Fonte | Total | Blocos | Lidos | Cobertura | Faltam | Conclusões | Estado |", "|---|---|---|---|---|---|---|---|"]
    for l in linhas:
        c = l["cob"]
        faltam = ", ".join(str(k) for k in c["faltam"][:12]) + (" …" if len(c["faltam"]) > 12 else "")
        saida.append(f"| `{l['nome']}` | {c['total']} {c['unidade']} | {c['blocos']} | {len(c['lidos'])} | {c['cobertura']*100:.1f} % | "
                     f"{faltam or '-'} | {l['conclusoes']} | {' '.join(l['estado']) or '-'} |")
    if pendencias:
        saida += ["", "## Obrigatórias pendentes", ""]
        for chave, c in pendencias:
            saida.append(f"- `{L.relativo(chave)}`: {c['cobertura']*100:.1f} % lido, faltam blocos {c['faltam'][:20]}"
                         + (" (desatualizada: o arquivo mudou depois da leitura)" if c["desatualizado"] else "")
                         + (" (ausente)" if c["ausente"] else ""))
    saida += ["", "## Conclusões por bloco", ""]
    for chave, f in sorted(inv["sources"].items(), key=lambda kv: L.relativo(kv[0])):
        for c in f.get("conclusoes", []):
            saida.append(f"- `{L.relativo(chave)}` bloco {c['bloco']}: {c['conclusao']}" + (f" → decisão: {c['decisao']}" if c.get("decisao") else ""))
    texto = "\n".join(saida) + "\n"
    destino = args[args.index("--md") + 1] if "--md" in args else os.path.join(L.PASTA, "coverage.md")
    os.makedirs(os.path.dirname(destino), exist_ok=True)
    with open(destino, "w", encoding="utf8") as f:
        f.write(texto)
    print(f"{len(linhas)} fontes, {len(pendencias)} obrigatórias pendentes -> {L.relativo(destino)}")
    return 0


def cmd_verificar():
    inv = L.inventario()
    _, pendencias = _linhas_relatorio(inv)
    for chave, c in pendencias:
        nome = chave if chave.startswith("http") else L.relativo(chave)
        print(f"PENDENTE {nome}: {c['cobertura']*100:.1f} %, faltam {c['faltam'][:10]}"
              + (" DESATUALIZADA" if c["desatualizado"] else "") + (" AUSENTE" if c["ausente"] else ""))
    print("leitura obrigatória completa" if not pendencias else f"{len(pendencias)} fonte(s) obrigatória(s) pendente(s)")
    return 0 if not pendencias else 1


CITACAO = re.compile(r"`([\w./\\-]+\.(?:md|txt|py|ts|cjs|mjs|json|sh|ps1|gd|cpp|h))`")


def cmd_cruzar(args):
    """Arquivos que os documentos de regra citam e que existem, mas não têm leitura registrada. Com `--tudo`
    entra também o mapa de `fonte/CLAUDE.md`, que cita todo arquivo do jogo."""
    inv = L.inventario()
    nomes = ["CLAUDE.md", "session-rules.md", "records/coordination.md", "records/coordinator-prompt.md",
             "records/hooks-and-locks.md", "converter/README.md"] + (["fonte/CLAUDE.md"] if "--tudo" in args else [])
    docs = [os.path.join(L.RAIZ, p) for p in nomes]
    faltam = set()
    for doc in docs:
        if not os.path.isfile(doc):
            continue
        texto = open(doc, encoding="utf8", errors="replace").read()
        for cit in CITACAO.findall(texto):
            for base in (L.RAIZ, os.path.join(L.RAIZ, "fonte"), os.path.join(L.RAIZ, "converter")):
                c = L.normalizar(os.path.join(base, cit))
                if os.path.isfile(c) and c not in inv["sources"]:
                    faltam.add(c)
                    break
    for c in sorted(faltam):
        print(L.relativo(c))
    print(f"{len(faltam)} arquivo(s) citado(s) sem leitura registrada")
    return 0


def cmd_contexto():
    dados = _entrada_hook()
    if dados and not _dentro_do_projeto(dados.get("cwd")):
        return 0
    inv = L.inventario()
    _, pendencias = _linhas_relatorio(inv)
    est = L.estado()
    partes = ["[Roadcraft: continuidade entre sessões, gerada por tools/reading/inventory.py contexto]"]
    if est:
        partes.append(f"Último checkpoint {est.get('quando')} (sessão {est.get('sessao', '?')[:8]}, cwd {est.get('cwd')}, "
                      f"motivo {est.get('motivo')}): git {est.get('git', {}).get('branch')} {est.get('git', {}).get('head')}, "
                      f"{est.get('git', {}).get('sujos', 0)} arquivo(s) sem commit.")
        if est.get("ultimo_texto"):
            partes.append("Último texto da sessão anterior: " + est["ultimo_texto"][:700])
    partes.append(f"Inventário de leitura: {len(inv['sources'])} fontes registradas; {len(pendencias)} obrigatória(s) pendente(s)"
                  + (": " + ", ".join(L.relativo(k) for k, _ in pendencias[:8]) if pendencias else "")
                  + ". Relatório: records/reading/coverage.md. Toda leitura por Read, cat, sed -n, head ou WebFetch é registrada pelo hook; "
                    "`inventory.py verificar` diz o que falta.")
    print(json.dumps({"hookSpecificOutput": {"hookEventName": "SessionStart", "additionalContext": "\n".join(partes)}},
                     ensure_ascii=False))
    return 0


def cmd_checkpoint():
    dados = _entrada_hook()
    if not dados or not _dentro_do_projeto(dados.get("cwd")):
        return 0
    cwd = dados.get("cwd") or L.RAIZ
    est = {"quando": L.agora(), "sessao": dados.get("session_id", ""), "cwd": cwd, "motivo": dados.get("hook_event_name"),
           "git": L.resumo_git(cwd), "ultimo_texto": (dados.get("last_assistant_message") or "")[:1500],
           "tarefas_em_fundo": [t.get("description", "")[:120] for t in dados.get("background_tasks") or []]}
    L.gravar(L.ESTADO, est)
    historico = os.path.join(L.PASTA, "checkpoints.jsonl")
    with open(historico, "a", encoding="utf8") as f:
        f.write(json.dumps({k: est[k] for k in ("quando", "sessao", "cwd", "motivo", "git")}, ensure_ascii=False) + "\n")
    return 0


def main(argv):
    if not argv:
        print(__doc__)
        return 2
    cmd, args = argv[0], argv[1:]
    if cmd == "hook":
        return cmd_hook()
    if cmd == "importar":
        return cmd_importar(args[0])
    if cmd == "registrar":
        return cmd_registrar(args)
    if cmd == "url":
        return cmd_url(args)
    if cmd == "concluir":
        return cmd_concluir(args)
    if cmd == "cobertura":
        return cmd_cobertura(args)
    if cmd == "verificar":
        return cmd_verificar()
    if cmd == "cruzar":
        return cmd_cruzar(args)
    if cmd == "contexto":
        return cmd_contexto()
    if cmd == "checkpoint":
        return cmd_checkpoint()
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.exit(main(sys.argv[1:]))
