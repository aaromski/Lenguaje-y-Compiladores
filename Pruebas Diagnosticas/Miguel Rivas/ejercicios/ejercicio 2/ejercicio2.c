#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Calcula la fila n del triangulo de Pascal utilizando asignación dinámica de
// memoria -malloc/free en C
double* generar_coeficientes_pascal(int n) {
    double *fila = (double *)malloc((n + 1) * sizeof(double));
    if (fila == NULL) {
        printf("Error: No se pudo asignar memoria dinámica.\n");
        exit(1);
    }

    fila[0] = 1.0;
    for (int i = 1; i <= n; i++) {
        fila[i] = 0.0;
    }

    for (int i = 1; i <= n; i++) {
        for (int j = i; j > 0; j--) {
            fila[j] = fila[j] + fila[j - 1];
        }
    }

    return fila;
}

//Formatea e imprime en pantalla la expresión algebraica expandida del binomio (x+1)^n.
void mostrar_polinomi(double *coefs, int n) {
    printf("(x + 1)^%d = ", n);
    for (int i = 0; i <= n; i++) {
        int exp = n - i;
        double c = coefs[i];
        
        if (i > 0) printf(" + ");
        
        if (exp > 1) {
            printf("%.0flx^%d", c, exp);
        } else if (exp == 1) {
            printf("%.0flx", c);
        } else {
            printf("%.0f", c);
        }
    }
    printf("\n");
}

//Sustituye el valor de x término por término mostrando el desglose operacional y la suma total f(x).
void evaluar_paso_a_paso(double *coefs, int n, double x) {
    printf("\n--- Evaluacion paso a paso para x = %.1f de (x + 1)^%d ---\n", x, n);
    double resultado_total = 0.0;

    for (int i = 0; i <= n; i++) {
        int exp = n - i;
        double c = coefs[i];
        
        double x_pow = 1.0;
        for(int k = 0; k < exp; k++) x_pow *= x;

        double valor_termino = c * x_pow;
        resultado_total += valor_termino;

        printf("Termino %d: Coeficiente (%.0f) * (%.1f^%d = %.1f) = %.2f\n", 
               i + 1, c, x, exp, x_pow, valor_termino);
    }
    printf("Suma total de los terminos: f(%.1f) = %.2f\n", x, resultado_total);
}

int main(int argc, char *argv[]) {
    int n_usuario;
    double x_usuario;

    // Si recibe los argumentos desde el .bat, los usa directamente; si no, los pide por consola
    if (argc >= 3) {
        n_usuario = atoi(argv[1]);
        x_usuario = atof(argv[2]);
    } else {
        printf("=== EVALUACION DINAMICA DE POLINOMIOS (EJERCICIO 2 - C) ===\n");
        printf("Ingrese el grado del binomio (n >= 0): ");
        if (scanf("%d", &n_usuario) != 1 || n_usuario < 0) return 1;
        printf("Ingrese el valor de x para evaluar f(x): ");
        if (scanf("%lf", &x_usuario) != 1) return 1;
    }

    printf("\n[C] Ejecutando para n = %d, x = %.1f\n", n_usuario, x_usuario);
    double *coefs_usuario = generar_coeficientes_pascal(n_usuario);
    
    mostrar_polinomi(coefs_usuario, n_usuario);
    evaluar_paso_a_paso(coefs_usuario, n_usuario, x_usuario);
    free(coefs_usuario);

    // Benchmark automático para n = 100
    int n_bench = 100;
    int repeticiones = 10000;

    clock_t start = clock();
    for (int r = 0; r < repeticiones; r++) {
        double *coefs_100 = generar_coeficientes_pascal(n_bench);
        free(coefs_100);
    }
    clock_t end = clock();

    double tiempo_seg = (double)(end - start) / CLOCKS_PER_SEC;

    FILE *f = fopen("tiempos_ej2.txt", "w");
    if (f) {
        fprintf(f, "=== RESULTADOS BENCHMARK EJERCICIO 2 (C) ===\n");
        fprintf(f, "Tiempo para generar n=%d (%d veces): %.6f segundos\n", n_bench, repeticiones, tiempo_seg);
        fclose(f);
    }

    printf("[C] Benchmark n=%d completado. Tiempo guardado en 'tiempos_ej2.txt': %.6f s\n", n_bench, tiempo_seg);
    return 0;
}