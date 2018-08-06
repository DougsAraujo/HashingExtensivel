#ifndef HASHINGEXTENSIVEL_H
#define HASHINGEXTENSIVEL_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_BLOCO 3
#define MAX_DIR 10
#define CABECALHO (MAX_DIR + 3) * sizeof(int)

typedef struct Produto
{
    char nome[20];
    int codigo;
    int estoque;
} Produto;

typedef struct Bloco
{
    int codigo[TAM_BLOCO];
    int RRN[TAM_BLOCO];
    int numChaves;
    int profundidade;
} Bloco;

typedef struct Hashing
{
    int diretorio[MAX_DIR];
    int profundidade;
    int RRNdados;
    int RRNindice;
    FILE *dados;
    FILE *indice;
} Hashing;

Produto* Cria_Produto(char*, int, int);
Hashing* Cria_Hashing();
void Salva_Hashing(Hashing*);
void Insere(Hashing*, Produto*);
void Remove(Hashing*, int);
void Busca(Hashing*, int);
void Imprime(Hashing*);
void Adiciona(Hashing*, int, int);
void Subtrai(Hashing*, int, int);

#endif

