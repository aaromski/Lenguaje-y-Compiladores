#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <conio.h>
using namespace std;

struct Resultado {
    bool correcto;
    int falla;
    int movidos;  // movimientos procesados de la cadena
    string motivo; // causa del fallo, vacia si es correcto

    string descripcion() const {
        if (correcto)
            return "movimiento CORRECTO (" + to_string(movidos) + " movimientos)";
        string detalle = motivo.empty() ? "" : ": " + motivo;
        return "movimiento INCORRECTO falla en el movimiento " + to_string(falla) + detalle;
    }
};


static string motivoDe(int codigo, char ch) {
    switch (codigo) {
    case 1: return "movimiento incompleto";
    case 2: return "la letra '" + string(1, ch) + "' no pertenece a un caballo (se esperaba K)";
    case 3: return "la columna '" + string(1, ch) + "' esta fuera del tablero (a-h o A-H)";
    case 4: return "la fila '" + string(1, ch) + "' esta fuera del tablero (1-8)";
    case 5: return "el movimiento no es de caballo";
    default: return "";
    }
}

// Valida sintaxis (K + columna + fila) y semantica (salto en L).
static Resultado validar(const string& ak, const string& c) {
    int fila = ak[1] - '1';
    int col = tolower((unsigned char)ak[0]) - 'a'; // columna en minuscula o mayuscula
    int movidos = 0, codigo = 0;
    char ch = 0;
    int largo = (int)c.size();
    int i = 0; // puntero: recorre el string verificando de 3 en 3 caracteres
    while (i < largo) {
        if (isspace((unsigned char)c[i])) { i++; continue; }
        movidos++;
        // se esperan 3 caracteres: 'K', columna (a-h o A-H), fila (1-8)
        if (i + 2 >= largo) { codigo = 1; break; }
        if (c[i] != 'K') { codigo = 2; ch = c[i]; break; }
        char colch = (char)tolower((unsigned char)c[i + 1]);
        if (colch < 'a' || colch > 'h') { codigo = 3; ch = c[i + 1]; break; }
        if (c[i + 2] < '1' || c[i + 2] > '8') { codigo = 4; ch = c[i + 2]; break; }
        //Semantica Geometrica: el movimiento debe ser de caballo (distancia al cuadrado = 5)
        int ncol = colch - 'a';
        int nfila = c[i + 2] - '1';
        // movimiento de caballo: distancia al cuadrado = 1^2 + 2^2 = 5 || distancia al cuadrado = 2^2 + 1^2 = 5
        int df = nfila - fila, dc = ncol - col;
        if (df * df + dc * dc != 5) { codigo = 5; break; }
        fila = nfila;
        col = ncol;
        i += 3;
    }
    int falla = 0;
    if (codigo != 0) falla = movidos;
    return { codigo == 0, falla, movidos, motivoDe(codigo, ch) };
}


static string recortar(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) a++;
    while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

static bool leerEntero(const string& texto, long& valor) {
    string s = recortar(texto);
    char* fin = nullptr;
    long v = strtol(s.c_str(), &fin, 10);
    if (s.empty() || *fin != '\0') return false;
    valor = v;
    return true;
}

static bool leerPosicion(string& ak) {
    while (true) {
        cout << "Posicion inicial Ak (ej. a1): " << flush;
        string linea;
        if (!getline(cin, linea)) return false;
        ak = recortar(linea);
        char col = ak.empty() ? 0 : (char)tolower((unsigned char)ak[0]);
        if (ak.size() == 2 && col >= 'a' && col <= 'h' && ak[1] >= '1' && ak[1] <= '8')
            return true;
        cout << "Posicion invalida: se espera una columna a-h (o A-H) y una fila 1-8, ej. a1" << endl;
    }
}

static string leerCadena() {
    string c;
    while (true) {
        string linea;
        if (!getline(cin, linea)) break;
        if (recortar(linea).empty()) {
            if (c.empty()) continue; // ignora lineas vacias antes del primer movimiento
            break;
        }
        c += linea + "\n";
    }
    return c;
}

static int leerRepeticiones() {
    while (true) {
        cout << "Repeticiones (Enter = 10): " << flush;
        string s;
        if (!getline(cin, s)) return 10;
        if (recortar(s).empty()) return 10;
        long v;
        if (leerEntero(s, v) && v > 0) return (int)v;
        cout << "Repeticiones invalidas: se espera un entero positivo" << endl;
    }
}

// historial: maximo 20 entradas; al llegar a 20 se descarta la mas antigua
static void escribirResultado(const Resultado& r, int repeticiones, double total, double promedio) {
    time_t ahora = time(nullptr);
    tm lt;
    localtime_s(&lt, &ahora);
    char sello[32];
    strftime(sello, sizeof sello, "%Y-%m-%d %H:%M:%S", &lt);

    char linea[512];
    snprintf(linea, sizeof linea,
             "[%s] C++ | Resultado: %s | Repeticiones verificadas: %d | Tiempo total: %.3f microsegundos | Promedio: %.3f microsegundos",
             sello, r.descripcion().c_str(), repeticiones, total, promedio);

    vector<string> lineas;
    {
        ifstream in("resultado.txt");
        string l;
        while (getline(in, l))
            if (!l.empty() && l[0] == '[') lineas.push_back(l);
    }
    lineas.push_back(linea);
    while (lineas.size() > 20) lineas.erase(lineas.begin());

    ofstream out("resultado.txt", ios::trunc);
    if (!out) return;
    for (const string& l : lineas) out << l << "\n";
}

static bool ejecutarDeNuevo() {
    cout << "\nEjecutar de nuevo? (s/n): " << flush;
    string s;
    if (!getline(cin, s)) return false;
    s = recortar(s);
    for (char& ch : s) ch = (char)tolower((unsigned char)ch);
    return s == "s" || s == "si";
}

static void pausaFinal() {
    cout << "\nPresione Enter para salir..." << flush;
    while (cin.rdbuf()->in_avail() > 0) cin.get();
#ifdef _WIN32
    while (_kbhit()) _getch();
#endif
    cin.get();
}

int main() {
    atexit(pausaFinal);
    cout << "=== Validador de Movimientos (caballo) ===" << endl;

    do {
        string ak, c;
        if (!leerPosicion(ak)) return 0;

        int repeticiones = leerRepeticiones();

        cout << "Cadena C (ej. Kc2 Kd4 Kb3 Kc5; Enter en linea vacia para terminar):" << endl;
        c = leerCadena();

        int hechas = 0;
        double total = 0;
        Resultado r{};
        for (int k = 1; k <= repeticiones; k++) {
            auto t0 = chrono::steady_clock::now();
            r = validar(ak, c);
            auto t1 = chrono::steady_clock::now();
            double micros = chrono::duration<double, micro>(t1 - t0).count();
            hechas++;
            total += micros;
            cout << "Repeticion " << k << ": " << fixed << setprecision(3) << micros
                 << " microsegundos" << endl;
            if (!r.correcto) break;
        }
        double promedio = total / hechas;
        cout << endl;
        cout << r.descripcion() << "\n";
        cout << "Tiempo total (" << hechas << (hechas == 1 ? " repeticion" : " repeticiones") << "): "
             << fixed << setprecision(3) << total << " microsegundos" << endl;
        cout << "Promedio: " << promedio << " microsegundos" << endl;
        escribirResultado(r, hechas, total, promedio);
    } while (ejecutarDeNuevo());
    return 0;
}
