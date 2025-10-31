#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5

// ----- Estruturas -----
typedef struct {
    char tipo; // 'I', 'O', 'T', 'L'
    int  id;   // identificador único sequencial
} Peca;

typedef struct {
    Peca v[TAM_FILA];
    int frente;  // índice do primeiro elemento
    int tras;    // índice da posição livre após o último
    int qtd;     // quantidade atual
} Fila;

// ----- Geração de peça -----
Peca gerarPeca(int id) {
    static const char tipos[4] = {'I', 'O', 'T', 'L'};
    Peca p;
    p.tipo = tipos[rand() % 4];
    p.id   = id;
    return p;
}

// ----- Fila: utilitários -----
void inicializarFila(Fila *f) {
    f->frente = 0;
    f->tras   = 0;
    f->qtd    = 0;
}

int filaVazia(const Fila *f) { return f->qtd == 0; }
int filaCheia(const Fila *f) { return f->qtd == TAM_FILA; }

int enqueue(Fila *f, Peca p) {
    if (filaCheia(f)) return 0;
    f->v[f->tras] = p;
    f->tras = (f->tras + 1) % TAM_FILA;
    f->qtd++;
    return 1;
}

int dequeue(Fila *f, Peca *out) {
    if (filaVazia(f)) return 0;
    if (out) *out = f->v[f->frente];
    f->frente = (f->frente + 1) % TAM_FILA;
    f->qtd--;
    return 1;
}

void mostrarFila(const Fila *f) {
    printf("\n=== Fila de Peças ===\n");
    if (filaVazia(f)) { puts("(vazia)"); return; }
    int idx = f->frente;
    for (int c = 0; c < f->qtd; c++) {
        printf("[%c %d] ", f->v[idx].tipo, f->v[idx].id);
        idx = (idx + 1) % TAM_FILA;
    }
    putchar('\n');
}

int main(void) {
    srand((unsigned)time(NULL));

    Fila fila;
    inicializarFila(&fila);

    // Inicializa a fila com 5 peças geradas automaticamente
    int proximoId = 0;
    for (int i = 0; i < TAM_FILA; i++) {
        enqueue(&fila, gerarPeca(proximoId++));
    }

    int op;
    do {
        mostrarFila(&fila);
        puts("\nMenu (Nível Novato)");
        puts("1 - Jogar peça (dequeue)");
        puts("2 - Inserir nova peça (enqueue)");
        puts("0 - Sair");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) { puts("Entrada inválida."); return 0; }

        switch (op) {
            case 1: {
                Peca jogada;
                if (!dequeue(&fila, &jogada)) {
                    puts("Fila vazia! Nada para jogar.");
                } else {
                    printf("Jogando peça [%c %d]\n", jogada.tipo, jogada.id);
                    // Repor automaticamente para manter a fila com 5 (como pede o enunciado)
                    if (!filaCheia(&fila)) {
                        Peca nova = gerarPeca(proximoId++);
                        enqueue(&fila, nova);
                        printf("Reposição automática: [%c %d]\n", nova.tipo, nova.id);
                    }
                }
                break;
            }
            case 2: {
                if (filaCheia(&fila)) {
                    puts("Fila cheia! Não é possível inserir manualmente agora.");
                } else {
                    Peca nova = gerarPeca(proximoId++);
                    enqueue(&fila, nova);
                    printf("Nova peça inserida: [%c %d]\n", nova.tipo, nova.id);
                }
                break;
            }
            case 0:
                puts("Encerrando...");
                break;
            default:
                puts("Opção inválida.");
        }
    } while (op != 0);

    return 0;
}
