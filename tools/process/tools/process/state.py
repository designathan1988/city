"""Estado do portão de processo: uma máquina de fases por sessão, gravada pelos hooks, nunca pelo modelo.

Fases: 1 compreender, 2 pesquisar, 3 estudar, 4 codificar, 5 testar, 6 parado. A fase diz o passo que falta registrar: o código só
pode mudar na 4, o jogo e os testes só rodam na 1 (medir) e na 5, e a 6 só termina com uma mensagem real do usuário. Duas chamadas
de ferramenta em paralelo disparam dois hooks ao mesmo tempo: toda alteração do estado passa pela trava (`Trava`), senão uma apaga
o que a outra gravou.
"""
import hashlib
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "reading"))
import leitura_lib as L  # noqa: E402

RAIZ = L.RAIZ.replace("\\", "/")
PASTA = os.environ.get("PROCESSO_PASTA") or os.path.join(L.RAIZ, "records", "process")
HISTORICO = "historico.jsonl"
PARADO = 6
TRAVA_ESPERA_S, TRAVA_VELHA_S, TRAVA_PASSO_S = 15, 20, 0.02
TEXTO_MAX = 40_000_000  # o que uma sessão guarda do que baixou, para conferir citações


class Trava:
    """Exclusão entre processos por arquivo criado com O_EXCL; uma trava mais velha que TRAVA_VELHA_S é de um hook morto."""

    def __init__(self, sessao):
        os.makedirs(PASTA, exist_ok=True)
        self.caminho = os.path.join(PASTA, sessao + ".trava")

    def __enter__(self):
        limite = time.time() + TRAVA_ESPERA_S
        while True:
            try:
                os.close(os.open(self.caminho, os.O_CREAT | os.O_EXCL | os.O_WRONLY))
                return self
            except FileExistsError:
                try:
                    if time.time() - os.path.getmtime(self.caminho) > TRAVA_VELHA_S:
                        os.remove(self.caminho)
                        continue
                except OSError:
                    pass
                if time.time() > limite:
                    raise TimeoutError("trava do estado presa: " + self.caminho)
                time.sleep(TRAVA_PASSO_S)

    def __exit__(self, *_):
        try:
            os.remove(self.caminho)
        except OSError:
            pass


def novo(sessao):
    return {"sessao": sessao, "fase": 1, "ciclo": 1, "pedidos": [], "leituras": {}, "imagens": {}, "web": [], "execucoes": {},
            "compilacoes": {}, "edicoes": 0, "landmarks": {}, "plano": [], "alterados": [], "linha_base": {}, "penalidades": 0,
            "registros": {}, "pendente": None}


def arquivo(sessao):
    return os.path.join(PASTA, sessao + ".json")


def carregar(sessao):
    return L.carregar(arquivo(sessao), None) or novo(sessao)


def gravar(est):
    L.gravar(arquivo(est["sessao"]), est)


def novo_ciclo(est, penalidade=False):
    """Volta ao passo 1. Por penalidade a leitura também se perde: o trabalho é invalidado e refeito."""
    est.update({"fase": 1, "ciclo": est["ciclo"] + 1, "web": [], "execucoes": {}, "compilacoes": {}, "edicoes": 0, "landmarks": {},
                "plano": [], "alterados": [], "registros": {}, "pendente": None, "rodou_no_5": False})
    if penalidade:
        est["penalidades"] += 1
        est["leituras"], est["imagens"] = {}, {}


def historico(est, evento, **dados):
    os.makedirs(PASTA, exist_ok=True)
    with open(os.path.join(PASTA, HISTORICO), "a", encoding="utf8") as f:
        f.write(json.dumps({"quando": L.agora(), "sessao": est["sessao"], "ciclo": est["ciclo"], "fase": est["fase"], "evento": evento,
                            **dados}, ensure_ascii=False) + "\n")


def normal_texto(t):
    return " ".join(str(t).split()).casefold()


def texto_caminho(sessao):
    return os.path.join(PASTA, sessao + ".baixado.txt")


def guardar_texto(sessao, texto):
    os.makedirs(PASTA, exist_ok=True)
    c = texto_caminho(sessao)
    if os.path.exists(c) and os.path.getsize(c) > TEXTO_MAX:
        return
    with open(c, "a", encoding="utf8", errors="replace") as f:
        f.write(normal_texto(texto) + "\n")


def citacao_existe(sessao, citacao):
    try:
        with open(texto_caminho(sessao), encoding="utf8", errors="replace") as f:
            return normal_texto(citacao) in f.read()
    except OSError:
        return False


def sha(caminho):
    with open(caminho, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def registrar_leitura(est, caminho, a, b):
    c = L.normalizar(caminho)
    if not os.path.isfile(c):
        return
    atual = sha(c)
    item = est["leituras"].get(c)
    if not item or item["sha"] != atual:
        item = {"sha": atual, "intervalos": []}  # o arquivo mudou: o que se leu dele antes não vale mais
    item["intervalos"] = L.unir(item["intervalos"] + [[a, b]])
    est["leituras"][c] = item


def cobre(est, caminho, a=None, b=None):
    """As linhas [a, b] (sem faixa: o arquivo inteiro) foram lidas nesta versão do arquivo."""
    c = L.normalizar(caminho)
    item = est["leituras"].get(c)
    if not item or not os.path.isfile(c) or item["sha"] != sha(c):
        return False
    total, _ = L.contar_linhas(c)
    a, b = a or 1, min(b or total, total)
    return any(x <= a and y >= b for x, y in item["intervalos"])


def leu_algo(est, caminho):
    """Um log longo se lê pelo fim: basta um trecho desta versão do arquivo."""
    c = L.normalizar(caminho)
    item = est["leituras"].get(c)
    return bool(item and item["intervalos"] and os.path.isfile(c) and item["sha"] == sha(c))


def git(args, cwd):
    try:
        r = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True, encoding="utf8", errors="replace", timeout=20)
        return r.stdout if r.returncode == 0 else ""
    except (OSError, subprocess.SubprocessError):
        return ""
