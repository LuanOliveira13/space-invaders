# DOCUMENTAÇÃO – SPACE INVADERS ALLEGRO

**Nome:** Luan Henrry Vidal Oliveira  
**Disciplina:** PDS 1
**Professor:** Pedro O.S. Vaz de Melo  

## Como o jogo funciona

O jogo Space Invaders é uma recriação do clássico arcade. No menu principal, o jogador pode selecionar entre três opções: iniciar jogo, alterar dificuldade ou sair. O jogo possui três níveis de dificuldade: Fácil (4x5 aliens, velocidade 1, 10 pontos), Normal (5x5 aliens, velocidade 2, 15 pontos) e Difícil (5x6 aliens, velocidade 3, 20 pontos).

Durante o jogo, o jogador controla uma nave na parte inferior da tela usando as teclas A (esquerda) e D (direita). A tecla ESPAÇO dispara um tiro vertical que pode destruir os aliens. Os aliens se movem horizontalmente em grupo, e quando qualquer alien atinge as bordas da tela, todos descem uma linha e invertem a direção.

O jogo termina quando todos os aliens são destruídos (vitória) ou quando um alien atinge o solo ou colide com a nave do jogador (derrota). O sistema mantém um recorde da maior pontuação alcançada, salvando-a em um arquivo de texto.

## Estruturas de Dados

Para construir o jogo, foram criadas e definidas 5 estruturas de dados principais:

**1. Dificuldade (enum):** Define os três níveis de dificuldade (FACIL, NORMAL, DIFICIL) com suas respectivas configurações de aliens, velocidade e pontuação.

**2. Nave:** Representa a nave controlada pelo jogador. Guarda informações como posição x, velocidade, direção de movimento (esquerda/direita) e cor.

**3. Alien:** Representa cada alien inimigo. Guarda posição x e y, velocidade horizontal e vertical, cor (aleatória) e status ativo/inativo.

**4. Tiro:** Representa os projéteis disparados pela nave. Guarda posição x e y, status ativo/inativo e cor.

**5. Jogo:** Estrutura principal que contém a configuração do jogo atual, incluindo número de linhas e colunas de aliens, velocidade dos aliens, pontos por alien destruído e a matriz de aliens (máximo 6x6).

O jogo utiliza constantes para definir dimensões da tela (960x540), tamanhos dos elementos (nave, aliens, tiros), espaçamentos entre aliens (30 pixels mínimo) e velocidade do tiro.

## Como o código funciona

O programa principal é dividido em três grandes partes: inicialização e menu, loop principal do jogo, e sistema de recordes.

**1. Inicialização e Menu:**
- Inicialização do Allegro e seus módulos (primitivas, fontes, imagens)
- Criação da janela, timer e fila de eventos
- Loop do menu principal com três opções
- Menu de seleção de dificuldade com configurações específicas
- Exibição do recorde atual no menu

**2. Loop Principal do Jogo:**
- Inicialização da nave, aliens e tiro baseada na dificuldade selecionada
- Loop de eventos que processa timer, teclado e fechamento de janela
- Atualização de posições (nave, aliens, tiro) a cada frame
- Verificação de colisões (tiro-alien, alien-solo, alien-nave)
- Desenho de todos os elementos na tela
- Verificação de condições de vitória/derrota

**3. Sistema de Recordes:**
- Leitura do recorde atual do arquivo "recorde.txt"
- Comparação da pontuação atual com o recorde
- Salvamento automático de novos recordes
- Exibição de mensagem especial para novos recordes

## Funções Principais

### Inicialização e Configuração
- **`void init_nave(Nave *nave)`**: Inicializa a nave na posição central da tela
- **`void init_alien(Alien *alien, int row, int col, float vel)`**: Inicializa um alien com posição, velocidade e cor aleatória
- **`void init_aliens(Jogo *jogo)`**: Inicializa todos os aliens da matriz baseada na dificuldade
- **`void init_jogo(Jogo *jogo, Dificuldade dificuldade)`**: Configura o jogo com parâmetros da dificuldade selecionada
- **`void init_tiro(Tiro *tiro)`**: Inicializa o tiro como inativo

### Controles e Movimento
- **`void update_nave(Nave *nave)`**: Atualiza a posição da nave baseada nas teclas pressionadas
- **`void update_aliens(Jogo *jogo)`**: Move todos os aliens horizontalmente e inverte direção quando atingem bordas
- **`void update_tiro(Tiro *tiro)`**: Move o tiro verticalmente para cima e o desativa quando sai da tela
- **`void disparar_tiro(Tiro *tiro, Nave nave)`**: Cria um novo tiro na posição da nave (apenas um por vez)

### Colisões e Detecção
- **`int colisao_tiro_alien(Tiro tiro, Alien alien)`**: Verifica colisão entre tiro e alien
- **`int colisao_alien_solo(Jogo *jogo)`**: Verifica se algum alien atingiu o solo
- **`int colisao_alien_nave(Jogo *jogo, Nave nave)`**: Verifica colisão entre aliens e nave
- **`int check_boundary_collision(Jogo *jogo)`**: Verifica se algum alien atingiu as bordas da tela
- **`int todos_aliens_destruidos(Jogo *jogo)`**: Verifica se todos os aliens foram eliminados

### Sistema de Pontuação e Recordes
- **`void check_tiro_aliens_collision(Tiro *tiro, Jogo *jogo, int *pontuacao)`**: Processa colisão tiro-alien e incrementa pontuação
- **`int ler_recorde()`**: Lê o recorde atual do arquivo "recorde.txt"
- **`void salvar_recorde(int novo_recorde)`**: Salva novo recorde no arquivo
- **`void mostrar_resultado_final(ALLEGRO_FONT *font, int pontuacao, int vitoria)`**: Exibe tela de resultado com pontuação e recorde

### Interface e Desenho
- **`void draw_scenario()`**: Desenha o fundo preto e a grama na parte inferior
- **`void draw_nave(Nave nave)`**: Desenha a nave como triângulo azul
- **`void draw_alien(Alien alien)`**: Desenha alien como retângulo colorido
- **`void draw_aliens(Jogo *jogo)`**: Desenha todos os aliens ativos
- **`void draw_tiro(Tiro tiro)`**: Desenha o tiro como retângulo amarelo
- **`void draw_pontuacao(int pontuacao, ALLEGRO_FONT *font)`**: Exibe pontuação atual no canto inferior esquerdo
- **`void draw_menu(ALLEGRO_FONT *font, Dificuldade dificuldade_atual)`**: Desenha menu principal com opções e recorde
- **`void draw_dificuldade_menu(ALLEGRO_FONT *font)`**: Desenha menu de seleção de dificuldade

### Configurações por Dificuldade
- **Fácil**: 4 linhas × 5 colunas = 20 aliens, velocidade 1, 10 pontos por alien (máximo 200 pontos)
- **Normal**: 5 linhas × 5 colunas = 25 aliens, velocidade 2, 15 pontos por alien (máximo 375 pontos)
- **Difícil**: 5 linhas × 6 colunas = 30 aliens, velocidade 3, 20 pontos por alien (máximo 600 pontos)

## Controles

- **A**: Move nave para esquerda
- **D**: Move nave para direita
- **ESPAÇO**: Dispara tiro (apenas um por vez)
- **1**: Iniciar jogo / Selecionar dificuldade fácil
- **2**: Alterar dificuldade / Selecionar dificuldade normal
- **3**: Sair do jogo / Selecionar dificuldade difícil
- **0**: Voltar ao menu principal (no menu de dificuldade)

## Arquivos Gerados

- **recorde.txt**: Arquivo de texto que armazena a maior pontuação alcançada
- O arquivo é criado automaticamente na primeira execução e atualizado quando há novos recordes