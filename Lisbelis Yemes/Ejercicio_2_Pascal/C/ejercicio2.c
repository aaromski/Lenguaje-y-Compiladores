/*
 *UNEG - Lenguajes y Compiladores (2026-II)
 *Profesor: Msc. Félix Márquez
 * Estudiante: Lisbelis del Valle Yemes Espinoza
 * Sección: 3 | Grupo: Sintaxis
 * Actividad: Prueba Diagnóstica - Ejercicio 2 - C: Triángulo de Pascal
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define BASE 1000000000U

/* Cada posicion guarda nueve digitos. El arreglo crece cuando el numero necesita mas memoria*/
typedef struct {
    uint32_t *digitos;
    size_t cantidad;
    size_t capacidad;
} EnteroGrande;

static void entero_inicializar(EnteroGrande *numero)
{
    numero->digitos = NULL;
    numero->cantidad = 0;
    numero->capacidad = 0;
}

static void entero_liberar(EnteroGrande *numero)
{
    free(numero->digitos);
    entero_inicializar(numero);
}

/* Amplía el arreglo de dígitos cuando una operación necesita más espacio. */
static int entero_reservar(EnteroGrande *numero, size_t capacidad)
{
    uint32_t *nuevo;
    if (capacidad <= numero->capacidad) return 1;
    nuevo = (uint32_t *)realloc(numero->digitos, capacidad * sizeof(uint32_t));
    if (nuevo == NULL) return 0;
    numero->digitos = nuevo;
    numero->capacidad = capacidad;
    return 1;
}

/* Convierte un valor pequeño a la representación dinámica usada por C. */
static int entero_fijar_uint64(EnteroGrande *numero, uint64_t valor)
{
    if (!entero_reservar(numero, 3)) return 0;
    numero->cantidad = 0;
    do {
        numero->digitos[numero->cantidad++] = (uint32_t)(valor % BASE);
        valor /= BASE;
    } while (valor != 0);
    return 1;
}

static int entero_copiar(EnteroGrande *destino, const EnteroGrande *origen)
{
    if (!entero_reservar(destino, origen->cantidad)) return 0;
    memcpy(destino->digitos, origen->digitos,
           origen->cantidad * sizeof(uint32_t));
    destino->cantidad = origen->cantidad;
    return 1;
}

static int entero_sumar(EnteroGrande *a, const EnteroGrande *b)
{
    size_t i, mayor = a->cantidad > b->cantidad ? a->cantidad : b->cantidad;
    uint64_t acarreo = 0;

    if (!entero_reservar(a, mayor + 1)) return 0;
    for (i = 0; i < mayor || acarreo != 0; i++) {
        uint64_t suma = acarreo;
        if (i < a->cantidad) suma += a->digitos[i];
        if (i < b->cantidad) suma += b->digitos[i];
        a->digitos[i] = (uint32_t)(suma % BASE);
        acarreo = suma / BASE;
    }
    if (i > a->cantidad) a->cantidad = i;
    return 1;
}

static int entero_multiplicar_pequeno(EnteroGrande *numero, uint32_t factor)
{
    size_t i;
    uint64_t acarreo = 0;
    if (!entero_reservar(numero, numero->cantidad + 1)) return 0;
    for (i = 0; i < numero->cantidad; i++) {
        uint64_t producto = (uint64_t)numero->digitos[i] * factor + acarreo;
        numero->digitos[i] = (uint32_t)(producto % BASE);
        acarreo = producto / BASE;
    }
    if (acarreo != 0) numero->digitos[numero->cantidad++] = (uint32_t)acarreo;
    return 1;
}
    
static int entero_multiplicar(const EnteroGrande *a, const EnteroGrande *b,
                              EnteroGrande *resultado)
{
    size_t i, j;
    uint64_t acarreo;
    EnteroGrande temporal;
    entero_inicializar(&temporal);

    if (!entero_reservar(&temporal, a->cantidad + b->cantidad + 1)) {
        entero_liberar(&temporal);
        return 0;
    }
    memset(temporal.digitos, 0,
           (a->cantidad + b->cantidad + 1) * sizeof(uint32_t));
    temporal.cantidad = a->cantidad + b->cantidad + 1;

    for (i = 0; i < a->cantidad; i++) {
        acarreo = 0;
        for (j = 0; j < b->cantidad || acarreo != 0; j++) {
            uint64_t producto = temporal.digitos[i + j] + acarreo;
            if (j < b->cantidad) producto +=
                (uint64_t)a->digitos[i] * b->digitos[j];
            temporal.digitos[i + j] = (uint32_t)(producto % BASE);
            acarreo = producto / BASE;
        }
    }
    while (temporal.cantidad > 1 &&
           temporal.digitos[temporal.cantidad - 1] == 0)
        temporal.cantidad--;

    entero_liberar(resultado);
    *resultado = temporal;
    return 1;
}

static int entero_potencia_uint(const EnteroGrande *base, size_t exponente,
                                EnteroGrande *resultado)
{
    size_t i;
    EnteroGrande producto;
    entero_inicializar(&producto);
    if (!entero_fijar_uint64(resultado, 1)) {
        entero_liberar(&producto);
        return 0;
    }
    for (i = 0; i < exponente; i++) {
        if (!entero_multiplicar(resultado, base, &producto) ||
            !entero_copiar(resultado, &producto)) {
            entero_liberar(&producto);
            return 0;
        }
    }
    entero_liberar(&producto);
    return 1;
}

static void entero_imprimir(const EnteroGrande *numero)
{
    size_t i = numero->cantidad;
    printf("%u", numero->digitos[i - 1]);
    while (i-- > 1) printf("%09u", numero->digitos[i - 1]);
}

static int entero_a_cadena(const EnteroGrande *numero, char **salida)
{
    size_t capacidad = numero->cantidad * 9 + 1;
    size_t i, posicion = 0;
    char *texto = (char *)malloc(capacidad);
    if (texto == NULL) return 0;
    posicion += (size_t)sprintf(texto + posicion, "%u",
                                numero->digitos[numero->cantidad - 1]);
    for (i = numero->cantidad - 1; i > 0; i--)
        posicion += (size_t)sprintf(texto + posicion, "%09u",
                                    numero->digitos[i - 1]);
    *salida = texto;
    return 1;
}

static int generar_fila_pascal(size_t exponente, EnteroGrande *coeficientes)
{
    size_t fila, posicion;
    if (!entero_fijar_uint64(&coeficientes[0], 1)) return 0;
    for (fila = 1; fila <= exponente; fila++) {
        if (!entero_fijar_uint64(&coeficientes[fila], 1)) return 0;
        for (posicion = fila - 1; posicion > 0; posicion--) {
            if (!entero_sumar(&coeficientes[posicion],
                              &coeficientes[posicion - 1]))
                return 0;
        }
    }
    return 1;
}

static void escribir_polinomio(const EnteroGrande *coeficientes,
                               size_t exponente_maximo)
{
    size_t exponente;
    char *numero;
    printf("Polinomio: ");
    for (exponente = exponente_maximo + 1; exponente-- > 0;) {
        if (exponente < exponente_maximo) printf(" + ");
        if (entero_a_cadena(&coeficientes[exponente], &numero)) {
            if (exponente == 0) printf("%s", numero);
            else {
                if (strcmp(numero, "1") != 0) printf("%s", numero);
                if (exponente == 1) printf("x");
                else printf("x^%zu", exponente);
            }
            free(numero);
        }
    }
    printf("\n");
}

static int leer_entero_no_negativo(const char *mensaje, EnteroGrande *resultado)
{
    char linea[256];
    size_t i;
    for (;;) {
        printf("%s", mensaje);
        if (!fgets(linea, sizeof linea, stdin)) return 0;
        i = 0;
        while (linea[i] == ' ' || linea[i] == '\t') i++;
        if (linea[i] == '-') {
            printf("No se aceptan numeros negativos. Escriba un entero >= 0.\n");
            continue;
        }
        entero_fijar_uint64(resultado, 0);
        if (linea[i] == '\n' || linea[i] == '\r' || linea[i] == '\0') {
            printf("Debe escribir un numero entero (sin letras ni decimales).\n");
            continue;
        }
        while (linea[i] >= '0' && linea[i] <= '9') {
            if (!entero_multiplicar_pequeno(resultado, 10) ||
                !entero_sumar(resultado, &(EnteroGrande){
                    (uint32_t[]){(uint32_t)(linea[i] - '0')}, 1, 1
                })) return 0;
            i++;
        }
        while (linea[i] == ' ' || linea[i] == '\t' ||
               linea[i] == '\n' || linea[i] == '\r') i++;
        if (linea[i] != '\0' || i == 0) {
            printf("Debe escribir un numero entero (sin letras ni decimales).\n");
            continue;
        }
        return 1;
    }
}

static int leer_exponente(size_t *exponente)
{
    EnteroGrande numero;
    entero_inicializar(&numero);
    if (!leer_entero_no_negativo("Potencia n debe ser un entero: ", &numero)) {
        entero_liberar(&numero);
        return 0;
    }
    if (numero.cantidad > 2) {
        printf("El valor de n es demasiado grande para reservar la fila.\n");
        entero_liberar(&numero);
        return 0;
    }
    *exponente = (size_t)numero.digitos[0];
    if (numero.cantidad == 2) *exponente += (size_t)numero.digitos[1] * BASE;
    entero_liberar(&numero);
    return 1;
}

static double ahora(void)
{
#ifdef _WIN32
    LARGE_INTEGER frecuencia, contador;
    QueryPerformanceFrequency(&frecuencia);
    QueryPerformanceCounter(&contador);
    return (double)contador.QuadPart / (double)frecuencia.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
#endif
}

static int calcular_por_pasos(const EnteroGrande *coeficientes,
                              size_t exponente_maximo,
                              const EnteroGrande *valor_x,
                              EnteroGrande *suma_final)
{
    size_t exponente, paso = 1;
    EnteroGrande potencia, termino;
    char *coeficiente_texto, *termino_texto, *suma_texto, *x_texto;
    entero_inicializar(&potencia);
    entero_inicializar(&termino);
    entero_inicializar(suma_final);
    if (!entero_fijar_uint64(suma_final, 0) ||
        !entero_a_cadena(valor_x, &x_texto)) return 0;

    for (exponente = exponente_maximo + 1; exponente-- > 0; paso++) {
        if (!entero_potencia_uint(valor_x, exponente, &potencia) ||
            !entero_multiplicar(&coeficientes[exponente], &potencia, &termino) ||
            !entero_sumar(suma_final, &termino) ||
            !entero_a_cadena(&coeficientes[exponente], &coeficiente_texto) ||
            !entero_a_cadena(&termino, &termino_texto) ||
            !entero_a_cadena(suma_final, &suma_texto)) {
            free(x_texto);
            entero_liberar(&potencia);
            entero_liberar(&termino);
            return 0;
        }
        printf("Paso %zu: %s * %s^%zu = %s   (suma parcial: %s)\n",
               paso, coeficiente_texto, x_texto, exponente,
               termino_texto, suma_texto);
        free(coeficiente_texto);
        free(termino_texto);
        free(suma_texto);
    }
    free(x_texto);
    entero_liberar(&potencia);
    entero_liberar(&termino);
    return 1;
}

static int preguntar_otro_calculo(void)
{
    char linea[128];
    int ch;
    for (;;) {
        printf("\nDesea calcular otro polinomio? (s = si, n = no): ");
        if (!fgets(linea, sizeof linea, stdin)) return 0;
        ch = tolower((unsigned char)linea[0]);
        if ((ch == 's' || ch == 'n') &&
            (linea[1] == '\n' || linea[1] == '\r'))
            return ch == 's';
        printf("Respuesta no valida. Escriba s o n.\n");
    }
}

static int ejecutar_calculo(void)
{
    size_t exponente, i;
    EnteroGrande valor_x, resultado, directo, valor_x_mas_uno, uno;
    EnteroGrande *coeficientes;
    double inicio, duracion;
    entero_inicializar(&valor_x);
    entero_inicializar(&resultado);
    entero_inicializar(&directo);
    entero_inicializar(&valor_x_mas_uno);
    entero_inicializar(&uno);

    if (!leer_exponente(&exponente) ||
        !leer_entero_no_negativo("Valor de x debe ser un entero no negativo: ", &valor_x)) {
        entero_liberar(&valor_x);
        return 1;
    }
    coeficientes = (EnteroGrande *)calloc(exponente + 1, sizeof(EnteroGrande));
    if (coeficientes == NULL) {
        entero_liberar(&valor_x);
        return 1;
    }
    for (i = 0; i <= exponente; i++) entero_inicializar(&coeficientes[i]);

    inicio = ahora();
    if (!generar_fila_pascal(exponente, coeficientes)) {
        for (i = 0; i <= exponente; i++) entero_liberar(&coeficientes[i]);
        free(coeficientes);
        entero_liberar(&valor_x);
        return 1;
    }
    duracion = ahora() - inicio;

    printf("\nRESULTADOS EN C\n\n");
    printf("Polinomio (x + 1)^%zu\n", exponente);
    printf("Coeficientes:");
    for (i = 0; i <= exponente; i++) {
        printf(" ");
        entero_imprimir(&coeficientes[i]);
    }
    printf("\n");
    escribir_polinomio(coeficientes, exponente);
    printf("\nCalculo por pasos con x = ");
    entero_imprimir(&valor_x);
    printf("\n");
    if (calcular_por_pasos(coeficientes, exponente, &valor_x, &resultado)) {
        printf("f(");
        entero_imprimir(&valor_x);
        printf(") = ");
        entero_imprimir(&resultado);
        printf("\n");
        entero_fijar_uint64(&uno, 1);
        if (entero_copiar(&valor_x_mas_uno, &valor_x) &&
            entero_sumar(&valor_x_mas_uno, &uno) &&
            entero_potencia_uint(&valor_x_mas_uno, exponente, &directo)) {
            printf("Comprobacion directa: (");
            entero_imprimir(&valor_x);
            printf(" + 1)^%zu = ", exponente);
            entero_imprimir(&directo);
            printf("\n");
        }
    }
    printf("\nMedicion de tiempo\n");
    printf("Tiempo para generar los coeficientes de n = %zu: %.9f segundos\n",
           exponente, duracion);

    for (i = 0; i <= exponente; i++) entero_liberar(&coeficientes[i]);
    free(coeficientes);
    entero_liberar(&valor_x);
    entero_liberar(&resultado);
    entero_liberar(&directo);
    entero_liberar(&valor_x_mas_uno);
    entero_liberar(&uno);
    return 0;
}

int main(void)
{
    do {
        if (ejecutar_calculo() != 0) return 1;
    } while (preguntar_otro_calculo());
    printf("\nPrograma finalizado.\n");
    return 0;
}
