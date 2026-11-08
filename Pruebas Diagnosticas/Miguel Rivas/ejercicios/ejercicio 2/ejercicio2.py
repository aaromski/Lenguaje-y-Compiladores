import sys
import time

#Calcula la fila n del Triángulo de Pascal utilizando manipulación de listas en memoria de Python
def generar_coeficientes_pascal(n):
   
    fila = [1.0] + [0.0] * n
    for i in range(1, n + 1):
        for j in range(i, 0, -1):
            fila[j] = fila[j] + fila[j - 1]
    return fila

#Construye dinámicamente e imprime en consola la expresión algebraica expandida del binomio (x+1)^n
def mostrar_polinomio(coefs, n):
    terminos = []
    for i in range(n + 1):
        exp = n - i
        c = coefs[i]
        if exp > 1:
            terminos.append(f"{int(c)}x^{exp}")
        elif exp == 1:
            terminos.append(f"{int(c)}x")
        else:
            terminos.append(f"{int(c)}")
    print(f"(x + 1)^{n} = " + " + ".join(terminos))

#Sustituye el valor de x termino a termino, mostrando el desglose de potencias y la suma total f(x).
def evaluar_paso_a_paso(coefs, n, x):
    print(f"\n--- Evaluacion paso a paso para x = {x} de (x + 1)^{n} ---")
    resultado_total = 0.0
    for i in range(n + 1):
        exp = n - i
        c = coefs[i]
        x_pow = x ** exp
        valor_termino = c * x_pow
        resultado_total += valor_termino
        print(f"Termino {i + 1}: Coeficiente ({int(c)}) * ({x}^{exp} = {x_pow}) = {valor_termino}")
    print(f"Suma total de los terminos: f({x}) = {resultado_total}")

def main():
  
    if len(sys.argv) >= 3:
        try:
            n_usuario = int(sys.argv[1])
            x_usuario = float(sys.argv[2])
        except ValueError:
            print("Error en los argumentos de entrada.")
            return
    else:
        print("=== EVALUACION DINAMICA DE POLINOMIOS (PYTHON - EJERCICIO 2) ===")
        try:
            n_usuario = int(input("Ingrese el grado del binomio (n >= 0): "))
            x_usuario = float(input("Ingrese el valor de x para evaluar f(x): "))
        except ValueError:
            print("Error: Debe ingresar un valor numerico valido.")
            return

    print(f"\n[Python] Ejecutando para n = {n_usuario}, x = {x_usuario}")
    coefs_usuario = generar_coeficientes_pascal(n_usuario)
    
    mostrar_polinomio(coefs_usuario, n_usuario)
    evaluar_paso_a_paso(coefs_usuario, n_usuario, x_usuario)

    # Benchmark automatico para n = 100
    n_bench = 100
    repeticiones = 10000

    start = time.perf_counter()
    for _ in range(repeticiones):
        _ = generar_coeficientes_pascal(n_bench)
    end = time.perf_counter()

    tiempo_seg = end - start

    with open("tiempos_ej2.txt", "a", encoding="utf-8") as f:
        f.write(f"\n=== RESULTADOS BENCHMARK EJERCICIO 2 (PYTHON) ===\n")
        f.write(f"Tiempo para generar n=%d (%d veces): %.6f segundos\n" % (n_bench, repeticiones, tiempo_seg))

    print(f"[Python] Benchmark n=%d completado. Tiempo registrado: {tiempo_seg:.6f} s (Añadido a tiempos_ej2.txt)")

if __name__ == "__main__":
    main()