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

const int ALIEN_ROWS = 5;
const int ALIEN_COLS = 5;
const int ALIEN_SPACING_X = 80;
const int ALIEN_SPACING_Y = 55;
const int ALIEN_START_X = 0;
const int ALIEN_START_Y = 0;

const int TIRO_W = 4;
const int TIRO_H = 10;
const float TIRO_VEL = 8;

const float FPS = 60;

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


void init_nave(Nave *nave){
    nave->x = SCREEN_W/2;
    nave->vel = 1;
    nave->dir = 0;
    nave->esq = 0;
    nave->cor = al_map_rgb(0, 0, 255);
}

void init_alien(Alien *alien, int row, int col){
    alien->x = ALIEN_START_X + col * ALIEN_SPACING_X;
    alien->y = ALIEN_START_Y + row * ALIEN_SPACING_Y;
    alien->x_vel = 5;
    alien->y_vel = ALIEN_H;
    alien->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
    alien->ativo = 1;
}

void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]){
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            init_alien(&aliens[i][j], i, j);
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

void check_tiro_aliens_collision(Tiro *tiro, Alien aliens[ALIEN_ROWS][ALIEN_COLS], int *pontuacao){
    if(!tiro->ativo) return;
    
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            if(colisao_tiro_alien(*tiro, aliens[i][j])){
                aliens[i][j].ativo = 0; // Destroy alien
                tiro->ativo = 0; // Destroy shot
                (*pontuacao) += 10; // Add 10 points for each alien destroyed
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

void draw_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]){
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            draw_alien(aliens[i][j]);
        }
    }
}

int check_boundary_collision(Alien aliens[ALIEN_ROWS][ALIEN_COLS]){
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            if(aliens[i][j].ativo){
                if(aliens[i][j].x + ALIEN_W + aliens[i][j].x_vel > SCREEN_W || 
                   aliens[i][j].x + aliens[i][j].x_vel < 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]){
    // Check if any alien hits the boundary
    if(check_boundary_collision(aliens)){
        // Reverse direction and descend for all aliens
        for(int i = 0; i < ALIEN_ROWS; i++){
            for(int j = 0; j < ALIEN_COLS; j++){
                if(aliens[i][j].ativo){
                    aliens[i][j].x_vel *= -1;
                    aliens[i][j].y += aliens[i][j].y_vel;
                }
            }
        }
    }
    
    // Move all aliens horizontally
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            if(aliens[i][j].ativo){
                aliens[i][j].x += aliens[i][j].x_vel;
            }
        }
    }
}

int colisao_alien_solo(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            if(aliens[i][j].ativo && aliens[i][j].y + ALIEN_H > SCREEN_H - GRASS_H) {
                return 1;
            }
        }
    }
    return 0;
}

int colisao_alien_nave(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Nave nave) {
    float nave_y_base = SCREEN_H - GRASS_H/2;
    float nave_y_top = nave_y_base - NAVE_H;
    float nave_y_bottom = nave_y_base;
    float nave_x_left = nave.x - NAVE_W/2;
    float nave_x_right = nave.x + NAVE_W/2;
    
    for(int i = 0; i < ALIEN_ROWS; i++){
        for(int j = 0; j < ALIEN_COLS; j++){
            if(aliens[i][j].ativo){
                // Check if alien rectangle overlaps with ship triangle area
                float alien_x_left = aliens[i][j].x;
                float alien_x_right = aliens[i][j].x + ALIEN_W;
                float alien_y_top = aliens[i][j].y;
                float alien_y_bottom = aliens[i][j].y + ALIEN_H;
                
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

    Nave nave;
    init_nave(&nave);

    Alien aliens[ALIEN_ROWS][ALIEN_COLS];
    init_aliens(aliens);

    Tiro tiro;
    init_tiro(&tiro);

    int pontuacao = 0;

    int playing = 1;
	//inicia o temporizador
	al_start_timer(timer);
	while(playing) 

	{
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

            draw_scenario();

            update_nave(&nave);

            update_aliens(aliens);
            
            update_tiro(&tiro);

            draw_nave(nave);

            draw_aliens(aliens);
            playing = !colisao_alien_solo(aliens) && !colisao_alien_nave(aliens, nave);

            draw_tiro(tiro);
            check_tiro_aliens_collision(&tiro, aliens, &pontuacao);

            draw_pontuacao(pontuacao, font);

            //atualiza a tela (quando houver algo para mostrar)
            al_flip_display();

            if(al_get_timer_count(timer)%(int)FPS == 0)
                printf("\n%d segundos se passaram...", (int)al_get_timer_count(timer)/(int)FPS);

			//limpa a tela
			//al_clear_to_color(al_map_rgb(0,0,0));

			//aplica fisica
			//desenha

			//atualiza a tela (quando houver algo para mostrar)
			//al_flip_display();
			
		}

		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { 
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

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
        //se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) { 
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

            switch(ev.keyboard.keycode){
                
                case ALLEGRO_KEY_A:
                    nave.esq = 0;
                break;

                case ALLEGRO_KEY_D:
                    nave.dir = 0;
                break;
            
            }
		}
	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(font);
   
 
	return 0;
}