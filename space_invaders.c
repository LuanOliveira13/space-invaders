#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int GRASS_H = 60;

const int NAVE_W = 100;
const int NAVE_H = 50;

const int ALIEN_W = 50;
const int ALIEN_H = 25;

const int ALIEN_SPACING_X = 80;  // 50 (alien width) + 30 (space) = 80
const int ALIEN_SPACING_Y = 55;  // 25 (alien height) + 30 (space) = 55
const int ALIEN_START_X = 0;
const int ALIEN_START_Y = 0;

const int TIRO_W = 4;
const int TIRO_H = 10;
const float TIRO_VEL = 8;

const float FPS = 60;

typedef enum {
    DIFICULDADE_FACIL,
    DIFICULDADE_NORMAL,
    DIFICULDADE_DIFICIL
} Dificuldade;

typedef struct Nave{
    float x;
    float vel;
    int dir, esq;
    ALLEGRO_COLOR cor;
} Nave;

typedef struct Alien{
    float x, y;
    float x_vel, y_vel;
    ALLEGRO_COLOR cor;
    int ativo;
} Alien;

typedef struct Tiro{
    float x, y;
    int ativo;
    ALLEGRO_COLOR cor;
} Tiro;

typedef struct Jogo{
    int alien_rows;
    int alien_cols;
    float alien_vel;
    int pontos_por_alien;
    Alien aliens[6][6]; // Max size for hard difficulty
} Jogo;


void init_nave(Nave *nave){
    nave->x = SCREEN_W/2;
    nave->vel = 1;
    nave->dir = 0;
    nave->esq = 0;
    nave->cor = al_map_rgb(0, 0, 255);
}

void init_alien(Alien *alien, int row, int col, float vel){
    alien->x = ALIEN_START_X + col * ALIEN_SPACING_X;
    alien->y = ALIEN_START_Y + row * ALIEN_SPACING_Y;
    alien->x_vel = vel;
    alien->y_vel = ALIEN_H;
    alien->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
    alien->ativo = 1;
}

void init_aliens(Jogo *jogo){
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            init_alien(&jogo->aliens[i][j], i, j, jogo->alien_vel);
        }
    }
}

void init_tiro(Tiro *tiro){
    tiro->ativo = 0;
    tiro->cor = al_map_rgb(255, 255, 0); // Yellow color
}

void disparar_tiro(Tiro *tiro, Nave nave){
    if(!tiro->ativo){
        tiro->x = nave.x;
        tiro->y = SCREEN_H - GRASS_H/2 - NAVE_H;
        tiro->ativo = 1;
    }
}

void update_tiro(Tiro *tiro){
    if(tiro->ativo){
        tiro->y -= TIRO_VEL;
        // Deactivate shot when it goes off screen
        if(tiro->y + TIRO_H < 0){
            tiro->ativo = 0;
        }
    }
}

void draw_tiro(Tiro tiro){
    if(tiro.ativo){
        al_draw_filled_rectangle(tiro.x - TIRO_W/2, tiro.y,
                                 tiro.x + TIRO_W/2, tiro.y + TIRO_H,
                                 tiro.cor);
    }
}

int colisao_tiro_alien(Tiro tiro, Alien alien){
    if(!tiro.ativo || !alien.ativo) return 0;
    
    float tiro_x_left = tiro.x - TIRO_W/2;
    float tiro_x_right = tiro.x + TIRO_W/2;
    float tiro_y_top = tiro.y;
    float tiro_y_bottom = tiro.y + TIRO_H;
    
    float alien_x_left = alien.x;
    float alien_x_right = alien.x + ALIEN_W;
    float alien_y_top = alien.y;
    float alien_y_bottom = alien.y + ALIEN_H;
    
    return (tiro_x_left < alien_x_right && 
            tiro_x_right > alien_x_left && 
            tiro_y_top < alien_y_bottom && 
            tiro_y_bottom > alien_y_top);
}

void check_tiro_aliens_collision(Tiro *tiro, Jogo *jogo, int *pontuacao){
    if(!tiro->ativo) return;
    
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(colisao_tiro_alien(*tiro, jogo->aliens[i][j])){
                jogo->aliens[i][j].ativo = 0; // Destroy alien
                tiro->ativo = 0; // Destroy shot
                (*pontuacao) += jogo->pontos_por_alien; // Add points based on difficulty
                return; // Exit after first collision
            }
        }
    }
}

void draw_alien(Alien alien){
    if(alien.ativo){
        al_draw_filled_rectangle(alien.x, alien.y,
                                 alien.x + ALIEN_W, alien.y + ALIEN_H,
                                 alien.cor);
    }
}

void draw_aliens(Jogo *jogo){
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            draw_alien(jogo->aliens[i][j]);
        }
    }
}

int check_boundary_collision(Jogo *jogo){
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(jogo->aliens[i][j].ativo){
                if(jogo->aliens[i][j].x + ALIEN_W + jogo->aliens[i][j].x_vel > SCREEN_W || 
                   jogo->aliens[i][j].x + jogo->aliens[i][j].x_vel < 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void update_aliens(Jogo *jogo){
    // Check if any alien hits the boundary
    if(check_boundary_collision(jogo)){
        // Reverse direction and descend for all aliens
        for(int i = 0; i < jogo->alien_rows; i++){
            for(int j = 0; j < jogo->alien_cols; j++){
                if(jogo->aliens[i][j].ativo){
                    jogo->aliens[i][j].x_vel *= -1;
                    jogo->aliens[i][j].y += jogo->aliens[i][j].y_vel;
                }
            }
        }
    }
    
    // Move all aliens horizontally
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(jogo->aliens[i][j].ativo){
                jogo->aliens[i][j].x += jogo->aliens[i][j].x_vel;
            }
        }
    }
}

int colisao_alien_solo(Jogo *jogo) {
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(jogo->aliens[i][j].ativo && jogo->aliens[i][j].y + ALIEN_H > SCREEN_H - GRASS_H) {
                return 1;
            }
        }
    }
    return 0;
}

int colisao_alien_nave(Jogo *jogo, Nave nave) {
    float nave_y_base = SCREEN_H - GRASS_H/2;
    float nave_y_top = nave_y_base - NAVE_H;
    float nave_y_bottom = nave_y_base;
    float nave_x_left = nave.x - NAVE_W/2;
    float nave_x_right = nave.x + NAVE_W/2;
    
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(jogo->aliens[i][j].ativo){
                // Check if alien rectangle overlaps with ship triangle area
                float alien_x_left = jogo->aliens[i][j].x;
                float alien_x_right = jogo->aliens[i][j].x + ALIEN_W;
                float alien_y_top = jogo->aliens[i][j].y;
                float alien_y_bottom = jogo->aliens[i][j].y + ALIEN_H;
                
                // Simple rectangle collision check
                if(alien_x_left < nave_x_right && 
                   alien_x_right > nave_x_left && 
                   alien_y_top < nave_y_bottom && 
                   alien_y_bottom > nave_y_top) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int todos_aliens_destruidos(Jogo *jogo) {
    for(int i = 0; i < jogo->alien_rows; i++){
        for(int j = 0; j < jogo->alien_cols; j++){
            if(jogo->aliens[i][j].ativo){
                return 0; // Found an active alien
            }
        }
    }
    return 1; // All aliens are destroyed
}

void draw_scenario(){
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(0, 255, 0));
}

void draw_nave(Nave nave){
    float y_base = SCREEN_H - GRASS_H/2;
    al_draw_filled_triangle(nave.x, y_base - NAVE_H, 
                            nave.x - NAVE_W/2, y_base, 
                            nave.x + NAVE_W/2, y_base,
                            nave.cor);
}

void draw_pontuacao(int pontuacao, ALLEGRO_FONT *font){
    char score_text[50];
    sprintf(score_text, "Score: %d", pontuacao);
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, SCREEN_H - GRASS_H - 40, 
                 ALLEGRO_ALIGN_LEFT, score_text);
}

void update_nave(Nave *nave){
    if(nave->dir && nave->x + nave->vel <= SCREEN_W) {
        nave->x += nave->vel;
    }
    if(nave->esq && nave->x - nave->vel >= 0) {
        nave->x -= nave->vel;
    }
}

void init_jogo(Jogo *jogo, Dificuldade dificuldade){
    switch(dificuldade){
        case DIFICULDADE_FACIL:
            jogo->alien_rows = 4;
            jogo->alien_cols = 5;
            jogo->alien_vel = 1;
            jogo->pontos_por_alien = 10;
            break;
        case DIFICULDADE_NORMAL:
            jogo->alien_rows = 5;
            jogo->alien_cols = 5;
            jogo->alien_vel = 2;
            jogo->pontos_por_alien = 15;
            break;
        case DIFICULDADE_DIFICIL:
            jogo->alien_rows = 5;
            jogo->alien_cols = 6;
            jogo->alien_vel = 3;
            jogo->pontos_por_alien = 20;
            break;
    }
    init_aliens(jogo);
}

void draw_menu(ALLEGRO_FONT *font, Dificuldade dificuldade_atual){
    al_clear_to_color(al_map_rgb(0,0,0));
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 100, 
                 ALLEGRO_ALIGN_CENTER, "SPACE INVADERS");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 200, 
                 ALLEGRO_ALIGN_CENTER, "1 - Iniciar Jogo");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 250, 
                 ALLEGRO_ALIGN_CENTER, "2 - Alterar Dificuldade");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 300, 
                 ALLEGRO_ALIGN_CENTER, "3 - Sair");
    
    char dificuldade_text[50];
    switch(dificuldade_atual){
        case DIFICULDADE_FACIL:
            sprintf(dificuldade_text, "Dificuldade Atual: FACIL");
            break;
        case DIFICULDADE_NORMAL:
            sprintf(dificuldade_text, "Dificuldade Atual: NORMAL");
            break;
        case DIFICULDADE_DIFICIL:
            sprintf(dificuldade_text, "Dificuldade Atual: DIFICIL");
            break;
    }
    
    al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_W/2, 350, 
                 ALLEGRO_ALIGN_CENTER, dificuldade_text);
    
    // Show current high score
    int recorde_atual = ler_recorde();
    char recorde_text[100];
    sprintf(recorde_text, "High Score: %d", recorde_atual);
    al_draw_text(font, al_map_rgb(0, 255, 255), SCREEN_W/2, 400, 
                 ALLEGRO_ALIGN_CENTER, recorde_text);
    
    al_flip_display();
}

void draw_dificuldade_menu(ALLEGRO_FONT *font){
    al_clear_to_color(al_map_rgb(0,0,0));
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 100, 
                 ALLEGRO_ALIGN_CENTER, "SELECIONAR DIFICULDADE");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 200, 
                 ALLEGRO_ALIGN_CENTER, "1 - FACIL (4x5 aliens, vel=1, 10pts)");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 250, 
                 ALLEGRO_ALIGN_CENTER, "2 - NORMAL (5x5 aliens, vel=2, 15pts)");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 300, 
                 ALLEGRO_ALIGN_CENTER, "3 - DIFICIL (5x6 aliens, vel=3, 20pts)");
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 350, 
                 ALLEGRO_ALIGN_CENTER, "0 - Voltar");
    
    al_flip_display();
}

int ler_recorde(){
    FILE *arquivo = fopen("recorde.txt", "r");
    int recorde = 0;
    
    if(arquivo != NULL){
        fscanf(arquivo, "%d", &recorde);
        fclose(arquivo);
    }
    
    return recorde;
}

void salvar_recorde(int novo_recorde){
    FILE *arquivo = fopen("recorde.txt", "w");
    
    if(arquivo != NULL){
        fprintf(arquivo, "%d", novo_recorde);
        fclose(arquivo);
    }
}

void mostrar_resultado_final(ALLEGRO_FONT *font, int pontuacao, int vitoria){
    int recorde_atual = ler_recorde();
    int novo_recorde = 0;
    
    // Check if current score is higher than record
    if(pontuacao > recorde_atual){
        novo_recorde = 1;
        salvar_recorde(pontuacao);
        recorde_atual = pontuacao;
    }
    
    al_clear_to_color(al_map_rgb(0,0,0));
    
    if(vitoria){
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_W/2, SCREEN_H/2 - 80, 
                     ALLEGRO_ALIGN_CENTER, "VICTORY! All aliens destroyed!");
    } else {
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W/2, SCREEN_H/2 - 80, 
                     ALLEGRO_ALIGN_CENTER, "GAME OVER!");
    }
    
    char score_text[100];
    sprintf(score_text, "Final Score: %d", pontuacao);
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2 - 40, 
                 ALLEGRO_ALIGN_CENTER, score_text);
    
    char recorde_text[100];
    sprintf(recorde_text, "High Score: %d", recorde_atual);
    al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_W/2, SCREEN_H/2, 
                 ALLEGRO_ALIGN_CENTER, recorde_text);
    
    if(novo_recorde){
        al_draw_text(font, al_map_rgb(0, 255, 255), SCREEN_W/2, SCREEN_H/2 + 40, 
                     ALLEGRO_ALIGN_CENTER, "NEW RECORD!");
    }
    
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2 + 80, 
                 ALLEGRO_ALIGN_CENTER, "Press any key to return to menu");
    
    al_flip_display();
}

int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    font = al_load_font("arial.ttf", 32, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  
    //registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

    Dificuldade dificuldade_atual = DIFICULDADE_FACIL;
    int jogando = 1;

    while(jogando) {
        int no_menu = 1;
        int no_dificuldade = 0;

        while(no_menu) {
            if(no_dificuldade) {
                draw_dificuldade_menu(font);
            } else {
                draw_menu(font, dificuldade_atual);
            }

            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);

            if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                no_menu = 0;
                jogando = 0;
            }
            else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if(no_dificuldade) {
                    switch(ev.keyboard.keycode){
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
                    switch(ev.keyboard.keycode){
                        case ALLEGRO_KEY_1:
                            no_menu = 0; // Start game
                            break;
                        case ALLEGRO_KEY_2:
                            no_dificuldade = 1;
                            break;
                        case ALLEGRO_KEY_3:
                            no_menu = 0; // Exit
                            jogando = 0;
                            break;
                    }
                }
            }
        }

        if(jogando) { // Start game
            Nave nave;
            init_nave(&nave);

            Jogo jogo;
            init_jogo(&jogo, dificuldade_atual);

            Tiro tiro;
            init_tiro(&tiro);

            int pontuacao = 0;
            int vitoria = 0;
            int playing = 1;

            //inicia o temporizador
            al_start_timer(timer);
            while(playing) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);

                if(ev.type == ALLEGRO_EVENT_TIMER) {
                    draw_scenario();
                    update_nave(&nave);
                    update_aliens(&jogo);
                    update_tiro(&tiro);
                    draw_nave(nave);
                    draw_aliens(&jogo);
                    draw_tiro(tiro);
                    check_tiro_aliens_collision(&tiro, &jogo, &pontuacao);
                    draw_pontuacao(pontuacao, font);
                    al_flip_display();
                }
                else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    playing = 0;
                    jogando = 0;
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { 
                    switch(ev.keyboard.keycode){
                        case ALLEGRO_KEY_A:
                            nave.esq = 1;
                            break;
                        case ALLEGRO_KEY_D:
                            nave.dir = 1;
                            break;
                        case ALLEGRO_KEY_SPACE:
                            disparar_tiro(&tiro, nave);
                            break;
                    }
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_UP) { 
                    switch(ev.keyboard.keycode){
                        case ALLEGRO_KEY_A:
                            nave.esq = 0;
                            break;
                        case ALLEGRO_KEY_D:
                            nave.dir = 0;
                            break;
                    }
                }

                // Check for victory or defeat
                if(todos_aliens_destruidos(&jogo)){
                    vitoria = 1;
                    playing = 0;
                }
                if(colisao_alien_solo(&jogo) || colisao_alien_nave(&jogo, nave)){
                    playing = 0;
                }
            }

            // Show end game message
            mostrar_resultado_final(font, pontuacao, vitoria);
            
            // Wait for key press
            ALLEGRO_EVENT ev;
            do {
                al_wait_for_event(event_queue, &ev);
                if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    jogando = 0;
                    break;
                }
            } while(ev.type != ALLEGRO_EVENT_KEY_DOWN);
        }
    }
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(font);
   
	return 0;
}