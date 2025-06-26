# DOCUMENTAÇÃO – SPACE INVADERS ALLEGRO

**Nome:** Luan Henrry Vidal Oliveira  
**Disciplina:** PDS 1  
**Professor:** Pedro O.S. Vaz de Melo  

## 📖 Descrição

Space Invaders é uma recriação completa do clássico jogo arcade desenvolvido em C usando a biblioteca Allegro 5. O jogo inclui sistema de menu, múltiplos níveis de dificuldade, sistema de pontuação e recordes persistentes.

## 🎮 Como Jogar

### Menu Principal
O jogo possui um menu principal com três opções:
- **1 - Iniciar Jogo**: Começa uma nova partida
- **2 - Alterar Dificuldade**: Permite escolher entre três níveis
- **3 - Sair**: Encerra o jogo

### Níveis de Dificuldade
- **Fácil**: 4×5 aliens, velocidade 1, 10 pontos por alien (máx: 200 pontos)
- **Normal**: 5×5 aliens, velocidade 2, 15 pontos por alien (máx: 375 pontos)  
- **Difícil**: 5×6 aliens, velocidade 3, 20 pontos por alien (máx: 600 pontos)

### Controles
- **A**: Move a nave para esquerda
- **D**: Move a nave para direita
- **ESPAÇO**: Dispara tiro (um por vez)
- **1/2/3**: Navegação nos menus
- **0**: Voltar (no menu de dificuldade)

### Objetivo
Destrua todos os alienígenas antes que eles atinjam o solo ou colidam com sua nave. Os aliens se movem em grupo e descem quando atingem as bordas da tela.

## 🏗️ Estruturas de Dados

### 1. **Dificuldade** (enum)
Define os três níveis de dificuldade com suas configurações específicas.

### 2. **Nave**
```c
typedef struct Nave {
    float x;                    // Posição horizontal
    float velocidade;           // Velocidade de movimento
    int direita, esquerda;      // Flags de movimento
    ALLEGRO_COLOR cor;          // Cor da nave
} Nave;
```

### 3. **Alien**
```c
typedef struct Alien {
    float x, y;                 // Posição
    float velocidade_x, velocidade_y; // Velocidades
    ALLEGRO_COLOR cor;          // Cor (aleatória)
    int ativo;                  // Status ativo/destruído
} Alien;
```

### 4. **Tiro**
```c
typedef struct Tiro {
    float x, y;                 // Posição
    int ativo;                  // Status ativo/inativo
    ALLEGRO_COLOR cor;          // Cor do tiro
} Tiro;
```

### 5. **Jogo**
```c
typedef struct Jogo {
    int linhas_alien;           // Número de linhas
    int colunas_alien;          // Número de colunas
    float velocidade_alien;     // Velocidade base
    int pontos_por_alien;       // Pontos por alien
    Alien aliens[6][6];         // Matriz de aliens
} Jogo;
```

## 🔧 Constantes do Jogo

```c
const int LARGURA_TELA = 960;
const int ALTURA_TELA = 540;
const int LARGURA_NAVE = 100;
const int ALTURA_NAVE = 50;
const int LARGURA_ALIEN = 50;
const int ALTURA_ALIEN = 25;
const float VELOCIDADE_TIRO = 8;
const float FPS = 60;
```

## 📏 Distâncias e Limites do Jogo

### 🎯 **Condições de Game Over**

O jogo termina quando os alienígenas atingem certas distâncias críticas:

#### **1. Colisão com a Grama (Solo)**
- **Distância vertical**: Quando qualquer alien atinge a linha Y = 480 pixels
- **Cálculo**: `alien.y + ALTURA_ALIEN > ALTURA_TELA - ALTURA_GRAMA`
- **Posição exata**: Y = 540 - 60 = 480 pixels (base da grama)
- **Resultado**: Game Over imediato

#### **2. Colisão com a Nave**
- **Posição da nave**: Centro na linha Y = 510 pixels (ALTURA_TELA - ALTURA_GRAMA/2)
- **Dimensões da nave**: 
  - Largura: 100 pixels (LARGURA_NAVE)
  - Altura: 50 pixels (ALTURA_NAVE)
  - Formato: Triângulo apontando para cima
- **Área de colisão da nave**:
  - **Horizontal**: X ± 50 pixels (nave.x ± LARGURA_NAVE/2)
  - **Vertical**: Y = 460 a 510 pixels (nave_y_topo a nave_y_base)
- **Detecção**: Colisão retangular entre alien (50×25) e área retangular da nave (100×50)
- **Resultado**: Game Over imediato

### 🔍 **Como Funciona a Detecção de Colisão**

#### **Visual vs. Colisão**
- **Visual da nave**: Triângulo azul apontando para cima
- **Área de colisão**: Retângulo completo de 100×50 pixels
- **Por que retângulo?**: Mais simples e eficiente para detectar colisões

#### **Cálculo da Área de Colisão**
```c
// Área retangular da nave (para colisão)
nave_x_esquerda = nave.x - 50;  // nave.x - LARGURA_NAVE/2
nave_x_direita = nave.x + 50;   // nave.x + LARGURA_NAVE/2
nave_y_topo = 460;              // nave_y_base - ALTURA_NAVE
nave_y_fundo = 510;             // nave_y_base
```

#### **Verificação de Colisão**
```c
// Se alien sobrepõe a área retangular da nave
if(alien_x_esquerda < nave_x_direita && 
   alien_x_direita > nave_x_esquerda && 
   alien_y_topo < nave_y_fundo && 
   alien_y_fundo > nave_y_topo) {
    // GAME OVER!
}
```

#### **Tiros**
- **Tamanho**: 4×10 pixels
- **Velocidade**: 8 pixels por frame (para cima)
- **Posição inicial**: Centro da nave (X = nave.x, Y = 460)
- **Limite superior**: Y = -10 pixels (desaparece da tela)

### 🔄 **Movimento dos Aliens**

#### **Padrão de Movimento**
- **Horizontal**: Movimento sincronizado em grupo
- **Vertical**: Descida de 25 pixels quando atingem bordas
- **Inversão**: Direção muda quando X = 0 ou X = 910

#### **Velocidades por Dificuldade**
- **Fácil**: 1 pixel/frame
- **Normal**: 2 pixels/frame  
- **Difícil**: 3 pixels/frame

## 🚀 Funcionalidades Principais

### Sistema de Menu
- Menu principal com navegação por teclas
- Menu de seleção de dificuldade
- Exibição do recorde atual
- Interface intuitiva e responsiva

### Sistema de Jogo
- Movimento suave da nave
- Movimento sincronizado dos aliens
- Sistema de tiro único
- Detecção de colisões precisa
- Verificação de vitória/derrota

### Sistema de Recordes
- Persistência de dados em arquivo
- Atualização automática de recordes
- Exibição de "NOVO RECORDE!" quando aplicável
- Arquivo `recorde.txt` criado automaticamente

## 📁 Estrutura do Projeto

```
space-invaders/
├── space_invaders.c    # Código fonte principal
├── README.md          # Esta documentação
├── Makefile           # Arquivo de compilação
├── arial.ttf          # Fonte TrueType para interface
└── recorde.txt        # Arquivo de recordes (gerado automaticamente)
```

## 🛠️ Compilação e Execução

### Pré-requisitos
- GCC (GNU Compiler Collection)
- Allegro 5 e suas bibliotecas:
  - `liballegro5-dev`
  - `liballegro5-ttf-dev`
  - `liballegro5-primitives-dev`
  - `liballegro5-image-dev`

### Instalação das Dependências (Ubuntu/Debian)
```bash
sudo apt-get install gcc make
sudo apt-get install liballegro5-dev liballegro5-ttf-dev liballegro5-primitives-dev liballegro5-image-dev
```

### Compilação
```bash
make
```

### Execução
```bash
./space_invaders
```

## 🎯 Funcões Principais

### Inicialização
- `inicializar_nave()` - Configura a nave do jogador
- `inicializar_alien()` - Cria um alienígena
- `inicializar_aliens()` - Inicializa todos os aliens
- `inicializar_jogo()` - Configura o jogo baseado na dificuldade

### Atualização
- `atualizar_nave()` - Processa movimento da nave
- `atualizar_aliens()` - Move os aliens e inverte direção
- `atualizar_tiro()` - Move o tiro e verifica saída da tela

### Colisões
- `verificar_colisao_tiro_alien()` - Detecta tiro atingindo alien
- `verificar_colisao_alien_solo()` - Verifica alien no solo
- `verificar_colisao_alien_nave()` - Detecta colisão alien-nave
- `todos_aliens_destruidos()` - Verifica vitória

### Interface
- `desenhar_menu()` - Menu principal
- `desenhar_menu_dificuldade()` - Menu de dificuldade
- `mostrar_resultado_final()` - Tela de fim de jogo
- `desenhar_pontuacao()` - Exibe pontuação atual

### Recordes
- `ler_recorde()` - Lê recorde do arquivo
- `salvar_recorde()` - Salva novo recorde

## 📊 Características Técnicas

- **Linguagem**: C (padrão C99)
- **Biblioteca**: Allegro 5
- **Resolução**: 960×540 pixels
- **FPS**: 60 quadros por segundo
- **Compatibilidade**: Linux (testado em Ubuntu)

## 🎨 Elementos Visuais

- **Nave**: Triângulo azul na parte inferior
- **Aliens**: Retângulos coloridos (cores aleatórias)
- **Tiros**: Retângulos amarelos
- **Cenário**: Fundo preto com grama verde na base