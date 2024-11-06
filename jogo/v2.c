#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Para getch() e kbhit() no Windows
#include <time.h> // Para gerar números aleatórios
#include <windows.h> // Para Sleep() no Windows


// Configurações de tela 
#define MAX_tela_X 50  
#define MAX_tela_y 16
#define MAX_margem 30

// Configurações de jogo
#define forma_jogador '^'
#define forma_tiro '|'
#define forma_tiro_monstro '!'
#define max_tiros 2
#define monstro_1 'M'
#define monstro_2 'H'
#define monstro_3 'W'
#define monstro_4  'X'
#define MAX_monstro 10 // Máximo de monstros por linha
#define max_tiro_monstro 3
#define ATRASO_TIQUE 1

//config barreiras
#define barreira_forma_1 '#'
#define barreira_forma_2 '*'
#define barreira_forma_3 '-'
#define max_barreira 49
#define max_grupo 4
#define max_barreiras_por_grupo 8
#define  max_espaco_grupo 4

int ponto = 0;
int vida = 1;
int direcao = 1;
int direcao2 = 1;
int tempo_monstro = 0;
int tempo_monstro_especial = 0;
int tempo_tiro_monstro = 0;
int velocidade = 1;
int velocidade1=0;
int level = 1;
int contador;
int pontuacao_maxima = 0;
char move;
char fim;
char opcao;

char imagem[MAX_tela_y][MAX_tela_X] = {0};

// Estruturas para tiros, monstros e jogador
typedef struct {
    int ativo;
    int x;
    int y;
} tiros;

tiros tiro[max_tiros] = {{0, 0, 0}};

typedef struct {
    int ativo;
    int x;
    int y;
} bicho;

bicho monstro[MAX_monstro * 2] = {{0, 0, 0}}; 

typedef struct {
    int ativo;
    int x;
    int y;
} bicho2;

bicho2 monstro2[MAX_monstro*2] = {{0, 0, 0}}; 

typedef struct {
    int ativo;
    int x;
    int y;
} bicho3;

bicho3 monstro3[MAX_monstro] = {{0, 0, 0}};

typedef struct {
    int ativo;
    int x;
    int y;
} bicho4;

bicho4 monstro4 = {0, 0, 0};

typedef struct {
    int ativo;
    int x;
    int y;
} tiros_monstros;

tiros_monstros tiro_monstros[max_tiro_monstro] = {{0, 0, 0}};

typedef struct {
    int x;
    int y;
} jogado;

jogado jogador_p;

typedef struct {
    int ativo;
    int x;
    int y;
    int resistencia;
} barreira;

barreira barreiras[max_barreira] = {{0, 0, 0, 0}};
int kbhit(void) {
    return _kbhit(); // Função do Windows para verificar se uma tecla foi pressionada
}

int getch(void) {
    return _getch(); // Função do Windows para obter uma tecla pressionada
}

// Função para limpar a tela
void limpar() {
    COORD coord = {0, 0};
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, coord);
}

void esconder_cursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100; // Define o tamanho do cursor
    info.bVisible = FALSE; // Define a visibilidade do cursor como falsa
    SetConsoleCursorInfo(consoleHandle, &info);
}


// Função para desenhar a tela
void tela() {
  
    printf("%*sPontos: %d", MAX_margem + 27, "", ponto);
    printf("%*snivel: %d", MAX_margem -15 , "", level);
    printf("%*svida: %d\n",15, "", vida);
    
    for (int i = 0; i < MAX_tela_y; i++) {
        for (int k = 0; k < MAX_margem; k++) {
            printf("  ");
        }
        for (int j = 0; j < MAX_tela_X; j++) {
            if (i==0) {
                printf("\033[37m_\033[0m");
             } else if (j==0 || j== MAX_tela_X-1)
             {
                printf("|");
             }else if (i == MAX_tela_y - 1)
             {
                printf("-");
             }else if (imagem[i][j] == forma_jogador) {
                printf("\033[34m%c\033[0m", forma_jogador);
            } else if (imagem[i][j] == monstro_1) {
                // Cor verde para monstro_1
                printf("\033[35m%c\033[0m", monstro_1);
            } else if (imagem[i][j] == monstro_2) {
                // Cor vermelha para monstro_2
                printf("\033[31m%c\033[0m", monstro_2);
            } else if (imagem[i][j] == monstro_3) {
                // Cor azul para monstro_3
                printf("\033[34m%c\033[0m", monstro_3);
            }else if (imagem[i][j] == monstro_4) {
                printf("\033[33m%c\033[0m", monstro_4);
            } else if (imagem[i][j] == barreira_forma_1) {
                printf("\033[32m%c\033[0m", barreira_forma_1);
            }else if (imagem[i][j]== barreira_forma_2){
                printf("\033[33m%c\033[0m", barreira_forma_2);
            }else if (imagem[i][j]== barreira_forma_3) {
                printf("\033[31m%c\033[0m", barreira_forma_3);
            } else if (imagem[i][j] == forma_tiro_monstro){
                 printf("\033[31m%c\033[0m", forma_tiro_monstro);
            }else if (imagem[i][j] == forma_tiro) {
                printf("\033[36m%c\033[0m", forma_tiro);
            } else {
                printf(" ");
            }
        }
        putchar('\n');
    }
}

// Função para inicializar o jogador
void inicia_jogador() {
    jogador_p.x = MAX_tela_X / 2;
    jogador_p.y = MAX_tela_y - 2;
    imagem[jogador_p.y][jogador_p.x] = forma_jogador;
}

// Função para inicializar monstros
void inicia_monstros() {
    for (int i = 0; i < MAX_monstro; i++) {
        
        monstro[i].ativo = 1;
        monstro[i].x = 1 + (2 * i);
        monstro[i].y = 6;
        imagem[monstro[i].y][monstro[i].x] = monstro_1;

        monstro[i + MAX_monstro].ativo = 1;
        monstro[i + MAX_monstro].x = 1 + (2 * i);
        monstro[i + MAX_monstro].y = 5;
        imagem[monstro[i + MAX_monstro].y][monstro[i + MAX_monstro].x] = monstro_1;

        
        monstro2[i].ativo = 1;
        monstro2[i].x = 1 + (2 * i);
        monstro2[i].y = 4;
        imagem[monstro2[i].y][monstro2[i].x] = monstro_2;

        monstro2[i+MAX_monstro].ativo = 1;
        monstro2[i+MAX_monstro].x = 1 + (2 * i);
        monstro2[i+ MAX_monstro].y = 3;
        imagem[monstro2[i + MAX_monstro].y][monstro2[i + MAX_monstro].x] = monstro_2;

        monstro3[i].ativo = 1;
        monstro3[i].x = 1 + (2 * i);
        monstro3[i].y = 2;
        imagem[monstro3[i].y][monstro3[i].x] = monstro_3;

    }
}

void inicia_barreira() {
    int inicio_grupo = (MAX_tela_X - (max_grupo * max_barreiras_por_grupo) - ((max_grupo - 1) * max_espaco_grupo)) / 2;
    int barreira_contagem = 0;

    for (int i = 0; i < max_grupo; i++) {
        for (int j = 0; j < max_barreiras_por_grupo; j++) {
            barreiras[barreira_contagem].ativo = 1;
            barreiras[barreira_contagem].x = inicio_grupo + (i * (max_barreiras_por_grupo + max_espaco_grupo)) + j;
            barreiras[barreira_contagem].y = MAX_tela_y - 5;
            barreiras[barreira_contagem].resistencia = 10;
            
            imagem[barreiras[barreira_contagem].y][barreiras[barreira_contagem].x] = barreira_forma_1;
            
            barreira_contagem++;
        }
    }
}
void configuracoes_iniciais(){
    inicia_jogador();
    inicia_monstros();
    inicia_barreira();
}
void tela_inicial() {
    do {
        limpar();
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s                  SPACE INVADERS                 \n", MAX_margem + 45, "");
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s  Pressione 's' para iniciar o jogo\n", MAX_margem + 45, "");
        printf("%*s  Pressione 'i' para ver as instrucoes\n", MAX_margem + 45, "");
        printf("%*s  Pressione 'q' para sair\n", MAX_margem + 45, "");
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        opcao = getch();
        if (opcao == 's') {
            system("cls");
            configuracoes_iniciais();
            break; 
        } else if (opcao == 'i') {
            system("cls");
    printf("%*s----------------------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*s                        INSTRUÇÕES DO JOGO               \n", MAX_margem + 45, "");
    printf("%*s----------------------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*sControles:\n", MAX_margem + 45, "");
    printf("%*s- Setas Direita/Esquerda ou 'd'/'a': Mover o jogador\n", MAX_margem + 45, "");
    printf("%*s- Barra de espaço: Disparar tiro\n", MAX_margem + 45, "");
    printf("%*s- 'q': Sair do jogo\n\n", MAX_margem + 45, "");
    printf("%*s----------------------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*sObjetivo:\n", MAX_margem + 45, "");
    printf("%*sDerrote todos os monstros e alcance a maior pontuação possível.\n", MAX_margem + 45, "");
    printf("%*s---------------------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*sPressione qualque tecla para voltar ao menu principal.\n", MAX_margem + 45, "");
     printf("%*s---------------------------------------------------------------\n", MAX_margem + 45, "");
    
    getch();
        } else if (opcao == 'q') {
            exit(0);
        }
    } while (1);
}

void limpa_tela() {
    for (int i = 0; i < MAX_tela_y; i++) {
        for (int j = 0; j < MAX_tela_X; j++) {
            imagem[i][j] = ' '; // Limpa toda a matriz imagem
        }
    }
}

void reinicia_jogo() {
    limpa_tela();                   
    ponto = 0;
    vida = 3;
    tempo_monstro = 0;
    velocidade = 1;
    velocidade1=0;
    level = 1;
    direcao = 1; 
    for (int i = 0; i < max_tiros; i++) {
        tiro[i].ativo = 0; // Reseta os tiros
    }
    for (int i = 0; i < max_tiro_monstro; i++) {
        tiro_monstros[i].ativo = 0; // Reseta os tiros dos monstros
    }
    for (int i = 0; i < MAX_monstro * 2; i++) {
        monstro[i].ativo = 0; // Reseta os monstros 
        monstro2[i].ativo = 0; 
        if (i < MAX_monstro) {
            monstro3[i].ativo = 0; 
        }
    }
         monstro4.ativo=0;
         imagem[monstro4.y][monstro4.x] = ' ';
    for (int i = 0; i < max_barreira; i++) {
        barreiras[i].ativo = 0; // Reseta as barreiras
    }
}
 

void tela_game_over() {
    system("cls");
    do
    {
    limpar();
    printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*s                   GAME OVER                     \n", MAX_margem + 45, "");
    printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
     printf("%*s        Pontuacao maxima: %d\n", MAX_margem + 45, "", pontuacao_maxima);
    printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*s        Pontuacao Final: %d\n", MAX_margem + 45, "", ponto);
    printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
     printf("%*s        Nivel Final: %d\n", MAX_margem + 45, "", level);
    printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
    printf("%*sPressione f para voltar ao jogo ou q para sair...\n", MAX_margem + 45, "");
    fim = getch();
    if (fim == 'f')
    {   system("cls");
       reinicia_jogo();
        limpar();
        configuracoes_iniciais();
        limpar();
        break;
    }else if (fim == 'q')
    {
       exit(0); 
    } 
    } while (1);
}

// Função para disparar tiros
void disparos() {
    for (int i = 0; i < max_tiros; i++) {
        if (!tiro[i].ativo) {
            tiro[i].ativo = 1;
            tiro[i].x = jogador_p.x;
            tiro[i].y = jogador_p.y-1;
            imagem[tiro[i].y][tiro[i].x] = forma_tiro;
            break;
        }
    }
}
// Função para mover monstros e atualizar suas posições
void gera_monstro_especial(){
     if (!monstro4.ativo) {
            // Gera um número aleatório para determinar se o monstro4 deve aparecer
            int num = rand() % 80; 
            if (num == 0) { 
                monstro4.ativo = 1;
                if(direcao2 == 1){
                monstro4.x = 1;
                 }
                else{
                    monstro4.x=MAX_tela_X-1;
                }
                monstro4.y = 1; 
            }
     }
      tempo_monstro_especial++;
     if( tempo_monstro_especial>= 3){
      tempo_monstro_especial=0;
      if (monstro4.ativo) imagem[monstro4.y][monstro4.x] = ' '; 
      if (monstro4.ativo)  monstro4.x+= direcao2; 
      if (monstro4.ativo && (monstro4.x >= MAX_tela_X - 1 || monstro4.x <= 0)) {
                monstro4.ativo = 0; 
                direcao2 *= -1;
            } 
      if(monstro4.ativo) imagem[monstro4.y][monstro4.x] = monstro_4; 

}
}
void limpa_posicao_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) imagem[monstro[i].y][monstro[i].x] = ' ';
        if (monstro2[i].ativo) imagem[monstro2[i].y][monstro2[i].x] = ' ';
        if (i < MAX_monstro && monstro3[i].ativo) imagem[monstro3[i].y][monstro3[i].x] = ' ';
    }           
}

void atualiza_posicao_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstro[i].x += direcao;
        if (monstro2[i].ativo) monstro2[i].x += direcao;
        if (i < MAX_monstro && monstro3[i].ativo) monstro3[i].x += direcao;
    }
    
}

void desce_linha_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstro[i].y += 1;
        if (monstro2[i].ativo) monstro2[i].y += 1;
        if (i < MAX_monstro && monstro3[i].ativo) monstro3[i].y += 1;

        if ((monstro[i].y >= MAX_tela_y - 3 || monstro2[i].y >= MAX_tela_y - 3 || (i< MAX_monstro && monstro3[i].y >= MAX_tela_y - 3)) && vida > 0) {
            vida-=vida;
        }
    }
    if (vida == 0) {
       tela_game_over();
    }
}

void desenha_monstros_na_tela() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) imagem[monstro[i].y][monstro[i].x] = monstro_1;
        if (monstro2[i].ativo) imagem[monstro2[i].y][monstro2[i].x] = monstro_2;
        if (i < MAX_monstro && monstro3[i].ativo) imagem[monstro3[i].y][monstro3[i].x] = monstro_3;
    }        
}

void verifica_bordas_e_atualiza_direcao() {
    int borda=0;
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo && (monstro[i].x == 1 || monstro[i].x == MAX_tela_X - 2) ||
            monstro2[i].ativo && (monstro2[i].x == 1 || monstro2[i].x == MAX_tela_X - 2) ||
            i < MAX_monstro && monstro3[i].ativo && (monstro3[i].x == 1 || monstro3[i].x == MAX_tela_X - 2)) {
            borda=1;
            velocidade++; 
            break;  
        }
    }

    if(borda){
        direcao *= -1;
            desce_linha_monstros();     
    }
}

void velocidade_monstro(){
    int monstros_ativos = 0;
     // Contagem de monstros do tipo 1 e 2
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstros_ativos++;
        if (monstro2[i].ativo) monstros_ativos++;
    }
    
    for (int i = 0; i < MAX_monstro; i++) {
        if (monstro3[i].ativo) monstros_ativos++;
    }

    // Ajuste da velocidade
    if (monstros_ativos == ((MAX_monstro * 5)/2)+15)
    {
        velocidade1=1;
    }else if (monstros_ativos == ((MAX_monstro * 5)/2)+10)
    {
        velocidade1=2;
    }else if (monstros_ativos == (MAX_monstro * 5)/2) {  
          velocidade1=3;
        } else if (monstros_ativos == ((MAX_monstro * 5)/2)-10)
        {
            velocidade1=4;
        } else if (monstros_ativos == ((MAX_monstro * 5)/2)-20)
        {
            velocidade1=5;
        }
    if (monstros_ativos == 0)
    { 
          level++;
          velocidade=0;
          velocidade1=0;
          monstro4.ativo=0;
          direcao = 1;
         imagem[monstro4.y][monstro4.x] = ' ';
         for (int i = 0; i < level; i++)
          {
            contador++;
          }
          velocidade= contador;
          contador=0;
         inicia_monstros();
         gera_monstro_especial();
          
    } 
}

void movimento_monstro() {
   tempo_monstro++;
   gera_monstro_especial();
if (tempo_monstro >= (16 -(velocidade + velocidade1))) {
    tempo_monstro = 0;
        velocidade_monstro();
        limpa_posicao_monstros();
        atualiza_posicao_monstros();
        verifica_bordas_e_atualiza_direcao();
        desenha_monstros_na_tela();
    }
}
void mover_jogador() {
    if (_kbhit()) { // Verifica se uma tecla foi pressionada
        move = _getch();
        imagem[jogador_p.y][jogador_p.x] = ' ';
        
        if (move == 'd' || move == 'D') { 
            if ( jogador_p.x < MAX_tela_X - 2){
                jogador_p.x++;
            }    
        } else if (move == 'a' || move == 'A') { 
            if (jogador_p.x > 1){
              jogador_p.x--;
            }       
        }else if (move == ' ') {
            disparos();
        } else if (move == 'q') {
            exit(0);
        }

        imagem[jogador_p.y][jogador_p.x] = forma_jogador; 
    }
}

void colisao_com_monstro(){
for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) {
            for (int j = 0; j < MAX_monstro * 2; j++) {
                if (monstro[j].ativo && tiro[i].x == monstro[j].x && tiro[i].y == monstro[j].y) {
                    ponto += 10; 
                    monstro[j].ativo = 0;
                    imagem[monstro[j].y][monstro[j].x] = ' ';
                    tiro[i].ativo = 0;
                    break; 
                }
            }

            
            for (int j = 0; j < MAX_monstro*2; j++) {
                if (monstro2[j].ativo && tiro[i].x == monstro2[j].x && tiro[i].y == monstro2[j].y) {
                    ponto += 20; 
                    monstro2[j].ativo = 0;
                    imagem[monstro2[j].y][monstro2[j].x] = ' ';
                    tiro[i].ativo = 0;
                    break; 
                }
            }
            for (int j = 0; j < MAX_monstro; j++) {
                if (monstro3[j].ativo && tiro[i].x == monstro3[j].x && tiro[i].y == monstro3[j].y) {
                    ponto += 40; 
                    monstro3[j].ativo = 0;
                    imagem[monstro3[j].y][monstro3[j].x] = ' ';
                    tiro[i].ativo = 0;
                    break; 
                }
            }
            if(monstro4.ativo && tiro[i].x == monstro4.x && tiro[i].y == monstro4.y){
                int numero = rand() %500;
                ponto+= numero;
                monstro4.ativo=0;
                imagem[monstro4.y] [monstro4.x] = ' ';
                tiro[i].ativo = 0;
                break;
            }
        }
    }
    if (pontuacao_maxima< ponto)
    {
        pontuacao_maxima=ponto;
    }
}

void atualizar_resistencia_barreira(){
    for (int j = 0; j < max_barreira; j++) {
        if (barreiras[j].ativo) {
            int monstro_na_posicao = 0;
            for (int k = 0; k < MAX_monstro; k++) {
                if ((monstro[k].ativo && monstro[k].x == barreiras[j].x && monstro[k].y == barreiras[j].y) ||
                    (monstro2[k].ativo && monstro2[k].x == barreiras[j].x && monstro2[k].y == barreiras[j].y) ||
                    (monstro3[k].ativo && monstro3[k].x == barreiras[j].x && monstro3[k].y == barreiras[j].y)) {
                    monstro_na_posicao = 1;
                    break;
                }
            }
            if (!monstro_na_posicao) {
                // Atualiza a aparência da barreira apenas se não houver monstro
                if (barreiras[j].resistencia > 7) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_1;
                } else if (barreiras[j].resistencia > 3) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_2;
                } else if (barreiras[j].resistencia > 0) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_3;
                } else {
                    imagem[barreiras[j].y][barreiras[j].x] = ' ';
                    barreiras[j].ativo = 0;
                }
            } else {
                imagem[barreiras[j].y][barreiras[j].x] = ' ';  // Monstro aparece na barreira
            }
        }
    }
}

void colisao_com_barreiras() {
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) {
            for (int j = 0; j < max_barreira; j++) {
                if (barreiras[j].ativo && 
                    tiro[i].x == barreiras[j].x && 
                    tiro[i].y == barreiras[j].y) {
        
                    barreiras[j].resistencia--;
                    tiro[i].ativo = 0;
                    imagem[tiro[i].y][tiro[i].x] = ' ';
                    atualizar_resistencia_barreira();
                    }
        }
    } 
}
}
void tiro_monster() {
    
    for (int i = 0; i < max_tiro_monstro; i++) {
        if (!tiro_monstros[i].ativo) {
            // Seleciona aleatoriamente um monstro para disparar
            int chance_tiro = rand() % (11 -(velocidade1*2));
            int monstro_atirador = rand () %MAX_monstro;
            if (chance_tiro==0 && monstro3[monstro_atirador].ativo) {
                tiro_monstros[i].ativo = 1;
                tiro_monstros[i].x = monstro3[monstro_atirador].x;
                tiro_monstros[i].y = monstro3[monstro_atirador].y + 1;
            }
        } else {
            tempo_tiro_monstro++;

        if (tempo_tiro_monstro > 1) {

            tempo_tiro_monstro=0;
            
                if (imagem[tiro_monstros[i].y][tiro_monstros[i].x] == forma_tiro_monstro) {
                    imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';
                }
                    int proxima_y = tiro_monstros[i].y + 1;
                    int monstro_na_proxima_posicao = 0;
                    for (int j = 0; j < MAX_monstro * 2; j++) {
                        if (monstro[j].ativo && monstro[j].x == tiro_monstros[i].x && monstro[j].y == proxima_y || monstro2[j].ativo && monstro2[j].x == tiro_monstros[i].x && monstro2[j].y == proxima_y ) {
                            monstro_na_proxima_posicao = 1;
                            break;
                        }
                    }

                    if (proxima_y < MAX_tela_y - 1 && !monstro_na_proxima_posicao) {
                    tiro_monstros[i].y++; // Move o tiro para baixo
                    imagem[tiro_monstros[i].y][tiro_monstros[i].x] = forma_tiro_monstro;
                } else if (monstro_na_proxima_posicao) {
                    tiro_monstros[i].y++; // Move o tiro para baixo sem desenhá-lo na posição do monstro
                } else {
                    tiro_monstros[i].ativo = 0; // Desativa o tiro ao chegar no limite inferior
                }
            
            }
            // Verifica se o tiro do monstro atingiu o jogador
            if (tiro_monstros[i].ativo && tiro_monstros[i].x == jogador_p.x && tiro_monstros[i].y == jogador_p.y) {
                vida--;
                tiro_monstros[i].ativo = 0;
                imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';
                if (vida == 0) {
                    tela_game_over();
                }
            }
            // verifica se atingiu a barreira
            for (int j = 0; j < max_barreira; j++)
            {
                if (barreiras[j].ativo && tiro_monstros[i].ativo &&
                    tiro_monstros[i].x == barreiras[j].x && 
                    tiro_monstros[i].y == barreiras[j].y) {
        
                    barreiras[j].resistencia--;
                    tiro_monstros[i].ativo = 0;
                    imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';}
                   atualizar_resistencia_barreira();
        }
    }
}   
}

// Função para mover os tiros e verificar colisão com monstros
void tiro_e_colisao() {
    tiro_monster();
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) {
            imagem[tiro[i].y][tiro[i].x] = ' ';
            tiro[i].y--;

            if (tiro[i].y < 0) {
                tiro[i].ativo = 0;
            } else {
                colisao_com_monstro();
                colisao_com_barreiras();

                if (tiro[i].ativo) {
                    imagem[tiro[i].y][tiro[i].x] = forma_tiro;
                }
            }
        }
    }
}

// Função principal do jogo
int main() {
    srand((unsigned int)time(NULL));
    tela_inicial();
         while (vida > 0) {
        esconder_cursor();
        limpar();
        tela();
        mover_jogador();
        movimento_monstro();
        tiro_e_colisao();
        Sleep(ATRASO_TIQUE);
    }
    return 0;
}
