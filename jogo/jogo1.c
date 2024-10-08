#include <stdio.h>
#include <stdlib.h>
#define MAX_tela_X 20
#define MAX_tela_y 10
#define forma_jogador '^'
#define forma_tiro '.'
#define max_tiros 1
// a para a esquerda. d para a direita. espaço para atirar. q para sair.

int jogador_x = MAX_tela_X/2;
int jogador_y= MAX_tela_y-2;
char imagem [MAX_tela_y] [MAX_tela_X]= {0};
typedef struct 
{
    int ativo;
    int x;
    int y;

}tiros;
tiros tiro[max_tiros]={{0,0,0}};
void limpar(){
        system("clear");

     }

void tela(){
    
    for (int i = 0; i < MAX_tela_y; i++)
    {
        
        for (int j = 0; j < MAX_tela_X; j++)
        {
            if (i== 0 || j==0 || i==MAX_tela_y-1 || j==MAX_tela_X-1)
            {
                printf("+");
            } else if (imagem[i][j] == forma_jogador) {
                printf("%c", forma_jogador);
            } else if (imagem [i][j]== forma_tiro)
            {
                printf("%c", forma_tiro);
            }
            
             else {
                printf(" ");
            }
        }
        putchar('\n');
    } 
}
void jogador (){
     imagem [jogador_y] [jogador_x]= forma_jogador; 
}
void disparos(){
    for (int i = 0; i < max_tiros; i++)
    {
        if(!tiro[i].ativo){
        tiro[i].ativo=1;
        tiro[i].x= jogador_x;
        tiro[i].y= jogador_y;
        imagem[tiro[i].y] [tiro[i].x]=forma_tiro;
        break;
        }
    }
    

}
void mover(){

    imagem [jogador_y] [jogador_x]= ' ';
    char move;
   scanf("%c", &move);

      if(move=='a' && jogador_x>1) 
     jogador_x--;

     else if(move=='d'&& jogador_x< MAX_tela_X-2) 
     jogador_x++;

     else if(move==' '){
       disparos();
     }

     else if(move=='q')
     exit(-1);

     imagem [jogador_y] [jogador_x]= forma_jogador;
}
void mover_tiro() {
    for (int i = 0; i < max_tiros; i++)
    {
    
    if (tiro[i].ativo) {
        
        imagem[tiro[i].y][tiro[i].x] = ' ';

       
        if (tiro[i].y > 1) {
            tiro[i].y--;
            imagem[tiro[i].y][tiro[i].x] = forma_tiro;  
        } else {
            tiro[i].ativo = 0;  
        }
    }
}
}

int main(){
    jogador();
   
    
    while (1)
    {
        limpar();
        tela();
        mover();
        mover_tiro();
    }
    return 0;
}