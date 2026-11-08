/*
 * Archivo : ejercicio3.c
 * Curso   : Lenguajes y Compiladores - UNEG
 * Desc.   : Ejercicio 3 - Carga dinamica de un programa C, deteccion de
 *           palabras reservadas y traduccion a su equivalente en espanol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
  #define MKDIR(d) _mkdir(d)
  static void limpiar_pantalla(void) { system("cls"); }
#else
  #include <sys/stat.h>
  #define MKDIR(d) mkdir(d, 0777)
  static void limpiar_pantalla(void) { system("clear"); }
#endif

#define CYAN   "\x1b[96m"
#define GREEN  "\x1b[92m"
#define RED    "\x1b[91m"
#define YELLOW "\x1b[93m"
#define BOLD   "\x1b[1m"
#define RESET  "\x1b[0m"
#define CHECK  "\xE2\x9C\x93"
#define CROSS  "X"

#define CHUNK 1024

static void setup_consola(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

//Tabla de palabras reservadas (ANSI C)
typedef struct { const char *palabra; const char *trad; } Reservada;

static const Reservada TABLA[] = {
    { "auto",     "automatico"         }, { "break",    "romper"             },
    { "case",     "caso"               }, { "char",     "caracter"           },
    { "const",    "constante"          }, { "continue", "continuar"          },
    { "default",  "por_defecto"        }, { "do",       "hacer"              },
    { "double",   "doble"              }, { "else",     "si_no"              },
    { "enum",     "enumeracion"        }, { "extern",   "externo"            },
    { "float",    "flotante"           }, { "for",      "para"               },
    { "goto",     "ir_a"               }, { "if",       "si"                 },
    { "int",      "entero"             }, { "long",     "largo"              },
    { "register", "registro"           }, { "return",   "retornar"           },
    { "short",    "corto"              }, { "signed",   "con_signo"          },
    { "sizeof",   "tamano_de"          }, { "static",   "estatico"           },
    { "struct",   "estructura"         }, { "switch",   "selector"           },
    { "typedef",  "definicion_de_tipo" }, { "union",    "union"              },
    { "unsigned", "sin_signo"          }, { "void",     "vacio"              },
    { "volatile", "volatil"            }, { "while",    "mientras"           }
};
#define N_RES ((int)(sizeof(TABLA) / sizeof(TABLA[0])))

static int conteo[sizeof(TABLA) / sizeof(TABLA[0])];
static int ignoradas = 0;

static int buscar_reservada(const char *w, size_t len) {
    for (int k = 0; k < N_RES; k++)
        if (strlen(TABLA[k].palabra) == len && strncmp(TABLA[k].palabra, w, len) == 0)
            return k;
    return -1;
}

static int contar_en(const char *s, size_t len) {
    int total = 0;
    size_t i = 0;
    while (i < len) {
        if (isalpha((unsigned char)s[i]) || s[i] == '_') {
            size_t j = i;
            while (j < len && (isalnum((unsigned char)s[j]) || s[j] == '_')) j++;
            if (buscar_reservada(s + i, j - i) >= 0) total++;
            i = j;
        } else i++;
    }
    return total;
}

// Carga dinamica del archivo
char *cargar_archivo_dinamico(const char *ruta, long *tam_out, int *reallocs_out) {
    FILE *f = fopen(ruta, "rb");
    if (!f) return NULL;

    size_t cap = CHUNK, len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) { fclose(f); return NULL; }

    int reallocs = 0;
    size_t leidos;
    while ((leidos = fread(buf + len, 1, cap - len, f)) > 0) {
        len += leidos;
        if (len == cap) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap);
            if (!tmp) { free(buf); fclose(f); return NULL; }
            buf = tmp;
            reallocs++;
        }
    }
    fclose(f);

    char *tmp = (char *)realloc(buf, len + 1);
    if (tmp) buf = tmp;
    buf[len] = '\0';
    *tam_out = (long)len;
    *reallocs_out = reallocs;
    return buf;
}

// Bofer de salida
typedef struct { char *d; size_t len, cap; } Buf;

static void buf_init(Buf *b, size_t cap) {
    b->d = (char *)malloc(cap);
    b->len = 0; b->cap = cap;
    b->d[0] = '\0';
}
static void buf_append_n(Buf *b, const char *s, size_t n) {
    if (b->len + n + 1 > b->cap) {
        while (b->len + n + 1 > b->cap) b->cap *= 2;
        b->d = (char *)realloc(b->d, b->cap);
    }
    memcpy(b->d + b->len, s, n);
    b->len += n;
    b->d[b->len] = '\0';
}
static void buf_append_str(Buf *b, const char *s) { buf_append_n(b, s, strlen(s)); }


char *analizar_y_traducir(const char *src, int *total_res, int *distintas) {
    Buf out;
    buf_init(&out, 4096);
    memset(conteo, 0, sizeof(conteo));
    ignoradas = 0;

    size_t i = 0, n = strlen(src);
    while (i < n) {
        char c = src[i];

        if (c == '/' && i + 1 < n && src[i + 1] == '/') {
            size_t j = i;
            while (j < n && src[j] != '\n') j++;
            ignoradas += contar_en(src + i, j - i);
            buf_append_n(&out, src + i, j - i);
            i = j;
            continue;
        }

        if (c == '/' && i + 1 < n && src[i + 1] == '*') {
            size_t j = i + 2;
            while (j + 1 < n && !(src[j] == '*' && src[j + 1] == '/')) j++;
            j = (j + 1 < n) ? j + 2 : n;
            ignoradas += contar_en(src + i, j - i);
            buf_append_n(&out, src + i, j - i);
            i = j;
            continue;
        }

        if (c == '"') {
            size_t j = i + 1;
            while (j < n && src[j] != '"') { if (src[j] == '\\') j++; j++; }
            j = (j < n) ? j + 1 : n;
            ignoradas += contar_en(src + i, j - i);
            buf_append_n(&out, src + i, j - i);
            i = j;
            continue;
        }

        if (c == '\'') {
            size_t j = i + 1;
            while (j < n && src[j] != '\'') { if (src[j] == '\\') j++; j++; }
            j = (j < n) ? j + 1 : n;
            buf_append_n(&out, src + i, j - i);
            i = j;
            continue;
        }
        // Identificador/palabra reservada
        if (isalpha((unsigned char)c) || c == '_') {
            size_t j = i;
            while (j < n && (isalnum((unsigned char)src[j]) || src[j] == '_')) j++;
            int idx = buscar_reservada(src + i, j - i);
            if (idx >= 0) {
                conteo[idx]++;
                buf_append_str(&out, TABLA[idx].trad);
            } else {
                buf_append_n(&out, src + i, j - i);
            }
            i = j;
            continue;
        }

        buf_append_n(&out, &c, 1);
        i++;
    }

    int tot = 0, dis = 0;
    for (int k = 0; k < N_RES; k++) {
        tot += conteo[k];
        if (conteo[k] > 0) dis++;
    }
    *total_res = tot;
    *distintas = dis;
    return out.d;
}

int main(void) {
    setup_consola();
    limpiar_pantalla();
    MKDIR("resultados");

    printf(CYAN "=================================================================\n"
                "      UNEG - LENGUAJES Y COMPILADORES - PRUEBA DIAGNOSTICA\n"
                "=================================================================\n" RESET);
    printf(BOLD "EJERCICIO 3: TRADUCTOR DE PALABRAS RESERVADAS DE C\n\n" RESET);

    char ruta[256];
    while (1) {
        printf(BOLD " Carga dinamica del programa\n" RESET);
        printf("-----------------------------------------------------------------\n");
        printf("  Ruta del programa C a analizar (ENTER = datos/programa_prueba.c): ");
        fflush(stdout);
        char buf[256];
        if (!fgets(buf, sizeof buf, stdin)) break;
        buf[strcspn(buf, "\r\n")] = '\0';
        if (buf[0] == '\0') strcpy(ruta, "datos/programa_prueba.c");
        else strcpy(ruta, buf);

        long tam = 0; int reallocs = 0;
        char *src = cargar_archivo_dinamico(ruta, &tam, &reallocs);
        if (!src) {
            printf("  " RED "X No se pudo abrir el archivo: %s" RESET "\n\n", ruta);
            printf("  Desea intentar con otro archivo? (s/n): ");
            char r[8];
            if (!fgets(r, sizeof r, stdin) || (r[0] != 's' && r[0] != 'S')) break;
            printf("\n");
            continue;
        }
        printf("  " GREEN CHECK " Archivo cargado en memoria dinamica.\n" RESET);
        printf("  Bytes leidos: " CYAN "%ld" RESET "  |  reallocs de crecimiento: " CYAN "%d" RESET "\n", tam, reallocs);

        // Analisis y traduccion
        int total_res = 0, distintas = 0;
        char *traducido = analizar_y_traducir(src, &total_res, &distintas);

        printf("\n" BOLD " Palabras reservadas detectadas\n" RESET);
        printf("-----------------------------------------------------------------\n");
        if (total_res == 0) {
            printf("  El programa no contiene palabras reservadas de C.\n");
        } else {
            printf("| %-10s | %-20s | %-11s |\n", "Palabra", "Traduccion", "Apariciones");
            printf("+------------+----------------------+-------------+\n");
            for (int k = 0; k < N_RES; k++) {
                if (conteo[k] > 0)
                    printf("| %-10s | %-20s | %-11d |\n", TABLA[k].palabra, TABLA[k].trad, conteo[k]);
            }
            printf("+------------+----------------------+-------------+\n");
        }
        printf("  Total reservadas traducidas : " GREEN "%d" RESET "\n", total_res);
        printf("  Palabras distintas           : " GREEN "%d" RESET "\n", distintas);
        printf("  Falsos positivos evitados     : " YELLOW "%d" RESET " (en comentarios y cadenas)\n", ignoradas);

        // Traducido
        printf("\n" BOLD " Programa traducido\n" RESET);
        printf("-----------------------------------------------------------------\n");
        printf("%s\n", traducido);


        FILE *f = fopen("resultados/ej3_resultados_c.txt", "w");
        if (f) {
            fprintf(f, "Ejercicio 3 - Traductor de palabras reservadas de C\n");
            fprintf(f, "Archivo analizado: %s (%ld bytes)\n\n", ruta, tam);
            fprintf(f, "--- Reporte de palabras reservadas ---\n");
            for (int k = 0; k < N_RES; k++)
                if (conteo[k] > 0)
                    fprintf(f, "%-10s -> %-20s (%d apariciones)\n", TABLA[k].palabra, TABLA[k].trad, conteo[k]);
            fprintf(f, "Total traducidas: %d | Distintas: %d | Falsos positivos evitados: %d\n\n",
                    total_res, distintas, ignoradas);
            fprintf(f, "--- Programa traducido ---\n%s\n", traducido);
            fclose(f);
            printf(GREEN CHECK " Resultados guardados en resultados/ej3_resultados_c.txt\n" RESET);
        }

        free(src);
        free(traducido);

        printf("\n  Desea analizar otro programa? (s/n): ");
        char r[8];
        if (!fgets(r, sizeof r, stdin) || (r[0] != 's' && r[0] != 'S')) break;
        printf("\n");
        limpiar_pantalla();
        printf(CYAN "=================================================================\n"
                    "      UNEG - LENGUAJES Y COMPILADORES - PRUEBA DIAGNOSTICA\n"
                    "=================================================================\n" RESET);
        printf(BOLD "EJERCICIO 3: TRADUCTOR DE PALABRAS RESERVADAS DE C\n\n" RESET);
    }

    return 0;
}
