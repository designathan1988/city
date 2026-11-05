# Analisador genérico de causas-raiz: varre qualquer cópia do projeto (qualquer versão) atrás das formas de defeito
# que geram efeitos de desempenho e consumo (detectores.py + os daqui) e, se pedido, compila e roda as sondas de
# tempo real que existirem. Não depende de nomes de classes nem de correções feitas: reporta causa, efeito, onde e prova.
# Uso: python tools/analise/causas.py [--raiz DIR] [--json SAIDA] [--compilar DIR_BUILD] [--medir GODOT]
import argparse
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import detectores as d  # noqa: E402

PROBES = ["tests/perf/main_thread_probe.tscn", "tests/perf/pending_probe.tscn"]


def consultas_com_efeito(raiz, fontes):
    """Uma 'consulta' do script (QUERIES) cujo método no núcleo muda estado: dois donos do trabalho por quadro."""
    out = []
    api = [p for p in d.arquivos(os.path.join(raiz, "project"), d.GD_EXT) if p.endswith("store_api.gd")]
    if not api:
        return out
    texto = d.ler(api[0])
    bloco = re.search(r"QUERIES\s*:?=\s*\{(.*?)\n\}", texto, re.S)
    nomes = set(re.findall(r'&"\w+"\s*:\s*"(?:\w+/)?(\w+)"', bloco.group(1))) if bloco else set() # "metodo" or "area/metodo"
    for p, t in fontes.items():
        if "/api/" not in p.replace("\\", "/"):
            continue
        for nome, linha, corpo, const in d.funcoes(t):
            curto = nome.split("::")[-1]
            mutadora = re.search(r"(?:\.|->)(sync|submit|step\w*|mark_\w*|touch|publish\w*)\s*\(", corpo) # any receiver: the class may change
            if curto in nomes and not const and mutadora:
                out.append(d.achado("CONSULTA_COM_EFEITO", f"a consulta '{curto}' altera estado (não é const e chama sync/submit/set)",
                                    "trabalho por quadro dirigido por quem lê; some quando a tela que lê não existe",
                                    f"{d.rel(raiz, p)}:{linha}", "método de QUERIES não-const com chamada mutadora"))
    return out


def polling_por_quadro(raiz):
    """_process/_draw que perguntam ao núcleo a cada quadro sem olhar uma revisão."""
    out = []
    for p in d.arquivos(os.path.join(raiz, "project"), d.GD_EXT):
        if "/tests/" in p.replace("\\", "/"):
            continue
        t = d.ler(p)
        for m in re.finditer(r"^func (_process|_draw|_physics_process)\(.*?(?=^func |\Z)", t, re.S | re.M):
            corpo = m.group(0)
            for q in re.finditer(r"\.query\(\s*&?\"(\w+)\"", corpo):
                if not re.search(r"revision|_seen|cached\(", corpo):
                    linha = t[:m.start()].count("\n") + corpo[:q.start()].count("\n") + 1
                    out.append(d.achado("POLLING_POR_QUADRO", f"{m.group(1)} pergunta '{q.group(1)}' ao núcleo a cada quadro",
                                        "custo e travas do núcleo pagos todo quadro mesmo sem mudança",
                                        f"{d.rel(raiz, p)}:{linha}", "query dentro de callback por quadro, sem revisão"))
    return out


def caches_de_revisao(raiz, fontes):
    """Cache guardado por uma revisão que não avança em todo caminho que troca o estado (mapa novo, carga)."""
    out = []
    sobe = {}
    for p, t in fontes.items():
        for nome, linha, corpo, _ in d.funcoes(t):
            for r in re.findall(r"\b(\w*revision)\s*(?:\+\+|\+=)", corpo):
                sobe.setdefault(r, set()).add(nome.split("::")[-1])
    for p, t in fontes.items():
        for m in re.finditer(r"if\s*\(\s*[\w.>-]*?(\w*revision)\s*!=\s*(\w+_)\s*\)", t):
            rev = m.group(1)
            quem = sobe.get(rev, set())
            troca = {f for f in quem if re.search(r"found|load|reset|new_map|generate|clear", f, re.I)}
            if not troca:
                linha = t[:m.start()].count("\n") + 1
                out.append(d.achado("CACHE_COM_CHAVE_INCOMPLETA", f"cache guardado por '{rev}', que só avança em: {', '.join(sorted(quem)) or 'nenhum lugar'}",
                                    "valores velhos depois de mapa novo/carga ou de mudanças fora dos comandos",
                                    f"{d.rel(raiz, p)}:{linha}", "nenhuma função de troca de estado (found/load/reset/generate) avança a revisão"))
    return out


def compilar(raiz, build):
    """Avisos do compilador são código morto ou intenção escondida: cada um é um achado."""
    out = []
    r = subprocess.run(["cmake", "--build", build, "--target", "citycore"], capture_output=True, text=True, cwd=raiz)
    for m in re.finditer(r"^(.*?):(\d+):\d+: warning: (.*)$", r.stdout + r.stderr, re.M):
        out.append(d.achado("AVISO_DO_COMPILADOR", m.group(3), "código morto ou retorno ignorado sem intenção explícita",
                            f"{os.path.relpath(m.group(1), raiz)}:{m.group(2)}", "warning do build"))
    return out


def medir(raiz, godot):
    """Roda as sondas de tempo real que a cópia tiver (cada uma pula o que a versão não oferece)."""
    saida = {}
    for cena in PROBES:
        if os.path.exists(os.path.join(raiz, "project", cena)):
            r = subprocess.run([godot, "--path", os.path.join(raiz, "project"), "--resolution", "1600x900", "res://" + cena, "--", "--no-flow"],
                               capture_output=True, text=True, encoding="utf-8", errors="replace")
            saida[cena] = [l for l in r.stdout.splitlines() if re.search(r"ms|acordadas|chaves|tráfego", l)]
    return saida


def analisar(raiz):
    fontes = {p: d.ler(p) for p in d.arquivos(os.path.join(raiz, "native", "src"), d.CPP_EXT)}
    achados = []
    for detector in (d.travas_de_trabalho, d.realocacao_por_quadro, d.mapas_sem_poda, d.threads_que_acordam, d.constantes_duplicadas,
                     consultas_com_efeito, caches_de_revisao):
        achados += detector(raiz, fontes)
    achados += polling_por_quadro(raiz)
    return achados


def relatorio(achados, medidas):
    por = {}
    for a in achados:
        por.setdefault(a["regra"], []).append(a)
    print(f"Causas encontradas: {len(achados)} em {len(por)} regras")
    for regra, lista in sorted(por.items()):
        print(f"\n[{regra}] {len(lista)} — efeito: {lista[0]['efeito']}")
        for a in lista:
            print(f"  {a['onde']}: {a['causa']} ({a['prova']})")
    for cena, linhas in medidas.items():
        print(f"\n[MEDIDA] {cena}")
        for l in linhas:
            print("  " + l)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--raiz", default=os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")))
    ap.add_argument("--json")
    ap.add_argument("--compilar", help="pasta de build do CMake: os warnings viram achados")
    ap.add_argument("--medir", help="executável do Godot: roda as sondas de tempo real")
    a = ap.parse_args()
    achados = analisar(a.raiz)
    if a.compilar:
        achados += compilar(a.raiz, a.compilar)
    medidas = medir(a.raiz, a.medir) if a.medir else {}
    relatorio(achados, medidas)
    if a.json:
        with open(a.json, "w", encoding="utf-8") as f:
            json.dump({"achados": achados, "medidas": medidas}, f, ensure_ascii=False, indent=1)


if __name__ == "__main__":
    main()
