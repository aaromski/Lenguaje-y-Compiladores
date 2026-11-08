#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estructura para la tabla de traducción de palabras reservadas de C
typedef struct {
    const char *original;
    const char *traducido;
} TraduccionKeyword;

// Diccionario de palabras reservadas comunes de C y su traducción
TraduccionKeyword diccionario[] = {
    {"int", "entero"},
    {"float", "decimal"},
    {"char", "caracter"},
    {"double", "doble"},
    {"void", "vacio"},
    {"if", "si"},
    {"else", "sino"},
    {"while", "mientras"},
    {"for", "para"},
    {"return", "retornar"},
    {"include", "incluir"},
    {"struct", "estructura"},
    {"typedef", "definir_tipo"},
    {"const", "constante"},
    {"break", "romper"},
    {"continue", "continuar"}
};

int total_keywords = sizeof(diccionario) / sizeof(TraduccionKeyword);

// Función para buscar si una palabra es reservada y retornar su traducción
const char* buscar_traduccion(const char *palabra) {
    for (int i = 0; i < total_keywords; i++) {
        if (strcmp(palabra, diccionario[i].original) == 0) {
            return diccionario[i].traducido;
        }
    }
    return NULL; // No es palabra reservada
}

// Función principal de análisis y traducción del código fuente
void traducir_codigo(const char *codigo_fuente, char *resultado_buffer, size_t max_len, int *contador_traducidas) {
    *contador_traducidas = 0;
    resultado_buffer[0] = '\0';

    char buffer_copia[4096];
    strncpy(buffer_copia, codigo_fuente, sizeof(buffer_copia) - 1);
    buffer_copia[sizeof(buffer_copia) - 1] = '\0';

    char *ptr = buffer_copia;
    char token[256];
    int t_idx = 0;

    while (*ptr != '\0') {
        // Si es una letra o guion bajo, forma parte de un identificador/palabra
        if (isalpha(*ptr) || *ptr == '_') {
            t_idx = 0;
            while (isalnum(*ptr) || *ptr == '_') {
                if (t_idx < sizeof(token) - 1) {
                    token[t_idx++] = *ptr;
                }
                ptr++;
            }
            token[t_idx] = '\0';

            // Verificar si es palabra reservada
            const char *trad = buscar_traduccion(token);
            if (trad != NULL) {
                strncat(resultado_buffer, trad, max_len - strlen(resultado_buffer) - 1);
                (*contador_traducidas)++;
            } else {
                strncat(resultado_buffer, token, max_len - strlen(resultado_buffer) - 1);
            }
        } else {
            // Caracteres especiales, espacios, símbolos {, }, ;, etc.
            char temp_str[2] = {*ptr, '\0'};
            strncat(resultado_buffer, temp_str, max_len - strlen(resultado_buffer) - 1);
            ptr++;
        }
    }
}

int main() {
    // Código fuente de prueba en C para traducir
    const char *codigo_original = 
        "#include <stdio.h>\n\n"
        "int main() {\n"
        "    int a = 10;\n"
        "    float b = 5.5;\n"
        "    if (a > 5) {\n"
        "        return a;\n"
        "    } else {\n"
        "        return 0;\n"
        "    }\n"
        "}\n";

    char codigo_traducido[8192];
    int total_traducidas = 0;

    traducir_codigo(codigo_original, codigo_traducido, sizeof(codigo_traducido), &total_traducidas);

    // Mostrar en consola
    printf("=== TRADUCCION DE PALABRAS RESERVADAS DE C ===\n\n");
    printf("--- Codigo Fuente Original ---\n%s\n", codigo_original);
    printf("--- Codigo Traducido ---\n%s\n", codigo_traducido);
    printf("Total de palabras reservadas traducidas: %d\n", total_traducidas);

    // Guardar el reporte persistente en 'resultados_ej3.txt'
    FILE *f = fopen("resultados_ej3.txt", "w");
    if (f) {
        fprintf(f, "=== REPORTE DE TRADUCCION DE PALABRAS RESERVADAS (EJERCICIO 3 - C) ===\n\n");
        fprintf(f, "[CODIGO ORIGINAL]:\n%s\n", codigo_original);
        fprintf(f, "--------------------------------------------------\n");
        fprintf(f, "[CODIGO TRADUCIDO]:\n%s\n", codigo_traducido);
        fprintf(f, "--------------------------------------------------\n");
        fprintf(f, "Total de palabras reservadas identificadas y traducidas: %d\n", total_traducidas);
        fclose(f);
        printf("\n[C] Reporte guardado exitosamente en 'resultados_ej3.txt'.\n");
    } else {
        printf("Error al crear el archivo de resultados.\n");
        return 1;
    }

    return 0;
}