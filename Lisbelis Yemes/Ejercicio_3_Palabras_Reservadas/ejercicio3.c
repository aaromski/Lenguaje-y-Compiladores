/*
 *UNEG - Lenguajes y Compiladores (2026-II)
 *Profesor: Msc. Félix Márquez
 * Estudiante: Lisbelis del Valle Yemes Espinoza
 * Sección: 3 | Grupo: Sintaxis
 * Actividad: Prueba Diagnóstica - Ejercicio 3 - C: Palabras Reservadas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* El programa escrito en C que se va a analizar puede estar en cualquier carpeta del equipo. Por ejemplo, para analizar el ejercicio2.c de este proyecto, se puede ejecutar desde esta carpeta: ejercicio3.exe "D:\Proyectos\PRUEBA DIAGNOSTICA\Lisbelis Yemes\Ejercicio_2_Pascal\C\ejercicio2.c" Las comillas son necesarias porque algunas carpetas pueden tener espacios. Si no se escribe la ruta como argumento, el programa la solicita por pantalla (ver pedir_ruta). */

/* Tabla con las 32 palabras reservadas de C (norma C89) y su equivalente en espanol */
typedef struct {
    const char *ingles;
    const char *espanol;
} Palabra;

const Palabra PALABRAS[] = {
    {"auto",     "automatica"},
    {"break",    "romper"},
    {"case",     "caso"},
    {"char",     "caracter"},
    {"const",    "constante"},
    {"continue", "continuar"},
    {"default",  "por_defecto"},
    {"do",       "hacer"},
    {"double",   "doble"},
    {"else",     "sino"},
    {"enum",     "enumeracion"},
    {"extern",   "externo"},
    {"float",    "flotante"},
    {"for",      "para"},
    {"goto",     "ir_a"},
    {"if",       "si"},
    {"int",      "entero"},
    {"long",     "largo"},
    {"register", "registro"},
    {"return",   "retornar"},
    {"short",    "corto"},
    {"signed",   "con_signo"},
    {"sizeof",   "tamano_de"},
    {"static",   "estatico"},
    {"struct",   "estructura"},
    {"switch",   "segun"},
    {"typedef",  "tipo_definido"},
    {"union",    "union"},
    {"unsigned", "sin_signo"},
    {"void",     "vacio"},
    {"volatile", "volatil"},
    {"while",    "mientras"}
};

#define TOTAL_PALABRAS ((int) (sizeof(PALABRAS) / sizeof(PALABRAS[0])))

int parte_palabra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}


/* Busca la palabra en la tabla. Devuelve su traduccion o NULL si no es reservada. */
const char *traducir_palabra(const char *palabra, size_t largo) {
    int k;
    for (k = 0; k < TOTAL_PALABRAS; k++) {
        if (strlen(PALABRAS[k].ingles) == largo &&
            strncmp(PALABRAS[k].ingles, palabra, largo) == 0) {
            return PALABRAS[k].espanol;
        }
    }
    return NULL;
}


/* CARGA DINAMICA*/
char *cargar_archivo(const char *ruta, size_t *tamano_salida) {
    FILE *archivo = fopen(ruta, "rb");
    long tamano;
    char *memoria;

    if (archivo == NULL) {
        printf("ERROR no se pudo abrir el archivo: %s (%s)\n", ruta, strerror(errno));
        return NULL;
    }

    if (fgetc(archivo) == EOF && ferror(archivo)) {
        printf("ERROR no se pudo leer el archivo: %s (%s)\n", ruta, strerror(errno));
        fclose(archivo);
        return NULL;
    }
    if (fseek(archivo, 0, SEEK_END) != 0) {
        printf("ERROR no se pudo medir el archivo: %s\n", ruta);
        fclose(archivo);
        return NULL;
    }
    tamano = ftell(archivo);
    if (tamano < 0) {
        printf("ERROR no se pudo medir el archivo: %s\n", ruta);
        fclose(archivo);
        return NULL;
    }
    rewind(archivo);

    memoria = (char *) malloc((size_t) tamano + 1);
    if (memoria == NULL) {
        printf("ERROR no hay memoria suficiente para cargar el archivo\n");
        fclose(archivo);
        return NULL;
    }
    if (fread(memoria, 1, (size_t) tamano, archivo) != (size_t) tamano) {
        printf("ERROR no se pudo leer el archivo completo: %s\n", ruta);
        free(memoria);
        fclose(archivo);
        return NULL;
    }
    fclose(archivo);

    memoria[tamano] = '\0';                
    *tamano_salida = (size_t) tamano;
    return memoria;
}


int factor_expansion(void) {
    int k;
    int factor = 1;
    for (k = 0; k < TOTAL_PALABRAS; k++) {
        int largo_ingles = (int) strlen(PALABRAS[k].ingles);
        int largo_espanol = (int) strlen(PALABRAS[k].espanol);
        int necesario = (largo_espanol + largo_ingles - 1) / largo_ingles;  
        if (necesario > factor) {
            factor = necesario;
        }
    }
    return factor;
}


void copiar(const char *programa, size_t *i, char *salida, size_t *j) {
    salida[*j] = programa[*i];
    *i = *i + 1;
    *j = *j + 1;
}


/* Recibe el programa ya cargado en memoria dinamica y devuelve la traduccion en otro bloque dinamico.*/ 
char *traducir_programa(const char *programa, size_t largo, unsigned long *encontradas, int mostrar) {
    size_t i = 0;                          
    size_t j = 0;                         
    
    int inicio_linea = 1;

    char *salida = (char *) malloc(largo * (size_t) factor_expansion() + 1);
    *encontradas = 0;
    if (salida == NULL) {
        return NULL;
    }

    while (i < largo) {
        char c = programa[i];

        if (c == '/' && i + 1 < largo && programa[i + 1] == '*') { //comentario de bloque-
            copiar(programa, &i, salida, &j);
            copiar(programa, &i, salida, &j);
            while (i < largo && !(programa[i] == '*' && i + 1 < largo && programa[i + 1] == '/')) {
                copiar(programa, &i, salida, &j);
            }
            if (i < largo) {
                copiar(programa, &i, salida, &j);
                copiar(programa, &i, salida, &j);
            }
            inicio_linea = 0;
            continue;
        }

        if (c == '/' && i + 1 < largo && programa[i + 1] == '/') { //comentario de linea
            while (i < largo && programa[i] != '\n') {
                if (programa[i] == '\\' && i + 1 < largo && programa[i + 1] == '\n') {
                    copiar(programa, &i, salida, &j);
                } else if (programa[i] == '\\' && i + 2 < largo &&
                           programa[i + 1] == '\r' && programa[i + 2] == '\n') {
                    copiar(programa, &i, salida, &j);
                    copiar(programa, &i, salida, &j);
                }
                copiar(programa, &i, salida, &j);
            }
            inicio_linea = 0;
            continue;
        }

        if (c == '"' || c == '\'') { //cadena entre comillas
            char comilla = c;
            copiar(programa, &i, salida, &j);
            while (i < largo && programa[i] != comilla && programa[i] != '\n') {
                if (programa[i] == '\\' && i + 1 < largo && programa[i + 1] != '\n') {
                    copiar(programa, &i, salida, &j);   
                }
                copiar(programa, &i, salida, &j);
            }
            if (i < largo && programa[i] == comilla) {
                copiar(programa, &i, salida, &j);
            }
            inicio_linea = 0;
            continue;
        }

        if (c == '#' && inicio_linea) { //linea #include
            size_t inicio_nombre;
            copiar(programa, &i, salida, &j);
            while (i < largo && (programa[i] == ' ' || programa[i] == '\t')) {
                copiar(programa, &i, salida, &j);
            }
            inicio_nombre = i;
            while (i < largo && parte_palabra(programa[i])) {
                copiar(programa, &i, salida, &j);
            }
            if (i - inicio_nombre == 7 && strncmp(programa + inicio_nombre, "include", 7) == 0) {
                while (i < largo && (programa[i] == ' ' || programa[i] == '\t')) {
                    copiar(programa, &i, salida, &j);
                }
                if (i < largo && programa[i] == '<') {
                    while (i < largo && programa[i] != '>' && programa[i] != '\n') {
                        copiar(programa, &i, salida, &j);
                    }
                    if (i < largo && programa[i] == '>') {
                        copiar(programa, &i, salida, &j);
                    }
                }
            }
            inicio_linea = 0;
            continue;
        }

        if (!parte_palabra(c)) {
            copiar(programa, &i, salida, &j);
            if (c == '\n') {
                inicio_linea = 1;
            } else if (c != ' ' && c != '\t' && c != '\r') {
                inicio_linea = 0;
            }
            continue;
        }

        {
            size_t inicio = i;
            size_t largo_palabra;
            const char *equivalente;

            while (i < largo && parte_palabra(programa[i])) {
                i = i + 1;                  /* la palabra termina aqui */
            }
            largo_palabra = i - inicio;
            inicio_linea = 0;

            equivalente = traducir_palabra(programa + inicio, largo_palabra);
            if (equivalente != NULL) {
                if (mostrar) {
                    printf("Palabra reservada encontrada: %.*s -> %s\n",
                           (int) largo_palabra, programa + inicio, equivalente);
                }
                *encontradas = *encontradas + 1;
                strcpy(salida + j, equivalente);
                j = j + strlen(equivalente);
            } else {
                memcpy(salida + j, programa + inicio, largo_palabra);
                j = j + largo_palabra;
            }
        }
    }
    salida[j] = '\0';

    return salida;
}


/* Pide por teclado la ruta del programa escrito en C que se va a analizar.*/ 
    int pedir_ruta(char *ruta, int largo) {
    int i = 0;

    printf("\nRuta (Enter sin escribir nada para salir): ");
    fflush(stdout);

    if (fgets(ruta, largo, stdin) == NULL) {
        return 0;
    }

    while (ruta[i] != '\0' && ruta[i] != '\n' && ruta[i] != '\r') {
        i = i + 1;
    }
    if (ruta[i] == '\0') {

        int ch, sobro = 0;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            sobro = 1;
        }
        if (sobro) {
            printf("ERROR la ruta es demasiado larga\n");
            return -1;
        }
    }
    ruta[i] = '\0';

    while (i > 0 && (ruta[i - 1] == ' ' || ruta[i - 1] == '\t')) {
        i = i - 1;
        ruta[i] = '\0';
    }
    if (ruta[0] == ' ' || ruta[0] == '\t') {
        int k = 0;
        while (ruta[k] == ' ' || ruta[k] == '\t') {
            k = k + 1;
        }
        memmove(ruta, ruta + k, (size_t) (i - k) + 1);
        i = i - k;
    }

    if (i >= 2 && ruta[0] == '"' && ruta[i - 1] == '"') {
        memmove(ruta, ruta + 1, (size_t) i - 2);
        ruta[i - 2] = '\0';
    }
    return ruta[0] != '\0';
}

/* Carga el programa en memoria dinamica y comprueba que sea texto. Devuelve NULL (explicando el motivo en pantalla) si no se puede usar. */
char *cargar_programa(const char *ruta, size_t *tamano) {
    char *programa = cargar_archivo(ruta, tamano);

    if (programa == NULL) {
        return NULL;
    }

    if (memchr(programa, '\0', *tamano) != NULL) {
        printf("ERROR el archivo no parece ser de texto (tiene bytes nulos): %s\n", ruta);
        free(programa);
        return NULL;
    }
    return programa;
}


/* Traduce el programa cargado y muestra el reporte en pantalla. Devuelve 0 si todo salio bien y 1 si no hubo memoria suficiente. */
int analizar_programa(const char *ruta, const char *programa, size_t tamano) {
    unsigned long encontradas = 0;
    char *traducido;

    printf("\nRESULTADOS EN C\n\n");
    printf("ARCHIVO ANALIZADO: %s\n\n", ruta);

    printf("PALABRAS RESERVADAS ENCONTRADAS\n");
    traducido = traducir_programa(programa, tamano, &encontradas, 1);
    if (traducido == NULL) {
        printf("ERROR no hay memoria suficiente\n");
        return 1;
    }
    if (encontradas == 0) {
        printf("No se encontro ninguna palabra reservada.\n");
    }

    printf("\nTotal de palabras reservadas encontradas: %lu\n", encontradas);

    /* Se libera la memoria dinamica donde se guardo la traduccion */
    free(traducido);
    return 0;
}


int preguntar_otro_archivo(void) {
    char linea[128];
    int ch;

    for (;;) {
        printf("\nDesea analizar otro archivo? (s = si, n = no): ");
        fflush(stdout);
        if (fgets(linea, sizeof linea, stdin) == NULL) {
            return 0;
        }
        if (strchr(linea, '\n') == NULL) {
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            printf("Respuesta no valida. Escriba s o n.\n");
            continue;
        }
        ch = linea[0];
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + ('a' - 'A');          /* a minuscula */
        }
        if ((ch == 's' || ch == 'n') && (linea[1] == '\n' || linea[1] == '\r')) {
            return ch == 's';
        }
        printf("Respuesta no valida. Escriba s o n.\n");
    }
}


int main(int argc, char *argv[]) {
    const char *argumento = (argc > 1) ? argv[1] : NULL;
    size_t tamano = 0;
    char entrada[4096];
    const char *ruta = NULL;
    char *programa;
    int respuesta, error;

    if (argumento == NULL) {
        printf("PROGRAMA ESCRITO EN C QUE SE VA A ANALIZAR\n");
        printf("Escriba la ruta del archivo, que puede estar en cualquier carpeta, por ejemplo:\n");
        printf("    C:\\Users\\Usuario\\Documentos\\programa.c\n");
        printf("    \"D:\\Proyectos\\PRUEBA DIAGNOSTICA\\Lisbelis Yemes\\Ejercicio_2_Pascal\\C\\ejercicio2.c\"\n");
    }

    do {
        programa = NULL;

        if (argumento != NULL) {
            ruta = argumento;
            argumento = NULL;
            /* 2) El programa escrito en C se carga en memoria de forma dinamica */
            programa = cargar_programa(ruta, &tamano);
            if (programa == NULL) {
                return 1;
            }
        } else {
            while (programa == NULL) {
                respuesta = pedir_ruta(entrada, (int) sizeof entrada);
                if (respuesta == 0) {
                    printf("No se indico ningun archivo. Programa finalizado.\n");
                    return 0;
                }
                if (respuesta < 0) {
                    continue;
                }
                ruta = entrada;
                programa = cargar_programa(ruta, &tamano);
            }
        }

        error = analizar_programa(ruta, programa, tamano);

        free(programa);
        if (error) {
            return 1;
        }
    } while (preguntar_otro_archivo());

    printf("\nPrograma finalizado.\n");
    return 0;
}