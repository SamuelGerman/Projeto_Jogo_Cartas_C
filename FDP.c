#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_BARALHO 40
#define RODADAS 10
#define VIDA_INICIO 5


//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//Definição de tads:
typedef struct carta{
    int face;   //4,5,6,7, 8 = Q, 9 = J, 10 = K, 11 = A, 12 = 2, 13 = 3, 14 = 7 OUROS, 15 = A ESPADAS, 16 = 7 COPAS, 17=4 PAUS
    char naipe[8];  
    int poder;  //define qual carta mata qual
}carta;

typedef struct baralho{
    int topo;
    carta vet[TAM_BARALHO];
}baralho;

typedef struct carta_jogo{ //carta que esta na mao do jogador ou na mesa de jogo, é um elemento da lista de cartas com um ponteiro.
    carta x;
    struct carta_jogo *prox;
}carta_jogo;

typedef struct lista_cartas_jogo{ //lista das cartas que o jogador pode jogar, será usado na lista mao jogador e na lista mesa
    carta_jogo *inicio;
}lista_cartas_jogo;


typedef struct jogador{ //elemento da lista de jogadores
    char nome[15];
    int vidas;
    int qtd_mata;
    lista_cartas_jogo *mao;
    struct jogador *prox;
}jogador;

typedef struct lista_jogadores{
    jogador *inicio;
}lista_jogadores;
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//Protótipos das funções:
// funções baralho

baralho* inicia_baralho(){
    baralho* b = (baralho*)malloc(sizeof(baralho));
    if(b==NULL)
        printf("Erro ao criar baralho.\n");
    b->topo = -1;
    return b;
}

carta* prepara_cartas(){
    carta *array = (carta*)malloc(TAM_BARALHO*sizeof(carta));
    int i,j=4;
    for(i=0;i<TAM_BARALHO;i++){
        array[i].face = j;
        array[i].poder = j;
        if((i+1)%4==0)
            j++;
    }
    for(i=0;i<TAM_BARALHO;i+=4){
        strcpy(array[i].naipe,"Ouros");
        strcpy(array[i+1].naipe,"Espadas");
        strcpy(array[i+2].naipe,"Copas");
        strcpy(array[i+3].naipe,"Paus");
    }
    array[3].poder=17;
    array[14].poder=16;
    array[29].poder=15;
    array[12].poder=14;
    return array;
}

int baralho_vazio (baralho *b)
{
    if(b->topo == -1)
    {
        return 1;
    }else{
        return 0;
    }
}

int baralho_cheio (baralho *b)
{
    if(b->topo == TAM_BARALHO-1)
    {
        return 1;
    }else{
        return 0;
    }
}

int empilha_baralho(baralho *b, carta x){
    if(baralho_cheio(b))
        return 0;
    b->topo++;
    b->vet[b->topo] = x;
    return 1;
}

int desempilha_baralho(baralho *b, carta *x){
    if(baralho_vazio(b))
        return 0;
    *x = b->vet[b->topo];
    b->topo--;
    return 1;
}

void libera_baralho(baralho *b){
    free(b);
}

int embaralha(carta *vet){
    srand(time(NULL));
    int n = TAM_BARALHO;
    int i, j;
    carta aux;
        for (i = n - 1; i > 0; i--){
        j = rand() % (i + 1); // seleciona um indice aleatorio para ser trocado com o indice i
        aux = vet[j];
        vet[j] = vet[i];
        vet[i] = aux; 
    }
    return 1;
}

//funcoes jogadores
lista_jogadores* cria_lista_jogadores(){
    lista_jogadores *LJ = (lista_jogadores*)malloc(sizeof(lista_jogadores));
    if (LJ==NULL)
        printf("Erro ao criar lista de jogadores.\n");
    LJ->inicio =NULL;
    return LJ;
}

void libera_lista_jogadores(lista_jogadores* LJ){
    jogador *aux_percorre = LJ->inicio;
    jogador *aux_apaga; //aux 1 percorre na frente e aux 2 apaga os elementos;
    while (aux_percorre!=NULL)
    {
        aux_apaga = aux_percorre;
        aux_percorre = aux_percorre->prox;
        free(aux_apaga);
    }
    free(LJ);
}

int lista_jogador_vazia(lista_jogadores *LJ){
    if(LJ->inicio==NULL)
        return 1;
    else
        return 0;
}
int insere_jogador_fim(lista_jogadores *LJ,jogador x){
    jogador *novo = (jogador*)malloc(sizeof(jogador));
    jogador *aux =LJ->inicio;

    if(novo == NULL)
        return 0;

    novo->prox =NULL;

    novo->mao = (lista_cartas_jogo*)malloc(sizeof(lista_cartas_jogo));
    if(novo->mao == NULL)
        return 0;

    strcpy(novo->nome,x.nome);
    novo->vidas = 5;
    if(lista_jogador_vazia(LJ)){
        LJ->inicio = novo;
        return 1;
    }
    while (aux->prox!=NULL)
        aux = aux->prox;
    aux->prox = novo;
    return 1;
}

//funções lista_cartas_jogo
lista_cartas_jogo* cria_lcj(){
    lista_cartas_jogo *mao = (lista_cartas_jogo*)malloc(sizeof(lista_cartas_jogo));
    if(mao==NULL)
        printf("Erro ao criar mao do jogador.\n");
    mao->inicio = NULL;
    return mao;
}

void libera_lcj(lista_cartas_jogo *m){
    carta_jogo *aux_percorre = m->inicio;
    carta_jogo *aux_apaga; //aux 1 percorre na frente e aux 2 apaga os elementos;
    while (aux_percorre!=NULL){
        aux_apaga = aux_percorre;
        aux_percorre = aux_percorre->prox;
        free(aux_apaga);
    }
    free(m);
}

int lcj_vazia(lista_cartas_jogo *m){
    if(m->inicio==NULL)
        return 1;
    else
        return 0;
}

int remove_carta_jogo(lista_cartas_jogo *mao, int posicao, carta *y){
    int i = 1;

    if(lcj_vazia)
        return 0;

    carta_jogo *ant;
    carta_jogo *atual = mao->inicio;

    while (atual!=NULL && i != posicao){
        ant = atual;
        atual = atual->prox;
        i++;
    }// achou a carta que será removida
    *y = atual->x;
    ant->prox = atual->prox; // faço a ligação pulando o elemento a ser retirado
    free(atual);
    return 1;
}

int dar_cartas_jogadores(baralho *b,lista_jogadores *LJ, int *qtd_cartas){
    jogador *temp = LJ->inicio;
    while(temp != NULL){        //percorrer todos os jogadores
        temp->mao = cria_lcj();      //inicializa a mao do jogador 
        for (int i = 0; i<*qtd_cartas;i++){
        carta_jogo *nova =(carta_jogo*)malloc(sizeof(carta_jogo));
        carta_jogo *aux = temp->mao->inicio;

        if(nova==NULL)
        return 0;
        nova->prox =NULL; //pois insere sempre na ultima posição
        desempilha_baralho(b,&nova->x); //o elemento novo "carta_jogo" recebe, em seu campo carta, a carta desempilhada do baralho embaralhado
        
        if(lcj_vazia(temp->mao))
            temp->mao->inicio = nova;
        else{
            while(aux->prox!=NULL)
                aux = aux->prox;
            aux->prox = nova;
            }
        }
        temp = temp->prox;
    }
    return 1;
}

//menu
int menu(lista_jogadores *LJ, int *qtd_jog){
    int opcao;
    int qtd = 0;
    jogador x;
    printf("Ola, bem-vindo(a) ao jogo de cartas FDP, por favor escolha uma das opcoes abaixo: \n");
    do{
        printf("Pressione '1' se voce nao sabe as regras do jogo.\n");
        printf("Pressione '2' para registrar um ou mais jogadores.\n");
        printf("Pressione '0' para comecar a jogar!\n");
        scanf("%d",&opcao);

        switch (opcao){
            case 1:
                printf("As regras do jogo FDP sao as seguintes: \n");
                printf("O jogo de cartas Fedaput* eh um jogo baseado no jogo de cartas muito conhecido denominado Truco. Este jogo utiliza-se de um baralho, mas nao por completo. Sao usadas as seguintes cartas:\n");
                printf("AS (A) (4 naipes)\n DOIS (4 naipes)\n TRES (4 naipes)\n QUATRO (4 naipes)\n CINCO (4 naipes)\n SEIS (4 naipes)\n SETE (4 naipes)\n DAMA (Q) (4 naipes)\n VALETE (J) (4 naipes)\n REI (K) (4 naipes)\n");
                printf("Para se jogar o jogo Fedaput*, eh necessario apenas saber quais as cartas sao jogadas no Truco, e principalmente quais as regras de valoracao das cartas.\nDesta forma, a seguir sera apresentada a sequencia de valor das cartas no Truco.\n");
                printf("Manilhas - Sao fixas. Da mais forte para a mais fraca: 4 de Paus / 7 de Copas / as de Espadas / 7 de Ouros;\n");
                printf("Ordem das cartas (da menor para maior): 4, 5, 6, 7, Q, J, K, A, 2, 3\n");
            break;
            
            case 2:
                printf("Nome do jogador: \n");
                setbuf(stdin, NULL);
                gets(x.nome);
                insere_jogador_fim(LJ,x);
                qtd++;
                if(qtd<2){
                    printf("Insira o segundo jogador: \n");
                    printf("Nome do jogador: \n");
                    setbuf(stdin, NULL);
                    gets(x.nome);
                    insere_jogador_fim(LJ,x);
                    qtd++;
                }
                else if(qtd>8){
                    printf("\nNumero maximo de jogadores atingido. Iniciando partida\n");
                    opcao =0;
                    break;
                }
            break;
        }
    }while(opcao!=0);
    *qtd_jog = qtd;
    return 1; //jogo iniciado
}

//rodada
int rodada(lista_jogadores *LJ, lista_cartas_jogo *mesa,int *qtd_cartas, int *nmr_rodada, int *qtd_jog){
    jogador *percorre = LJ->inicio;
    carta_jogo *aux = percorre->mao->inicio; //temporaria: fazer sistema de alternancia entre jogadores que iniciam falando quantas matam.
    int soma_chutes = 0; //soma da quantidade de rodadas que os jogadores acham que fazem.
    int i = 1;

    for(int j = 0; j< *qtd_jog;j++){
        printf("%s, veja suas cartas: \n",percorre->nome);
        while(aux!=NULL){
            printf("Posicao %d: %d de %s.\n",i,aux->x.face,aux->x.naipe);
            i++;
            aux = aux->prox;
        }
        if(j == *qtd_jog - 1){
            do{
                printf("%s, Quantas rodadas voce acha que mata? \n",percorre->nome);
                scanf("%d",&percorre->qtd_mata);
            }while(percorre->qtd_mata < 0 ||percorre->qtd_mata > *qtd_cartas || soma_chutes + percorre->qtd_mata == *qtd_cartas);
            soma_chutes += percorre->qtd_mata;
        }else{
            do{
                printf("%s, Quantas rodadas voce acha que mata? \n",percorre->nome);
                scanf("%d",&percorre->qtd_mata);
            }while(percorre->qtd_mata < 0 ||percorre->qtd_mata > *qtd_cartas);
            soma_chutes += percorre->qtd_mata;
        }
        percorre = percorre->prox;
        if(percorre == NULL) // faz o percorre voltar para o inicio apos chegar no ultimo jogador da lista
            percorre = LJ->inicio;
        aux = percorre->mao->inicio;
        i=1;
    }
    // cartas e chutes dados, agora começa o jogo em si./ percorre está no primeiro jogador que recebeu as cartas
    for(int k = 0; k< *qtd_jog; k++){
        printf("%s, veja suas cartas: \n",percorre->nome);
        while(aux!=NULL){
            printf("Posicao %d: %d de %s.\n",i,aux->x.face,aux->x.naipe);
            i++;
            aux = aux->prox;
        }
    }


    return 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
int main(){
    int qtd_jog;
    int qtd_cartas=5;
    int nmr_rodada =1;
    baralho *b = inicia_baralho();
    carta *cartas = prepara_cartas(); //preparo as cartas temporariamente para passa-las posteriormente para a pilha baralho.
    lista_jogadores *LJ = cria_lista_jogadores();
    lista_cartas_jogo *mesa = cria_lcj();

    embaralha(cartas);
    for(int i = 0;i<TAM_BARALHO;i++) // primeiro empilhamento com todas as cartas do baralho.
        empilha_baralho(b,cartas[i]);
    menu(LJ,&qtd_jog);
    dar_cartas_jogadores(b,LJ,&qtd_cartas);
    rodada(LJ,mesa,&qtd_cartas,&nmr_rodada,&qtd_jog);

    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------//