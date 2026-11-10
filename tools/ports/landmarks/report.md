# Relatório do porte marcos

**Estado: parcial.** Commit na ponta de `convert/landmarks`, sobre `ed0b723` do `convert/build`; cópia no clone `ports/landmarks/workspace`, branch `convert/marcos-porte`.

Feito: 38 projetos como dados com schema, 38 de 38 colocados na foto de grade (`ports/build/evidence/lotes1`), nomes, função e zona do documento, aba Lotes na prateleira. Gates OK, DLLs de release e debug, testes 13/13 com teste novo sobre os 38.

Causa achada: os lotes foram gravados na unidade antiga de 0,4 m (`TUNED_UNIT`) e o 4.0 de hoje os lê como metros, sem converter. Convertidos para metros aqui.

**NÃO FEITO:** o desenho próprio de cada lote (`signature.ts`, `landmarks.ts`, `facadeKit.ts`, cerca de 2.700 linhas). O 4.0 desenha esses prédios por partes próprias, chave `signature:<nome>`. No jogo eles saem com a fachada genérica: muitos brancos, sem vidro nem cor de assinatura. O rastreamento do `landmarks` não incluía esses arquivos.

Não verificado: os cartões da aba Lotes em foto; par com o 4.0.

Estádio: não existe no 4.0; pendência removida.
