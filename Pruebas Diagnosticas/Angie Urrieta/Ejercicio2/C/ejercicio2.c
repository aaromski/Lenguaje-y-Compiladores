/*
 * Archivo : ejercicio2.c
 * Curso   : Lenguajes y Compiladores - UNEG
 * Desc.   : Ejercicio 2 - Polinomio (x+1)^n via Triangulo de Pascal
 *           Memoria dinamica (realloc).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#define BIG_INIT_CAP 8
#define REPS_BENCHMARK 50

static void setup_consola(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

// Memoria dinamica con realloc
typedef struct {
    unsigned char *d;
    int n;
    int cap;
} Big;

static void *xmalloc(size_t s) { void *p = malloc(s);  if (!p) { perror("malloc");  exit(1); } return p; }
static void *xcalloc(size_t n, size_t s) { void *p = calloc(n, s); if (!p) { perror("calloc"); exit(1); } return p; }
static void *xrealloc(void *p, size_t s) { void *q = realloc(p, s); if (!q) { perror("realloc"); exit(1); } return q; }

static void big_ensure(Big *b, int need) {
    if (need <= b->cap) return;
    int nc = b->cap > 0 ? b->cap : BIG_INIT_CAP;
    while (nc < need) nc *= 2;
    b->d = xrealloc(b->d, nc);
    memset(b->d + b->cap, 0, nc - b->cap);
    b->cap = nc;
}

Big *big_new(void) {
    Big *b = xmalloc(sizeof(Big));
    b->cap = BIG_INIT_CAP;
    b->d = xcalloc(b->cap, 1);
    b->n = 1;
    return b;
}

Big *big_from_int(int v) {
    Big *b = big_new();
    if (v == 0) return b;
    int n = 0;
    while (v > 0) { big_ensure(b, n + 1); b->d[n++] = v % 10; v /= 10; }
    b->n = n;
    return b;
}

Big *big_from_string(const char *s) {
    Big *b = big_new();
    int len = (int)strlen(s);
    big_ensure(b, len);
    for (int i = 0; i < len; i++) b->d[i] = s[len - 1 - i] - '0';
    int n = len;
    while (n > 1 && b->d[n - 1] == 0) n--;
    b->n = n;
    return b;
}

void big_free(Big *b) { if (!b) return; free(b->d); free(b); }

char *big_to_string(const Big *b) {
    char *s = xmalloc(b->n + 1);
    for (int i = 0; i < b->n; i++) s[i] = '0' + b->d[b->n - 1 - i];
    s[b->n] = '\0';
    return s;
}

int big_equals(const Big *a, const Big *b) {
    if (a->n != b->n) return 0;
    return memcmp(a->d, b->d, a->n) == 0;
}

int big_is_one(const Big *b) { return b->n == 1 && b->d[0] == 1; }

void big_add(Big *r, const Big *a, const Big *b) {
    int n = a->n > b->n ? a->n : b->n;
    big_ensure(r, n + 2);
    int carry = 0;
    for (int i = 0; i < n; i++) {
        int s = carry + (i < a->n ? a->d[i] : 0) + (i < b->n ? b->d[i] : 0);
        r->d[i] = s % 10;
        carry = s / 10;
    }
    if (carry) r->d[n++] = carry;
    r->n = n;
}

void big_mul_small(Big *r, const Big *a, int m) {
    big_ensure(r, a->n + 32);
    long long carry = 0;
    for (int i = 0; i < a->n; i++) {
        long long s = carry + (long long)a->d[i] * m;
        r->d[i] = s % 10;
        carry = s / 10;
    }
    int n = a->n;
    while (carry) { big_ensure(r, n + 1); r->d[n++] = carry % 10; carry /= 10; }
    r->n = n;
}

void big_mul_big(Big *r, const Big *a, const Big *b) {
    int rmax = a->n + b->n + 1;
    unsigned char *res = xcalloc(rmax, 1);
    for (int i = 0; i < a->n; i++) {
        int carry = 0, j;
        for (j = 0; j < b->n; j++) {
            int s = res[i + j] + a->d[i] * b->d[j] + carry;
            res[i + j] = s % 10;
            carry = s / 10;
        }
        int idx = i + j;
        while (carry > 0) {
            int s = res[idx] + carry;
            res[idx] = s % 10;
            carry = s / 10;
            idx++;
        }
    }
    int rn = a->n + b->n;
    while (rn > 1 && res[rn - 1] == 0) rn--;
    big_ensure(r, rn);
    memcpy(r->d, res, rn);
    r->n = rn;
    free(res);
}

// Triangulo de Pascal (fila n)
Big **pascal_row(int n) {
    Big **prev = xmalloc(sizeof(Big *));
    prev[0] = big_from_int(1);
    for (int r = 1; r <= n; r++) {
        Big **curr = xmalloc(sizeof(Big *) * (r + 1));
        curr[0] = big_from_int(1);
        curr[r] = big_from_int(1);
        for (int k = 1; k < r; k++) {
            curr[k] = big_new();
            big_add(curr[k], prev[k - 1], prev[k]);
        }
        for (int k = 0; k < r; k++) big_free(prev[k]);
        free(prev);
        prev = curr;
    }
    return prev;
}

void free_row(Big **row, int n) {
    for (int k = 0; k <= n; k++) big_free(row[k]);
    free(row);
}

Big *evaluate(Big **coef, int n, int x) {
    Big *acc = big_from_int(0);
    Big *pow_x = big_from_int(1);
    for (int k = 0; k <= n; k++) {
        Big *term = big_new();
        big_mul_big(term, coef[k], pow_x);
        Big *new_acc = big_new();
        big_add(new_acc, acc, term);
        big_free(acc); big_free(term);
        acc = new_acc;
        if (k < n) {
            Big *np = big_new();
            big_mul_small(np, pow_x, x);
            big_free(pow_x);
            pow_x = np;
        }
    }
    big_free(pow_x);
    return acc;
}

char *polynomial_string(Big **coef, int n) {
    int total = n + 1;
    char **terms = (char **)xmalloc(sizeof(char *) * total);
    size_t len = 0;
    for (int k = n; k >= 0; k--) {
        char *cs = big_to_string(coef[k]);
        char var[24];
        if (k == 0) var[0] = '\0';
        else if (k == 1) strcpy(var, "x");
        else sprintf(var, "x^%d", k);
        int omit_one = (k > 0 && big_is_one(coef[k]));
        char *t = (char *)xmalloc(strlen(cs) + strlen(var) + 2);
        if (omit_one) sprintf(t, "%s", var);
        else sprintf(t, "%s%s", cs, var);
        terms[n - k] = t;
        len += strlen(t) + 3;
        free(cs);
    }
    char *out = (char *)xmalloc(len + 1);
    out[0] = '\0';
    for (int i = 0; i < total; i++) {
        if (i) strcat(out, " + ");
        strcat(out, terms[i]);
        free(terms[i]);
    }
    free(terms);
    return out;
}

void print_polynomial(Big **coef, int n, const char *pref) {
    char *s = polynomial_string(coef, n);
    printf("%s", pref);
    printf(GREEN "%s" RESET "\n", s);
    free(s);
}

void show_steps(Big **coef, int n, int x) {
    char *ps = polynomial_string(coef, n);
    printf("  Polinomio: (x+1)^%d = " GREEN "%s" RESET "\n", n, ps);

    FILE *fp = fopen("resultados/ej2_pasos_c.txt", "w");
    if (fp) {
        fprintf(fp, "Ejercicio 2 - Evaluacion paso a paso \n");
        fprintf(fp, "Polinomio: (x+1)^%d = %s\n", n, ps);
        fprintf(fp, "Evaluacion para x = %d:\n", x);
    }

    printf("  " CYAN "Evaluacion para x = %d :" RESET "\n", x);

    Big *acc = big_from_int(0);
    Big *pow_x = big_from_int(1);
    for (int k = 0; k <= n; k++) {
        Big *term = big_new();
        big_mul_big(term, coef[k], pow_x);
        Big *new_acc = big_new();
        big_add(new_acc, acc, term);

        char *sc = big_to_string(coef[k]);
        char *sp = big_to_string(pow_x);
        char *st = big_to_string(term);
        char *sa = big_to_string(new_acc);

        printf("  " YELLOW "Paso %3d:" RESET " %s*(%d^%d) = %s*%s = " CYAN "%s" RESET
               "  -> acumulado: " GREEN "%s" RESET "\n",
               k + 1, sc, x, k, sc, sp, st, sa);
        if (fp) fprintf(fp, "  Paso %3d: %s*(%d^%d) = %s*%s = %s  -> acumulado: %s\n",
                        k + 1, sc, x, k, sc, sp, st, sa);

        free(sc); free(sp); free(st); free(sa);
        big_free(acc); big_free(term);
        acc = new_acc;
        if (k < n) {
            Big *np = big_new();
            big_mul_small(np, pow_x, x);
            big_free(pow_x);
            pow_x = np;
        }
    }

    char *sf = big_to_string(acc);
    printf("  " GREEN "Resultado:" RESET " f(%d) = " BOLD "%s" RESET "\n", x, sf);

    Big *ver = big_from_int(1);
    for (int i = 0; i < n; i++) {
        Big *t = big_new();
        big_mul_small(t, ver, x + 1);
        big_free(ver);
        ver = t;
    }
    char *sv = big_to_string(ver);
    int ok = big_equals(ver, acc);
    printf("  Verificacion directa: (%d+1)^%d = %s  %s%s" RESET "\n",
           x, n, sv, ok ? GREEN : RED, ok ? CHECK : CROSS);

    if (fp) {
        fprintf(fp, "Resultado: f(%d) = %s\n", x, sf);
        fprintf(fp, "Verificacion directa: (%d+1)^%d = %s  %s\n", x, n, sv, ok ? "OK" : "ERROR");
        fclose(fp);
    }
    printf("  " GREEN CHECK " Paso a paso guardado en resultados/ej2_pasos_c.txt\n" RESET, 0);

    free(sf); free(sv); free(ps);
    big_free(ver); big_free(acc); big_free(pow_x);
}

int leer_entero(const char *mensaje, int minimo, int maximo) {
    char buf[64];
    while (1) {
        printf("%s", mensaje);
        fflush(stdout);
        if (!fgets(buf, sizeof buf, stdin)) continue;
        if (buf[0] == '\n') return -1;
        char *end;
        long val = strtol(buf, &end, 10);
        if (end == buf || val < minimo || val > maximo) {
            printf("  " RED "Entrada invalida. Ingrese un entero entre %d y %d." RESET "\n", minimo, maximo);
            continue;
        }
        return (int)val;
    }
}

int main(void) {
    setup_consola();
    limpiar_pantalla();
    MKDIR("resultados");

    printf(CYAN "=================================================================\n"
                "      UNEG - LENGUAJES Y COMPILADORES - PRUEBA DIAGNOSTICA\n"
                "=================================================================\n" RESET);
    printf(BOLD "EJERCICIO 2: POLINOMIO (x+1)^n - TRIANGULO DE PASCAL\n\n" RESET);

    // Polinomios de referencia y verificacion
    printf(BOLD " Polinomios de referencia y verificaciones\n" RESET);
    printf("-----------------------------------------------------------------\n");
    for (int n = 0; n <= 7; n++) {
        Big **c = pascal_row(n);
        char lab[40];
        sprintf(lab, "  (x+1)^%-2d = ", n);
        print_polynomial(c, n, lab);
        free_row(c, n);
    }

    printf("\n  " BOLD "Verificaciones automaticas:" RESET "\n");
    struct { int n, x; const char *esp; } checks[] = {
        { 5, 2, "243" },
        { 10, 2, "59049" },
        { 100, 1, "1267650600228229401496703205376" }
    };
    for (int i = 0; i < 3; i++) {
        Big **c = pascal_row(checks[i].n);
        Big *f = evaluate(c, checks[i].n, checks[i].x);
        Big *e = big_from_string(checks[i].esp);
        int ok = big_equals(f, e);
        char *got = big_to_string(f);
        printf("  %s%s" RESET " f(%d), n=%-3d = %s\n",
               ok ? GREEN : RED, ok ? CHECK : CROSS, checks[i].x, checks[i].n, got);
        free(got); big_free(e); big_free(f); free_row(c, checks[i].n);
    }

    while (1) {
        printf("\n" BOLD " Generacion y evaluacion interactiva\n" RESET);
        printf("-----------------------------------------------------------------\n");
        int n = leer_entero("  Ingrese el valor de n (0 a 1000, ENTER para salir): ", 0, 1000);
        if (n == -1) break;
        int x = leer_entero("  Ingrese el valor de x (0 a 1000): ", 0, 1000);
        if (x == -1) break;

        Big **c = pascal_row(n);
        show_steps(c, n, x);
        free_row(c, n);
    }

    // Prueba de estres
    printf("\n" BOLD " Prueba de estres (generacion n=100, %d reps)\n" RESET, REPS_BENCHMARK);
    printf("-----------------------------------------------------------------\n");

    Big **c100 = pascal_row(100);
    char *central = big_to_string(c100[50]);
    printf("  Coeficiente central (100,50): " CYAN "%s" RESET "\n", central);
    free_row(c100, 100);

    printf("  Ejecutando %d generaciones de n=100...\n", REPS_BENCHMARK);
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < REPS_BENCHMARK; r++) {
        Big **c = pascal_row(100);
        free_row(c, 100);
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double t_total = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    double t_prom = t_total / REPS_BENCHMARK;

    char b1[32], b2[32], b3[32];
    snprintf(b1, sizeof b1, "%.6f", t_total);
    snprintf(b2, sizeof b2, "%.6f", t_prom);
    snprintf(b3, sizeof b3, "%.3f", t_prom * 1000.0);

    printf("\n" BOLD "----------------------------------------------------------\n");
    printf("| " CYAN "Metrica" RESET "           | " CYAN "Valor" RESET "          | " CYAN "Detalle" RESET "           |\n");
    printf("----------------------------------------------------------\n");
    printf("| %-17s | %-14d | %-17s |\n", "Repeticiones", REPS_BENCHMARK, "Generaciones");
    printf("| %-17s | %-14s | %-17s |\n", "Tiempo Total", b1, "Segundos");
    printf("| %-17s | %-14s | %-17s |\n", "Tiempo Promedio", b2, "Segundos");
    printf("| %-17s | %-14s | %-17s |\n", "Promedio (ms)", b3, "Por generacion");
    printf("----------------------------------------------------------\n" RESET);

    FILE *f = fopen("resultados/ej2_tiempos_c.txt", "w");
    if (f) {
        fprintf(f, "Ejercicio 2 - Polinomio (x+1)^n - Triangulo de Pascal\n");
        fprintf(f, "Repeticiones benchmark: %d\n", REPS_BENCHMARK);
        fprintf(f, "Tiempo total: %.6f segundos\n", t_total);
        fprintf(f, "Tiempo promedio por generacion: %.6f segundos (%.3f ms)\n", t_prom, t_prom * 1000.0);
        fprintf(f, "Coeficiente central (100, 50): %s\n", central);
        fclose(f);
        printf("\n" GREEN CHECK " Resultados guardados en resultados/ej2_tiempos_c.txt\n" RESET);
    }
    free(central);

    return 0;
}
