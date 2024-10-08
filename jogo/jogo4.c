#include <stdio.h>
#include <stdlib.h>

// Definições relacionadas à tela
#define MAX_tela_X 50   // Largura máxima da tela
#define MAX_tela_y 20   // Altura máxima da tela
#define MAX_margem 30   // Margem adicional para a exibição de pontuação e vida

// Definições relacionadas ao jogo
#define forma_jogador '^' // Representação do jogador na tela
#define forma_tiro '|'    // Representação do tiro na tela
#define max_tiros 5       // Número máximo de tiros simultâneos
#define monstro_1 'M'     // Representação do monstro na tela
#define MAX_monstro 15    // Máximo de monstros por linha

// Mensagem de instruções para controle do jogador
// 'a' move para a esquerda, 'd' move para a direita, ' ' (espaço) dispara, 'q' sai do jogo

int ponto = 0;                    // Pontuação do jogador
int vida = 0;                     // Quantidade de vida do jogador
int posicao = MAX_tela_X / 10;    // Posição inicial dos monstros
char imagem[MAX_tela_y][MAX_tela_X] = {0}; // Matriz que representa a tela do jogo

// Estrutura para representar os tiros
typedef struct {
    int ativo; // 1 se o tiro está ativo, 0 caso contrário
    int x;     // Posição x do tiro
    int y;     // Posição y do tiro
} tiros;
tiros tiro[max_tiros] = {{0, 0, 0}}; // Vetor de tiros, inicialmente todos inativos

// Estrutura para representar os monstros
typedef struct {
    int ativo; // 1 se o monstro está ativo, 0 caso contrário
    int x;     // Posição x do monstro
    int y;     // Posição y do monstro
} bicho;
bicho monstro[MAX_monstro] = {{0, 0, 0}}; // Vetor de monstros, inicialmente todos inativos

// Estrutura para representar o jogador
typedef struct {
    int x; // Posição x do jogador
    int y; // Posição y do jogador
} jogado;
jogado jogador_p; // Variável que representa o jogador

// Função que limpa a tela
void limpar() {
    system("clear"); // Usar "cls" se estiver no Windows
}

// Função que exibe a tela do jogo
void tela() {
    printf("%*sPontos: %d", MAX_margem + 15, " ", ponto);   // Exibe a pontuação com alinhamento
    printf("%*svida: %d\n", MAX_margem + 15, "", vida);     // Exibe a vida com alinhamento

    // Desenha a tela com os elementos do jogo
    for (int i = 0; i < MAX_tela_y; i++) {
        for (int k = 0; k < MAX_margem; k++) {
            printf("  "); // Cria margem na tela
        }

        for (int j = 0; j < MAX_tela_X; j++) {
            if (i == MAX_tela_y - 1) {
                printf("_"); // Linha inferior da tela
            } else if (imagem[i][j] == forma_jogador) {
                printf("%c", forma_jogador); // Desenha o jogador
            } else if (imagem[i][j] == monstro_1) {
                printf("%c", monstro_1); // Desenha um monstro
            } else if (imagem[i][j] == forma_tiro) {
                printf("%c", forma_tiro); // Desenha um tiro
            } else {
                printf(" "); // Espaço vazio
            }
        }
        putchar('\n'); // Nova linha
    }
}

// Função que inicializa a posição do jogador
void jogador() {
    jogador_p.x = MAX_tela_X / 2;         // Posiciona o jogador no centro da tela
    jogador_p.y = MAX_tela_y - 2;         // Posiciona o jogador na penúltima linha
    imagem[jogador_p.y][jogador_p.x] = forma_jogador; // Atualiza a tela com a posição do jogador
}

// Função que inicializa a posição dos monstros
void inicia_monstros() {
    for (int i = 0; i < MAX_monstro; i++) {
        monstro[i].ativo = 1;                      // Ativa o monstro
        monstro[i].x = posicao + (2 * i);          // Define a posição x com espaçamento entre monstros
        monstro[i].y = MAX_tela_y / 2;             // Define a posição y na metade da tela
        imagem[monstro[i].y][monstro[i].x] = monstro_1; // Atualiza a tela com a posição dos monstros
    }
}

// Função que exibe os monstros na tela
void monstros() {
    for (int i = 0; i < MAX_monstro; i++) {
        if (monstro[i].ativo) {
            imagem[monstro[i].y][monstro[i].x] = monstro_1; // Atualiza a tela com os monstros ativos
        }
    }
}

// Função que movimenta os monstros
void movimento_monstro() {
    for (int i = 0; i < MAX_monstro; i++) {
        if (monstro[i].ativo) {
            imagem[monstro[i].y][monstro[i].x] = ' '; // Limpa a posição anterior do monstro

            if (monstro[i].x < MAX_tela_X / 10) {
                monstro[i].x++; // Movimenta o monstro para a direita se estiver no início
            } else if (monstro[i].x < MAX_tela_X - 1) {
                monstro[i].x--; // Movimenta o monstro para a esquerda
            }

            imagem[monstro[i].y][monstro[i].x] = monstro_1; // Atualiza a nova posição do monstro
        }
    }
}

// Função que dispara um tiro
void disparos() {
    for (int i = 0; i < max_tiros; i++) {
        if (!tiro[i].ativo) { // Verifica se o tiro está inativo
            tiro[i].ativo = 1; // Ativa o tiro
            tiro[i].x = jogador_p.x; // Posição x do tiro é a mesma do jogador
            tiro[i].y = jogador_p.y; // Posição y do tiro é a mesma do jogador
            imagem[tiro[i].y][tiro[i].x] = forma_tiro; // Atualiza a tela com o tiro
            break; // Interrompe o laço após ativar um tiro
        }
    }
}

// Função que movimenta o jogador e dispara tiros
void mover() {
    imagem[jogador_p.y][jogador_p.x] = ' '; // Limpa a posição anterior do jogador
    char move;
    scanf("%c", &move); // Recebe o comando de movimento do jogador

    if (move == 'a' && jogador_p.x > 1) 
        jogador_p.x--; // Move o jogador para a esquerda
    else if (move == 'd' && jogador_p.x < MAX_tela_X - 2) 
        jogador_p.x++; // Move o jogador para a direita
    else if (move == ' ') {
        disparos(); // Dispara um tiro
    }
    else if (move == 'q')
        exit(0); // Sai do jogo

    imagem[jogador_p.y][jogador_p.x] = forma_jogador; // Atualiza a nova posição do jogador
}

// Função que movimenta os tiros e verifica colisões
void mover_tiro() {
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) { 
            imagem[tiro[i].y][tiro[i].x] = ' '; // Limpa a posição anterior do tiro

            if (tiro[i].y > 1) {
                tiro[i].y--; // Movimenta o tiro para cima
                imagem[tiro[i].y][tiro[i].x] = forma_tiro; // Atualiza a nova posição do tiro
            } else {
                tiro[i].ativo = 0; // Desativa o tiro se sair da tela
            }

            // Verifica colisão com os monstros
            for (int j = 0; j < MAX_monstro; j++) {
                if (monstro[j].ativo && tiro[i].x == monstro[j].x && tiro[i].y == monstro[j].y) {
                    ponto += 10; // Aumenta a pontuação ao acertar um monstro
                    monstro[j].ativo = 0; // Desativa o monstro
                    imagem[monstro[j].y][monstro[j].x] = ' '; // Limpa a posição do monstro na tela
                    tiro[i].ativo = 0; // Desativa o tiro
                }
            }
        } 
}
}


int main(){
    jogador();
    inicia_monstros();
    while (1)
    {
        limpar();
        tela();
        mover();
        mover_tiro();
        movimento_monstro();
    }
    return 0;
}