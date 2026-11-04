"""Vigia das mensagens da conversa da coordenadora: imprime cada mensagem nova que chega, do usuário ou de outra sessão.

O histórico grava a mensagem dada no meio de um turno como `queue-operation` e a que abre um turno como `user` com
`origin.kind`; a mesma mensagem pode aparecer nos dois, por isso o texto é conferido antes de imprimir.
Uso: python -u vigia_mensagens.py <session id da conversa> [--do-inicio]
"""
import json
import sys
import time
from pathlib import Path

HISTORICO = Path.home() / '.claude' / 'projects' / 'C--Projects-Roadcraft-windows-fonte'
INTERVALO_S = 3
TAMANHO = 400
# Avisos automáticos do sistema, que não são mensagens de ninguém.
IGNORAR = ('<task-notification', '<system-reminder', '<side-sessions-event', '<ci-monitor-event')


def texto_de(conteudo):
    if isinstance(conteudo, str):
        return conteudo
    if isinstance(conteudo, list):
        return ' '.join(b.get('text', '') for b in conteudo if isinstance(b, dict) and b.get('type') == 'text')
    return ''


def mensagem(item):
    """(remetente, texto) de uma linha do histórico, ou None se ela não é mensagem."""
    if item.get('type') == 'queue-operation' and item.get('operation') == 'enqueue':
        return 'fila', texto_de(item.get('content'))
    if item.get('type') == 'user' and item.get('origin'):
        return item['origin'].get('kind', '?'), texto_de((item.get('message') or {}).get('content'))
    return None


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    arquivo = HISTORICO / f'{sys.argv[1]}.jsonl'
    lido = 0 if '--do-inicio' in sys.argv else arquivo.stat().st_size
    vistas = set()
    while True:
        with open(arquivo, 'rb') as f:
            f.seek(lido)
            dados = f.read()
        fim = dados.rfind(b'\n') + 1
        lido += fim
        for linha in dados[:fim].splitlines():
            try:
                achada = mensagem(json.loads(linha))
            except ValueError:
                continue
            if not achada:
                continue
            remetente, texto = achada
            texto = ' '.join(texto.split())
            if not texto or texto.startswith(IGNORAR) or texto in vistas:
                continue
            vistas.add(texto)
            print(f'MENSAGEM ({remetente}): {texto[:TAMANHO]}', flush=True)
        if '--uma-vez' in sys.argv:
            return
        time.sleep(INTERVALO_S)


if __name__ == '__main__':
    main()
