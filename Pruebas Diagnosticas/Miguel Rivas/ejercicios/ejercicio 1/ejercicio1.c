#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

//Traduce la letra de la columna de ajedrez (a-h) a un valor entero del 1 al 8.
int col_to_int(char c) {
    c = tolower(c);
    if (c >= 'a' && c <= 'h') return c - 'a' + 1;
    return 0;
}

//Convierte el carácter de la fila (1-8) a su equivalente numérico entero.
int row_to_int(char c) {
    if (c >= '1' && c <= '8') return c - '0';
    return 0;
}

//Comprueba la regla geométrica del salto del caballo evaluando variaciones absolutas de 2 y 1 casillas.
int es_movimiento_valido(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

//Procesa la cadena de texto paso a paso, validando formato, tableros y legalidad de cada salto.
int verificar_movimientos(const char* ak, int n, const char* cadena_c, char* mensaje_error) {
    int x_actual = col_to_int(ak[0]);
    int y_actual = row_to_int(ak[1]);

    if (!x_actual || !y_actual) {
        sprintf(mensaje_error, "Posicion inicial %s invalida", ak);
        return 0;
    }

    char buffer[16384]; // Crea un espacio de almacenamiento temporal (un búfer de 16 KB) 
    //lo suficientemente grande para contener la cadena masiva de los 500 movimientos.
    strncpy(buffer, cadena_c, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char* token = strtok(buffer, " \t\n\r");
    int i = 1;

    while (token != NULL && i <= n) {
        if (strlen(token) != 3) {
            sprintf(mensaje_error, "falla en el movimiento %d: formato incorrecto '%s'", i, token);
            return 0;
        }

        if (token[0] != 'K') {
            sprintf(mensaje_error, "falla letra %c no pertenece a un caballo", token[0]);
            return 0;
        }

        int x_sig = col_to_int(token[1]);
        int y_sig = row_to_int(token[2]);

        if (!x_sig || !y_sig) {
            sprintf(mensaje_error, "falla en el movimiento %d: casilla fuera de tablero '%s'", i, token);
            return 0;
        }

        if (!es_movimiento_valido(x_actual, y_actual, x_sig, y_sig)) {
            sprintf(mensaje_error, "falla en el movimiento %d", i);
            return 0;
        }

        x_actual = x_sig;
        y_actual = y_sig;
        token = strtok(NULL, " \t\n\r");
        i++;
    }

    return 1;
}

int main() {
    char err_msg[256];

    // Secuencia completa de 500 movimientos provista por el profesor
    const char* secuencia_500 = 
        "Kb4 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg1 Kh3 Kg5 Ke6 Kc7 Ka8 Kc7 Kb5 Kd6 Kb7 Ka5 Kc4 Ka3 Kc2 "
        "Ka1 Kc2 Kb4 Ka6 Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 "
        "Kc7 Kb5 Kd4 Kb3 Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 "
        "Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg1 Kh3 Kg5 Ke6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 "
        "Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd6 Kb7 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Ka6 Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 "
        "Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd4 Kb3 Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 "
        "Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg1 "
        "Kh3 Kg5 Ke6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd6 Kb7 Ka5 Kc4 Ka3 "
        "Kc2 Ka1 Kc2 Kb4 Ka6 Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 "
        "Ka8 Kc7 Kb5 Kd4 Kb3 Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 "
        "Kd5 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd4 Kb3 Ka1 "
        "Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 "
        "Kc3 Ka2 Kc1 Ke2 Kg1 Kh3 Kg5 Ke6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 "
        "Kd6 Kb7 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Ka6 Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 "
        "Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd4 Kb3 Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 "
        "Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg1 Kh3 Kg5 Ke6 Kc5 Ka4 "
        "Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd6 Kb7 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Ka6 "
        "Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd4 Kb3 "
        "Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 Ka6 Kc5 "
        "Ka4 Kc3 Ka2 Kc1 Ke2 Kg1 Kh3 Kg5 Ke6 Kc5 Ka4 Kc3 Ka2 Kc1 Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 "
        "Kb5 Kd6 Kb7 Ka5 Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Ka6 Kb8 Kd7 Kb6 Ka8 Kc7 Ka6 Kc5 Ka4 Kc3 Ka2 Kc1 "
        "Ke2 Kg3 Kh5 Kg7 Ke8 Kc7 Ka8 Kc7 Kb5 Kd4 Kb3 Ka1 Kc2 Ka3 Kc4 Ka5 Kc6 Ka7 Kc8 Ka7 Kc6 Ka5 "
        "Kc4 Ka3 Kc2 Ka1 Kc2 Kb4 Kd5 Kb6 Ka8 Kc7 Ka6 Kc9 Ka4";

    // Contar total de movimientos de manera dinámica
    char temp_buf[16384];
    strcpy(temp_buf, secuencia_500);
    int total_movs = 0;
    char* t = strtok(temp_buf, " \t\n\r");
    while(t != NULL) {
        total_movs++;
        t = strtok(NULL, " \t\n\r");
    }

    // Abrir archivo en modo "w" para que se borre y se escriba limpio con cada ejecucion
    FILE* f_res = fopen("resultados_ej1.txt", "w");
    if (!f_res) {
        printf("Error al crear el archivo de resultados.\n");
        return 1;
    }

    fprintf(f_res, "=== REPORTE DE VALIDACION Y BENCHMARK (EJERCICIO 1 - C) ===\n\n");
    
    // 1. Evaluar validez de la secuencia completa
    int es_valido = verificar_movimientos("a2", total_movs, secuencia_500, err_msg);
    if (es_valido) {
        fprintf(f_res, "Validacion de la secuencia (%d movimientos desde a2): MOVIMIENTO CORRECTO.\n", total_movs);
        printf("[C] Secuencia de %d movimientos validada: CORRECTO.\n", total_movs);
    } else {
        fprintf(f_res, "Validacion de la secuencia (%d movimientos desde a2): INCORRECTO -> %s\n", total_movs, err_msg);
        printf("[C] Secuencia de %d movimientos validada: INCORRECTO (%s)\n", total_movs, err_msg);
    }

   // 2. Ejecutar Benchmark de tiempos con alta precision nativa de Windows (QPC)
    LARGE_INTEGER frecuencia, t_inicio, t_fin;
    QueryPerformanceFrequency(&frecuencia);
    
    QueryPerformanceCounter(&t_inicio);

    for (int rep = 0; rep < 10; rep++) {
        verificar_movimientos("a2", total_movs, secuencia_500, err_msg);
    }

    QueryPerformanceCounter(&t_fin);

    double tiempo_seg = (double)(t_fin.QuadPart - t_inicio.QuadPart) / (double)frecuencia.QuadPart;
    double tiempo_us = tiempo_seg * 1e6;

    fprintf(f_res, "\n--- BENCHMARK (ALTA PRECISION NATIVA) ---\n");
    fprintf(f_res, "Tiempo para verificar 10 veces los %d movimientos: %.9f segundos (%.2f microsegundos)\n", 
            total_movs, tiempo_seg, tiempo_us);
    fclose(f_res);

    printf("[C] Benchmark de alta precision completado. Resultados guardados en 'resultados_ej1.txt'.\n");
    return 0;
    return 0;
}