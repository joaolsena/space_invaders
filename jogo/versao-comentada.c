// para rodar: clang -o versao-comentada /Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/versoes/versao-comentada.c -lSDL2 -lSDL2_mixer

#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Para configuração do terminal sem necessidade de pressionar Enter
#include <unistd.h> // Para configurar modo não-bloqueante do terminal
#include <time.h> // Para geração de números aleatórios
#include <fcntl.h> // Para manipulação de arquivos e modos de leitura
#include <SDL2/SDL.h> // Biblioteca SDL para gráficos e áudio
#include <SDL2/SDL_mixer.h> // Biblioteca SDL_mixer para manipulação de som

// Configurações de tela 
#define MAX_tela_X 50  // Largura máxima da tela
#define MAX_tela_y 16  // Altura máxima da tela
#define MAX_margem 30  // Largura da margem na tela

// Configurações de jogo
#define forma_jogador '^'  // Símbolo do jogador
#define forma_tiro '|'  // Símbolo do tiro do jogador
#define forma_tiro_monstro '!'  // Símbolo do tiro do monstro
#define max_tiros 2  // Número máximo de tiros simultâneos do jogador
#define monstro_1 'M'  // Símbolo do tipo de monstro 1
#define monstro_2 'H'  // Símbolo do tipo de monstro 2
#define monstro_3 'W'  // Símbolo do tipo de monstro 3
#define monstro_4  'X'  // Símbolo do tipo de monstro 4
#define MAX_monstro 10 // Máximo de monstros por linha
#define max_tiro_monstro 3  // Máximo de tiros simultâneos dos monstros
#define ATRASO_TIQUE 40000  // Intervalo de tempo para controle de movimentação

// Configuração das barreiras
#define barreira_forma_1 '#'  // Símbolo da primeira forma de barreira
#define barreira_forma_2 '*'  // Símbolo da segunda forma de barreira
#define barreira_forma_3 '-'  // Símbolo da terceira forma de barreira
#define max_barreira 49  // Número máximo de barreiras
#define max_grupo 4  // Máximo de grupos de barreiras
#define max_barreiras_por_grupo 8  // Máximo de barreiras por grupo
#define max_espaco_grupo 4  // Espaço entre grupos de barreiras

// Variáveis globais do jogo
int ponto = 0;  // Pontuação do jogador
int vida = 3;  // Vidas do jogador
int direcao = 1;  // Direção dos monstros
int direcao2 = 1;  // direcao do monstro especial
int tempo_monstro = 0;  // Controle de tempo para movimento dos monstros
int tempo_monstro_especial = 0;  // Controle para monstro especial
int tempo_tiro_monstro = 0;  // Controle de tempo para tiros dos monstros
int velocidade = 1;  // Velocidade geral dos monstros
int velocidade1=0;  // Velocidade adicional para casos específicos
int level = 1;  // Nível do jogo
int contador;  // Contador de nivel
int pontuacao_maxima = 0;  // Pontuação máxima do jogador
char move;  // Variável para captura de movimentos do jogador
char fim;  // Variável para  captura do game over
char opcao;  // Variável para capturar opções do menu

char imagem[MAX_tela_y][MAX_tela_X] = {0};  // Array para a representação da tela

// Estruturas para tiros, monstros e jogador
typedef struct {
    int ativo;  // Indica se o tiro está ativo
    int x;  // Posição x do tiro
    int y;  // Posição y do tiro
} tiros;

tiros tiro[max_tiros] = {{0, 0, 0}};  // Array de tiros do jogador

typedef struct {
    int ativo;  // Indica se o monstro está ativo
    int x;  // Posição x do monstro
    int y;  // Posição y do monstro
} bicho;

bicho monstro[MAX_monstro * 2] = {{0, 0, 0}};  // Array de monstros do tipo 1

typedef struct {
    int ativo;
    int x;
    int y;
} bicho2;

bicho2 monstro2[MAX_monstro*2] = {{0, 0, 0}};  // Array de monstros do tipo 2

typedef struct {
    int ativo;
    int x;
    int y;
} bicho3;

bicho3 monstro3[MAX_monstro] = {{0, 0, 0}};  // Array de monstros do tipo 3

typedef struct {
    int ativo;
    int x;
    int y;
} bicho4;

bicho4 monstro4 = {0, 0, 0};  // Estrutura para monstro especial do tipo 4

typedef struct {
    int ativo;
    int x;
    int y;
} tiros_monstros;

tiros_monstros tiro_monstros[max_tiro_monstro] = {{0, 0, 0}};  // Array para tiros dos monstros

typedef struct {
    int x;
    int y;
} jogado;

jogado jogador_p;  // Estrutura para a posição do jogador

typedef struct {
    int ativo;
    int x;
    int y;
    int resistencia;  // Resistência da barreira, indicando quantos tiros pode suportar
} barreira;

barreira barreiras[max_barreira] = {{0, 0, 0, 0}};  // Array de barreiras com resistência

// Função para verificar se uma tecla foi pressionada, sem bloquear
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    // Salva configurações atuais do terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Desativa modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica as novas configurações
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0); 
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // Configura o modo não-bloqueante

    ch = getchar();  // Captura caractere pressionado

    // Restaura configurações originais do terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin); // Devolve o caractere para o buffer de entrada
        return 1; // Indica que uma tecla foi pressionada
    }

    return 0; // Nenhuma tecla foi pressionada
}

// Função para capturar uma tecla pressionada sem eco na tela
int getch(void) {
    struct termios oldt, newt;
    int move;
    tcgetattr(STDIN_FILENO, &oldt); // Salva configurações atuais do terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Desativa modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica as novas configurações
    move = getchar(); // Captura caractere pressionado
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura configurações originais
    return move; // Retorna o caractere capturado
}

// Função para tocar um som pontual (no MacOS)
void tocar_som(char *arquivo_som) {
    char comando[100];
    snprintf(comando, sizeof(comando), "afplay '%s' &", arquivo_som); // Usa afplay para tocar o som
    system(comando);
}

// Inicializa o áudio com SDL e SDL_mixer
void iniciarAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { // Inicializa o sistema de áudio
        printf("Erro ao iniciar SDL: %s\n", SDL_GetError());
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { // Configura SDL_mixer para áudio
        printf("Erro ao iniciar SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }
}

// Carrega e toca a música de fundo em loop
Mix_Music* tocarMusicaFundo(const char* caminho_musica) {
    Mix_Music *musica = Mix_LoadMUS(caminho_musica); // Carrega o arquivo de música
    if (!musica) { // Verifica se o arquivo foi carregado
        printf("Erro ao carregar música: %s\n", Mix_GetError());
        exit(1);
    }
    Mix_PlayMusic(musica, -1);  // Toca a música em loop infinito
    return musica; // Ret
}

// Função para limpar a tela
void limpar() {
    system("clear"); // Comando para limpar a tela no Linux/Mac (usar "cls" no Windows)
}

// Função para desenhar a tela do jogo
void tela() {
    // Exibe a pontuação, nível e vidas do jogador com formatação
    printf("%*sPontos: %d", MAX_margem + 27, "", ponto);
    printf("%*snivel: %d", MAX_margem -15 , "", level);
    printf("%*svida: %d\n",15, "", vida);

    // Desenha o campo de jogo
    for (int i = 0; i < MAX_tela_y; i++) {
        for (int k = 0; k < MAX_margem; k++) {
            printf("  "); // Espaço para alinhar a tela ao centro
        }
        for (int j = 0; j < MAX_tela_X; j++) {
            if (i == 0) {
                printf("\033[37m_\033[0m"); // Linha superior da borda
            } else if (j == 0 || j == MAX_tela_X - 1) {
                printf("|"); // Bordas laterais
            } else if (i == MAX_tela_y - 1) {
                printf("-"); // Linha inferior da borda
            } else if (imagem[i][j] == forma_jogador) {
                printf("\033[34m%c\033[0m", forma_jogador); // Jogador em azul
            } else if (imagem[i][j] == monstro_1) {
                printf("\033[35m%c\033[0m", monstro_1); // Monstro tipo 1 em roxo
            } else if (imagem[i][j] == monstro_2) {
                printf("\033[31m%c\033[0m", monstro_2); // Monstro tipo 2 em vermelho
            } else if (imagem[i][j] == monstro_3) {
                printf("\033[34m%c\033[0m", monstro_3); // Monstro tipo 3 em azul
            } else if (imagem[i][j] == monstro_4) {
                printf("\033[33m%c\033[0m", monstro_4); // Monstro especial tipo 4 em amarelo
            } else if (imagem[i][j] == barreira_forma_1) {
                printf("\033[32m%c\033[0m", barreira_forma_1); // Primeira forma de barreira em verde
            } else if (imagem[i][j] == barreira_forma_2) {
                printf("\033[33m%c\033[0m", barreira_forma_2); // Segunda forma de barreira em amarelo
            } else if (imagem[i][j] == barreira_forma_3) {
                printf("\033[31m%c\033[0m", barreira_forma_3); // Terceira forma de barreira em vermelho
            } else if (imagem[i][j] == forma_tiro_monstro) {
                printf("\033[31m%c\033[0m", forma_tiro_monstro); // Tiro do monstro em vermelho
            } else if (imagem[i][j] == forma_tiro) {
                printf("\033[36m%c\033[0m", forma_tiro); // Tiro do jogador em ciano
            } else {
                printf(" "); // Espaço vazio
            }
        }
        putchar('\n');
    }
}

// Função para inicializar a posição do jogador
void inicia_jogador() {
    jogador_p.x = MAX_tela_X / 2; // Posiciona o jogador ao centro na largura
    jogador_p.y = MAX_tela_y - 2; // Posiciona o jogador na parte inferior da tela
    imagem[jogador_p.y][jogador_p.x] = forma_jogador; // Define o símbolo do jogador na tela
}

// Função para inicializar monstros
void inicia_monstros() {
    // Inicializa monstros em linhas e colunas especificadas
    for (int i = 0; i < MAX_monstro; i++) {
        // Monstros tipo 1
        monstro[i].ativo = 1;
        monstro[i].x = 1 + (2 * i);
        monstro[i].y = 6;
        imagem[monstro[i].y][monstro[i].x] = monstro_1;

        monstro[i + MAX_monstro].ativo = 1;
        monstro[i + MAX_monstro].x = 1 + (2 * i);
        monstro[i + MAX_monstro].y = 5;
        imagem[monstro[i + MAX_monstro].y][monstro[i + MAX_monstro].x] = monstro_1;

        // Monstros tipo 2
        monstro2[i].ativo = 1;
        monstro2[i].x = 1 + (2 * i);
        monstro2[i].y = 4;
        imagem[monstro2[i].y][monstro2[i].x] = monstro_2;

        monstro2[i + MAX_monstro].ativo = 1;
        monstro2[i + MAX_monstro].x = 1 + (2 * i);
        monstro2[i + MAX_monstro].y = 3;
        imagem[monstro2[i + MAX_monstro].y][monstro2[i + MAX_monstro].x] = monstro_2;

        // Monstros tipo 3
        monstro3[i].ativo = 1;
        monstro3[i].x = 1 + (2 * i);
        monstro3[i].y = 2;
        imagem[monstro3[i].y][monstro3[i].x] = monstro_3;
    }
}

// Função para inicializar barreiras
void inicia_barreira() {
    int inicio_grupo = (MAX_tela_X - (max_grupo * max_barreiras_por_grupo) - ((max_grupo - 1) * max_espaco_grupo)) / 2;
    int barreira_contagem = 0;

    // Cria grupos de barreiras com espaçamento entre eles
    for (int i = 0; i < max_grupo; i++) {
        for (int j = 0; j < max_barreiras_por_grupo; j++) {
            barreiras[barreira_contagem].ativo = 1;
            barreiras[barreira_contagem].x = inicio_grupo + (i * (max_barreiras_por_grupo + max_espaco_grupo)) + j;
            barreiras[barreira_contagem].y = MAX_tela_y - 5;
            barreiras[barreira_contagem].resistencia = 10; // Resistência inicial das barreiras
            
            imagem[barreiras[barreira_contagem].y][barreiras[barreira_contagem].x] = barreira_forma_1;
            barreira_contagem++;
        }
    }
}

// Função para configurar o jogo no início
void configuracoes_iniciais() {
    inicia_jogador();   // Inicializa o jogador
    inicia_monstros();  // Inicializa os monstros
    inicia_barreira();  // Inicializa as barreiras
}

// Função para exibir a tela inicial do jogo
void tela_inicial() {
    do {
        limpar(); // Limpa a tela antes de mostrar o menu
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s                  SPACE INVADERS                 \n", MAX_margem + 45, "");
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s  Pressione 's' para iniciar o jogo\n", MAX_margem + 45, "");
        printf("%*s  Pressione 'i' para ver as instruções\n", MAX_margem + 45, "");
        printf("%*s  Pressione 'q' para sair\n", MAX_margem + 45, "");
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        opcao = getch(); // Captura a opção do usuário sem exibir na tela
        if (opcao == 's') {
            configuracoes_iniciais(); // Configurações iniciais do jogo
            break;
        } else if (opcao == 'i') {
            // Em uma versão futura, exibir instruções do jogo
        } else if (opcao == 'q') {
            exit(0); // Sai do jogo
        }
    } while (1); // Loop até o usuário escolher uma opção válida
}

// Função para limpar toda a tela do jogo
void limpa_tela() {
    for (int i = 0; i < MAX_tela_y; i++) {
        for (int j = 0; j < MAX_tela_X; j++) {
            imagem[i][j] = ' '; // Limpa cada posição da matriz imagem
        }
    }
}

// Função para reiniciar o jogo, resetando todas as variáveis e objetos
void reinicia_jogo() {
    limpa_tela(); // Limpa a tela
    ponto = 0;
    vida = 3;
    tempo_monstro = 0;
    velocidade = 1;
    velocidade1 = 0;
    level = 1;
    direcao = 1;
    
    // Desativa todos os tiros do jogador
    for (int i = 0; i < max_tiros; i++) {
        tiro[i].ativo = 0;
    }
    // Desativa todos os tiros dos monstros
    for (int i = 0; i < max_tiro_monstro; i++) {
        tiro_monstros[i].ativo = 0;
    }
    // Desativa todos os monstros
    for (int i = 0; i < MAX_monstro * 2; i++) {
        monstro[i].ativo = 0;
        monstro2[i].ativo = 0;
        if (i < MAX_monstro) {
            monstro3[i].ativo = 0;
        }
    }
    // Desativa monstro especial
    monstro4.ativo = 0;
    imagem[monstro4.y][monstro4.x] = ' ';
    
    // Desativa todas as barreiras
    for (int i = 0; i < max_barreira; i++) {
        barreiras[i].ativo = 0;
    }
}

// Função para exibir a tela de "Game Over"
void tela_game_over() {
    Mix_Music* musica = tocarMusicaFundo("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/musica-jogo2.mp3");
    Mix_FreeMusic(musica); // Libera a música de fundo
    Mix_CloseAudio();
    SDL_Quit(); // Encerra SDL
    tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/death-sound.mp3"); // Som de morte

    do {
        limpar(); // Limpa a tela antes de mostrar a tela de Game Over
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s                   GAME OVER                     \n", MAX_margem + 45, "");
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s        Pontuação maxima: %d\n", MAX_margem + 45, "", pontuacao_maxima);
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s        Pontuação Final: %d\n", MAX_margem + 45, "", ponto);
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*s        Nivel Final: %d\n", MAX_margem + 45, "", level);
        printf("%*s-------------------------------------------------\n", MAX_margem + 45, "");
        printf("%*sPressione f para voltar ao jogo ou q para sair...\n", MAX_margem + 45, "");
        
        fim = getch(); // Captura a escolha do usuário
        if (fim == 'f') {
            reinicia_jogo(); // Reinicia o jogo
            limpar();
            configuracoes_iniciais(); // Reconfigura o jogo
            limpar();
            iniciarAudio(); // Reinicia o áudio
            Mix_Music* musica = tocarMusicaFundo("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/musica-jogo2.mp3");
            break;
        } else if (fim == 'q') {
            exit(0); // Sai do jogo
        }
    } while (1); // Loop até o usuário escolher uma opção válida
}

// Função para o jogador disparar tiros
void disparos() {
    for (int i = 0; i < max_tiros; i++) {
        if (!tiro[i].ativo) { // Verifica se o tiro está inativo
            tiro[i].ativo = 1; // Ativa o tiro
            tiro[i].x = jogador_p.x; // Define a posição inicial do tiro no eixo x
            tiro[i].y = jogador_p.y - 1; // Define a posição inicial do tiro no eixo y (acima do jogador)
            imagem[tiro[i].y][tiro[i].x] = forma_tiro; // Desenha o tiro na tela
            tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/laser-104024.mp3"); // Som do tiro
            break;
        }
    }
}

// Função para gerar um monstro especial que aparece aleatoriamente
void gera_monstro_especial(){
    if (!monstro4.ativo) {
        // Gera um número aleatório para determinar se o monstro especial deve aparecer
        int num = rand() % 80; 
        if (num == 0) { // Chance de 1 em 80 para aparecer
            monstro4.ativo = 1;
            tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/ufo_lowpitch.wav"); // Som do monstro especial
            if(direcao2 == 1){
                monstro4.x = 1; // Inicia do lado esquerdo
            } else {
                monstro4.x = MAX_tela_X - 1; // Inicia do lado direito
            }
            monstro4.y = 1; // Posiciona o monstro especial no topo
        }
    }
    tempo_monstro_especial++;
    if(tempo_monstro_especial >= 3){
        tempo_monstro_especial = 0;
        if (monstro4.ativo) imagem[monstro4.y][monstro4.x] = ' '; // Limpa a posição anterior do monstro especial
        if (monstro4.ativo) monstro4.x += direcao2; // Move o monstro especial
        // Desativa o monstro se ele alcança as bordas
        if (monstro4.ativo && (monstro4.x >= MAX_tela_X - 1 || monstro4.x <= 0)) {
            monstro4.ativo = 0; 
            direcao2 *= -1; // Inverte a direção para a próxima vez que o monstro especial aparecer
        }
        if(monstro4.ativo) imagem[monstro4.y][monstro4.x] = monstro_4; // Atualiza a posição do monstro especial
    }
}

// Função para limpar a posição atual de todos os monstros na tela
void limpa_posicao_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) imagem[monstro[i].y][monstro[i].x] = ' ';
        if (monstro2[i].ativo) imagem[monstro2[i].y][monstro2[i].x] = ' ';
        if (i < MAX_monstro && monstro3[i].ativo) imagem[monstro3[i].y][monstro3[i].x] = ' ';
    }           
}

// Função para atualizar a posição horizontal dos monstros
void atualiza_posicao_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstro[i].x += direcao;
        if (monstro2[i].ativo) monstro2[i].x += direcao;
        if (i < MAX_monstro && monstro3[i].ativo) monstro3[i].x += direcao;
    }
}

// Função para mover os monstros uma linha para baixo ao alcançar as bordas
void desce_linha_monstros() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstro[i].y += 1;
        if (monstro2[i].ativo) monstro2[i].y += 1;
        if (i < MAX_monstro && monstro3[i].ativo) monstro3[i].y += 1;

        // Verifica se algum monstro chegou ao fim da tela, resultando em "Game Over"
        if ((monstro[i].y >= MAX_tela_y - 3 || monstro2[i].y >= MAX_tela_y - 3 || (i < MAX_monstro && monstro3[i].y >= MAX_tela_y - 3)) && vida > 0) {
            vida = 0; // Perde o jogo se algum monstro alcançar o jogador
        }
    }
    if (vida == 0) {
        tela_game_over(); // Exibe tela de "Game Over" se o jogador perder todas as vidas
    }
}

// Função para desenhar monstros na tela nas posições atualizadas
void desenha_monstros_na_tela() {
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) imagem[monstro[i].y][monstro[i].x] = monstro_1;
        if (monstro2[i].ativo) imagem[monstro2[i].y][monstro2[i].x] = monstro_2;
        if (i < MAX_monstro && monstro3[i].ativo) imagem[monstro3[i].y][monstro3[i].x] = monstro_3;
    }        
}

// Função para verificar se monstros atingiram as bordas e ajustar a direção e velocidade
void verifica_bordas_e_atualiza_direcao() {
    int borda = 0;
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo && (monstro[i].x <= 1 || monstro[i].x >= MAX_tela_X - 2) ||
            monstro2[i].ativo && (monstro2[i].x <= 1 || monstro2[i].x >= MAX_tela_X - 2) ||
            i < MAX_monstro && monstro3[i].ativo && (monstro3[i].x <= 1 || monstro3[i].x >= MAX_tela_X - 2)) {
            borda = 1;
            velocidade++; // Aumenta a velocidade ao atingir as bordas
            break;  
        }
    }

    if (borda) {
        direcao *= -1; // Inverte a direção de movimento dos monstros
        desce_linha_monstros(); // Move os monstros para baixo
    }
}

// Função para ajustar a velocidade dos monstros conforme diminui a quantidade de monstros ativos
void velocidade_monstro() {
    int monstros_ativos = 0;
    // Conta os monstros ativos de cada tipo
    for (int i = 0; i < MAX_monstro * 2; i++) {
        if (monstro[i].ativo) monstros_ativos++;
        if (monstro2[i].ativo) monstros_ativos++;
    }
    
    for (int i = 0; i < MAX_monstro; i++) {
        if (monstro3[i].ativo) monstros_ativos++;
    }

    // Ajusta a velocidade com base no número de monstros ativos
    if (monstros_ativos == ((MAX_monstro * 5) / 2) + 15) {
        velocidade1 = 1;
    } else if (monstros_ativos == ((MAX_monstro * 5) / 2) + 10) {
        velocidade1 = 2;
    } else if (monstros_ativos == (MAX_monstro * 5) / 2) {  
        velocidade1 = 3;
    } else if (monstros_ativos == ((MAX_monstro * 5) / 2) - 10) {
        velocidade1 = 4;
    } else if (monstros_ativos == ((MAX_monstro * 5) / 2) - 20) {
        velocidade1 = 5;
    }

    // Quando todos os monstros são destruídos, aumenta o nível e reinicia os monstros
    if (monstros_ativos == 0) { 
        tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/level-completed.mp3"); // Som de nível concluído
        level++;
        velocidade = 0;
        velocidade1 = 0;
        monstro4.ativo = 0;
        direcao = 1;
        imagem[monstro4.y][monstro4.x] = ' ';
        for (int i = 0; i < level; i++) {
            contador++;
        }
        velocidade = contador;
        contador = 0;
        inicia_monstros(); // Reinicia monstros para o próximo nível
        gera_monstro_especial(); // Reintroduz o monstro especial
    } 
}

// Função para controlar o movimento dos monstros
void movimento_monstro() {
    tempo_monstro++;
    gera_monstro_especial(); // Gera monstro especial de tempos em tempos
    if (tempo_monstro >= (16 - (velocidade + velocidade1))) {
        tempo_monstro = 0;
        velocidade_monstro();
        limpa_posicao_monstros();
        atualiza_posicao_monstros();
        verifica_bordas_e_atualiza_direcao();
        desenha_monstros_na_tela();
    }
}

// Função para mover o jogador com as teclas de seta, 'A' e 'D', e atirar com a barra de espaço
void mover_jogador() {
    move = getch(); 
    imagem[jogador_p.y][jogador_p.x] = ' '; // Limpa posição anterior do jogador
    
    if (move == '\033') { // Início da sequência de escape para teclas de seta
        getch(); // Ignora o próximo caractere na sequência
        switch (getch()) { 
            case 'C': if (jogador_p.x < MAX_tela_X - 2) jogador_p.x++; break; // Move para a direita
            case 'D': if (jogador_p.x > 1) jogador_p.x--; break; // Move para a esquerda
        }
    } else if (move == 'd' && jogador_p.x < MAX_tela_X - 2) { 
        jogador_p.x++; // Move para a direita com 'D'
    } else if (move == 'a' && jogador_p.x > 1) { 
        jogador_p.x--; // Move para a esquerda com 'A'
    } else if (move == ' ') { 
        disparos(); // Atira com a barra de espaço
    } else if (move == 'q' || move == 'Q') { 
       exit(0); // Encerra o jogo com 'Q'
    }

    imagem[jogador_p.y][jogador_p.x] = forma_jogador; // Atualiza a posição do jogador
}

// Função para verificar colisões entre tiros do jogador e monstros
void colisao_com_monstro() {
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) { // Verifica se o tiro está ativo
            // Verifica colisão com monstros do tipo 1
            for (int j = 0; j < MAX_monstro * 2; j++) {
                if (monstro[j].ativo && tiro[i].x == monstro[j].x && tiro[i].y == monstro[j].y) {
                    ponto += 10; // Aumenta a pontuação
                    monstro[j].ativo = 0; // Desativa o monstro atingido
                    imagem[monstro[j].y][monstro[j].x] = ' '; // Remove monstro da tela
                    tiro[i].ativo = 0; // Desativa o tiro
                    tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion.mp3"); // Som de explosão
                    break;
                }
            }
            // Verifica colisão com monstros do tipo 2
            for (int j = 0; j < MAX_monstro * 2; j++) {
                if (monstro2[j].ativo && tiro[i].x == monstro2[j].x && tiro[i].y == monstro2[j].y) {
                    ponto += 20;
                    monstro2[j].ativo = 0;
                    imagem[monstro2[j].y][monstro2[j].x] = ' ';
                    tiro[i].ativo = 0;
                    tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion.mp3");
                    break;
                }
            }
            // Verifica colisão com monstros do tipo 3
            for (int j = 0; j < MAX_monstro; j++) {
                if (monstro3[j].ativo && tiro[i].x == monstro3[j].x && tiro[i].y == monstro3[j].y) {
                    ponto += 40;
                    monstro3[j].ativo = 0;
                    imagem[monstro3[j].y][monstro3[j].x] = ' ';
                    tiro[i].ativo = 0;
                    tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion.mp3");
                    break;
                }
            }
            // Verifica colisão com o monstro especial
            if (monstro4.ativo && tiro[i].x == monstro4.x && tiro[i].y == monstro4.y) {
                int numero = rand() % 500; // Pontuação aleatória entre 0 e 499
                ponto += numero;
                monstro4.ativo = 0;
                imagem[monstro4.y][monstro4.x] = ' ';
                tiro[i].ativo = 0;
                tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion.mp3");
                break;
            }
        }
    }
    if (pontuacao_maxima < ponto) {
        pontuacao_maxima = ponto; // Atualiza pontuação máxima
    }
}

// Função para atualizar a resistência das barreiras
void atualizar_resistencia_barreira() {
    for (int j = 0; j < max_barreira; j++) {
        if (barreiras[j].ativo) {
            int monstro_na_posicao = 0;
            // Verifica se há um monstro na posição da barreira
            for (int k = 0; k < MAX_monstro; k++) {
                if ((monstro[k].ativo && monstro[k].x == barreiras[j].x && monstro[k].y == barreiras[j].y) ||
                    (monstro2[k].ativo && monstro2[k].x == barreiras[j].x && monstro2[k].y == barreiras[j].y) ||
                    (monstro3[k].ativo && monstro3[k].x == barreiras[j].x && monstro3[k].y == barreiras[j].y)) {
                    monstro_na_posicao = 1;
                    break;
                }
            }
            // Atualiza aparência da barreira conforme a resistência
            if (!monstro_na_posicao) {
                if (barreiras[j].resistencia > 7) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_1;
                } else if (barreiras[j].resistencia > 3) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_2;
                } else if (barreiras[j].resistencia > 0) {
                    imagem[barreiras[j].y][barreiras[j].x] = barreira_forma_3;
                } else {
                    imagem[barreiras[j].y][barreiras[j].x] = ' '; // Remove barreira se resistência for 0
                    barreiras[j].ativo = 0;
                }
            } else {
                imagem[barreiras[j].y][barreiras[j].x] = ' '; // Monstro aparece na barreira
            }
        }
    }
}

// Função para verificar colisões entre tiros do jogador e barreiras
void colisao_com_barreiras() {
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) {
            for (int j = 0; j < max_barreira; j++) {
                if (barreiras[j].ativo && 
                    tiro[i].x == barreiras[j].x && 
                    tiro[i].y == barreiras[j].y) {
                    barreiras[j].resistencia--; // Reduz a resistência da barreira
                    tiro[i].ativo = 0;
                    imagem[tiro[i].y][tiro[i].x] = ' ';
                    tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion2.mp3");
                    atualizar_resistencia_barreira();
                }
            }
        }
    }
}

// Função para disparos dos monstros
void tiro_monster() {
    for (int i = 0; i < max_tiro_monstro; i++) {
        if (!tiro_monstros[i].ativo) {
            // Seleciona aleatoriamente um monstro para disparar
            int chance_tiro = rand() % (11 - (velocidade1 * 2));
            int monstro_atirador = rand() % MAX_monstro;
            if (chance_tiro == 0 && monstro3[monstro_atirador].ativo) {
                tiro_monstros[i].ativo = 1;
                tiro_monstros[i].x = monstro3[monstro_atirador].x;
                tiro_monstros[i].y = monstro3[monstro_atirador].y + 1;
            }
        } else {
            tempo_tiro_monstro++;
            if (tempo_tiro_monstro > 4) {
                tempo_tiro_monstro = 0;
                if (imagem[tiro_monstros[i].y][tiro_monstros[i].x] == forma_tiro_monstro) {
                    imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';
                }
                int proxima_y = tiro_monstros[i].y + 1;
                int monstro_na_proxima_posicao = 0;
                // Verifica se há monstros na próxima posição do tiro
                for (int j = 0; j < MAX_monstro * 2; j++) {
                    if (monstro[j].ativo && monstro[j].x == tiro_monstros[i].x && monstro[j].y == proxima_y ||
                        monstro2[j].ativo && monstro2[j].x == tiro_monstros[i].x && monstro2[j].y == proxima_y) {
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
                vida--; // Reduz vida do jogador
                tiro_monstros[i].ativo = 0;
                imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';
                tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion2.mp3");
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
                    imagem[tiro_monstros[i].y][tiro_monstros[i].x] = ' ';
                   tocar_som("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/explosion2.mp3");
                    
            }
                   atualizar_resistencia_barreira();
        }
    }
}   
}

// Função para mover os tiros e verificar colisão com monstros
void tiro_e_colisao() {
    tiro_monster(); // Atualiza os tiros disparados pelos monstros
    for (int i = 0; i < max_tiros; i++) {
        if (tiro[i].ativo) { // Verifica se o tiro está ativo
            imagem[tiro[i].y][tiro[i].x] = ' '; // Remove o tiro da tela
            tiro[i].y--; // Move o tiro para cima

            if (tiro[i].y < 0) { // Verifica se o tiro saiu da tela
                tiro[i].ativo = 0; // Desativa o tiro
            } else {
                colisao_com_monstro(); // Verifica colisões com monstros
                colisao_com_barreiras(); // Verifica colisões com barreiras

                if (tiro[i].ativo) { // Se o tiro ainda estiver ativo
                    imagem[tiro[i].y][tiro[i].x] = forma_tiro; // Redesenha o tiro na nova posição
                }
            }
        }
    }
}

// Função principal do jogo
int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    iniciarAudio(); // Inicia o sistema de áudio
    Mix_Music* musica = tocarMusicaFundo("/Users/user01/Documents/GitHub/programacao1/jogo/spaceinvadrs/sons/musica-jogo2.mp3"); // Toca a música de fundo
    tela_inicial(); // Exibe a tela inicial do jogo

    // Loop principal do jogo
    while (vida > 0) { // Enquanto o jogador tiver vida
        limpar(); // Limpa a tela
        tela(); // Desenha a tela do jogo
        if (kbhit()) mover_jogador(); // Verifica se uma tecla foi pressionada e move o jogador
        movimento_monstro(); // Atualiza a posição dos monstros
        tiro_e_colisao(); // Atualiza os tiros e verifica colisões
        usleep(ATRASO_TIQUE); // Aguarda um intervalo de tempo antes do próximo ciclo
    }
    Mix_FreeMusic(musica); // Libera a música da memória
    Mix_CloseAudio(); // Fecha o sistema de áudio
    SDL_Quit(); // Finaliza a biblioteca SDL
    return 0; // Retorna 0 ao sistema, indicando que o programa terminou corretamente
}




