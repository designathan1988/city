"""Portão de processo: bloqueia ANTES a tentativa e erro, a escolha barata e a escrita sem leitura, nos 5 passos pedidos pelo usuário
(compreender, pesquisar, estudar, codificar, testar) e PARA ao fim do teste.

Uso pelo modelo, pelo Bash: `portao passo <1-5> <registro.json>`, `portao-ver modelo <1-5>`, `portao-ver estado` (~/bin). O nome próprio
existe porque o `if` de um hook só escapa de disparar em todo comando com `$` quando cita apenas o nome do comando.
Uso pelos hooks: process.py hook (lê o JSON do evento em stdin). Ligado em ~/.claude/settings.json. Nenhuma recusa daqui é beco sem saída:
o passo 1 recomeça o ciclo de qualquer fase, e o próprio portão muda como código, na fase 4, no plano julgado. Aqui só entra o que um
programa decide de verdade: ordem dos passos, plano, leitura, execução única, disco. Se um turno parou sem motivo é questão de significado,
e não se decide com lista de frases: quem julga é o hook por agente no Stop, em settings.json, que pode errar.
"""
import json
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import comandos as C  # noqa: E402
import estado as E  # noqa: E402
import passos as P  # noqa: E402
import simbolos as S  # noqa: E402

L = E.L
FORMA = "gate"
VER = "gate-check"
PASSO = re.compile(r"^" + re.escape(FORMA) + r" passo ([1-5]) (\S+)\s*$")
CONSULTA = re.compile(r"^" + re.escape(FORMA) + r"-ver (?:modelo [1-5]|estado)\s*$")
EDICAO = ("Edit", "Write", "MultiEdit", "NotebookEdit")
SHELL = ("Bash", "PowerShell")
MAX_COMPILA = 3  # compilar de novo só depois de editar; passou disso, o erro não é de digitação: o plano está errado
TURNO_AUTOMATICO = re.compile(r"^\s*<(?!pasted_content)")
NOME_FASE = {1: "compreender", 2: "pesquisar", 3: "estudar", 4: "codificar", 5: "testar", 6: "parado"}


def no_escopo(dados):
    inp = dados.get("tool_input") or {}
    alvos = [dados.get("cwd") or "", inp.get("file_path") or ""]
    raiz = L.RAIZ.replace("\\", "/").lower()
    return any(a and L.normalizar(a).lower().startswith(raiz) for a in alvos) or C.ABSOLUTOS.search(" ".join(map(str, inp.values()))) is not None


def negar(motivo):
    print(json.dumps({"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny",
                                             "permissionDecisionReason": "Portão de processo: " + motivo}}, ensure_ascii=False))
    return 0


def fase_txt(est):
    return f"fase {est['fase']} ({NOME_FASE[est['fase']]}), ciclo {est['ciclo']}"


def faltou_passo(est, o_que):
    return (f"{o_que} não é permitido na {fase_txt(est)}. Registre o passo {est['fase']}: `{FORMA} passo {est['fase']} <registro.json>` "
            f"(formato: `{VER} modelo {est['fase']}`).")


def pre_edicao(est, dados, inp):
    caminho = inp.get("file_path") or inp.get("notebook_path") or ""
    alvo = L.normalizar(caminho)
    if "records/process" in alvo.lower():
        return "o estado do portão é escrito só pelos hooks."
    if not C.protegido(caminho) and not C.eh_codigo(caminho):
        return None
    if est["fase"] != 4:
        return faltou_passo(est, "Alterar código")
    if alvo not in est["plano"]:
        return f"{L.relativo(alvo)} não está no plano do passo 3. Um plano errado se refaz: `{FORMA} passo 1 <registro.json>` recomeça o ciclo de qualquer fase."
    antigo = S.texto_antigo(alvo)
    novo = inp.get("content") if "content" in inp else inp.get("new_string", "")
    faltam = S.faltam(est, alvo, novo or "", antigo)
    if faltam:
        lista = "; ".join(f"{n} ({', '.join(d)})" for n, d in list(faltam.items())[:8])
        return f"o código novo chama o que você não leu inteiro: {lista}. Leia um declarador de cada antes de escrever."
    return None


def pre_shell(est, dados, inp):
    comando, cwd = inp.get("command", ""), dados.get("cwd")
    if PASSO.match(comando.strip()):
        m = PASSO.match(comando.strip())
        erros = P.conferir(est, int(m.group(1)), m.group(2), dados)
        return ("passo recusado: " + " | ".join(erros[:12])) if erros else None
    if CONSULTA.match(comando.strip()):
        return None
    viola = C.viola_protecao(comando)
    if viola:
        return f"este comando escreve no portão (`{viola}`). O portão muda pelo Edit, na fase 4, com ele no plano. Ler é livre."
    for alvo in C.escritas(comando, cwd):
        if C.eh_codigo(alvo) and (est["fase"] != 4 or alvo not in est["plano"]):
            return faltou_passo(est, f"Escrever {L.relativo(alvo)} por shell") if est["fase"] != 4 else f"{L.relativo(alvo)} não está no plano."
    tipo = C.classe(comando)
    chave = C.chave(comando, cwd)
    if tipo == "jogo":
        if est["fase"] not in (1, 5):
            return faltou_passo(est, "Rodar o jogo ou os testes")
        if est["execucoes"].get(chave):
            return ("esta execução já rodou neste ciclo. Rodar de novo é tentativa e erro. Se falhou, registre o resultado no passo 5 e pare; "
                    "se está medindo, mude o que mede." if est["fase"] == 5 else "esta medição já rodou neste ciclo: repetir não traz informação nova.")
    if tipo == "compila":
        if est["fase"] not in (1, 4, 5):
            return faltou_passo(est, "Compilar ou rodar os gates")
        c = est["compilacoes"].get(chave)
        if c and c["edicoes"] == est["edicoes"]:
            return "nada mudou desde a última execução deste comando: repetir não traz informação nova."
        if c and c["n"] >= MAX_COMPILA:
            return f"{MAX_COMPILA} execuções deste comando neste ciclo: o plano está errado. Recomece: `{FORMA} passo 1 <registro.json>`."
    return None


def pre(dados):
    sessao, tool, inp = dados.get("session_id", "sem"), dados.get("tool_name", ""), dados.get("tool_input") or {}
    with E.Trava(sessao):
        est = E.carregar(sessao)
        if not est.get("base_pronta"):  # o que já estava sujo quando a sessão chegou não é obra dela
            est["linha_base"], est["base_pronta"] = C.varrer(est, dados.get("cwd")), True
            est["repos_vistos"] = C.repos_de(est, dados.get("cwd"))
            E.gravar(est)
    if est["fase"] == E.PARADO:
        return negar("sessão parada depois do passo 5, para avaliação. Só uma mensagem do usuário abre o próximo ciclo.")
    motivo = pre_edicao(est, dados, inp) if tool in EDICAO else pre_shell(est, dados, inp) if tool in SHELL else None
    if motivo:
        with E.Trava(sessao):
            E.historico(est, "negado", ferramenta=tool, motivo=motivo[:300])
        return negar(motivo)
    return 0


def textos_de(valor):
    """Todo texto de uma resposta de ferramenta, sem o escape do JSON: é contra ele que as citações se conferem."""
    if isinstance(valor, dict):
        return " ".join(textos_de(v) for v in valor.values())
    if isinstance(valor, list):
        return " ".join(textos_de(v) for v in valor)
    return valor if isinstance(valor, str) else ""


def pos_leituras(est, dados, tool, inp, resposta):
    texto = textos_de(resposta)
    agora = time.time()
    if tool == "Read":
        c = L.normalizar(inp.get("file_path", ""))
        if c.lower().endswith(P.IMAGEM):
            est["imagens"][c] = agora
    for item in L.leituras_da_ferramenta(tool, inp, resposta, dados.get("cwd")):
        if item[0] == "arquivo":
            E.registrar_leitura(est, item[1], item[2], item[3])
    if tool == "WebFetch":
        est["web"].append({"url": inp.get("url", ""), "quando": agora})
        E.guardar_texto(est["sessao"], texto)
    if tool in SHELL:
        urls = C.urls_baixadas(inp.get("command", ""))
        if urls:
            est["web"] += [{"url": u, "quando": agora} for u in urls]
            E.guardar_texto(est["sessao"], texto)


def pos_shell(est, dados, inp):
    comando, cwd = inp.get("command", ""), dados.get("cwd")
    tipo, chave = C.classe(comando), C.chave(comando, cwd)
    if tipo == "jogo":
        est["execucoes"][chave] = 1
        est["rodou_no_5"] = est.get("rodou_no_5") or est["fase"] == 5
    if tipo == "compila":
        c = est["compilacoes"].setdefault(chave, {"n": 0, "edicoes": -1})
        c["n"] += 1
        c["edicoes"] = est["edicoes"]
        est["rodou_no_5"] = est.get("rodou_no_5") or est["fase"] == 5


def conferir_disco(est, dados):
    """A prova que não depende do texto do comando: o que mudou no disco, com a fase e o plano. Fora deles o ciclo é invalidado."""
    atual = C.varrer(est, dados.get("cwd"))
    novos = C.novidades(est, atual)
    est["linha_base"] = atual
    repos = C.repos_de(est, dados.get("cwd"))
    estreantes = [r.lower() + "/" for r in repos if r not in est.get("repos_vistos", [])]
    est["repos_vistos"] = sorted(set(est.get("repos_vistos", [])) | set(repos))
    # O que já estava sujo num repositório que entra agora na varredura não é obra de agora; do plano, conta como alterado.
    novos = [c for c in novos if c in est["plano"] or not any(c.lower().startswith(r) for r in estreantes)]
    fora = [c for c in novos if est["fase"] != 4 or c not in est["plano"]]
    est["alterados"] = sorted(set(est["alterados"]) | set(novos))
    est["edicoes"] += len(novos)
    if not fora:
        return None
    nomes = ", ".join(L.relativo(c) for c in fora[:6])
    E.historico(est, "invalidado", arquivos=nomes)
    E.novo_ciclo(est, penalidade=True)
    return (f"Portão de processo: ciclo invalidado. Código mudou fora da fase ou do plano: {nomes}. O trabalho volta ao passo 1 e as "
            f"leituras deste ciclo foram descartadas (penalidades: {est['penalidades']}).")


def pos(dados):
    sessao, tool, inp = dados.get("session_id", "sem"), dados.get("tool_name", ""), dados.get("tool_input") or {}
    parar = None
    with E.Trava(sessao):
        est = E.carregar(sessao)
        pos_leituras(est, dados, tool, inp, dados.get("tool_response"))
        if tool in EDICAO and L.normalizar(inp.get("file_path", "")) in est["plano"]:
            est["alterados"] = sorted(set(est["alterados"]) | {L.normalizar(inp["file_path"])})
        m = PASSO.match(inp.get("command", "").strip()) if tool == "Bash" else None
        if m:
            n = int(m.group(1))
            r = P.aplicar(est, n, m.group(2), dados)
            E.historico(est, "passo", passo=n)
            if n == 5:
                parar = f"PARE: ciclo {est['ciclo']} concluído ({r['resultado']}).\n\n{r['relatorio']}"
        elif tool in SHELL:
            pos_shell(est, dados, inp)
        aviso = conferir_disco(est, dados) if tool in EDICAO + SHELL else None
        E.gravar(est)
    if parar:
        print(json.dumps({"continue": False, "stopReason": parar}, ensure_ascii=False))
    elif aviso:
        print(json.dumps({"decision": "block", "reason": aviso}, ensure_ascii=False))
    return 0


def prompt(dados):
    sessao, texto = dados.get("session_id", "sem"), dados.get("prompt", "")
    if TURNO_AUTOMATICO.match(texto):  # aviso de tarefa em segundo plano, evento do app: não é o usuário
        return 0
    with E.Trava(sessao):
        est = E.carregar(sessao)
        est["pedidos"].append(texto)
        if est["fase"] == E.PARADO:
            E.novo_ciclo(est)
        E.gravar(est)
    print(json.dumps({"hookSpecificOutput": {"hookEventName": "UserPromptSubmit", "additionalContext":
                      f"Portão de processo do Roadcraft: {fase_txt(est)}. Código só muda na fase 4 e o jogo só roda nas fases 1 e 5. "
                      f"Os passos se registram com `{FORMA} passo N arquivo.json`; o formato de cada um sai de `{VER} modelo N`."}},
                     ensure_ascii=False))
    return 0


def hook():
    try:
        dados = json.load(sys.stdin)
    except ValueError:
        return 0
    evento = dados.get("hook_event_name", "")
    if evento != "UserPromptSubmit" and not no_escopo(dados):
        return 0
    if evento == "UserPromptSubmit":
        if not no_escopo(dados):
            return 0
        return prompt(dados)
    if evento == "PreToolUse":
        return pre(dados)
    if evento in ("PostToolUse", "PostToolUseFailure"):
        return pos(dados)
    return 0


def main(argv):
    sys.stdout.reconfigure(encoding="utf-8")
    cmd = argv[0] if argv else ""
    if cmd == "hook":
        return hook()
    if cmd == "modelo":
        print(json.dumps(P.MODELOS[int(argv[1])], ensure_ascii=False, indent=1))
        return 0
    if cmd == "passo":
        print(f"passo {argv[1]} aceito pelo portão")
        return 0
    if cmd == "estado":
        arquivos = [os.path.join(E.PASTA, f) for f in os.listdir(E.PASTA) if f.endswith(".json") and not f.startswith("indice")] if os.path.isdir(E.PASTA) else []
        if arquivos:
            est = L.carregar(max(arquivos, key=os.path.getmtime), {})
            print(json.dumps({k: est.get(k) for k in ("sessao", "fase", "ciclo", "penalidades", "plano", "alterados", "landmarks")}, ensure_ascii=False, indent=1))
        return 0
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
