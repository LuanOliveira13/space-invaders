#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// Constantes da tela
const int LARGURA_TELA = 960;
const int ALTURA_TELA = 540;

// Constantes do cenário
const int ALTURA_GRAMA = 60;

// Constantes da nave do jogador
const int LARGURA_NAVE = 100;
const int ALTURA_NAVE = 50;

// Constantes dos alienígenas
const int LARGURA_ALIEN = 50;
const int ALTURA_ALIEN = 30;

// Espaçamento entre alienígenas
const int ESPACAMENTO_ALIEN_X = 80;  // 50 (largura do alien) + 30 (espaço) = 80
const int ESPACAMENTO_ALIEN_Y = 55;  // 25 (altura do alien) + 30 (espaço) = 55
const int POSICAO_INICIAL_ALIEN_X = 0;
const int POSICAO_INICIAL_ALIEN_Y = 0;

// Constantes dos tiros
const int LARGURA_TIRO = 4;
const int ALTURA_TIRO = 10;
const float VELOCIDADE_TIRO = 8;

// Taxa de quadros por segundo
const float FPS = 60;

/**
 * @brief Enumeração para os níveis de dificuldade do jogo
 */
typedef enum {
    DIFICULDADE_FACIL,
    DIFICULDADE_NORMAL,
    DIFICULDADE_DIFICIL
} Dificuldade;

/**
 * @brief Estrutura que representa a nave do jogador
 */
typedef struct Nave{
    float x;                    ///< Posição horizontal da nave
    float velocidade;           ///< Velocidade de movimento da nave
    int direita, esquerda;      ///< Flags para movimento (1 = ativo, 0 = inativo)
    ALLEGRO_COLOR cor;          ///< Cor da nave
} Nave;

/**
 * @brief Estrutura que representa um alienígena
 */
typedef struct Alien{
    float x, y;                 ///< Posição do alienígena
    float velocidade_x, velocidade_y; ///< Velocidades de movimento
    ALLEGRO_COLOR cor;          ///< Cor do alienígena
    int ativo;                  ///< Flag indicando se o alien está ativo (1) ou destruído (0)
} Alien;

/**
 * @brief Estrutura que representa um tiro
 */
typedef struct Tiro{
    float x, y;                 ///< Posição do tiro
    int ativo;                  ///< Flag indicando se o tiro está ativo (1) ou inativo (0)
    ALLEGRO_COLOR cor;          ///< Cor do tiro
} Tiro;

/**
 * @brief Estrutura principal do jogo
 */
typedef struct Jogo{
    int linhas_alien;           ///< Número de linhas de alienígenas
    int colunas_alien;          ///< Número de colunas de alienígenas
    float velocidade_alien;     ///< Velocidade base dos alienígenas
    int pontos_por_alien;       ///< Pontos ganhos por alienígena destruído
    Alien aliens[6][6];         ///< Matriz de alienígenas (tamanho máximo para dificuldade difícil)
} Jogo;

// Declarações de funções
int ler_recorde(void);
void salvar_recorde(int novo_recorde);

/**
 * @brief Inicializa a nave do jogador com valores padrão
 * @param nave Ponteiro para a estrutura da nave
 */
void inicializar_nave(Nave *nave){
    nave->x = LARGURA_TELA/2;
    nave->velocidade = 3;
    nave->direita = 0;
    nave->esquerda = 0;
    nave->cor = al_map_rgb(0, 0, 255);
}

/**
 * @brief Inicializa um alienígena com posição e velocidade específicas
 * @param alien Ponteiro para a estrutura do alienígena
 * @param linha Linha do alienígena na matriz
 * @param coluna Coluna do alienígena na matriz
 * @param velocidade Velocidade horizontal do alienígena
 */
void inicializar_alien(Alien *alien, int linha, int coluna, float velocidade){
    alien->x = POSICAO_INICIAL_ALIEN_X + coluna * ESPACAMENTO_ALIEN_X;
    alien->y = POSICAO_INICIAL_ALIEN_Y + linha * ESPACAMENTO_ALIEN_Y;
    alien->velocidade_x = velocidade;
    alien->velocidade_y = ALTURA_ALIEN;
    alien->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
    alien->ativo = 1;
}

/**
 * @brief Inicializa todos os alienígenas do jogo
 * @param jogo Ponteiro para a estrutura do jogo
 */
void inicializar_aliens(Jogo *jogo){
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            inicializar_alien(&jogo->aliens[i][j], i, j, jogo->velocidade_alien);
        }
    }
}

/**
 * @brief Inicializa um tiro com valores padrão
 * @param tiro Ponteiro para a estrutura do tiro
 */
void inicializar_tiro(Tiro *tiro){
    tiro->ativo = 0;
    tiro->cor = al_map_rgb(255, 255, 0); // Cor amarela
}

/**
 * @brief Dispara um tiro da nave do jogador
 * @param tiro Ponteiro para a estrutura do tiro
 * @param nave Estrutura da nave do jogador
 */
void disparar_tiro(Tiro *tiro, Nave nave){
    if(!tiro->ativo){
        tiro->x = nave.x;
        tiro->y = ALTURA_TELA - ALTURA_GRAMA/2 - ALTURA_NAVE;
        tiro->ativo = 1;
    }
}

/**
 * @brief Atualiza a posição do tiro e verifica se saiu da tela
 * @param tiro Ponteiro para a estrutura do tiro
 */
void atualizar_tiro(Tiro *tiro){
    if(tiro->ativo){
        tiro->y -= VELOCIDADE_TIRO;
        // Desativa o tiro quando sai da tela
        if(tiro->y + ALTURA_TIRO < 0){
            tiro->ativo = 0;
        }
    }
}

/**
 * @brief Desenha o tiro na tela
 * @param tiro Estrutura do tiro a ser desenhado
 */
void desenhar_tiro(Tiro tiro){
    if(tiro.ativo){
        al_draw_filled_rectangle(tiro.x - LARGURA_TIRO/2, tiro.y,
                                 tiro.x + LARGURA_TIRO/2, tiro.y + ALTURA_TIRO,
                                 tiro.cor);
    }
}

/**
 * @brief Verifica colisão entre um tiro e um alienígena
 * @param tiro Estrutura do tiro
 * @param alien Estrutura do alienígena
 * @return 1 se há colisão, 0 caso contrário
 */
int verificar_colisao_tiro_alien(Tiro tiro, Alien alien){
    if(!tiro.ativo || !alien.ativo) return 0;
    
    float tiro_x_esquerda = tiro.x - LARGURA_TIRO/2;
    float tiro_x_direita = tiro.x + LARGURA_TIRO/2;
    float tiro_y_topo = tiro.y;
    float tiro_y_base = tiro.y + ALTURA_TIRO;
    
    float alien_x_esquerda = alien.x;
    float alien_x_direita = alien.x + LARGURA_ALIEN;
    float alien_y_topo = alien.y;
    float alien_y_base = alien.y + ALTURA_ALIEN;
    
    return (tiro_x_esquerda < alien_x_direita && 
            tiro_x_direita > alien_x_esquerda && 
            tiro_y_topo < alien_y_base && 
            tiro_y_base > alien_y_topo);
}

/**
 * @brief Verifica colisões entre tiro e alienígenas, destruindo-os e adicionando pontos
 * @param tiro Ponteiro para a estrutura do tiro
 * @param jogo Ponteiro para a estrutura do jogo
 * @param pontuacao Ponteiro para a pontuação atual
 */
void verificar_colisao_tiro_aliens(Tiro *tiro, Jogo *jogo, int *pontuacao){
    if(!tiro->ativo) return;
    
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(verificar_colisao_tiro_alien(*tiro, jogo->aliens[i][j])){
                jogo->aliens[i][j].ativo = 0; // Destrói o alienígena
                tiro->ativo = 0; // Destrói o tiro
                (*pontuacao) += jogo->pontos_por_alien; // Adiciona pontos baseado na dificuldade
                return; // Sai após a primeira colisão
            }
        }
    }
}

/**
 * @brief Desenha um alienígena na tela
 * @param alien Estrutura do alienígena a ser desenhado
 */
void desenhar_alien(Alien alien){
    if(alien.ativo){
        al_draw_filled_rectangle(alien.x, alien.y,
                                 alien.x + LARGURA_ALIEN, alien.y + ALTURA_ALIEN,
                                 alien.cor);
    }
}

/**
 * @brief Desenha todos os alienígenas ativos na tela
 * @param jogo Ponteiro para a estrutura do jogo
 */
void desenhar_aliens(Jogo *jogo){
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            desenhar_alien(jogo->aliens[i][j]);
        }
    }
}

/**
 * @brief Verifica se algum alienígena atingiu os limites da tela
 * @param jogo Ponteiro para a estrutura do jogo
 * @return 1 se algum alien atingiu o limite, 0 caso contrário
 */
int verificar_colisao_limite(Jogo *jogo){
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(jogo->aliens[i][j].ativo){
                if(jogo->aliens[i][j].x + LARGURA_ALIEN + jogo->aliens[i][j].velocidade_x > LARGURA_TELA || 
                   jogo->aliens[i][j].x + jogo->aliens[i][j].velocidade_x < 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * @brief Atualiza a posição de todos os alienígenas ativos
 * @param jogo Ponteiro para a estrutura do jogo
 */
void atualizar_aliens(Jogo *jogo){
    // Verifica se algum alienígena atingiu o limite
    if(verificar_colisao_limite(jogo)){
        // Inverte a direção e desce para todos os alienígenas
        for(int i = 0; i < jogo->linhas_alien; i++){
            for(int j = 0; j < jogo->colunas_alien; j++){
                if(jogo->aliens[i][j].ativo){
                    jogo->aliens[i][j].velocidade_x *= -1;
                    jogo->aliens[i][j].y += jogo->aliens[i][j].velocidade_y;
                }
            }
        }
    }
    
    // Move todos os alienígenas horizontalmente
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(jogo->aliens[i][j].ativo){
                jogo->aliens[i][j].x += jogo->aliens[i][j].velocidade_x;
            }
        }
    }
}

/**
 * @brief Verifica se algum alienígena atingiu o solo (grama)
 * @param jogo Ponteiro para a estrutura do jogo
 * @return 1 se algum alien atingiu o solo, 0 caso contrário
 */
int verificar_colisao_alien_solo(Jogo *jogo) {
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(jogo->aliens[i][j].ativo && jogo->aliens[i][j].y + ALTURA_ALIEN > ALTURA_TELA - ALTURA_GRAMA) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Verifica colisão entre alienígenas e a nave do jogador
 * @param jogo Ponteiro para a estrutura do jogo
 * @param nave Estrutura da nave do jogador
 * @return 1 se há colisão, 0 caso contrário
 */
int verificar_colisao_alien_nave(Jogo *jogo, Nave nave) {
    float nave_y_base = ALTURA_TELA - ALTURA_GRAMA/2;
    float nave_y_topo = nave_y_base - ALTURA_NAVE;
    float nave_y_fundo = nave_y_base;
    float nave_x_esquerda = nave.x - LARGURA_NAVE/2;
    float nave_x_direita = nave.x + LARGURA_NAVE/2;
    
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(jogo->aliens[i][j].ativo){
                // Verifica se o retângulo do alienígena sobrepõe a área triangular da nave
                float alien_x_esquerda = jogo->aliens[i][j].x;
                float alien_x_direita = jogo->aliens[i][j].x + LARGURA_ALIEN;
                float alien_y_topo = jogo->aliens[i][j].y;
                float alien_y_fundo = jogo->aliens[i][j].y + ALTURA_ALIEN;
                
                // Verificação simples de colisão retangular
                if(alien_x_esquerda < nave_x_direita && 
                   alien_x_direita > nave_x_esquerda && 
                   alien_y_topo < nave_y_fundo && 
                   alien_y_fundo > nave_y_topo) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * @brief Verifica se todos os alienígenas foram destruídos
 * @param jogo Ponteiro para a estrutura do jogo
 * @return 1 se todos os aliens foram destruídos, 0 caso contrário
 */
int todos_aliens_destruidos(Jogo *jogo) {
    for(int i = 0; i < jogo->linhas_alien; i++){
        for(int j = 0; j < jogo->colunas_alien; j++){
            if(jogo->aliens[i][j].ativo){
                return 0; // Encontrou um alienígena ativo
            }
        }
    }
    return 1; // Todos os alienígenas foram destruídos
}

/**
 * @brief Desenha o cenário do jogo (fundo preto e grama verde)
 */
void desenhar_cenario(){
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_filled_rectangle(0, ALTURA_TELA - ALTURA_GRAMA, LARGURA_TELA, ALTURA_TELA, al_map_rgb(0, 255, 0));
}

/**
 * @brief Desenha a nave do jogador na tela
 * @param nave Estrutura da nave a ser desenhada
 */
void desenhar_nave(Nave nave){
    float y_base = ALTURA_TELA - ALTURA_GRAMA/2;
    al_draw_filled_triangle(nave.x, y_base - ALTURA_NAVE, 
                            nave.x - LARGURA_NAVE/2, y_base, 
                            nave.x + LARGURA_NAVE/2, y_base,
                            nave.cor);
}

/**
 * @brief Desenha a pontuação atual na tela
 * @param pontuacao Pontuação atual do jogador
 * @param fonte Fonte para renderizar o texto
 */
void desenhar_pontuacao(int pontuacao, ALLEGRO_FONT *fonte){
    char texto_pontuacao[50];
    sprintf(texto_pontuacao, "Pontuação: %d", pontuacao);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), 10, ALTURA_TELA - ALTURA_GRAMA - 40, 
                 ALLEGRO_ALIGN_LEFT, texto_pontuacao);
}

/**
 * @brief Atualiza a posição da nave baseada nas teclas pressionadas
 * @param nave Ponteiro para a estrutura da nave
 */
void atualizar_nave(Nave *nave){
    if(nave->direita && nave->x + nave->velocidade <= LARGURA_TELA) {
        nave->x += nave->velocidade;
    }
    if(nave->esquerda && nave->x - nave->velocidade >= 0) {
        nave->x -= nave->velocidade;
    }
}

/**
 * @brief Inicializa o jogo com base na dificuldade selecionada
 * @param jogo Ponteiro para a estrutura do jogo
 * @param dificuldade Nível de dificuldade escolhido
 */
void inicializar_jogo(Jogo *jogo, Dificuldade dificuldade){
    switch(dificuldade){
        case DIFICULDADE_FACIL:
            jogo->linhas_alien = 4;
            jogo->colunas_alien = 5;
            jogo->velocidade_alien = 1;
            jogo->pontos_por_alien = 10;
            break;
        case DIFICULDADE_NORMAL:
            jogo->linhas_alien = 5;
            jogo->colunas_alien = 5;
            jogo->velocidade_alien = 2;
            jogo->pontos_por_alien = 15;
            break;
        case DIFICULDADE_DIFICIL:
            jogo->linhas_alien = 5;
            jogo->colunas_alien = 6;
            jogo->velocidade_alien = 3;
            jogo->pontos_por_alien = 20;
            break;
    }
    inicializar_aliens(jogo);
}

/**
 * @brief Desenha o menu principal do jogo
 * @param fonte Fonte para renderizar o texto
 * @param dificuldade_atual Dificuldade atualmente selecionada
 */
void desenhar_menu(ALLEGRO_FONT *fonte, Dificuldade dificuldade_atual){
    al_clear_to_color(al_map_rgb(0,0,0));
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 100, 
                 ALLEGRO_ALIGN_CENTER, "SPACE INVADERS");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 200, 
                 ALLEGRO_ALIGN_CENTER, "1 - Iniciar Jogo");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 250, 
                 ALLEGRO_ALIGN_CENTER, "2 - Alterar Dificuldade");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 300, 
                 ALLEGRO_ALIGN_CENTER, "3 - Sair");
    
    char texto_dificuldade[50];
    switch(dificuldade_atual){
        case DIFICULDADE_FACIL:
            sprintf(texto_dificuldade, "Dificuldade Atual: FÁCIL");
            break;
        case DIFICULDADE_NORMAL:
            sprintf(texto_dificuldade, "Dificuldade Atual: NORMAL");
            break;
        case DIFICULDADE_DIFICIL:
            sprintf(texto_dificuldade, "Dificuldade Atual: DIFÍCIL");
            break;
    }
    
    al_draw_text(fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, 350, 
                 ALLEGRO_ALIGN_CENTER, texto_dificuldade);
    
    // Mostra o recorde atual
    int recorde_atual = ler_recorde();
    char texto_recorde[100];
    sprintf(texto_recorde, "Recorde: %d", recorde_atual);
    al_draw_text(fonte, al_map_rgb(0, 255, 255), LARGURA_TELA/2, 400, 
                 ALLEGRO_ALIGN_CENTER, texto_recorde);
    
    al_flip_display();
}

/**
 * @brief Desenha o menu de seleção de dificuldade
 * @param fonte Fonte para renderizar o texto
 */
void desenhar_menu_dificuldade(ALLEGRO_FONT *fonte){
    al_clear_to_color(al_map_rgb(0,0,0));
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 100, 
                 ALLEGRO_ALIGN_CENTER, "SELECIONAR DIFICULDADE");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 200, 
                 ALLEGRO_ALIGN_CENTER, "1 - FÁCIL (4x5 aliens, vel=1, 10pts)");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 250, 
                 ALLEGRO_ALIGN_CENTER, "2 - NORMAL (5x5 aliens, vel=2, 15pts)");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 300, 
                 ALLEGRO_ALIGN_CENTER, "3 - DIFÍCIL (5x6 aliens, vel=3, 20pts)");
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 350, 
                 ALLEGRO_ALIGN_CENTER, "0 - Voltar");
    
    al_flip_display();
}

/**
 * @brief Lê o recorde salvo no arquivo
 * @return O recorde atual ou 0 se não existir arquivo
 */
int ler_recorde(void){
    FILE *arquivo = fopen("recorde.txt", "r");
    int recorde = 0;
    
    if(arquivo != NULL){
        fscanf(arquivo, "%d", &recorde);
        fclose(arquivo);
    }
    
    return recorde;
}

/**
 * @brief Salva um novo recorde no arquivo
 * @param novo_recorde Novo recorde a ser salvo
 */
void salvar_recorde(int novo_recorde){
    FILE *arquivo = fopen("recorde.txt", "w");
    
    if(arquivo != NULL){
        fprintf(arquivo, "%d", novo_recorde);
        fclose(arquivo);
    }
}

/**
 * @brief Mostra a tela de resultado final do jogo
 * @param fonte Fonte para renderizar o texto
 * @param pontuacao Pontuação final do jogador
 * @param vitoria Flag indicando se o jogador venceu (1) ou perdeu (0)
 */
void mostrar_resultado_final(ALLEGRO_FONT *fonte, int pontuacao, int vitoria){
    int recorde_atual = ler_recorde();
    int novo_recorde = 0;
    
    // Verifica se a pontuação atual é maior que o recorde
    if(pontuacao > recorde_atual){
        novo_recorde = 1;
        salvar_recorde(pontuacao);
        recorde_atual = pontuacao;
    }
    
    al_clear_to_color(al_map_rgb(0,0,0));
    
    if(vitoria){
        al_draw_text(fonte, al_map_rgb(0, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 80, 
                     ALLEGRO_ALIGN_CENTER, "VITÓRIA! Todos os aliens destruídos!");
    } else {
        al_draw_text(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 80, 
                     ALLEGRO_ALIGN_CENTER, "GAME OVER!");
    }
    
    char texto_pontuacao[100];
    sprintf(texto_pontuacao, "Pontuação Final: %d", pontuacao);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 40, 
                 ALLEGRO_ALIGN_CENTER, texto_pontuacao);
    
    char texto_recorde[100];
    sprintf(texto_recorde, "Recorde: %d", recorde_atual);
    al_draw_text(fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2, 
                 ALLEGRO_ALIGN_CENTER, texto_recorde);
    
    if(novo_recorde){
        al_draw_text(fonte, al_map_rgb(0, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 40, 
                     ALLEGRO_ALIGN_CENTER, "NOVO RECORDE!");
    }
    
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 80, 
                 ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar ao menu");
    
    al_flip_display();
}

/**
 * @brief Função principal do programa
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int main(void){
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
	ALLEGRO_TIMER *temporizador = NULL;
	ALLEGRO_FONT *fonte = NULL;
   
	//----------------------- Rotinas de inicialização ---------------------------------------
    
	// Inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "Falha ao inicializar o Allegro!\n");
		return -1;
	}
	
    // Inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "Falha ao inicializar primitivas!\n");
        return -1;
    }	
	
	// Inicializa o módulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "Falha ao inicializar módulo de imagem!\n");
		return -1;
	}

	// Inicializa o módulo allegro que carrega as fontes
	al_init_font_addon();

	// Inicializa o módulo allegro que entende arquivos ttf de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "Falha ao carregar módulo de fonte ttf!\n");
		return -1;
	}
	
	// Cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    temporizador = al_create_timer(1.0 / FPS);
    if(!temporizador) {
		fprintf(stderr, "Falha ao criar temporizador!\n");
		return -1;
	}
 
	// Cria uma tela com dimensões de LARGURA_TELA, ALTURA_TELA pixels
	display = al_create_display(LARGURA_TELA, ALTURA_TELA);
	if(!display) {
		fprintf(stderr, "Falha ao criar display!\n");
		al_destroy_timer(temporizador);
		return -1;
	}

	// Carrega o arquivo arial.ttf da fonte Arial e define que será usado o tamanho 32
    fonte = al_load_font("arial.ttf", 32, 1);   
	if(fonte == NULL) {
		fprintf(stderr, "Arquivo de fonte não existe ou não pode ser acessado!\n");
	}

 	// Cria a fila de eventos
	fila_eventos = al_create_event_queue();
	if(!fila_eventos) {
		fprintf(stderr, "Falha ao criar fila de eventos!\n");
		al_destroy_display(display);
		al_destroy_timer(temporizador);
		return -1;
	}
   
	// Instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "Falha ao instalar teclado!\n");
		return -1;
	}
	
	// Instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "Falha ao inicializar mouse!\n");
		return -1;
	}

	// Registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	// Registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	// Registra na fila os eventos de mouse (ex: clicar em um botão do mouse)
	al_register_event_source(fila_eventos, al_get_mouse_event_source());  
    // Registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(fila_eventos, al_get_timer_event_source(temporizador));

    Dificuldade dificuldade_atual = DIFICULDADE_FACIL;
    int jogando = 1;

    // Loop principal do jogo
    while(jogando) {
        int no_menu = 1;
        int no_dificuldade = 0;

        // Loop do menu
        while(no_menu) {
            if(no_dificuldade) {
                desenhar_menu_dificuldade(fonte);
            } else {
                desenhar_menu(fonte, dificuldade_atual);
            }

            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                no_menu = 0;
                jogando = 0;
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                if(no_dificuldade) {
                    switch(evento.keyboard.keycode){
                        case ALLEGRO_KEY_1:
                            dificuldade_atual = DIFICULDADE_FACIL;
                            no_dificuldade = 0;
                            break;
                        case ALLEGRO_KEY_2:
                            dificuldade_atual = DIFICULDADE_NORMAL;
                            no_dificuldade = 0;
                            break;
                        case ALLEGRO_KEY_3:
                            dificuldade_atual = DIFICULDADE_DIFICIL;
                            no_dificuldade = 0;
                            break;
                        case ALLEGRO_KEY_0:
                            no_dificuldade = 0;
                            break;
                    }
                } else {
                    switch(evento.keyboard.keycode){
                        case ALLEGRO_KEY_1:
                            no_menu = 0; // Iniciar jogo
                            break;
                        case ALLEGRO_KEY_2:
                            no_dificuldade = 1;
                            break;
                        case ALLEGRO_KEY_3:
                            no_menu = 0; // Sair
                            jogando = 0;
                            break;
                    }
                }
            }
        }

        if(jogando) { // Iniciar jogo
            Nave nave;
            inicializar_nave(&nave);

            Jogo jogo;
            inicializar_jogo(&jogo, dificuldade_atual);

            Tiro tiro;
            inicializar_tiro(&tiro);

            int pontuacao = 0;
            int vitoria = 0;
            int jogando_partida = 1;

            // Inicia o temporizador
            al_start_timer(temporizador);
            
            // Loop principal da partida
            // NOTA: Usa al_get_next_event() em vez de al_wait_for_event() para evitar
            // bloqueio quando a janela perde o foco, garantindo responsividade consistente
            while(jogando_partida) {
                ALLEGRO_EVENT evento;
                bool redraw = false;
                
                // Processa todos os eventos pendentes sem bloquear
                // Isso garante que os controles respondam imediatamente mesmo quando
                // a janela não está em foco
                while(al_get_next_event(fila_eventos, &evento)) {
                    if(evento.type == ALLEGRO_EVENT_TIMER) {
                        redraw = true;
                    }
                    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                        jogando_partida = 0;
                        jogando = 0;
                        break;
                    }
                    else if(evento.type == ALLEGRO_EVENT_KEY_DOWN) { 
                        switch(evento.keyboard.keycode){
                            case ALLEGRO_KEY_A:
                                nave.esquerda = 1;
                                break;
                            case ALLEGRO_KEY_D:
                                nave.direita = 1;
                                break;
                            case ALLEGRO_KEY_SPACE:
                                disparar_tiro(&tiro, nave);
                                break;
                        }
                    }
                    else if(evento.type == ALLEGRO_EVENT_KEY_UP) { 
                        switch(evento.keyboard.keycode){
                            case ALLEGRO_KEY_A:
                                nave.esquerda = 0;
                                break;
                            case ALLEGRO_KEY_D:
                                nave.direita = 0;
                                break;
                        }
                    }
                }
                
                // Só atualiza e desenha se o temporizador disparou
                // Isso mantém a taxa de quadros consistente
                if(redraw) {
                    desenhar_cenario();
                    atualizar_nave(&nave);
                    atualizar_aliens(&jogo);
                    atualizar_tiro(&tiro);
                    desenhar_nave(nave);
                    desenhar_aliens(&jogo);
                    desenhar_tiro(tiro);
                    verificar_colisao_tiro_aliens(&tiro, &jogo, &pontuacao);
                    desenhar_pontuacao(pontuacao, fonte);
                    al_flip_display();
                }

                // Verifica vitória ou derrota
                if(todos_aliens_destruidos(&jogo)){
                    vitoria = 1;
                    jogando_partida = 0;
                }
                if(verificar_colisao_alien_solo(&jogo) || verificar_colisao_alien_nave(&jogo, nave)){
                    jogando_partida = 0;
                }
                
                // Pequena pausa para não consumir 100% da CPU
                al_rest(0.001);
            }

            // Mostra mensagem de fim de jogo
            mostrar_resultado_final(fonte, pontuacao, vitoria);
            
            // Aguarda pressionamento de tecla
            ALLEGRO_EVENT evento;
            do {
                al_wait_for_event(fila_eventos, &evento);
                if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    jogando = 0;
                    break;
                }
            } while(evento.type != ALLEGRO_EVENT_KEY_DOWN);
        }
    }
     
	// Procedimentos de fim de jogo (fecha a tela, limpa a memória, etc)
	al_destroy_timer(temporizador);
	al_destroy_display(display);
	al_destroy_event_queue(fila_eventos);
	al_destroy_font(fonte);
   
	return 0;
}
