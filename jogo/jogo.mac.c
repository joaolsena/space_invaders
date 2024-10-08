#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Biblioteca para manipular terminal no Unix
#include <unistd.h>  // Biblioteca para funções de POSIX no Unix
#include <fcntl.h>   // Biblioteca para manipular controle de arquivos
#include <time.h>    // Biblioteca para nanosleep()

// Definições relacionadas à tela
#define MAX_tela_X 50   // Largura máxima da tela
#define MAX_tela_y 20   // Altura máxima da tela
#define MAX_margem 30   // Margem adicional para a exibição de pontuação e vida

// Definições relacionadas ao jogo
#define forma_jogador '^' // Representação do jogador na tela
#define forma_tiro '|'    // Representação do tiro na tela
#define max_tiros 5       // Número máximo de tiros simultâneos
#define monstro_1 'M'     // Representação do monstro na tela
#define MAX_monstro 5     // Apenas 5 monstros por linha

int ponto = 0;                    // Pontuação do jogador
int vida = 3;                     // Quantidade de vida do jogador
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

// Função para configurar o terminal para não esperar pelo enter
void configura_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Desabilita o modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Função para restaurar configurações do terminal
void restaura_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); // Reabilita o modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Função que verifica se uma tecla foi pressionada
int kbhit() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    struct termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term2);

    int bytes_waiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytes_waiting);

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    return bytes_waiting > 0;
}

// Função que lê um caractere sem precisar do enter
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

// Função que limpa a tela
void limpar() {
    printf("\033[H\033[J"); // Escape sequence para limpar a tela no Unix
}

// Protótipo da função de disparo
void disparos();

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

// Restante do código permanece igual...

// Função principal
int main() {
    configura_terminal();
    atexit(restaura_terminal); // Garante que o terminal será restaurado ao sair

    jogador();
    inicia_monstros();
    while (1) {
        limpar();
        tela();
        mover();
        mover_tiro();
        movimento_monstro();
        struct timespec req = {0, 50000000}; // 50 ms de delay
        nanosleep(&req, NULL); // Substitui Sleep(50) por nanosleep no Unix
    }
    return 0;
}