#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#define MAX_N 500
#define MAX_HISTORIAL 20

// ---------- Entero grande con memoria dinamica ----------
// digitos en base 1e9
typedef struct {
    unsigned int* d;
    int n;
    int cap;
    int signo; // -1, 0, 1
} BigInt;

static const unsigned int BASE = 1000000000u;

static void biSumarMagEnSitio(BigInt* a, const BigInt* b);

static void biInit(BigInt* a) {
    a->d = NULL;
    a->n = 0;
    a->cap = 0;
    a->signo = 0;
}

static void biLiberar(BigInt* a) {
    free(a->d);
    biInit(a);
}

static void biReservar(BigInt* a, int cap) {
    if (cap <= a->cap) return;
    unsigned int* nd = realloc(a->d, (size_t)cap * sizeof(unsigned int));
    a->d = nd;
    a->cap = cap;
}

static void biAsegurar(BigInt* a, int n) {
    if (n <= a->cap) return;
    int cap = a->cap ? a->cap : 4;
    while (cap < n) cap *= 2;
    biReservar(a, cap);
}

static void biCopiar(BigInt* dst, const BigInt* src) {
    if (dst == src) return;
    biAsegurar(dst, src->n > 0 ? src->n : 1);
    for (int i = 0; i < src->n; i++) dst->d[i] = src->d[i];
    dst->n = src->n;
    dst->signo = src->signo;
}

static unsigned long long magnitudLL(long long v) {
    return v < 0 ? (unsigned long long)(-(v + 1)) + 1ULL : (unsigned long long)v;
}

static void biSetLL(BigInt* a, long long v) {
    a->n = 0;
    a->signo = 0;
    if (v == 0) return;
    unsigned long long m = magnitudLL(v);
    a->signo = v < 0 ? -1 : 1;
    int k = 0;
    while (m > 0) {
        biAsegurar(a, k + 1);
        a->d[k++] = (unsigned int)(m % BASE);
        m /= BASE;
    }
    a->n = k;
}

static int biCompararMag(const BigInt* a, const BigInt* b) {
    if (a->n != b->n) return a->n < b->n ? -1 : 1;
    for (int i = a->n - 1; i >= 0; i--)
        if (a->d[i] != b->d[i]) return a->d[i] < b->d[i] ? -1 : 1;
    return 0;
}

static void biSumarMag(BigInt* dst, const BigInt* a, const BigInt* b) {
    biCopiar(dst, a);
    biSumarMagEnSitio(dst, b);
    dst->signo = 1;
}

// requiere |a| >= |b|
static void biRestarMag(BigInt* dst, const BigInt* a, const BigInt* b) {
    biAsegurar(dst, a->n > 0 ? a->n : 1);
    int prestamo = 0;
    for (int i = 0; i < a->n; i++) {
        long long v = (long long)a->d[i] - prestamo - (i < b->n ? (long long)b->d[i] : 0LL);
        if (v < 0) { v += BASE; prestamo = 1; } else prestamo = 0;
        dst->d[i] = (unsigned int)v;
    }
    dst->n = a->n;
    while (dst->n > 0 && dst->d[dst->n - 1] == 0) dst->n--;
    dst->signo = dst->n ? 1 : 0;
}

static void biSumarFirmado(BigInt* a, const BigInt* b) {
    if (b->signo == 0) return;
    if (a->signo == 0) { biCopiar(a, b); return; }
    BigInt t;
    biInit(&t);
    if (a->signo == b->signo) {
        biSumarMag(&t, a, b);
        t.signo = a->signo;
    } else {
        int cmp = biCompararMag(a, b);
        if (cmp == 0) {
            biLiberar(&t);
            a->n = 0;
            a->signo = 0;
            return;
        } else if (cmp > 0) {
            biRestarMag(&t, a, b);
            t.signo = a->signo;
        } else {
            biRestarMag(&t, b, a);
            t.signo = b->signo;
        }
    }
    free(a->d);
    *a = t;
}

static void biMultiplicarLL(BigInt* a, long long x) {
    if (a->signo == 0 || x == 0) {
        a->n = 0;
        a->signo = 0;
        return;
    }
    unsigned long long m = magnitudLL(x);
    int signo = x < 0 ? -a->signo : a->signo;
    biAsegurar(a, a->n + 4);
    unsigned __int128 carry = 0;
    for (int i = 0; i < a->n; i++) {
        unsigned __int128 v = (unsigned __int128)a->d[i] * m + carry;
        a->d[i] = (unsigned int)(v % BASE);
        carry = v / BASE;
    }
    int n = a->n;
    while (carry > 0) {
        biAsegurar(a, n + 1);
        a->d[n++] = (unsigned int)(carry % BASE);
        carry /= BASE;
    }
    a->n = n;
    a->signo = signo;
}

static void biMultiplicar(BigInt* r, const BigInt* a, const BigInt* b) {
    if (a->signo == 0 || b->signo == 0) {
        r->n = 0;
        r->signo = 0;
        return;
    }
    int n = a->n + b->n;
    unsigned int* tmp = calloc((size_t)n, sizeof(unsigned int));
    for (int i = 0; i < a->n; i++) {
        unsigned __int128 carry = 0;
        for (int j = 0; j < b->n; j++) {
            unsigned __int128 v = (unsigned __int128)a->d[i] * b->d[j] + tmp[i + j] + carry;
            tmp[i + j] = (unsigned int)(v % BASE);
            carry = v / BASE;
        }
        int k = i + b->n;
        while (carry > 0) {
            unsigned __int128 v = (unsigned __int128)tmp[k] + carry;
            tmp[k] = (unsigned int)(v % BASE);
            carry = v / BASE;
            k++;
        }
    }
    int m = n;
    while (m > 0 && tmp[m - 1] == 0) m--;
    free(r->d);
    r->d = tmp;
    r->n = m;
    r->cap = n;
    r->signo = m ? a->signo * b->signo : 0;
}

static char* biTexto(const BigInt* a) {
    if (a->n == 0) {
        char* s = malloc(2);
        strcpy(s, "0");
        return s;
    }
    size_t cap = (size_t)a->n * 9 + 2;
    char* s = malloc(cap);
    size_t pos = 0;
    if (a->signo < 0) s[pos++] = '-';
    pos += (size_t)snprintf(s + pos, cap - pos, "%u", a->d[a->n - 1]);
    for (int i = a->n - 2; i >= 0; i--)
        pos += (size_t)snprintf(s + pos, cap - pos, "%09u", a->d[i]);
    return s;
}

// buffer de texto dinamico 

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

static void bufLiberar(Buf* b) {
    free(b->p);
    bufInit(b);
}

static void bufReservar(Buf* b, size_t extra) {
    size_t need = b->n + extra + 1;
    if (need <= b->cap) return;
    size_t cap = b->cap ? b->cap : 256;
    while (cap < need) cap *= 2;
    b->p = realloc(b->p, cap);
    b->cap = cap;
}

static void bufAgregar(Buf* b, const char* s) {
    size_t len = strlen(s);
    bufReservar(b, len);
    memcpy(b->p + b->n, s, len + 1);
    b->n += len;
}

static void bufAgregarEntero(Buf* b, long long v) {
    char t[32];
    snprintf(t, sizeof t, "%lld", v);
    bufAgregar(b, t);
}

static double ahoraMicros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

// Suma b sobre a en base 1e9 propagando el acarreo.
static void biSumarMagEnSitio(BigInt* a, const BigInt* b) {
    int n = a->n > b->n ? a->n : b->n;
    biAsegurar(a, n + 1);
    unsigned int carry = 0;
    for (int i = 0; i < n; i++) {
        unsigned int s = carry;
        if (i < a->n) s += a->d[i];
        if (i < b->n) s += b->d[i];
        if (s >= BASE) { s -= BASE; carry = 1; } else carry = 0;
        a->d[i] = s;
    }
    a->n = n;
    if (carry) { a->d[n] = carry; a->n = n + 1; }
    a->signo = a->n ? 1 : 0;
}

// Genera los n+1 coeficientes de (x+1)^n con el triangulo de Pascal.
// Un solo arreglo dinamico, actualizado de atras hacia adelante.
static BigInt* generarPascal(int n, double* micros) {
    double t0 = ahoraMicros();
    BigInt* fila = malloc((size_t)(n + 1) * sizeof(BigInt));
    for (int i = 0; i <= n; i++) biInit(&fila[i]);
    biSetLL(&fila[0], 1);
    for (int k = 1; k <= n; k++)
        for (int j = k; j >= 1; j--)
            biSumarMagEnSitio(&fila[j], &fila[j - 1]);
    double t1 = ahoraMicros();
    *micros = t1 - t0;
    return fila;
}

static void liberarPascal(BigInt* fila, int n) {
    for (int i = 0; i <= n; i++) biLiberar(&fila[i]);
    free(fila);
}

static char* polinomioTexto(BigInt* coef, int n) {
    Buf b;
    bufInit(&b);
    for (int k = n; k >= 0; k--) {
        char* c = biTexto(&coef[k]);
        if (k == 0) {
            bufAgregar(&b, c);
        } else {
            if (strcmp(c, "1") != 0) bufAgregar(&b, c);
            bufAgregar(&b, "x");
            if (k > 1) {
                bufAgregar(&b, "^");
                bufAgregarEntero(&b, k);
            }
        }
        free(c);
        if (k > 0) bufAgregar(&b, " + ");
    }
    return b.p;
}

static char* evaluacionTexto(BigInt* coef, int n, long long x, char** valorFinal) {
    BigInt* pot = malloc((size_t)(n + 1) * sizeof(BigInt));
    for (int i = 0; i <= n; i++) biInit(&pot[i]);
    biSetLL(&pot[0], 1);
    for (int k = 1; k <= n; k++) {
        biCopiar(&pot[k], &pot[k - 1]);
        biMultiplicarLL(&pot[k], x);
    }

    BigInt total;
    biInit(&total);
    Buf s;
    bufInit(&s);
    for (int k = n; k >= 0; k--) {
        BigInt termino;
        biInit(&termino);
        biMultiplicar(&termino, &coef[k], &pot[k]);
        biSumarFirmado(&total, &termino);
        bufAgregar(&s, "Paso ");
        bufAgregarEntero(&s, n - k + 1);
        bufAgregar(&s, ": ");
        char* c = biTexto(&coef[k]);
        bufAgregar(&s, c);
        free(c);
        bufAgregar(&s, " * (");
        bufAgregarEntero(&s, x);
        bufAgregar(&s, ")^");
        bufAgregarEntero(&s, k);
        bufAgregar(&s, " = ");
        c = biTexto(&termino);
        bufAgregar(&s, c);
        free(c);
        bufAgregar(&s, "\n");
        biLiberar(&termino);
    }
    *valorFinal = biTexto(&total);
    biLiberar(&total);
    for (int i = 0; i <= n; i++) biLiberar(&pot[i]);
    free(pot);
    return s.p;
}

// historial y consola 

static void agregarLineaHistorial(char lineas[][512], int* total, const char* linea) {
    if (*total < MAX_HISTORIAL) {
        strcpy(lineas[(*total)++], linea);
        return;
    }
    for (int i = 1; i < MAX_HISTORIAL; i++) strcpy(lineas[i - 1], lineas[i]);
    strcpy(lineas[MAX_HISTORIAL - 1], linea);
}

static void escribirResultado(int n, long long x, double micros) {
    time_t ahora = time(NULL);
    struct tm* lt = localtime(&ahora);
    char sello[32];
    if (lt) strftime(sello, sizeof sello, "%Y-%m-%d %H:%M:%S", lt);
    else sello[0] = '\0';

    char linea[512];
    snprintf(linea, sizeof linea,
             "[%s] C | Resultado: (x+1)^%d generado (%d coeficientes); f(%lld) evaluado | "
             "Tiempo de generacion: %.3f microsegundos",
             sello, n, n + 1, x, micros);

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

static void presentarResultado(int n, long long x, BigInt* coef, double micros, double tInicio) {
    char* polinomio = polinomioTexto(coef, n);
    char* valorFinal = NULL;
    char* pasos = evaluacionTexto(coef, n, x, &valorFinal);

    Buf salida;
    bufInit(&salida);
    char buf[160];
    snprintf(buf, sizeof buf, "Generacion del polinomio (x+1)^n\nn = %d\n", n);
    bufAgregar(&salida, buf);
    snprintf(buf, sizeof buf,
             "Coeficientes generados: %d\nTiempo de generacion: %.3f microsegundos\n\n",
             n + 1, micros);
    bufAgregar(&salida, buf);
    bufAgregar(&salida, "Polinomio generado con el triangulo de Pascal:\n(x+1)^");
    bufAgregarEntero(&salida, n);
    bufAgregar(&salida, " = ");
    bufAgregar(&salida, polinomio);
    bufAgregar(&salida, "\n\n");
    snprintf(buf, sizeof buf, "Evaluacion paso a paso para x = %lld\n", x);
    bufAgregar(&salida, buf);
    bufAgregar(&salida, pasos);
    bufAgregar(&salida, "\nf(");
    bufAgregarEntero(&salida, x);
    bufAgregar(&salida, ") = ");
    bufAgregar(&salida, valorFinal);
    bufAgregar(&salida, "\n\n");
    double totalMicros = ahoraMicros() - tInicio;
    snprintf(buf, sizeof buf, "Tiempo total del programa: %.3f microsegundos\n", totalMicros);
    bufAgregar(&salida, buf);

    printf("%s", salida.p ? salida.p : "");

    free(polinomio);
    free(pasos);
    free(valorFinal);
    bufLiberar(&salida);
}

static void recortar(char* s) {
    char* a = s;
    while (*a && isspace((unsigned char)*a)) a++;
    if (a != s) memmove(s, a, strlen(a) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

static int parseLL(const char* s, long long* v) {
    const char* p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return 0;
    char* fin = NULL;
    errno = 0;
    long long r = strtoll(p, &fin, 10);
    if (errno == ERANGE) return 0;
    while (*fin && isspace((unsigned char)*fin)) fin++;
    if (*fin) return 0;
    *v = r;
    return 1;
}

static void pausaFinal(void) {
    printf("\nPresione Enter para salir...");
    fflush(stdout);
    getchar();
}

static int ejecutarDeNuevo(void) {
    printf("\nEjecutar de nuevo? (s/n): ");
    fflush(stdout);
    char linea[16];
    if (!fgets(linea, sizeof linea, stdin)) return 0;
    recortar(linea);
    return linea[0] == 's' || linea[0] == 'S';
}

int main(void) {
    atexit(pausaFinal);
    printf("=== Polinomio (x+1)^n (C) ===\n");

    do {
        char entrada[8192];
        printf("n (entero no negativo, 0-500): ");
        fflush(stdout);
        if (!fgets(entrada, sizeof entrada, stdin)) return 0;
        recortar(entrada);
        long long nll = 0, x = 0;
        if (!parseLL(entrada, &nll) || nll < 0 || nll > MAX_N) {
            printf("Entrada invalida: n debe ser un entero entre 0 y 500.\n");
            return 1;
        }

        printf("x (entero): ");
        fflush(stdout);
        if (!fgets(entrada, sizeof entrada, stdin)) return 0;
        recortar(entrada);
        if (!parseLL(entrada, &x)) {
            printf("Entrada invalida: x debe ser un numero entero.\n");
            return 1;
        }
        int n = (int)nll;

        double tInicio = ahoraMicros();
        double micros = 0.0;
        BigInt* coef = generarPascal(n, &micros);
        presentarResultado(n, x, coef, micros, tInicio);
        escribirResultado(n, x, micros);
        liberarPascal(coef, n);
    } while (ejecutarDeNuevo());
    return 0;
}
