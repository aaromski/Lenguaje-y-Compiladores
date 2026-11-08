#include <stdio.h>
#include <stdlib.h>
//int for while struct
#define MAX 100

typedef struct {
    int integer;           // identificador que "int"
    float formato;         // identificador que "for"
    char *cadena;
} Datos;

int main(void) {
    int n = 5;
    char *texto = "int for while return";   // cadena con trampa
    double suma = 0.0;
    const int LIMITE = MAX;
    static unsigned long contador = 0;

    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            suma += i;
            continue;
        } else {
            suma -= i;
        }
        contador++;
    }

    while (contador > 0) {
        contador--;
        switch ((int)contador % 3) {
            case 0: break;
            case 1: continue;
            default: goto fin;
        }
    }
fin:
    printf("%f %s\n", suma, texto);
    return (int)sizeof(Datos);
}
