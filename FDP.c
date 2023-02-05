#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_BARALHO 40
#define RODADAS 10
#define VIDA_INICIO 5

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
    int acha_mata;
    int matou;
    lista_cartas_jogo *mao;
    struct jogador *prox;
}jogador;

typedef struct lista_jogadores{
    jogador *inicio;
}lista_jogadores;

typedef struct jogada{
    jogador *jog;
    carta_jogo c;
}jogada;

//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//Protótipos das funções:

baralho* inicia_baralho();
carta* prepara_cartas();
int baralho_vazio (baralho *b);
int baralho_cheio (baralho *b);
int empilha_baralho(baralho *b, carta x);
int embaralha(carta *vet);
int desempilha_baralho(baralho *b, carta *x);
void libera_baralho(baralho *b);
lista_jogadores* cria_lista_jogadores();
int lista_jogador_vazia(lista_jogadores *LJ);
int insere_jogador_fim(lista_jogadores *LJ,jogador x);
lista_cartas_jogo* cria_lcj();
void libera_lcj(lista_cartas_jogo *m);
void libera_lista_jogadores(lista_jogadores* LJ);
int lcj_vazia(lista_cartas_jogo *m);
int remove_carta_jogo(jogador *jog, int posicao, carta *y);
int verifica_vencedor_rodada(jogada *jogadas, int num_jogadores);
int dar_cartas_jogadores(baralho *b,lista_jogadores *LJ, int *qtd_cartas,int qtd_jog);
int verif_vidas(lista_jogadores* LJ, FILE *arq,int qtd_jog);
int menu(lista_jogadores *LJ, int *qtd_jog);
//rodada
int rodada(lista_jogadores *LJ, baralho *b,int *qtd_cartas, int *nmr_rodada, int *qtd_jog,FILE *arq);
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
int main(){
    FILE *arq = fopen("log.txt", "w");
    int qtd_jog;
    int qtd_cartas=5;
    int nmr_rodada =1;
    baralho *b = inicia_baralho();
    //preparo as cartas temporariamente para passa-las posteriormente para a pilha baralho.
    carta *cartas = prepara_cartas();
    lista_jogadores *LJ = cria_lista_jogadores();
    

    embaralha(cartas);
    //primeiro empilhamento com todas as cartas do baralho.
    for(int i = 0;i<TAM_BARALHO;i++)
        empilha_baralho(b,cartas[i]);
    menu(LJ,&qtd_jog);
    dar_cartas_jogadores(b,LJ,&qtd_cartas,qtd_jog);
    rodada(LJ,b,&qtd_cartas,&nmr_rodada,&qtd_jog,arq);
    printf("\n==========\n==========\nObrigado Por Jogar\n==========\n==========");
    libera_baralho(b);
    libera_lista_jogadores(LJ); // dentro dessa função eu chamo a função para liberar a mao dos jogadores também.
    fclose(arq);
    free(arq);

    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
// funções:

baralho* inicia_baralho(){
    baralho* b = (baralho*)malloc(sizeof(baralho));
    if(b==NULL)
        printf("Erro ao criar baralho.\n");
    b->topo = -1;
    return b;
}

carta* prepara_cartas(){
    carta *array = (carta*)malloc(TAM_BARALHO * sizeof(carta));
    int i, j = 4;
    char faces[] = {'4', '5', '6', '7', 'Q', 'J', 'K', 'A', '2', '3', '7', 'A', '7', '4'};
    for(i = 0; i < TAM_BARALHO; i++){
        array[i].face = faces[j - 4];
        array[i].poder = j;
        if((i + 1) % 4 == 0)
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

int desempilha_baralho(baralho *b, carta *x){
    if(baralho_vazio(b)){
        carta *cartas = prepara_cartas();
        embaralha(cartas);
        //   empilhamento com todas as cartas do baralho.
        for(int i = 0;i<TAM_BARALHO;i++)
            empilha_baralho(b,cartas[i]);
    }

    *x = b->vet[b->topo];
    b->topo--;
    return 1;
}

void libera_baralho(baralho *b){
    free(b);
}



lista_jogadores* cria_lista_jogadores(){
    lista_jogadores *LJ = (lista_jogadores*)malloc(sizeof(lista_jogadores));
    if (LJ==NULL)
        printf("Erro ao criar lista de jogadores.\n");
    LJ->inicio =NULL;
    return LJ;
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
    novo->mao->inicio= NULL; //indica q a lista começa vazia.

    novo->matou = 0;
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

void libera_lista_jogadores(lista_jogadores* LJ){
    jogador *aux_percorre = LJ->inicio;
    jogador *aux_apaga; //aux 1 percorre na frente e aux 2 apaga os elementos;
    while (aux_percorre!=NULL)
    {
        aux_apaga = aux_percorre;
        aux_percorre = aux_percorre->prox;
        libera_lcj(aux_apaga->mao);
        free(aux_apaga);
    }
    free(LJ);
}

int lcj_vazia(lista_cartas_jogo *m){
    if(m->inicio==NULL)
        return 1;
    else
        return 0;
}

int remove_carta_jogo(jogador *jog, int posicao, carta *y){
    int i = 1;

    if(lcj_vazia(jog->mao)==1)
        return 0;

    carta_jogo *ant=NULL;
    carta_jogo *atual = jog->mao->inicio;
    
    if(posicao==1){
        jog->mao->inicio = atual->prox;
    }else{
        while (atual!=NULL && i != posicao){
        ant = atual;
        atual = atual->prox;
        i++;
        }// achou a carta que será removida
        ant->prox = atual->prox; // faço a ligação pulando o elemento a ser retirado
    }

    *y = atual->x;
    free(atual);
    return 1;
}

int verifica_vencedor_rodada(jogada *jogadas, int num_jogadores){

    int i, j;
    int maximo = -1; // Inicializa o valor máximo como -1
    int atual;
    int encontrado;
    int indice_vencedor = -1;
    for (i = 0; i < num_jogadores; i++) {
        atual = jogadas[i].c.x.poder;
        encontrado = 0;
        for (j = 0; j < num_jogadores; j++) { // compara com o resto do vetor para ver se kangou.
            if (i != j && atual == jogadas[j].c.x.poder) {
                encontrado = 1;
                break;
            }
        }
        if (encontrado == 0 && atual > maximo) { // se nao achou poder repetido o indice vai para ele
            maximo = atual;
            indice_vencedor = i;
        }
    }
    return indice_vencedor; // retorna -1 se todas kangarem.
}

int dar_cartas_jogadores(baralho *b,lista_jogadores *LJ, int *qtd_cartas,int qtd_jog){
    jogador *temp = LJ->inicio;
    while(temp != NULL){        //percorrer todos os jogadores
        if(lcj_vazia(temp->mao)==0){
            libera_lcj(temp->mao);
            temp->mao = cria_lcj();   //inicializa a mao do jogador
        }
        for (int i = 0; i<*qtd_cartas;i++){
            carta_jogo *nova =(carta_jogo*)malloc(sizeof(carta_jogo));
            carta_jogo *aux = temp->mao->inicio;

            if(nova==NULL)
            return 0;
            nova->prox =NULL; //pois insere sempre na ultima posição
            desempilha_baralho(b,&nova->x); //o elemento novo "carta_jogo" recebe, em seu campo carta, a carta desempilhada do baralho embaralhado

            if(lcj_vazia(temp->mao)==1){
                temp->mao->inicio = nova; 
            }
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

int verif_vidas(lista_jogadores* LJ, FILE *arq,int qtd_jog){
    jogador* atual = LJ->inicio;
    jogador* ant = NULL;
    int eliminou =0; // essa função retornará o número de pessoas eliminadas e o código de erro será -1 ao inves de 0
    if(lista_jogador_vazia(LJ)==1)
        return -1;
    int i =0;
    
    if(lista_jogador_vazia(LJ))
        return -1;
    while(atual !=NULL){
        if(atual->acha_mata != atual->matou){
            atual->vidas--;
            fprintf(arq,"%s falou que matava %d mas matou %d e perdeu uma vida\n",atual->nome,atual->acha_mata,atual->matou);
            printf("%s falou que matava %d mas matou %d e perdeu uma vida\n",atual->nome,atual->acha_mata,atual->matou);
            if(atual->vidas == 0){
                eliminou++;
                if(LJ->inicio == atual){    
                    //caso for remover o primeiro elemento
                    LJ->inicio = atual->prox;
                }else if(atual->prox == NULL){
                    ant->prox = NULL; // atual é o último elemento
                }else{
                    ant->prox= atual->prox;
                }
                fprintf(arq,"%s zerou as vidas e foi ELIMINADO.\n",atual->nome);
                printf("%s zerou as vidas e foi ELIMINADO.\n",atual->nome);
            }
           
        }
        ant = atual;
        atual = atual->prox;
        i++;
    }
    return eliminou;
}
//menu
int menu(lista_jogadores *LJ, int *qtd_jog){
    int opcao;
    int registrou =0;
    int qtd = 0;
    jogador x;
    printf("Ola, bem-vindo(a) ao jogo de cartas FDP, por favor escolha uma das opcoes abaixo: \n");
    do{
        printf("Pressione '1' se voce nao sabe as regras do jogo.\n");
        printf("Pressione '2' para registrar um ou mais jogadores.\n");
        printf("Pressione '0' para comecar a jogar!\n");
        scanf("%d",&opcao);

        switch (opcao){
            case 0:
                if(registrou == 0){
                    printf("Voce deve registrar ao menos dois jogadores antes de iniciar o jogo! \n\n");
                    opcao =2;
                }
            break;
            
            case 1:
                printf("As regras do jogo FDP sao as seguintes: \n");
                printf("O jogo de cartas Fedaput* eh um jogo baseado no jogo de cartas muito conhecido denominado Truco. Este jogo utiliza-se de um baralho, mas nao por completo. Sao usadas as seguintes cartas:\n");
                printf("AS (11) (4 naipes)\n DOIS (12) (4 naipes)\n TRES (13) (4 naipes)\n QUATRO (4 naipes)\n CINCO (4 naipes)\n SEIS (4 naipes)\n SETE (4 naipes)\n DAMA (8) (4 naipes)\n VALETE (9) (4 naipes)\n REI (10) (4 naipes)\n");
                printf("Para se jogar o jogo Fedaput*, eh necessario apenas saber quais as cartas sao jogadas no Truco, e principalmente quais as regras de valoracao das cartas.\nDesta forma, a seguir sera apresentada a sequencia de valor das cartas no Truco.\n");
                printf("Manilhas - Sao fixas. Da mais forte para a mais fraca: 4 de Paus / 7 de Copas / as de Espadas / 7 de Ouros;\n");
                printf("Ordem das cartas (da menor para maior): 4, 5, 6, 7, Q, J, K, A, 2, 3\n");
            break;

            case 2:
                registrou = 1;
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
                else if(qtd>=8){
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
int rodada(lista_jogadores *LJ, baralho *b,int *qtd_cartas, int *nmr_rodada, int *qtd_jog, FILE *arq){
    jogador *percorre = LJ->inicio; //acessar os jogadores
    carta_jogo *aux = (carta_jogo*)malloc(sizeof(carta_jogo)); // mostrar a mao do jogador
    aux->prox =NULL;
    int soma_chutes = 0; //soma da quantidade de rodadas que os jogadores acham que fazem.
    int i = 1;
    int posicao;
    
    if(arq==NULL)
        return -1;
    printf("\n\n --------------- Comecando o Jogo -------------------\n\n");
    jogada *jogadas;
    int indice_v=0;
    int quantia_cartas=0;
    int z=0;  //variavel pra controla o indice do vetor jogadas.
    int k=0;
    int incremento=-1;
    // a cada rodada, cada jogador vai ver suas cartas e jogar uma por vez. depois q todos os jogadores jogarem 1 carta, aquele que matou a rodada é quem começara jogando na proxima,
    printf("LEMBRETE!\n Para facilitar a realizacao do trabalho e da jogatina as cartas foram transformadas em numeros, e o poder das cartas cresce junto com o numero delas, logo, a carta 9 mata a 8 e assim por diante\nentretanto, as manilhas do truco mineiro foram mantidas e por isso, para essas 4 cartas a ordem numerica nao e valida e mantem-se a ordem do truco.\n");
    for(int rnd = 0; rnd <RODADAS;rnd++){
        aux = percorre->mao->inicio;
        fprintf(arq,"\n ========== COMECANDO A %d RODADA ==========\n",rnd+1);
        printf("\n\n ========== COMECANDO A %d RODADA ==========\n\n",rnd+1);
        // for pra fazer a leitura de chutes
        for(int j = 0; j< *qtd_jog;j++){
            printf("--------------- %s, veja suas cartas: --------------- \n\n",percorre->nome);
            i=1;
            while(aux!=NULL){
                printf("Posicao %d: %c de %s.\n",i,aux->x.face,aux->x.naipe);
                i++;
                aux = aux->prox;
            }
            printf("\n\n");
            if(j == *qtd_jog - 1){
                do{
                    printf("%s, Quantas rodadas voce acha que mata? \n",percorre->nome);
                    scanf("%d",&percorre->acha_mata);
                }while(percorre->acha_mata < 0 ||percorre->acha_mata > *qtd_cartas || soma_chutes + percorre->acha_mata == *qtd_cartas); //condições para o ultimo jogador a falar quantas mata
                soma_chutes += percorre->acha_mata;
            }else{
                do{
                    printf("%s, Quantas rodadas voce acha que mata? \n\n",percorre->nome);
                    scanf("%d",&percorre->acha_mata);
                }while(percorre->acha_mata < 0 ||percorre->acha_mata > *qtd_cartas);
                soma_chutes += percorre->acha_mata;
            }
            percorre->matou =0; // zera quantas ele matou para começar a nova rodada.
            percorre = percorre->prox;
            if(percorre== NULL) // faz o percorre voltar para o inicio apos chegar no ultimo jogador da lista
                percorre = LJ->inicio;
            aux = percorre->mao->inicio;
        }
            soma_chutes =0; // zera pra proxima rodada
        for (int r = 0; r < *qtd_cartas;r++){ //r == auxiliar
            z = 0;
            k =0;
            jogadas = (jogada*)malloc(*qtd_jog*sizeof(jogada));
            for( k = 0; k< *qtd_jog ; k++){
                i = 1;
                printf("--------------- %s (%d vidas), veja suas cartas: --------------- \n\n",percorre->nome,percorre->vidas);
                while(aux!=NULL){
                    printf("Posicao %d: %c de %s.\n",i,aux->x.face,aux->x.naipe);
                    aux = aux->prox;                    
                    i+=1;
                    }
                printf("\n--------------- %s , informe a posicao da carta que deseja jogar: --------------- \n\n",percorre->nome);
                do{
                    posicao =0;
                    scanf("%d",&posicao);
                    if(posicao<1 || posicao>=i)
                        printf("Posicao invalida, escolha um dos numeros acima.\n");
                }while(posicao<1 || posicao >=i);
                
                jogadas[z].jog = percorre; // primeiro jogador a jogar vai pro indice 0 do array de jogadas.
                remove_carta_jogo(percorre,posicao,&jogadas[z].c.x);
                fprintf(arq," ========== %s jogou %c de %s ==========\n",jogadas[z].jog->nome,jogadas[z].c.x.face,jogadas[z].c.x.naipe);
                z++;
                percorre = percorre->prox;
                if(percorre == NULL)
                    percorre = LJ->inicio;
                aux = percorre->mao->inicio;
            }
            indice_v = verifica_vencedor_rodada(jogadas, *qtd_jog);

            if(indice_v==-1){
                fprintf(arq," ---------- todas as cartas empataram, ninguem venceu essa rodada ----------\n");
                printf("\n ---------- todas as cartas empataram, ninguem venceu essa rodada ----------\n");
            }else{
                fprintf(arq, "--------------- %s ganhou essa rodada.--------------- \n",jogadas[indice_v].jog->nome);
                printf("\n--------------- %s ganhou essa rodada.--------------- \n\n",jogadas[indice_v].jog->nome);
                percorre = jogadas[indice_v].jog;
                aux = percorre->mao->inicio;
                free(jogadas);
            }
        }
        *qtd_jog-=verif_vidas(LJ,arq,*qtd_jog); //remove quantos jogadores forem eliminados.
            
        if(*qtd_jog ==1){
            fprintf(arq,"==========O jogador %s foi o ultimo restante e venceu o jogo com %d vidas restantes!!\n==========\n ",LJ->inicio->nome,LJ->inicio->vidas);
            printf("==========\nO jogador %s foi o ultimo restante e venceu o jogo com %d vidas restantes!!\n==========\n ",LJ->inicio->nome,LJ->inicio->vidas);
            return 1;
        }else if(*qtd_jog==0){
            fprintf(arq,"TODOS OS JOGAODRES ZERARAM A VIDA E POR ISSO NINGUEM GANHOU\n");
            printf("TODOS OS JOGADORES ZERARAM A VIDA E POR ISSO NENHUM GANHOU\n");
            return 1;
        }
        *nmr_rodada +=1;
        
        *qtd_cartas += incremento;  // faz a quantiade de cartas ir de 5 ate 1 e depois de 1 ate 5 novamente
        if(*qtd_cartas <1){     
            *qtd_cartas =1;
            incremento = 1;
        }
        if(*qtd_cartas>5){
            *qtd_cartas =5;
            incremento =-1;
        }

        //esse pedaço de codigo está assim pois nao consegui passar por referencia a variavel *qtd_cartas. acho que devido à ela ja ter sido passada por referencia para a função rodada.
        quantia_cartas = *qtd_cartas;
        while (percorre!=NULL){
                percorre->mao->inicio=NULL;
                percorre = percorre->prox;
            }
        dar_cartas_jogadores(b,LJ,&quantia_cartas,*qtd_jog);  
        percorre = LJ->inicio;
        i=1;
        //esse while é oq faz cada hora um jogador começar jogando.
        while(i<*nmr_rodada){
            percorre = percorre->prox;
            if(percorre == NULL)
                percorre = LJ->inicio;
            i++;
        }  
    }
    return 1;
}
