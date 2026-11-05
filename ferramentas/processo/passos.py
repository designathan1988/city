"""O que cada passo precisa provar. Tudo aqui é conferido contra o que os hooks viram acontecer, não contra o que o registro afirma:
trecho do pedido contra as mensagens do usuário, arquivo contra as leituras gravadas, citação contra o texto que a sessão baixou,
artefato contra a data e a leitura dele. O julgamento que um programa não faz (se a solução trata a causa, se a foto mostra o que se
diz) fica com o hook por agente nos passos 3 e 5.
"""
import json
import os
import re
import time

import comandos as C
import estado as E

L = E.L
MIN_TEXTO = 40  # um campo de análise mais curto que isto não diz nada
MIN_CITACAO = 30
MAX_RELATORIO = 3000
IMAGEM = (".png", ".jpg", ".jpeg", ".webp")
MODELOS = {
    1: {"pedido": ["trecho literal da mensagem do usuário, um por exigência"], "problema": "o que está errado, nas suas palavras",
        "causa": "a causa raiz, com o mecanismo", "evidencias": [{"arquivo": "caminho", "linhas": "a-b (omita: arquivo inteiro)"}]},
    2: {"fontes": [{"url": "URL aberta depois do passo 1 (WebFetch ou curl)", "citacao": "trecho literal do que a página diz",
                    "conclusao": "o que isso muda"}]},
    3: {"solucao": "a solução escolhida", "causa_tratada": "como ela elimina a causa do passo 1",
        "plano": [{"arquivo": "todo arquivo que vai mudar ou nascer", "mudanca": "o que muda nele"}],
        "simbolos": [{"nome": "função ou tipo usado", "arquivo": "quem o declara"}],
        "alternativas": [{"solucao": "a opção mais barata ou mais fácil", "por_que_nao": "por que ela não resolve a causa"}],
        "fontes": ["URLs do passo 2 que sustentam a solução"]},
    4: {"feito": [{"arquivo": "arquivo do plano", "o_que": "o que foi feito"}]},
    5: {"resultado": "passou ou falhou", "artefatos": ["log, foto ou saída produzidos no teste, lidos por você"],
        "analise": "o que cada artefato mostra, inclusive defeitos", "relatorio": "o que o usuário lê na parada: feito, verificado, não verificado"},
}


def pedidos_da_transcricao(caminho):
    saida = []
    try:
        with open(caminho, encoding="utf8", errors="replace") as f:
            for linha in f:
                try:
                    d = json.loads(linha)
                except ValueError:
                    continue
                if d.get("type") != "user":
                    continue
                c = (d.get("message") or {}).get("content")
                if isinstance(c, str):
                    saida.append(c)
                elif isinstance(c, list):
                    saida += [b.get("text", "") for b in c if isinstance(b, dict) and b.get("type") == "text"]
    except OSError:
        pass
    return saida


def _texto(r, campo, erros):
    if len(str(r.get(campo, "")).strip()) < MIN_TEXTO:
        erros.append(f"campo `{campo}` vazio ou curto demais (mínimo {MIN_TEXTO} caracteres)")


def _lista(r, campo, erros):
    v = r.get(campo)
    if not isinstance(v, list) or not v:
        erros.append(f"campo `{campo}` precisa de ao menos um item")
        return []
    return v


def _p1(est, r, dados, erros):
    falas = E.normal_texto(" \n ".join(est["pedidos"] + pedidos_da_transcricao(dados.get("transcript_path", ""))))
    for t in _lista(r, "pedido", erros):
        if len(str(t)) < 8 or E.normal_texto(t) not in falas:
            erros.append(f"o trecho do pedido não está nas mensagens do usuário: {str(t)[:80]!r}")
    _texto(r, "problema", erros)
    _texto(r, "causa", erros)
    for ev in _lista(r, "evidencias", erros):
        a = b = None
        m = re.match(r"^(\d+)-(\d+)$", str(ev.get("linhas", "")))
        if m:
            a, b = int(m.group(1)), int(m.group(2))
        if not E.cobre(est, L.normalizar(ev.get("arquivo", ""), dados.get("cwd")), a, b):
            erros.append(f"evidência não lida nesta versão: {ev.get('arquivo')} {ev.get('linhas', 'inteiro')} (busca textual não é leitura)")


def _p2(est, r, dados, erros):
    marco = est["marcos"].get("1", 0)
    abertas = [w["url"] for w in est["web"] if w["quando"] >= marco]
    for f in _lista(r, "fontes", erros):
        url = str(f.get("url", "")).split("#")[0].rstrip("/")
        if not any(a.split("#")[0].rstrip("/").removesuffix(".md") == url.removesuffix(".md") for a in abertas):
            erros.append(f"URL não aberta depois do passo 1: {url} (resultado de busca não é leitura)")
        if len(str(f.get("citacao", ""))) < MIN_CITACAO or not E.citacao_existe(est["sessao"], f.get("citacao", "")):
            erros.append(f"citação não encontrada no que a sessão baixou: {str(f.get('citacao'))[:80]!r}")
        _texto(f, "conclusao", erros)


def _p3(est, r, dados, erros):
    _texto(r, "solucao", erros)
    _texto(r, "causa_tratada", erros)
    for p in _lista(r, "plano", erros):
        c = L.normalizar(p.get("arquivo", ""), dados.get("cwd"))
        if os.path.isfile(c):
            if not E.cobre(est, c):
                erros.append(f"arquivo do plano não lido inteiro nesta versão: {L.relativo(c)}")
        elif not os.path.isdir(os.path.dirname(c)):
            erros.append(f"arquivo novo em pasta que não existe: {L.relativo(c)}")
    for s in r.get("simbolos") or []:
        c = L.normalizar(s.get("arquivo", ""), dados.get("cwd"))
        if not E.cobre(est, c):
            erros.append(f"declarador não lido inteiro: {L.relativo(c)} ({s.get('nome')})")
        elif str(s.get("nome")) not in open(c, encoding="utf8", errors="replace").read():
            erros.append(f"{s.get('nome')} não está em {L.relativo(c)}")
    for a in _lista(r, "alternativas", erros):
        _texto(a, "por_que_nao", erros)
    do_passo2 = {str(f.get("url", "")) for f in est["registros"].get("2", {}).get("fontes", [])}
    for u in _lista(r, "fontes", erros):
        if u not in do_passo2:
            erros.append(f"fonte fora do passo 2: {u}")


def _p4(est, r, dados, erros):
    alterados = set(est["alterados"])
    for c in est["plano"]:
        if c not in alterados:
            erros.append(f"arquivo do plano não alterado: {L.relativo(c)} (um plano errado volta ao passo 1)")
    _lista(r, "feito", erros)


def _p5(est, r, dados, erros):
    if r.get("resultado") not in ("passou", "falhou"):
        erros.append("`resultado` é passou ou falhou")
    if not est.get("rodou_no_5"):
        erros.append("nada rodou no passo 5: o teste é a execução real, não a conclusão")
    marco = est["marcos"].get("4", 0)
    for a in _lista(r, "artefatos", erros):
        c = L.normalizar(a, dados.get("cwd"))
        if not os.path.isfile(c):
            erros.append(f"artefato não existe: {a}")
            continue
        if os.path.getmtime(c) < marco:
            erros.append(f"artefato mais velho que o código: {a}")
        if c.lower().endswith(IMAGEM):
            if est["imagens"].get(c, 0) < os.path.getmtime(c):
                erros.append(f"imagem não aberta depois de gerada: {a}")
        elif not E.cobre(est, c) and not E.leu_algo(est, c):
            erros.append(f"artefato não lido: {a}")
    _texto(r, "analise", erros)
    _texto(r, "relatorio", erros)
    if len(str(r.get("relatorio", ""))) > MAX_RELATORIO:
        erros.append(f"`relatorio` com mais de {MAX_RELATORIO} caracteres")


REGRAS = {1: _p1, 2: _p2, 3: _p3, 4: _p4, 5: _p5}


def conferir(est, n, caminho, dados):
    """Lista de erros do registro do passo `n` no arquivo `caminho`; vazia: aceito."""
    if n != est["fase"] and not (n == 1 and est["fase"] < E.PARADO):  # recomeçar é sempre possível: nenhuma recusa é beco sem saída
        return [f"fora de ordem: o passo que falta é o {est['fase']}, não o {n}"]
    c = L.normalizar(caminho, dados.get("cwd"))
    try:
        with open(c, encoding="utf8") as f:
            r = json.load(f)
    except (OSError, ValueError) as e:
        return [f"registro ilegível ({c}): {e}"]
    erros = []
    REGRAS[n](est, r, dados, erros)
    return erros


def aplicar(est, n, caminho, dados):
    """O passo aceito: o hook pós-execução chega aqui, porque o agente juiz pode ter negado o comando."""
    c = L.normalizar(caminho, dados.get("cwd"))
    with open(c, encoding="utf8") as f:
        r = json.load(f)
    if n == 1 and est["fase"] > 1:
        E.novo_ciclo(est)
    est["registros"][str(n)] = r
    est["marcos"][str(n)] = time.time()
    est["fase"] = n + 1
    if n == 3:
        est["plano"] = [L.normalizar(p["arquivo"], dados.get("cwd")) for p in r["plano"]]
        est["alterados"] = []
    return r


__all__ = ["MODELOS", "conferir", "aplicar", "C"]
