#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Palabras reservadas de C y su equivalente en espanol 

typedef struct {
    const char* clave;
    const char* espanol;
} PalabraClave;

static const PalabraClave CLAVES[] = {
    { "auto", "automatico" },
    { "break", "romper" },
    { "case", "caso" },
    { "char", "caracter" },
    { "const", "constante" },
    { "continue", "continuar" },
    { "default", "predeterminado" },
    { "do", "hacer" },
    { "double", "doble" },
    { "else", "sino" },
    { "enum", "enumeracion" },
    { "extern", "externo" },
    { "float", "flotante" },
    { "for", "para" },
    { "goto", "ir_a" },
    { "if", "si" },
    { "inline", "en_linea" },
    { "int", "entero" },
    { "long", "largo" },
    { "register", "registro" },
    { "restrict", "restringido" },
    { "return", "retornar" },
    { "short", "corto" },
    { "signed", "con_signo" },
    { "sizeof", "tamano_de" },
    { "static", "estatico" },
    { "struct", "estructura" },
    { "switch", "segun" },
    { "typedef", "definir_tipo" },
    { "union", "union" },
    { "unsigned", "sin_signo" },
    { "void", "vacio" },
    { "volatile", "volatil" },
    { "while", "mientras" },
    { "_Bool", "booleano" },
    { "_Complex", "complejo" },
    { "_Imaginary", "imaginario" },
    { "_Alignas", "alinear_como" },
    { "_Alignof", "alineacion_de" },
    { "_Atomic", "atomico" },
    { "_Generic", "generico" },
    { "_Noreturn", "sin_retorno" },
    { "_Static_assert", "verificacion_estatica" },
    { "_Thread_local", "hilo_local" }
};

#define NCLAVES ((int)(sizeof(CLAVES) / sizeof(CLAVES[0])))
#define MAX_HISTORIAL 20

typedef struct {
    int indice;
    int conteo;
    int primeraLinea;
} Encontrada;

typedef struct {
    char* reporte;
    char* traducido;
    int distintas;
    int apariciones;
    double micros;
} Analisis;

//  buffer de texto dinamico 

typedef struct {
    char* p;
    size_t n;
    size_t cap;
} Buf;

static void bufInit(Buf* b) {
    b->p = NULL;
    b->n = 0;
    b->cap = 0;
}

static void bufReservar(Buf* b, size_t extra) {
    size_t need = b->n + extra + 1;
    if (need <= b->cap) return;
    size_t cap = b->cap ? b->cap : 256;
    while (cap < need) cap *= 2;
    b->p = realloc(b->p, cap);
    b->cap = cap;
}

static void bufAgregarN(Buf* b, const char* s, size_t len) {
    bufReservar(b, len);
    memcpy(b->p + b->n, s, len);
    b->n += len;
    b->p[b->n] = '\0';
}

static void bufAgregar(Buf* b, const char* s) {
    bufAgregarN(b, s, strlen(s));
}

static void bufAgregarChar(Buf* b, char c) {
    bufAgregarN(b, &c, 1);
}

//  busqueda de hallazgos y armado del reporte 

static int buscarHallada(const Encontrada* halladas, int total, int indice) {
    for (int k = 0; k < total; k++)
        if (halladas[k].indice == indice) return k;
    return -1;
}

static char* construirReporte(const Encontrada* halladas, int distintas, int apariciones) {
    Buf reporte;
    bufInit(&reporte);
    char buf[256];
    snprintf(buf, sizeof buf, "Palabras reservadas encontradas: %d distintas (%d apariciones)\n",
             distintas, apariciones);
    bufAgregar(&reporte, buf);
    if (distintas == 0) {
        bufAgregar(&reporte, "No se encontraron palabras reservadas de C.\n");
    } else {
        for (int k = 0; k < distintas; k++) {
            const PalabraClave* pc = &CLAVES[halladas[k].indice];
            snprintf(buf, sizeof buf, "  %s -> %s (veces: %d, primera linea: %d)\n",
                     pc->clave, pc->espanol, halladas[k].conteo, halladas[k].primeraLinea);
            bufAgregar(&reporte, buf);
        }
    }
    return reporte.p;
}

static size_t finLinea(const char* codigo, size_t largo, size_t i) {
    while (i < largo && codigo[i] != '\n') i++;
    return i;
}

static size_t finBloque(const char* codigo, size_t largo, size_t i) {
    size_t j = i + 2;
    while (j < largo) {
        if (codigo[j] == '*' && j + 1 < largo && codigo[j + 1] == '/')
            return j + 2;
        j++;
    }
    return largo;
}

static size_t finCadena(const char* codigo, size_t largo, size_t i, int* saltos) {
    char comilla = codigo[i];
    size_t j = i + 1;
    while (j < largo) {
        if (codigo[j] == '\\' && j + 1 < largo) {
            j += 2;
            continue;
        }
        if (codigo[j] == '\n') (*saltos)++;
        if (codigo[j] == comilla) return j + 1;
        j++;
    }
    return largo;
}

static int buscarClave(const char* palabra, size_t len) {
    for (int k = 0; k < NCLAVES; k++)
        if (strlen(CLAVES[k].clave) == len &&
            strncmp(CLAVES[k].clave, palabra, len) == 0)
            return k;
    return -1;
}

// Escaner lexico: recorre el codigo con control de estados, copia lo que no
// debe traducir (preprocesador, comentarios y cadenas) y sustituye claves.
static Analisis analizar(const char* codigo, size_t largo) {
    Encontrada* halladas = malloc((size_t)NCLAVES * sizeof(Encontrada));
    int nhalladas = 0;
    Buf traducido;
    bufInit(&traducido);
    int apariciones = 0;
    int linea = 1;
    int inicio = 1; // solo espacios desde el inicio de la linea
    size_t i = 0;

    while (i < largo) {
        char c = codigo[i];
        if (c == '\n') {
            bufAgregarChar(&traducido, c);
            linea++;
            i++;
            inicio = 1;
            continue;
        }
        if (inicio && (c == ' ' || c == '\t' || c == '\r')) {
            bufAgregarChar(&traducido, c);
            i++;
            continue;
        }
        // las directivas del preprocesador se copian sin traducir
        if (inicio && c == '#') {
            size_t j = finLinea(codigo, largo, i);
            bufAgregarN(&traducido, codigo + i, j - i);
            i = j;
            inicio = 0;
            continue;
        }
        // comentario de linea
        if (c == '/' && i + 1 < largo && codigo[i + 1] == '/') {
            size_t j = finLinea(codigo, largo, i);
            bufAgregarN(&traducido, codigo + i, j - i);
            i = j;
            inicio = 0;
            continue;
        }
        // comentario de bloque
        if (c == '/' && i + 1 < largo && codigo[i + 1] == '*') {
            size_t j = finBloque(codigo, largo, i);
            for (size_t k = i; k < j; k++)
                if (codigo[k] == '\n') linea++;
            bufAgregarN(&traducido, codigo + i, j - i);
            i = j;
            inicio = 0;
            continue;
        }
        // cadena o caracter: se copian sin traducir
        if (c == '"' || c == '\'') {
            int saltos = 0;
            size_t j = finCadena(codigo, largo, i, &saltos);
            linea += saltos;
            bufAgregarN(&traducido, codigo + i, j - i);
            i = j;
            inicio = 0;
            continue;
        }
        // identificador o palabra reservada
        if (isalpha((unsigned char)c) || c == '_') {
            size_t ini = i;
            while (i < largo && (isalnum((unsigned char)codigo[i]) || codigo[i] == '_')) i++;
            size_t len = i - ini;
            int idx = buscarClave(codigo + ini, len);
            if (idx >= 0) {
                apariciones++;
                int pos = buscarHallada(halladas, nhalladas, idx);
                if (pos < 0) {
                    pos = nhalladas++;
                    halladas[pos].indice = idx;
                    halladas[pos].conteo = 0;
                    halladas[pos].primeraLinea = linea;
                }
                halladas[pos].conteo++;
                bufAgregar(&traducido, CLAVES[idx].espanol);
            } else {
                bufAgregarN(&traducido, codigo + ini, len);
            }
            inicio = 0;
            continue;
        }
        bufAgregarChar(&traducido, c);
        i++;
        inicio = 0;
    }

    Analisis a;
    a.reporte = construirReporte(halladas, nhalladas, apariciones);
    a.traducido = traducido.p;
    if (!a.traducido) {
        a.traducido = malloc(1);
        a.traducido[0] = '\0';
    }
    a.distintas = nhalladas;
    a.apariciones = apariciones;
    a.micros = 0.0;
    free(halladas);
    return a;
}

// archivo, historial y consola 

static char* leerArchivo(const char* ruta, size_t* largo) {
    FILE* f = fopen(ruta, "r");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long tam = ftell(f);
    if (tam < 0) { fclose(f); return NULL; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }
    char* buf = malloc((size_t)tam + 1);
    size_t leido = fread(buf, 1, (size_t)tam, f);
    fclose(f);
    buf[leido] = '\0';
    *largo = leido;
    return buf;
}

static const char* nombreBase(const char* ruta) {
    const char* p = ruta;
    for (const char* q = ruta; *q; q++)
        if (*q == '\\' || *q == '/') p = q + 1;
    return p;
}

static double ahoraMicros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

// historial: maximo 20 entradas
static void agregarLineaHistorial(char lineas[][512], int* total, const char* linea) {
    if (*total < MAX_HISTORIAL) {
        strcpy(lineas[(*total)++], linea);
        return;
    }
    for (int i = 1; i < MAX_HISTORIAL; i++) strcpy(lineas[i - 1], lineas[i]);
    strcpy(lineas[MAX_HISTORIAL - 1], linea);
}

static void escribirResultado(const Analisis* a, const char* nombreArchivo) {
    time_t ahora = time(NULL);
    struct tm* lt = localtime(&ahora);
    char sello[32];
    if (lt) strftime(sello, sizeof sello, "%Y-%m-%d %H:%M:%S", lt);
    else sello[0] = '\0';

    char linea[512];
    snprintf(linea, sizeof linea,
             "[%s] C | Archivo: %s | Palabras reservadas: %d distintas "
             "(%d apariciones) | Tiempo de analisis: %.3f microsegundos",
             sello, nombreArchivo, a->distintas, a->apariciones, a->micros);

    char lineas[MAX_HISTORIAL][512];
    int total = 0;
    FILE* in = fopen("resultado.txt", "r");
    if (in) {
        char l[512];
        while (fgets(l, sizeof l, in)) {
            if (l[0] != '[') continue;
            l[strcspn(l, "\r\n")] = '\0';
            agregarLineaHistorial(lineas, &total, l);
        }
        fclose(in);
    }
    agregarLineaHistorial(lineas, &total, linea);

    FILE* out = fopen("resultado.txt", "w");
    if (!out) return;
    for (int i = 0; i < total; i++) fprintf(out, "%s\n", lineas[i]);
    fclose(out);
}

static void presentarResultado(const Analisis* a, const char* nombre, size_t largo) {
    printf("Archivo: %s\n", nombre);
    printf("Caracteres analizados: %d\n", (int)largo);
    printf("Tiempo de analisis: %.3f microsegundos\n\n", a->micros);
    printf("%s", a->reporte);
    printf("\nPrograma traducido:\n%s", a->traducido);
    size_t lt = strlen(a->traducido);
    if (lt == 0 || a->traducido[lt - 1] != '\n') printf("\n");
}

static void pausaFinal(void) {
    printf("\nPresione Enter para salir...");
    fflush(stdout);
    getchar();
}

int main(void) {
    atexit(pausaFinal);
    char ruta[1024];
    printf("=== Traductor de palabras reservadas C (C) ===\n");
    printf("Archivo C (Enter para datos-prueba.c): ");
    fflush(stdout);
    if (!fgets(ruta, sizeof ruta, stdin)) return 0;
    ruta[strcspn(ruta, "\r\n")] = '\0';
    if (ruta[0] == '\0') snprintf(ruta, sizeof ruta, "%s", "datos-prueba.c");

    size_t largo = 0;
    char* codigo = leerArchivo(ruta, &largo);
    if (!codigo) {
        printf("Error al leer el archivo: %s\n", ruta);
        return 1;
    }

    const char* nombre = nombreBase(ruta);
    double t0 = ahoraMicros();
    Analisis a = analizar(codigo, largo);
    a.micros = ahoraMicros() - t0;

    presentarResultado(&a, nombre, largo);
    escribirResultado(&a, nombre);

    free(codigo);
    free(a.reporte);
    free(a.traducido);
    return 0;
}
