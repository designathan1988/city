"""Hook das regras (ordem do usuário): em toda mensagem e antes de toda ferramenta, entrega o que a sessão tem de ler.

- Toda sessão recebe o Processo de trabalho, tirado do próprio `fonte/CLAUDE.md`, o dono dele: nunca há cópia.
- A sessão listada em `coordenadoras.txt` recebe antes dele a ordem do usuário, palavra por palavra, de
  `ordem_do_usuario.txt` (o dono do texto), e o usuário vê uma linha (`systemMessage`) a cada entrega.
- Falha de leitura vira aviso visível ao usuário e no contexto, nunca silêncio.
"""
import json
import sys
from pathlib import Path

PASTA = Path(__file__).resolve().parent
DOCUMENTO = Path(r'C:\Projects\Roadcraft-windows\fonte\CLAUDE.md')
ORDEM = PASTA / 'ordem_do_usuario.txt'
COORDENADORAS = PASTA / 'coordenadoras.txt'
INICIO = '## Processo de trabalho'
FIM = '## Regras de arquivo'.replace('arquivo', 'arquitetura')
CABECALHO = ('ORDEM DO USUÁRIO. Leia agora, inteira, antes de qualquer ação: mensagem recebida, arquivo aberto, arquivo editado, '
             'mensagem enviada, qualquer ferramenta. Vale para tudo, sempre.')


def ler(caminho):
    return caminho.read_text(encoding='utf8').replace('\r\n', '\n').strip()


def regras():
    texto = ler(DOCUMENTO)
    return texto[texto.index(INICIO):texto.index(FIM)].strip()


def coordenadoras():
    try:
        return {linha.strip() for linha in ler(COORDENADORAS).split('\n') if linha.strip() and not linha.startswith('#')}
    except OSError:
        return set()


def entrada_do_hook():
    try:
        return json.loads(sys.stdin.buffer.read().decode('utf8', 'replace') or '{}')
    except ValueError:
        return {}


def main():
    entrada = entrada_do_hook()
    evento = entrada.get('hook_event_name', 'UserPromptSubmit')
    sessao = entrada.get('session_id', '')
    coordenadora = sessao in coordenadoras()
    partes, avisos = [], []
    if coordenadora:
        try:
            ordem = ler(ORDEM)
            partes.append(CABECALHO + '\n\n' + ordem)
            primeira = ordem.split('\n')[0]
        except OSError as erro:
            primeira = ''
            avisos.append(f'HOOK COM FALHA: não li {ORDEM.name} ({erro}). A ordem do usuário NÃO foi entregue.')
    try:
        partes.append(regras())
    except (OSError, ValueError) as erro:
        avisos.append(f'HOOK COM FALHA: não li o Processo de trabalho de {DOCUMENTO.name} ({erro}).')
    if not coordenadora:
        partes.append(f'[hook] session_id desta sessão: {sessao}')
    partes.extend(avisos)
    saida = {'hookSpecificOutput': {'hookEventName': evento, 'additionalContext': '\n\n'.join(partes)}}
    if coordenadora:
        alvo = entrada.get('tool_name') or 'mensagem recebida'
        saida['systemMessage'] = ' '.join(avisos) if avisos else f'Hook da ordem do usuário entregue à coordenadora ({evento}, {alvo}): «{primeira}»'
    sys.stdout.reconfigure(encoding='utf-8')
    print(json.dumps(saida, ensure_ascii=False))


if __name__ == '__main__':
    main()
