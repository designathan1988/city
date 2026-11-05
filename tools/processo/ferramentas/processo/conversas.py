"""Prova do juiz de fim de turno em sessões reais do Claude, com tudo ligado. Nenhuma frase é dada à sessão: nos casos de hesitação, um
system prompt só a torna cautelosa, e ela pede confirmação com as próprias palavras, então a redação não existe em arquivo nenhum e o
juiz não pode ter sido escrito para ela. Os controles são conversas sem relação com o projeto, que não podem ser seguradas. Um caso que
falhar fica como falha: esta bateria mede o juiz, e juiz de modelo erra.
"""
import concurrent.futures as cf
import json
import os
import shutil
import stat
import subprocess
import sys

AQUI = os.path.dirname(os.path.abspath(__file__))
RAIZ = os.path.abspath(os.path.join(AQUI, "..", ".."))
BASE = os.path.join(RAIZ, "ports", "_portao_conversas").replace("\\", "/")
LIMITE_S = 420
PARALELO = 4
CAUTELA = ("Before changing anything you always stop and ask the user to confirm, in your own words, and end your turn without using tools. "
           "If you are then told to go on, do the work.")
HESITA = [  # (nome, pedido, arquivo que precisa existir no fim)
    ("cria arquivo em português", "Crie o arquivo notas.txt com a palavra ok.", "notas.txt"),
    ("creates a file in English", "Create the file todo.md containing the line hello.", "todo.md"),
    ("copia arquivo", "Copie semente.txt para copia.txt.", "copia.txt"),
]
CONTROLES = [  # (nome, pedido): o turno precisa terminar na primeira resposta
    ("pergunta de geografia", "Qual é a capital da França? Responda em uma frase."),
    ("explicação técnica", "Explique em duas frases o que é um mutex."),
    ("explicar a frase de chat", "O que significa alguém escrever 'aguardando sua resposta' num chat? Uma frase."),
    ("pedido de opinião com pergunta de volta", "Quero escolher um nome para meu gato. Me faça uma pergunta para ajudar."),
    ("ordem de não fazer nada", "Não faça nada e não use ferramentas. Só diga ok."),
]


def apagar(pasta):
    def forcar(funcao, caminho, _):
        os.chmod(caminho, stat.S_IWRITE)
        funcao(caminho)
    if os.path.isdir(pasta):
        shutil.rmtree(pasta, onexc=forcar)


def sessao(i, pedido, cautela):
    pasta = f"{BASE}/{i}/workspace"
    os.makedirs(pasta)
    with open(pasta + "/semente.txt", "w") as f:
        f.write("x\n")
    subprocess.run(["git", "init", "-q"], cwd=pasta, check=True)
    cmd = [shutil.which("claude"), "-p", "--output-format", "json"] + (["--append-system-prompt", CAUTELA] if cautela else [])
    try:
        r = subprocess.run(cmd, input=pedido, cwd=pasta, capture_output=True, text=True, encoding="utf8", errors="replace", timeout=LIMITE_S)
        return pasta, json.loads(r.stdout)
    except (subprocess.TimeoutExpired, ValueError) as e:
        return pasta, {"erro": str(e)[:200]}


def respostas_e_ferramentas(saida):
    """Quantas vezes a sessão escreveu ao usuário e quantas ferramentas chamou, lido da transcrição dela."""
    achados = []
    base = os.path.expanduser("~/.claude/projects")
    for d in os.listdir(base):
        c = os.path.join(base, d, str(saida.get("session_id")) + ".jsonl")
        if os.path.isfile(c):
            achados.append(c)
    textos = ferramentas = 0
    primeira = ""
    for c in achados:
        for linha in open(c, encoding="utf8", errors="replace"):
            d = json.loads(linha)
            conteudo = (d.get("message") or {}).get("content")
            if d.get("type") != "assistant" or not isinstance(conteudo, list):
                continue
            for b in conteudo:
                if b.get("type") == "text" and b.get("text", "").strip():
                    textos += 1
                    primeira = primeira or b["text"][:140].replace("\n", " ")
                elif b.get("type") == "tool_use":
                    ferramentas += 1
    return textos, ferramentas, primeira


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    apagar(BASE)
    casos = [(n, p, a, True) for n, p, a in HESITA] + [(n, p, None, False) for n, p in CONTROLES]
    falhas = []
    with cf.ThreadPoolExecutor(PARALELO) as ex:
        feitos = list(ex.map(lambda t: sessao(t[0], t[1][1], t[1][3]), enumerate(casos)))
    for (nome, _, arquivo, cautela), (pasta, saida) in zip(casos, feitos):
        textos, ferramentas, primeira = respostas_e_ferramentas(saida)
        if cautela:
            ok = os.path.isfile(f"{pasta}/{arquivo}") and textos >= 2
            veredito = "segurou e a sessão fez o trabalho" if ok else "NÃO fez o trabalho"
        else:
            ok = textos == 1 and "erro" not in saida
            veredito = "terminou na primeira resposta" if ok else "SEGURADO sem motivo"
        print(("ok   " if ok else "FAIL ") + f"{nome}: {veredito} (textos {textos}, ferramentas {ferramentas}) | {primeira}")
        if not ok:
            falhas.append(nome)
    apagar(BASE)
    print("FALHAS:", len(falhas), falhas)
    return 1 if falhas else 0
