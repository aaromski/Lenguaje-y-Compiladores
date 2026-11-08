"""
Archivo : ejercicio2.py
Curso   : Lenguajes y Compiladores — UNEG
Desc.   : Ejercicio 2 — Polinomio (x+1)^n vía Triangulo de Pascal
"""
import time
import os
BASE = os.path.dirname(os.path.abspath(__file__))
os.chdir(BASE)

REPS_BENCHMARK = 50

class Color:
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BOLD = '\033[1m'
    RESET = '\033[0m'

def limpiar():
    os.system('cls' if os.name == 'nt' else 'clear')

def leer_entero(mensaje, minimo, maximo):
    while True:
        texto = input(mensaje).strip()
        if texto == "":
            return -1
        if texto.isdigit() and minimo <= int(texto) <= maximo:
            return int(texto)
        print(f"  {Color.RED}Entrada invalida. Ingrese un entero entre {minimo} y {maximo}.{Color.RESET}")

def generar_pascal(n):
    """Fila n del Triangulo de Pascal: fila[k] = C(n,k) (coeficiente de x^k)."""
    fila = [1]
    for i in range(1, n + 1):
        nueva = [1]
        for j in range(1, i):
            nueva.append(fila[j-1] + fila[j])
        nueva.append(1)
        fila = nueva
    return fila

def evaluar(fila, x):
    return sum(c * (x ** k) for k, c in enumerate(fila))

def polynomial_string(fila, n):
    terms = []
    for k in range(n, -1, -1):
        c = fila[k]
        if k == 0:   var = ""
        elif k == 1: var = "x"
        else:        var = f"x^{k}"
        if k > 0 and c == 1:
            terms.append(var)
        else:
            terms.append(f"{c}{var}")
    return " + ".join(terms)

def print_polynomial(fila, n, pref):
    print(f"{pref}{Color.GREEN}{polynomial_string(fila, n)}{Color.RESET}")

def show_steps(fila, n, x):
    ps = polynomial_string(fila, n)
    print(f"  Polinomio: (x+1)^{n} = {Color.GREEN}{ps}{Color.RESET}")

    os.makedirs("resultados", exist_ok=True)
    with open("resultados/ej2_pasos_python.txt", "w") as fp:
        fp.write("Ejercicio 2 - Evaluacion paso a paso \n")
        fp.write(f"Polinomio: (x+1)^{n} = {ps}\n")
        fp.write(f"Evaluacion para x = {x}:\n")

        print(f"  {Color.CYAN}Evaluacion para x = {x} :{Color.RESET}")
        acc = 0
        pow_x = 1
        for k in range(0, n + 1):
            term = fila[k] * pow_x
            new_acc = acc + term
            print(f"  {Color.YELLOW}Paso {k+1:3d}:{Color.RESET} {fila[k]}*({x}^{k}) = "
                  f"{fila[k]}*{pow_x} = {Color.CYAN}{term}{Color.RESET}"
                  f"  -> acumulado: {Color.GREEN}{new_acc}{Color.RESET}")
            fp.write(f"  Paso {k+1:3d}: {fila[k]}*({x}^{k}) = {fila[k]}*{pow_x} = {term}"
                     f"  -> acumulado: {new_acc}\n")
            acc = new_acc
            pow_x *= x

        print(f"  {Color.GREEN}Resultado:{Color.RESET} f({x}) = {Color.BOLD}{acc}{Color.RESET}")
        ver = (x + 1) ** n
        ok = ver == acc
        icono = "✔" if ok else "X"
        col = Color.GREEN if ok else Color.RED
        print(f"  Verificacion directa: ({x}+1)^{n} = {ver}  {col}{icono}{Color.RESET}")
        fp.write(f"Resultado: f({x}) = {acc}\n")
        fp.write(f"Verificacion directa: ({x}+1)^{n} = {ver}  {'OK' if ok else 'ERROR'}\n")

    print(f"  {Color.GREEN}✔ Paso a paso guardado en resultados/ej2_pasos_python.txt{Color.RESET}")

def main():
    limpiar()
    print(f"{Color.CYAN}{'='*65}")
    print(f"      UNEG - LENGUAJES Y COMPILADORES - PRUEBA DIAGNOSTICA")
    print(f"{'='*65}{Color.RESET}")
    print(f"{Color.BOLD}EJERCICIO 2: POLINOMIO (x+1)^n - TRIANGULO DE PASCAL\n{Color.RESET}")


    print(f"{Color.BOLD} Polinomios de referencia y verificaciones{Color.RESET}")
    print("-" * 65)
    for n in range(0, 8):
        print_polynomial(generar_pascal(n), n, f"  (x+1)^{n:<2} = ")

    print(f"\n  {Color.BOLD}Verificaciones automaticas:{Color.RESET}")
    checks = [(5, 2, 243), (10, 2, 59049), (100, 1, 2 ** 100)]
    for n, x, esp in checks:
        got = evaluar(generar_pascal(n), x)
        ok = got == esp
        icono = "✔" if ok else "X"
        col = Color.GREEN if ok else Color.RED
        print(f"  {col}{icono}{Color.RESET} f({x}), n={n:<3} = {got}")


    while True:
        print(f"\n{Color.BOLD} Generacion y evaluacion interactiva{Color.RESET}")
        print("-" * 65)
        n = leer_entero("  Ingrese el valor de n (0 a 1000, ENTER para salir): ", 0, 1000)
        if n == -1:
            break
        x = leer_entero("  Ingrese el valor de x (0 a 1000): ", 0, 1000)
        if x == -1:
            break
        show_steps(generar_pascal(n), n, x)

    
    print(f"\n{Color.BOLD} Prueba de estres (generacion n=100, {REPS_BENCHMARK} reps){Color.RESET}")
    print("-" * 65)
    c100 = generar_pascal(100)
    print(f"  Coeficiente central (100,50): {Color.CYAN}{c100[50]}{Color.RESET}")
    print(f"  Ejecutando {REPS_BENCHMARK} generaciones de n=100...")

    t0 = time.perf_counter()
    for _ in range(REPS_BENCHMARK):
        generar_pascal(100)
    t1 = time.perf_counter()
    t_total = t1 - t0
    t_prom = t_total / REPS_BENCHMARK

    print(f"\n{Color.BOLD}----------------------------------------------------------")
    print(f"| {Color.CYAN}Metrica{Color.RESET}           | {Color.CYAN}Valor{Color.RESET}          | {Color.CYAN}Detalle{Color.RESET}           |")
    print(f"----------------------------------------------------------")
    print(f"| Repeticiones      | {REPS_BENCHMARK:<14} | Generaciones      |")
    print(f"| Tiempo Total      | {t_total:<14.6f} | Segundos          |")
    print(f"| Tiempo Promedio   | {t_prom:<14.6f} | Segundos          |")
    print(f"| Promedio (ms)     | {t_prom*1000:<14.3f} | Por generacion    |")
    print(f"----------------------------------------------------------{Color.RESET}")

    os.makedirs("resultados", exist_ok=True)
    with open("resultados/ej2_tiempos_python.txt", "w") as f:
        f.write("Ejercicio 2 - Polinomio (x+1)^n - Triangulo de Pascal\n")
        f.write(f"Repeticiones benchmark: {REPS_BENCHMARK}\n")
        f.write(f"Tiempo total: {t_total:.6f} segundos\n")
        f.write(f"Tiempo promedio por generacion: {t_prom:.6f} segundos ({t_prom*1000:.3f} ms)\n")
        f.write(f"Coeficiente central (100, 50): {c100[50]}\n")

    print(f"\n{Color.GREEN}✔ Resultados guardados en resultados/ej2_tiempos_python.txt{Color.RESET}")

if __name__ == "__main__":
    main()
