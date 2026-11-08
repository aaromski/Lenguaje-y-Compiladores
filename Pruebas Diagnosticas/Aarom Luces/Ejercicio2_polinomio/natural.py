import datetime
import os
import time

BASE = os.path.dirname(os.path.abspath(__file__))
HISTORIAL = os.path.join(BASE, "resultado.txt")
MAX_HISTORIAL = 20
MAX_N = 500
MIN_X = -(2 ** 63)
MAX_X = 2 ** 63 - 1


def generar_pascal(n):
    fila = [1] + [0] * n
    for k in range(1, n + 1):
        for j in range(k, 0, -1):
            fila[j] += fila[j - 1]
    return fila


def texto_polinomio(coef):
    terminos = []
    for k in range(len(coef) - 1, -1, -1):
        if k == 0:
            terminos.append(str(coef[k]))
            continue
        variable = "x" if k == 1 else "x^%d" % k
        terminos.append(("" if coef[k] == 1 else str(coef[k])) + variable)
    return " + ".join(terminos)


def evaluar_pasos(coef, x):
    n = len(coef) - 1
    potencias = [1] * (n + 1)
    for k in range(1, n + 1):
        potencias[k] = potencias[k - 1] * x
    pasos = []
    total = 0
    for paso, k in enumerate(range(n, -1, -1), 1):
        valor = coef[k] * potencias[k]
        total += valor
        pasos.append("Paso %d: %d * (%d)^%d = %d" % (paso, coef[k], x, k, valor))
    return pasos, total


def escribir_resultado(linea):
    filas = []
    if os.path.exists(HISTORIAL):
        with open(HISTORIAL, "r", encoding="utf-8", errors="replace") as f:
            filas = [ln.rstrip("\n") for ln in f if ln.startswith("[")]
    filas = (filas + [linea])[-MAX_HISTORIAL:]
    with open(HISTORIAL, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(filas) + "\n")


def leer_entero(mensaje):
    try:
        return int(input(mensaje).strip())
    except (ValueError, EOFError):
        return None


def ejecutar():
    n = leer_entero("n (entero no negativo, 0-500): ")
    if n is None:
        print("Entrada invalida: n debe ser un numero entero.")
        return
    if n < 0 or n > MAX_N:
        print("Entrada invalida: n debe estar entre 0 y %d." % MAX_N)
        return
    x = leer_entero("x (entero): ")
    if x is None:
        print("Entrada invalida: x debe ser un numero entero.")
        return
    if x < MIN_X or x > MAX_X:
        print("Entrada invalida: x debe caber en un entero de 64 bits.")
        return

    inicio_total = time.perf_counter_ns()
    inicio = time.perf_counter_ns()
    coef = generar_pascal(n)
    micros = (time.perf_counter_ns() - inicio) / 1000.0
    polinomio = texto_polinomio(coef)
    pasos, total = evaluar_pasos(coef, x)

    lineas = [
        "Generacion del polinomio (x+1)^n",
        "n = %d" % n,
        "Coeficientes generados: %d" % (n + 1),
        "Tiempo de generacion: %.3f microsegundos" % micros,
        "",
        "Polinomio generado con el triangulo de Pascal:",
        "(x+1)^%d = %s" % (n, polinomio),
        "",
        "Evaluacion paso a paso para x = %d" % x,
    ]
    lineas.extend(pasos)
    lineas.append("")
    lineas.append("f(%d) = %d" % (x, total))
    lineas.append("")
    total_micros = (time.perf_counter_ns() - inicio_total) / 1000.0
    lineas.append("Tiempo total del programa: %.3f microsegundos" % total_micros)
    print("\n".join(lineas))

    sello = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    linea = ("[%s] Python | Resultado: (x+1)^%d generado (%d coeficientes); "
             "f(%d) evaluado | Tiempo de generacion: %.3f microsegundos"
             % (sello, n, n + 1, x, micros))
    escribir_resultado(linea)


def ejecutar_de_nuevo():
    try:
        return input("\nEjecutar de nuevo? (s/n): ").strip().lower().startswith("s")
    except EOFError:
        return False


def main():
    print("=== Polinomio (x+1)^n (Python) ===")
    try:
        while True:
            ejecutar()
            if not ejecutar_de_nuevo():
                break
    finally:
        try:
            input("\nPresione Enter para salir...")
        except EOFError:
            pass


if __name__ == "__main__":
    main()
