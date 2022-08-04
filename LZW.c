#include <stdio.h>
#include <string.h>

typedef unsigned short int ui16;

#define TAM_DICT 4096
#define NUM_SIMB 256

void codifica(int *in, int n){
  ui16 dicionario[TAM_DICT][NUM_SIMB];
  int posDict;            //Posicao no dicionario
  int simbolo;            //Simbolo na entrada
  int corrente = in[0];   //Indice atual da tabela 
  // Inicia dicionario
  memset(dicionario, 0, TAM_DICT*NUM_SIMB*sizeof(ui16));
  posDict = NUM_SIMB;
  int i = 1;
  while(i < n){
    simbolo = in[i++];
    ui16 prox = dicionario[corrente][simbolo];
    if(prox!=0){
      corrente = prox;
    }
    else{
      printf("[%d] ", corrente);
      if(posDict < TAM_DICT){
        dicionario[corrente][simbolo] = posDict++;
      }
      else{
        memset(dicionario, 0, TAM_DICT*NUM_SIMB*sizeof(ui16));
        posDict = NUM_SIMB;
      }
      corrente = simbolo;
    }
  };
  printf("[%d]\n", corrente);
}
int main(){
  int in[16] = {39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126};
  codifica(in, 16);
  return 0;
}