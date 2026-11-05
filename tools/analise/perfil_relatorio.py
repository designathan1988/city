# Relatório do amostrador (perfil.py): por thread, quanto roda e quanto espera, as funções que mais custam (próprio e
# inclusivo), onde espera (o primeiro quadro da aplicação acima do sistema) e as cadeias quentes; e a linha do tempo da
# thread principal em janelas (a abertura são as primeiras). Sistema = módulos da pasta do Windows, sem lista de nomes.
import os
import re
from collections import Counter, defaultdict

ATIVA = 0.25  # a parte do tempo entre duas amostras que a thread passou numa CPU: abaixo disso, ela estava esperando
TOPO = 8
PROFUNDIDADE = 6


def _funcao(nome):
    """'mod!f(args) (arq.cpp:12:3)' -> 'mod!f (arq.cpp)': a função e o arquivo, sem os argumentos nem a linha."""
    m = re.match(r"^(.*?!)?(.*?)(\(.*\))?\s*\(([^()]*?):\d+(?::\d+)?\)$", nome)
    if not m:
        return nome
    return f"{m.group(1) or ''}{re.sub(r'\(.*\)$', '', m.group(2)).strip()} ({m.group(4)})"


def _modulo(nome):
    return nome.split("!", 1)[0].lower()


def _do_sistema(nome, sistema):
    return _modulo(nome) in sistema


def relatorio(amostras, nomes, principal, janela_s, intervalo_ms, sistema=None):
    sistema = sistema if sistema is not None else {m for m in (_modulo(n) for n in nomes.values()) if m in SISTEMA_PADRAO}
    por_thread = defaultdict(lambda: {"n": 0, "espera": 0, "proprio": Counter(), "inclusivo": Counter(), "esperas": Counter(), "cadeias": Counter()})
    linha = defaultdict(lambda: {"n": 0, "espera": 0, "quente": Counter()})
    for t, tid, pilha, cpu, relogio in amostras:
        linhas = [nomes.get(a, hex(a)) for a in pilha]
        fr = [_funcao(f) for f in linhas]  # por função: as linhas de uma mesma função somam juntas
        app = [f for f in fr if not _do_sistema(f, sistema)]
        esperando = cpu < ATIVA * relogio  # pelo tempo de CPU da própria thread, não pelo nome do que está no topo
        r = por_thread[tid]
        r["n"] += 1
        topo_app = app[0] if app else (fr[0] if fr else "?")
        if esperando:
            r["espera"] += 1
            r["esperas"][topo_app] += 1
        else:
            r["proprio"][topo_app] += 1
            r["cadeias"][" <- ".join(app[:PROFUNDIDADE])] += 1
        for f in set(app):
            r["inclusivo"][f] += 1
        if tid == principal:
            j = linha[int(t // janela_s)]
            j["n"] += 1
            j["espera"] += esperando
            j["quente"][(("espera: " if esperando else "") + topo_app)] += 1
    ms = intervalo_ms
    dados = {"principal": principal, "threads": {}, "linha_do_tempo": []}
    print(f"Amostras: {len(amostras)} ({ms:g} ms entre varreduras); thread principal: {principal}")
    ordem = sorted(por_thread.items(), key=lambda kv: (kv[0] != principal, -(kv[1]["n"] - kv[1]["espera"])))
    for tid, r in ordem[:TOPO]:
        ativo = r["n"] - r["espera"]
        print(f"\n== thread {tid}{' (principal)' if tid == principal else ''}: {r['n']} amostras, ativa {100 * ativo / max(1, r['n']):.0f}%,"
              f" esperando {100 * r['espera'] / max(1, r['n']):.0f}%")
        for titulo, cont in (("próprio (onde a CPU está)", r["proprio"]), ("inclusivo (quem paga)", r["inclusivo"]),
                             ("esperas (quem espera)", r["esperas"]), ("cadeias quentes", r["cadeias"])):
            if cont:
                print(f"  {titulo}:")
                for f, n in cont.most_common(TOPO):
                    print(f"    {100 * n / max(1, r['n']):5.1f}%  {f}")
        dados["threads"][str(tid)] = {k: (v.most_common(40) if isinstance(v, Counter) else v) for k, v in r.items()}
    if linha:
        print(f"\n== linha do tempo da thread principal (janelas de {janela_s:g} s; a abertura são as primeiras)")
        for j in sorted(linha):
            v = linha[j]
            topo = ", ".join(f"{f} {100 * n / v['n']:.0f}%" for f, n in v["quente"].most_common(3))
            print(f"  {j * janela_s:6.1f} s: espera {100 * v['espera'] / v['n']:3.0f}% | {topo}")
            dados["linha_do_tempo"].append({"t": j * janela_s, "espera": v["espera"] / v["n"], "quente": v["quente"].most_common(5)})
    return dados


# Módulos do sistema operacional e dos runtimes comuns (não do programa): o que está acima deles é o código que se analisa.
SISTEMA_PADRAO = {m.lower() for m in os.listdir(os.path.join(os.environ.get("WINDIR", "C:\\Windows"), "System32")) if m.lower().endswith(".dll")}
