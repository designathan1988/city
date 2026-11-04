"""Hook das regras (ordem do usuário): em toda mensagem e antes de toda ferramenta, entrega o Processo de trabalho.

O texto sai do próprio `fonte/CLAUDE.md`, o dono dele: nunca há uma cópia para desatualizar.
"""
import json
import sys
from pathlib import Path

DOCUMENTO = Path(r'C:\Projects\Roadcraft-windows\fonte\CLAUDE.md')
INICIO = '## Processo de trabalho'
FIM = '## Regras de arquitetura'


def regras():
    texto = DOCUMENTO.read_text(encoding='utf8').replace('\r\n', '\n')
    return texto[texto.index(INICIO):texto.index(FIM)].strip()


def main():
    entrada = json.loads(sys.stdin.buffer.read().decode('utf8', 'replace') or '{}')
    saida = {'hookSpecificOutput': {'hookEventName': entrada.get('hook_event_name', 'UserPromptSubmit'),
                                    'additionalContext': regras()}}
    sys.stdout.reconfigure(encoding='utf-8')
    print(json.dumps(saida, ensure_ascii=False))


if __name__ == '__main__':
    main()
