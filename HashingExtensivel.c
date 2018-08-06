#include "HashingExtensivel.h"

Produto* Cria_Produto(char *nome, int codigo, int estoque)
{
    Produto *produto = malloc(sizeof(Produto));
    strncpy(produto->nome, nome, 18);
    produto->codigo = codigo;
    produto->estoque = estoque;
    return produto;
}

Bloco* Cria_Bloco()
{
    int i;
    Bloco *bloco = malloc(sizeof(Bloco));
    for (i = 0; i < TAM_BLOCO; i++) bloco->codigo[i] = bloco->RRN[i] = -1;
    bloco->numChaves = bloco->profundidade = 0;
    return bloco;
}

Bloco* Read_Bloco(Hashing *hash, int RRN)
{
    Bloco *bloco = malloc(sizeof(Bloco));
    fseek(hash->indice, CABECALHO + RRN * sizeof(Bloco), SEEK_SET);
    fread(bloco, sizeof(Bloco), 1, hash->indice);
    return bloco;
}

void Write_Bloco(Hashing *hash, Bloco *bloco, int RRN)
{
    fseek(hash->indice, CABECALHO + RRN * sizeof(Bloco), SEEK_SET);
    fwrite(bloco, sizeof(Bloco), 1, hash->indice);
}

Hashing* Cria_Hashing()
{
    Hashing *hash = malloc(sizeof(Hashing));
    hash->dados = fopen("Hashing.dados", "rb+");
    hash->indice = fopen("Hashing.diretorio", "rb+");
    if (hash->dados && hash->indice)
    {
        fread(hash->diretorio, sizeof(int), MAX_DIR, hash->indice);
        fread(&(hash->profundidade), sizeof(int), 1, hash->indice);
        fread(&(hash->RRNdados), sizeof(int), 1, hash->indice);
        fread(&(hash->RRNindice), sizeof(int), 1, hash->indice);
    }
    else
    {
        int i;
        hash->indice = fopen("Hashing.diretorio", "wb+");
        hash->dados = fopen("Hashing.dados", "wb+");
        hash->RRNindice = 1;
        hash->profundidade = hash->RRNdados = hash->diretorio[0] = 0;
        for (i = 1; i < MAX_DIR; i++) hash->diretorio[i] = -1;
        Bloco *bloco = Cria_Bloco();
        Write_Bloco(hash, bloco, 0);
        free(bloco);
    }
    return hash;
}

void Salva_Hashing(Hashing *hash)
{
    fseek(hash->indice, 0, SEEK_SET);
    fwrite(hash->diretorio, sizeof(int), MAX_DIR, hash->indice);
    fwrite(&(hash->profundidade), sizeof(int), 1, hash->indice);
    fwrite(&(hash->RRNdados), sizeof(int), 1, hash->indice);
    fwrite(&(hash->RRNindice), sizeof(int), 1, hash->indice);
}

Produto* Read_Produto(Hashing *hash, int RRN)
{
    Produto *produto = malloc(sizeof(Produto));
    fseek(hash->dados, RRN * sizeof(Produto), SEEK_SET);
    fread(produto, sizeof(Produto), 1, hash->dados);
    return produto;
}

void Write_Produto(Hashing *hash, Produto *produto, int RRN)
{
    fseek(hash->dados, RRN * sizeof(Produto), SEEK_SET);
    fwrite(produto, sizeof(Produto), 1, hash->dados);
}

int Gera_Chave(int codigo, int profundidade)
{
    int i;
    unsigned int b = 1, resultado = 0;
    for (i = 0; i < profundidade; resultado <<= 1, resultado |= b&codigo, codigo >>= 1, i++);
    return resultado;
}

void Aumenta_Diretorio(Hashing *hash)
{
    int i, tam_diretorio = pow(2, hash->profundidade);
    for (i = tam_diretorio-1; i >= 0; i--) hash->diretorio[2*i] = hash->diretorio[2*i+1] = hash->diretorio[i];
    hash->profundidade++;
}

void Diminui_Diretorio(Hashing *hash)
{
    int i, tam_diretorio = pow(2, hash->profundidade);
    for (i = 0; i < tam_diretorio/2; i++) hash->diretorio[i] = hash->diretorio[2*i];
    hash->profundidade--;
}

void Split(Hashing *hash, int pos)
{
    int i;
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    int p = hash->profundidade - bloco->profundidade;
    int num_apontadores = pow(2, p);
    pos >>= p; pos <<= p;
    int RRNb1 = hash->RRNindice++;
    int RRNb2 = hash->RRNindice++;
    Bloco *bloco1 = Cria_Bloco();
    bloco1->profundidade = bloco->profundidade+1;
    Write_Bloco(hash, bloco1, RRNb1);
    Write_Bloco(hash, bloco1, RRNb2);
    for(i = 0; i < num_apontadores/2; i++) hash->diretorio[pos+i] = RRNb1;
    for(i = num_apontadores/2; i < num_apontadores; i++) hash->diretorio[pos+i] = RRNb2;
    for(i = 0; i < bloco->numChaves; i++) Insere_Interno(hash, bloco->codigo[i], bloco->RRN[i]);
    free(bloco);
    free(bloco1);
}

int Insere_Interno(Hashing *hash, int codigo, int RRN)
{
    int i;
    int pos = Gera_Chave(codigo, hash->profundidade);
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    for (i = 0; i < bloco->numChaves; i++)
    {
        if (bloco->codigo[i] == codigo) { free(bloco); return 0; }
    }
    if (bloco->numChaves < TAM_BLOCO)
    {
        bloco->codigo[bloco->numChaves] = codigo;
        bloco->RRN[bloco->numChaves++] = RRN;
        Write_Bloco(hash, bloco, hash->diretorio[pos]);
        free(bloco);
        return 1;
    }
    if(bloco->profundidade == hash->profundidade)
    {
        if (pow(2, hash->profundidade+1) > MAX_DIR) { free(bloco); return -1; }
        Aumenta_Diretorio(hash);
        pos *= 2;
    }
    free(bloco);
    Split(hash, pos);
    return Insere_Interno(hash, codigo, RRN);
}

void Insere(Hashing *hash, Produto *produto)
{
    int op = Insere_Interno(hash, produto->codigo, hash->RRNdados);
    if (op == 1)
    {
        Write_Produto(hash, produto, hash->RRNdados);
        hash->RRNdados++;
        printf("Sucesso: produto cadastrado!\n\n\n");
    }
    else if (op == 0) printf("Erro: codigo ja cadastrado!\n\n\n");
    else printf("Erro: diretorio atingiu o tamanho maximo!\n\n\n");
}

int Remove_Interno(Hashing *hash, int codigo)
{
    int i, op = 0;
    int pos = Gera_Chave(codigo, hash->profundidade);
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    for (i = 0; i < bloco->numChaves; i++)
    {
        if (bloco->codigo[i] == codigo)
        {
            op = 1;
            bloco->codigo[i] = bloco->codigo[bloco->numChaves-1];
            bloco->RRN[i] = bloco->RRN[bloco->numChaves-1];
            bloco->numChaves--;
            Write_Bloco(hash, bloco, hash->diretorio[pos]);
            break;
        }
    }
    if (!op) { free(bloco); return 0; }
    if (hash->profundidade == 0) { free(bloco); return 1; }
    if (bloco->profundidade != hash->profundidade) { free(bloco); return 1; }
    int pos2 = pos;
    if (pos2%2) pos2--;
    else pos2++;
    Bloco *auxB = Read_Bloco(hash, hash->diretorio[pos2]);
    if (bloco->numChaves + auxB->numChaves > TAM_BLOCO) { free(bloco); free(auxB); return 1; }
    for (i = 0; i < auxB->numChaves; i++)
    {
        bloco->codigo[bloco->numChaves] = auxB->codigo[i];
        bloco->RRN[bloco->numChaves++] = auxB->RRN[i];
    }
    bloco->profundidade--;
    hash->diretorio[pos2] = hash->diretorio[pos];
    Write_Bloco(hash, bloco, hash->diretorio[pos]);
    free(bloco);
    free(auxB);
    int tam_diretorio = pow(2, hash->profundidade);
    for (i = 0; i < tam_diretorio; i++)
    {
        auxB = Read_Bloco(hash, hash->diretorio[i]);
        if (auxB->profundidade == hash->profundidade) { free(auxB); return 1; }
        free(auxB);
    }
    Diminui_Diretorio(hash);
    return 1;
}

void Remove(Hashing *hash, int codigo)
{
    int op = Remove_Interno(hash, codigo);
    if (op == 1) printf("Sucesso: produto removido!\n\n\n");
    else printf("Erro: produto nao encontrado!\n\n\n");
}

void Busca(Hashing *hash, int codigo)
{
    int i;
    int pos = Gera_Chave(codigo, hash->profundidade);
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    for (i = 0; i < bloco->numChaves; i++)
    {
        if (bloco->codigo[i] == codigo) {
            Produto *produto = Read_Produto(hash, bloco->RRN[i]);
            printf("Nome: %s\n", produto->nome);
            printf("Codigo: %d\n", produto->codigo);
            printf("Estoque: %d\n\n\n", produto->estoque);
            free(produto);
            free(bloco);
            return;
        }
    }
    free(bloco);
    printf("Erro: produto nao encontrado!\n\n");
}

void Imprime(Hashing *hash)
{
    int i, j;
    int tam_diretorio = pow(2, hash->profundidade);
    for (i = 0; i < tam_diretorio; i++)
    {
        Bloco *bloco = Read_Bloco(hash, hash->diretorio[i]);
        printf("%d: [ ", i);
        for (j = 0; j < bloco->numChaves; j++) printf("%d ", bloco->codigo[j]);
        printf("]\n");
        free(bloco);
    }
    printf("\n\n");
}

void Adiciona(Hashing *hash, int codigo, int quantidade)
{
    int i;
    int pos = Gera_Chave(codigo, hash->profundidade);
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    for (i = 0; i < bloco->numChaves; i++)
    {
        if (bloco->codigo[i] == codigo)
        {
            Produto *produto = Read_Produto(hash, bloco->RRN[i]);
            produto->estoque += quantidade;
            Write_Produto(hash, produto, bloco->RRN[i]);
            free(produto);
            free(bloco);
            printf("Sucesso: quantidade atualizada!\n\n\n");
            return;
        }
    }
    free(bloco);
    printf("Erro: produto nao encontrado!\n\n\n");
}

void Subtrai(Hashing *hash, int codigo, int quantidade)
{
    int i;
    int pos = Gera_Chave(codigo, hash->profundidade);
    Bloco *bloco = Read_Bloco(hash, hash->diretorio[pos]);
    for (i = 0; i < bloco->numChaves; i++)
    {
        if (bloco->codigo[i] == codigo)
        {
            Produto *produto = Read_Produto(hash, bloco->RRN[i]);
            if (produto->estoque < quantidade) printf("Erro: quantidade indisponivel!\n\n\n");
            else {
                produto->estoque -= quantidade;
                Write_Produto(hash, produto, bloco->RRN[i]);
                printf("Sucesso: quantidade atualizada!\n\n\n");
            }
            free(produto);
            free(bloco);
            return;
        }
    }
    free(bloco);
    printf("Erro: produto nao encontrado!\n\n\n");
}

