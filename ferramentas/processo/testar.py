"""Bateria do portão: cada caso manda ao hook o JSON que o Claude Code mandaria e confere a decisão. Roda num repositório de verdade,
criado em ports/_portao_teste/workspace e apagado no fim, com o estado em pasta temporária: nada do projeto é tocado.
Uso: python ferramentas/processo/testar.py
"""
import json
import os
import shutil
import stat
import subprocess
import sys
import tempfile
import time

AQUI = os.path.dirname(os.path.abspath(__file__))
RAIZ = os.path.abspath(os.path.join(AQUI, "..", ".."))
CLONE = os.path.join(RAIZ, "ports", "_portao_teste", "workspace").replace("\\", "/")
FORMA = "python C:/Projects/Roadcraft-windows/ferramentas/processo/processo.py"
PEDIDO = "resolva a queda da bandeira com pesquisa de verdade"
falhas = []


def hook(pasta, evento, **campos):
    dados = {"session_id": "t", "cwd": CLONE, "hook_event_name": evento, "transcript_path": "", **campos}
    r = subprocess.run([sys.executable, os.path.join(AQUI, "processo.py"), "hook"], input=json.dumps(dados), capture_output=True, text=True,
                       encoding="utf8", env=dict(os.environ, PROCESSO_PASTA=pasta))
    if r.returncode != 0 or r.stderr.strip():
        raise SystemExit(f"hook quebrou em {evento}: {r.stderr}")
    return json.loads(r.stdout) if r.stdout.strip() else {}


def negado(saida):
    return (saida.get("hookSpecificOutput") or {}).get("permissionDecision") == "deny"


def motivo(saida):
    return (saida.get("hookSpecificOutput") or {}).get("permissionDecisionReason", "")


def caso(nome, ok, detalhe=""):
    print(("ok   " if ok else "FAIL ") + nome + ("" if ok else f"  -> {detalhe}"))
    if not ok:
        falhas.append(nome)


def edita(pasta, caminho, novo, antigo="x"):
    return hook(pasta, "PreToolUse", tool_name="Edit", tool_input={"file_path": caminho, "old_string": antigo, "new_string": novo})


def shell(pasta, comando):
    return hook(pasta, "PreToolUse", tool_name="Bash", tool_input={"command": comando})


def depois(pasta, ferramenta, entrada, resposta=""):
    return hook(pasta, "PostToolUse", tool_name=ferramenta, tool_input=entrada, tool_response=resposta)


def ler(pasta, caminho):
    depois(pasta, "Read", {"file_path": caminho}, "conteudo")


def registro(nome, dados):
    c = os.path.join(CLONE, nome).replace("\\", "/")
    with open(c, "w", encoding="utf8") as f:
        json.dump(dados, f, ensure_ascii=False)
    return c


def passo(pasta, n, caminho):
    cmd = f"{FORMA} passo {n} {caminho}"
    pre = shell(pasta, cmd)
    if negado(pre):
        return pre, {}
    return pre, depois(pasta, "Bash", {"command": cmd}, {"stdout": "aceito"})


def apagar(pasta):
    """Os objetos do git são somente leitura no Windows: sem tirar isso o rmtree desiste em silêncio."""
    def forcar(funcao, caminho, _):
        os.chmod(caminho, stat.S_IWRITE)
        funcao(caminho)
    if os.path.isdir(pasta):
        shutil.rmtree(pasta, onexc=forcar)


def preparar():
    apagar(os.path.dirname(CLONE))
    os.makedirs(CLONE + "/native/src")
    os.makedirs(CLONE + "/project/game")
    with open(CLONE + "/native/src/a.cpp", "w") as f:
        f.write("// a\nint soma(int x) {\n\treturn x;\n}\n")
    with open(CLONE + "/project/game/cam.gd", "w") as f:
        f.write("## cam\nfunc jump(p):\n\treturn p\n")
    with open(CLONE + "/project/game/outro.gd", "w") as f:
        f.write("func helper_distante(x):\n\treturn x\n")
    for args in (["init", "-q"], ["add", "."], ["-c", "user.name=t", "-c", "user.email=t@t", "commit", "-qm", "base"]):
        subprocess.run(["git", *args], cwd=CLONE, check=True)


def ordem_e_bloqueio(p):
    a, gd = CLONE + "/native/src/a.cpp", CLONE + "/project/game/cam.gd"
    hook(p, "UserPromptSubmit", prompt=PEDIDO)
    caso("editar código na fase 1 é negado", negado(edita(p, a, "y")))
    caso("redirecionar para código por shell é negado", negado(shell(p, f"echo oi > {a}")))
    caso("sed -i em código é negado", negado(shell(p, f"sed -i s/a/b/ {a}")))
    caso("texto entre aspas citando > não é escrita", not negado(shell(p, 'echo "a > native/src/a.cpp"')))
    caso("editar documento passa", not negado(edita(p, CLONE + "/notas.md", "y")))
    caso("editar o portão é negado", negado(edita(p, "C:/Projects/Roadcraft-windows/ferramentas/processo/processo.py", "y")))
    caso("settings do usuário é negado", negado(edita(p, "C:/Users/Jonathan/.claude/settings.json", "y")))
    caso("apagar o estado por shell é negado", negado(shell(p, "rm C:/Projects/Roadcraft-windows/registro/processo/t.json")))
    caso("ler o portão passa", not negado(shell(p, "cat C:/Projects/Roadcraft-windows/ferramentas/processo/processo.py")))
    caso("passo fora de ordem é negado", "fora de ordem" in motivo(passo(p, 3, "x.json")[0]))
    ruim = registro("r1.json", {"pedido": ["algo que ninguém pediu aqui"], "problema": "p" * 50, "causa": "c" * 50, "evidencias": [{"arquivo": a}]})
    m = motivo(passo(p, 1, ruim)[0])
    caso("pedido inventado é recusado", "não está nas mensagens" in m, m)
    caso("evidência não lida é recusada", "não lida" in m, m)
    ler(p, a)
    ler(p, gd)
    bom = registro("r1b.json", {"pedido": ["queda da bandeira"], "problema": "p" * 50, "causa": "c" * 50, "evidencias": [{"arquivo": a}]})
    caso("passo 1 com prova é aceito", not negado(passo(p, 1, bom)[0]))
    caso("rodar o jogo na fase 2 é negado", negado(shell(p, "godot_console.exe --path project res://x.tscn")))


def pesquisa(p):
    url = "https://code.claude.com/docs/en/hooks.md"
    frase = "a hook's deny holds in every permission mode"
    r2 = registro("r2.json", {"fontes": [{"url": url, "citacao": frase, "conclusao": "c" * 50}]})
    caso("URL não aberta é recusada", "não aberta" in motivo(passo(p, 2, r2)[0]))
    cmd = f"curl -sL {url} | tail -c 100"
    shell(p, cmd)
    depois(p, "Bash", {"command": cmd}, {"stdout": "outra coisa qualquer sem a frase"})
    m = motivo(passo(p, 2, r2)[0])
    caso("citação que a página não tem é recusada", "citação não encontrada" in m, m)
    depois(p, "Bash", {"command": cmd}, {"stdout": "Note:  " + frase + "\n fim"})
    caso("passo 2 com URL e citação reais é aceito", not negado(passo(p, 2, r2)[0]))
    return url


def estudo_e_codigo(p, url):
    a, gd, novo = CLONE + "/native/src/a.cpp", CLONE + "/project/game/cam.gd", CLONE + "/project/game/novo.gd"
    outro = CLONE + "/project/game/outro.gd"
    base = {"solucao": "s" * 50, "causa_tratada": "t" * 50, "alternativas": [{"solucao": "b", "por_que_nao": "n" * 50}], "fontes": [url]}
    r3 = registro("r3.json", {**base, "plano": [{"arquivo": outro, "mudanca": "m"}], "simbolos": []})
    m = motivo(passo(p, 3, r3)[0])
    caso("plano com arquivo não lido é recusado", "não lido inteiro" in m, m)
    r3 = registro("r3b.json", {**base, "plano": [{"arquivo": gd, "mudanca": "m"}, {"arquivo": novo, "mudanca": "n"}], "simbolos": [{"nome": "jump", "arquivo": gd}]})
    ler(p, gd)
    caso("passo 3 aceito", not negado(passo(p, 3, r3)[0]))
    caso("arquivo fora do plano é negado", negado(edita(p, a, "y")))
    m = motivo(edita(p, gd, "func nova():\n\thelper_distante(1)\n"))
    caso("chamar função de arquivo não lido é negado", "helper_distante" in m, m)
    caso("chamar função já lida passa", not negado(edita(p, gd, "func nova2():\n\tjump(1)\n")))
    return gd, novo


def teste_e_parada(p, gd, novo):
    with open(gd, "a") as f:
        f.write("func nova2():\n\tjump(1)\n")
    depois(p, "Edit", {"file_path": gd}, {})
    r4 = registro("r4.json", {"feito": [{"arquivo": gd, "o_que": "x"}]})
    m = motivo(passo(p, 4, r4)[0])
    caso("passo 4 com arquivo do plano sem mudar é recusado", "não alterado" in m, m)
    with open(novo, "w") as f:
        f.write("func z():\n\tpass\n")
    depois(p, "Write", {"file_path": novo}, {})
    caso("passo 4 completo é aceito", not negado(passo(p, 4, r4)[0]))
    caso("editar código na fase 5 é negado", negado(edita(p, gd, "y")))
    jogo = "godot_console.exe --path project res://tests/a.tscn"
    r5 = {"resultado": "falhou", "analise": "a" * 50, "relatorio": "r" * 50}
    log = CLONE + "/saida.log"
    m = motivo(passo(p, 5, registro("r5.json", {**r5, "artefatos": [log]}))[0])
    caso("passo 5 sem execução é recusado", "nada rodou" in m, m)
    caso("primeira execução passa", not negado(shell(p, jogo)))
    depois(p, "Bash", {"command": jogo}, {"stdout": "x"})
    caso("repetir a execução é negado", "tentativa e erro" in motivo(shell(p, jogo)))
    time.sleep(0.05)
    foto = CLONE + "/foto.png"
    for c in (log, foto):
        with open(c, "w") as f:
            f.write("x\n")
    m = motivo(passo(p, 5, registro("r5b.json", {**r5, "artefatos": [foto]}))[0])
    caso("imagem não aberta é recusada", "imagem não aberta" in m, m)
    depois(p, "Read", {"file_path": foto}, "img")
    pre, pos = passo(p, 5, registro("r5c.json", {**r5, "artefatos": [foto]}))
    caso("passo 5 aceito", not negado(pre), motivo(pre))
    caso("o passo 5 para a sessão pelo programa", pos.get("continue") is False and "PARE" in pos.get("stopReason", ""), pos)
    caso("parada nega ler", negado(hook(p, "PreToolUse", tool_name="Read", tool_input={"file_path": gd})))
    hook(p, "UserPromptSubmit", prompt="<task-notification>tarefa</task-notification>")
    caso("aviso de tarefa não destrava", negado(shell(p, "ls")))
    hook(p, "UserPromptSubmit", prompt="agora faça outra coisa de verdade")
    caso("mensagem do usuário abre o ciclo seguinte", not negado(shell(p, "ls")))


def invalidacao(p):
    a = CLONE + "/native/src/a.cpp"
    shell(p, "ls")
    with open(a, "a") as f:  # um programa que escreve por dentro: o texto do comando não mostra nada
        f.write("// fora do processo\n")
    saida = depois(p, "Bash", {"command": "python gera.py"}, {"stdout": ""})
    caso("escrita por dentro de um script invalida o ciclo", "invalidado" in saida.get("reason", ""), saida)
    subprocess.run(["git", "checkout", "-q", "--", "native/src/a.cpp"], cwd=CLONE)


def sujo_de_antes():
    p = tempfile.mkdtemp()
    a = CLONE + "/native/src/a.cpp"
    with open(a, "a") as f:
        f.write("// sujo antes da sessão\n")
    shell(p, "ls")
    saida = depois(p, "Bash", {"command": "ls"}, {"stdout": ""})
    caso("código já sujo antes da sessão não invalida", "invalidado" not in saida.get("reason", ""), saida)
    subprocess.run(["git", "checkout", "-q", "--", "native/src/a.cpp"], cwd=CLONE)
    shutil.rmtree(p, ignore_errors=True)


def fora_do_projeto():
    p = tempfile.mkdtemp()
    d = {"session_id": "f", "cwd": "C:/Users/Jonathan/outro", "hook_event_name": "PreToolUse", "tool_name": "Edit",
         "tool_input": {"file_path": "C:/Users/Jonathan/outro/.claude/settings.json", "old_string": "a", "new_string": "b"}}
    r = subprocess.run([sys.executable, os.path.join(AQUI, "processo.py"), "hook"], input=json.dumps(d), capture_output=True, text=True,
                       encoding="utf8", env=dict(os.environ, PROCESSO_PASTA=p))
    caso("outro projeto não é da conta do portão", r.stdout.strip() == "", r.stdout)
    shutil.rmtree(p, ignore_errors=True)


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    preparar()
    pasta = tempfile.mkdtemp()
    try:
        ordem_e_bloqueio(pasta)
        url = pesquisa(pasta)
        gd, novo = estudo_e_codigo(pasta, url)
        teste_e_parada(pasta, gd, novo)
        invalidacao(pasta)
        sujo_de_antes()
        fora_do_projeto()
    finally:
        shutil.rmtree(pasta, ignore_errors=True)
        apagar(os.path.dirname(CLONE))
    print("FALHAS:", len(falhas), falhas)
    return 1 if falhas else 0


if __name__ == "__main__":
    sys.exit(main())
