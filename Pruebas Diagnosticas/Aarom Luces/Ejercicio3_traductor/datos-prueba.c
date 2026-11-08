/* Programa de prueba: usa varias palabras reservadas de C.
   Los comentarios y las cadenas no deben traducirse:
   int for while struct return */
#include <stdio.h>
#include <stdlib.h>

typedef struct Nodo {
    int valor;
    struct Nodo *siguiente;
} Nodo;

static int contar(const Nodo *cabeza) {
    int total = 0;
    const Nodo *actual = cabeza;
    while (actual != NULL) {
        total += actual->valor;
        actual = actual->siguiente;
    }
    return total;
}

int main(void) {
    Nodo *cabeza = NULL;
    unsigned int i;
    char letra = 'K';
    double promedio = 0.0;
    for (i = 0; i < 10; i++) {
        Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
        if (nuevo == NULL) {
            break;
        }
        nuevo->valor = (int) i;
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
    }
    if (cabeza != NULL) {
        promedio = (double) contar(cabeza) / 10.0;
        printf("Letra: %c, promedio: %f\n", letra, promedio);
    } else {
        goto fin;
    }
fin:
    return 0;
}
