# Space Invaders

Um jogo simples de Space Invaders desenvolvido em C usando a biblioteca Allegro 5.

## Pré-requisitos

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y liballegro5-dev liballegro5.2 liballegro-acodec5-dev liballegro-audio5-dev liballegro-dialog5-dev liballegro-image5-dev liballegro-physfs5-dev liballegro-ttf5-dev liballegro-video5-dev liballegro-primitives5-dev fonts-liberation
```

## Como compilar e executar

### Usando o Makefile (Recomendado)
```bash
# Compilar o projeto
make

# Executar o jogo
make run

# Ou executar diretamente
./space_invaders

# Limpar arquivos compilados
make clean
```

### Compilação manual
```bash
gcc -Wall -Wextra -std=c99 -o space_invaders space_invaders.c $(pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5)
```

## Controles

- **Mouse**: Clique para interagir
- **Teclado**: Pressione teclas para ver os códigos
- **Fechar**: Clique no X da janela

## Funcionalidades atuais

- ✅ Janela do jogo (960x540 pixels)
- ✅ Loop principal do jogo (60 FPS)
- ✅ Detecção de eventos (mouse, teclado, fechamento)
- ✅ Sistema de fontes
- ✅ Tela preta (pronto para desenhar elementos do jogo)

## Próximos passos

- Adicionar sprites e imagens
- Implementar jogador
- Adicionar inimigos
- Sistema de pontuação
- Sons e música