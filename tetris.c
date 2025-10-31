#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA   5
#define TAM_PILHA  3

/* ----------------- Tipos ----------------- */
typedef struct {
    char tipo; // 'I','O','T','L'
    int  id;   // identificador sequencial
} Peca;

typedef struct {
    Peca v[TAM_FILA];
    int frente;   // índice do primeiro
    int tras;     // índice da posição livre após o último
    int qtd;      // quantidade atual
} Fila;

typedef struct {
    Peca v[TAM_PILHA];
    int topo;     // tamanho atual (próxima posição livre); topo real = topo-1
} Pilha;

/* --------------- Geração ----------------- */
Peca gerarPeca(int *proximoId) {
    static const char tipos[4] = {'I','O','T','L'};
    Peca p;
    p.tipo = tipos[rand() % 4];
    p.id   = (*proximoId)++;
    return p;
}

/* ----------------- Fila ------------------ */
void filaInit(Fila *f){ f->frente=0; f->tras=0; f->qtd=0; }
int  filaVazia(const Fila *f){ return f->qtd==0; }
int  filaCheia(const Fila *f){ return f->qtd==TAM_FILA; }

int enqueue(Fila *f, Peca p){
    if (filaCheia(f)) return 0;
    f->v[f->tras] = p;
    f->tras = (f->tras + 1) % TAM_FILA;
    f->qtd++;
    return 1;
}

int dequeue(Fila *f, Peca *out){
    if (filaVazia(f)) return 0;
    if (out) *out = f->v[f->frente];
    f->frente = (f->frente + 1) % TAM_FILA;
    f->qtd--;
    return 1;
}

/* ----------------- Pilha ----------------- */
void pilhaInit(Pilha *p){ p->topo=0; }
int  pilhaVazia(const Pilha *p){ return p->topo==0; }
int  pilhaCheia (const Pilha *p){ return p->topo==TAM_PILHA; }

int push(Pilha *p, Peca x){
    if (pilhaCheia(p)) return 0;
    p->v[p->topo++] = x;
    return 1;
}

int pop(Pilha *p, Peca *out){
    if (pilhaVazia(p)) return 0;
    if (out) *out = p->v[--p->topo];
    else     --p->topo;
    return 1;
}

/* ------------- Exibição ------------------ */
void mostrarFila(const Fila *f){
    printf("\n=== Fila (frente → tras) ===\n");
    if (filaVazia(f)) { puts("(vazia)"); return; }
    int idx = f->frente;
    for (int i=0; i<f->qtd; i++){
        printf("[%c %d] ", f->v[idx].tipo, f->v[idx].id);
        idx = (idx + 1) % TAM_FILA;
    }
    putchar('\n');
}

void mostrarPilha(const Pilha *p){
    printf("=== Pilha de Reserva (base → topo) ===\n");
    if (pilhaVazia(p)) { puts("(vazia)"); return; }
    for (int i=0; i<p->topo; i++){
        printf("[%c %d] ", p->v[i].tipo, p->v[i].id);
    }
    printf("<- topo\n");
}

/* Mantém a fila sempre cheia (5 peças) repondo com gerarPeca() */
void reporAteCheia(Fila *f, int *proximoId){
    while (!filaCheia(f)){
        Peca nova = gerarPeca(proximoId);
        enqueue(f, nova);
        printf("Reposição automática: [%c %d]\n", nova.tipo, nova.id);
    }
}

/* --------- Operações de troca avançadas --------- */

/* 4) Trocar frente da fila com topo da pilha */
int trocarFrenteComTopo(Fila *f, Pilha *p){
    if (filaVazia(f) || pilhaVazia(p)) return 0;
    int idxFrente = f->frente;
    int idxTopo   = p->topo - 1;
    Peca tmp = f->v[idxFrente];
    f->v[idxFrente] = p->v[idxTopo];
    p->v[idxTopo]   = tmp;
    return 1;
}

/* 5) Trocar 3 primeiros da fila com as 3 peças da pilha */
int trocarTres(Fila *f, Pilha *p){
    if (p->topo != 3 || f->qtd < 3) return 0;
    for (int i=0; i<3; i++){
        int idxFila = (f->frente + i) % TAM_FILA;
        int idxPilha = i; // base=0,1,2 (topo-1=2)
        Peca tmp = f->v[idxFila];
        f->v[idxFila] = p->v[idxPilha];
        p->v[idxPilha] = tmp;
    }
    return 1;
}

/* -------------------- Main -------------------- */
int main(void){
    srand((unsigned)time(NULL));

    Fila fila; Pilha reserva;
    filaInit(&fila); pilhaInit(&reserva);

    int proximoId = 0;

    /* Inicializa a fila com 5 peças */
    for (int i=0; i<TAM_FILA; i++){
        enqueue(&fila, gerarPeca(&proximoId));
    }

    int op;
    do {
        mostrarFila(&fila);
        mostrarPilha(&reserva);

        puts("\nMenu (Nível Mestre)");
        puts("1 - Jogar peça (dequeue)");
        puts("2 - Reservar peça (fila -> pilha)");
        puts("3 - Usar peça reservada (pop)");
        puts("4 - Trocar frente da fila com topo da pilha");
        puts("5 - Trocar 3 primeiros da fila com as 3 da pilha");
        puts("0 - Sair");
        printf("Escolha: ");

        if (scanf("%d", &op) != 1){
            puts("Entrada inválida. Encerrando.");
            return 0;
        }

        switch (op){
            case 1: { /* Jogar da fila */
                Peca jogada;
                if (!dequeue(&fila, &jogada)){
                    puts("Fila vazia!");
                } else {
                    printf("Jogando peça: [%c %d]\n", jogada.tipo, jogada.id);
                    reporAteCheia(&fila, &proximoId);
                }
            } break;

            case 2: { /* Reservar: frente da fila -> pilha */
                if (pilhaCheia(&reserva)){
                    puts("Reserva cheia (capacidade 3).");
                    break;
                }
                Peca frente;
                if (!dequeue(&fila, &frente)){
                    puts("Fila vazia!");
                    break;
                }
                push(&reserva, frente);
                printf("Reservada: [%c %d]\n", frente.tipo, frente.id);
                reporAteCheia(&fila, &proximoId);
            } break;

            case 3: { /* Usar peça reservada (pop) */
                Peca usada;
                if (!pop(&reserva, &usada)){
                    puts("Não há peça reservada.");
                } else {
                    printf("Usando peça reservada: [%c %d]\n", usada.tipo, usada.id);
                }
            } break;

            case 4: { /* Trocar frente <-> topo */
                if (trocarFrenteComTopo(&fila, &reserva))
                    puts("Troca realizada: frente da fila ↔ topo da pilha.");
                else
                    puts("Não foi possível trocar (verifique se fila/pilha possuem itens).");
            } break;

            case 5: { /* Trocar 3 primeiros da fila com as 3 da pilha */
                if (trocarTres(&fila, &reserva))
                    puts("Troca de trio realizada: 3 primeiros da fila ↔ 3 da pilha.");
                else
                    puts("Não foi possível trocar (requer pilha com 3 e fila com ≥ 3).");
            } break;

            case 0:
                puts("Encerrando...");
                break;

            default:
                puts("Opção inválida.");
        }

    } while (op != 0);

    return 0;
}
