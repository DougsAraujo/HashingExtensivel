#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashingExtensivel.h"

int main()
{
    Hashing *hash = Cria_Hashing();
    char nome[20];
    int codigo, estoque, op = -1;
    while (op != 0)
    {
        printf("[ 1 - Cadastrar um novo Produto      ]\n");
        printf("[ 2 - Remover um Produto existente   ]\n");
        printf("[ 3 - Adicionar Estoque a um Produto ]\n");
        printf("[ 4 - Remover estoque de um Produto  ]\n");
        printf("[ 5 - Consultar um Produto           ]\n");
        printf("[ 6 - Imprimir Diretorio             ]\n");
        printf("[ 0 - Sair                           ]\n");
        printf(">>>   ");
        scanf("%d", &op);

        switch (op)
        {
            case 1:
                printf("\n<< Cadastrando um novo Produto >>");
                printf("\nNome do produto: "); scanf("%s", nome);
                printf("Codigo do produto: "); scanf("%d", &codigo);
                printf("Estoque do produto: "); scanf("%d", &estoque);
                Produto *produto = Cria_Produto(nome, codigo, estoque);
                Insere(hash, produto);
                free(produto);
                break;
            case 2:
                printf("\n<< Removendo um Produto existente >>");
                printf("\nCodigo do produto: "); scanf("%d", &codigo);
                Remove(hash, codigo);
                break;
            case 3:
                printf("\n<< Adicionando Estoque a um Produto >>");
                printf("\nCodigo do produto: "); scanf("%d", &codigo);
                printf("Quantidade: "); scanf("%d", &estoque);
                Adiciona(hash, codigo, estoque);
                break;
            case 4:
                printf("\n<< Removendo Estoque de um Produto >>");
                printf("\nCodigo do produto: "); scanf("%d", &codigo);
                printf("Quantidade: "); scanf("%d", &estoque);
                Subtrai(hash, codigo, estoque);
                break;
            case 5:
                printf("\n<< Consultando um Produto >>");
                printf("\nCodigo do produto: "); scanf("%d", &codigo);
                Busca(hash, codigo);
                break;
            case 6:
                Imprime(hash);
                break;
            case 0:
                Salva_Hashing(hash);
                break;
            default: printf("Opcao invalida.\n\n\n");
        }
    }
    free(hash);
    return 0;
}

