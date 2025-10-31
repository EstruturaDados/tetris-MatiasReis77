#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA  5
#define TAM_PILHA 3

/* ----------------- Tipos ----------------- */
typedef struct {
    char tipo; // 'I', 'O', 'T', 'L'
    int  id;   // identificador único
} Peca;

typedef struct {
    Peca v[TAM_FILA];
    int frente;  // índice do primeiro elemento
    int tras;    // índice da posição livre após o último
    int qtd;     // quantidade atual
} Fila;

typedef struct {
    Peca v[TAM_PILHA];
    int topo;    // índice do próximo espaço livre (tamanho atual)
} Pilha;

/* --------------- Gerador ----------------- */
Peca gerarPeca(int *proximoId) {
    static const char tipos[4] = {'I','O','T','L'};
    Peca p;
    p.tipo = tipos[rand()%4];
    p.id   = (*proximoId)++;
    return p;
}

/* --------------- Fila -------------------- */
void filaInit(Fila *f){ f->frente=0; f->tras=0; f->qtd=0; }
int  filaVazia(const Fila *f){ return f->qtd==0; }
int  filaCheia(const Fila *f){ return f->qtd==TAM_FILA; }

int enqueue(Fila *f, Peca p){
    if(filaCheia(f)) return 0;
    f->v[f->tras]=p;
    f->tras=(f->tras+1)%TAM_FILA;
    f->qtd++;
    return 1;
}
int dequeue(Fila *f, Peca *out){
    if(filaVazia(f)) return 0;
    if(out) *out=f->v[f->frente];
    f->frente=(f->frente+1)%TAM_FILA;
    f->qtd--;
    return 1;
}

/* --------------- Pilha ------------------- */
void pilhaInit(Pilha *p){ p->topo=0; }
int  pilhaVazia(const Pilha *p){ return p->topo==0; }
int  pilhaCheia (const Pilha *p){ return p->topo==TAM_PILHA; }

int push(Pilha *p, Peca x){
    if(pilhaCheia(p)) return 0;
    p->v[p->topo++]=x;
    return 1;
}
int pop(Pilha *p, Peca *out){
    if(pilhaVazia(p)) return 0;
    if(out) *out=p->v[--p->topo];
    else p->topo--;
    return 1;
}

/* ------------- Exibição ------------------ */
void mostrarFila(const Fila *f){
    printf("\n=== Fila (frente → tras) ===\n");
    if(filaVazia(f)){ puts("(vazia)"); return; }
    int idx=f->frente;
    for(int i=0;i<f->qtd;i++){
        printf("[%c %d] ", f->v[idx].tipo, f->v[idx].id);
        idx=(idx+1)%TAM_FILA;
    }
    putchar('\n');
}
void mostrarPilha(const Pilha *p){
    printf("=== Pilha de Reserva (base → topo) ===\n");
    if(pilhaVazia(p)){ puts("(vazia)"); return; }
    for(int i=0;i<p->topo;i++){
        printf("[%c %d] ", p->v[i].tipo, p->v[i].id);
    }
    printf("<- topo\n");
}

/* Mantém a fila com 5 elementos */
void reporAteCheia(Fila *f, int *proximoId){
    while(!filaCheia(f)){
        Peca nova = gerarPeca(proximoId);
        enqueue(f,nova);
        printf("Reposição automática: [%c %d]\n", nova.tipo, nova.id);
    }
}

int main(void){
    srand((unsigned)time(NULL));

    Fila fila; Pilha reserva;
    filaInit(&fila); pilhaInit(&reserva);

    int proximoId=0;
    for(int i=0;i<TAM_FILA;i++) enqueue(&fila, gerarPeca(&proximoId));

    int op;
    do{
        mostrarFila(&fila);
        mostrarPilha(&reserva);

        puts("\nMenu (Nível Aventureiro)");
        puts("1 - Jogar peça (dequeue)");
        puts("2 - Reservar peça (push)");
        puts("3 - Usar peça reservada (pop)");
        puts("0 - Sair");
        printf("Escolha: ");
        if(scanf("%d",&op)!=1){ puts("Entrada inválida."); return 0; }

        switch(op){
            case 1: {
                Peca jogada;
                if(!dequeue(&fila,&jogada)) puts("Fila vazia!");
                else{
                    printf("Jogando peça da fila: [%c %d]\n", jogada.tipo, jogada.id);
                    reporAteCheia(&fila,&proximoId);
                }
            } break;
            case 2: {
                if(pilhaCheia(&reserva)){ puts("Reserva cheia! (capacidade 3)"); break; }
                Peca frente;
                if(!dequeue(&fila,&frente)){ puts("Fila vazia!"); break; }
                if(!push(&reserva,frente)){ puts("Falha ao reservar."); }
                else{
                    printf("Reservada: [%c %d]\n", frente.tipo, frente.id);
                    reporAteCheia(&fila,&proximoId);
                }
            } break;
            case 3: {
                Peca usada;
                if(!pop(&reserva,&usada)) puts("Não há peça reservada.");
                else printf("Usando peça reservada: [%c %d]\n", usada.tipo, usada.id);
            } break;
            case 0: puts("Encerrando..."); break;
            default: puts("Opção inválida.");
        }
    }while(op!=0);

    return 0;
}
