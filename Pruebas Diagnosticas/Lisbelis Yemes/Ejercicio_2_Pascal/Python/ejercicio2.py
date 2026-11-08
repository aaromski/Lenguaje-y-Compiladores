"""
============================================================================
UNEG - Lenguajes y Compiladores (2026-II)
Profesor: Msc. Félix Márquez
Estudiante: Lisbelis del Valle Yemes Espinoza
Sección: 3
Grupo: Sintaxis
Actividad: Prueba Diagnóstica - Ejercicio 2 - Python: Triángulo de Pascal
============================================================================
"""

import time


def generar_fila_pascal(exponente):
    """Devuelve los coeficientes de (x + 1)^n usando el triángulo de Pascal.

    La lista crece dinámicamente.
    coeficientes[k] es el coeficiente de x^k.
    """
    coeficientes = [1]
    for numero_fila in range(1, exponente + 1):
        coeficientes.append(1)
        for posicion in range(numero_fila - 1, 0, -1):
            coeficientes[posicion] += coeficientes[posicion - 1]
    return coeficientes


def escribir_polinomio(coeficientes):
    """Escribe el polinomio correspondiente, de la potencia mayor a la menor."""
    exponente_maximo = len(coeficientes) - 1
    polinomio = ""
    for exponente in range(exponente_maximo, -1, -1):
        coeficiente = coeficientes[exponente]
        if exponente == 0:
            termino = str(coeficiente)
        else:
            if coeficiente == 1:
                termino = ""
            else:
                termino = str(coeficiente)
            if exponente == 1:
                termino += "x"
            else:
                termino += "x^" + str(exponente)
        if polinomio:
            polinomio += " + "
        polinomio += termino
    return polinomio


def calcular_por_pasos(coeficientes, valor_x):
    """Calcula el polinomio mostrando cada término y la suma parcial."""
    exponente_maximo = len(coeficientes) - 1
    suma_parcial = 0
    for numero_paso, exponente in enumerate(
            range(exponente_maximo, -1, -1), start=1):
        valor_termino = coeficientes[exponente] * valor_x ** exponente
        suma_parcial += valor_termino
        print("Paso " + str(numero_paso) + ": "
              + str(coeficientes[exponente]) + " * " + str(valor_x) + "^"
              + str(exponente) + " = " + str(valor_termino)
              + "   (suma parcial: " + str(suma_parcial) + ")")
    return suma_parcial


def leer_entero_no_negativo(mensaje):
    """Lee un entero NO NEGATIVO y repite la solicitud si no es válido."""
    while True:
        try:
            valor = int(input(mensaje))
        except ValueError:
            print("Debe escribir un número entero (sin letras ni decimales).")
            continue

        if valor < 0:
            print("No se aceptan números negativos. Escriba un entero.")
            continue
        return valor


def preguntar_otro_calculo():
    """Pregunta si desea calcular otro polinomio. Devuelve True (sí) o False (no)."""
    while True:
        respuesta = input("\nDesea calcular otro polinomio? (s = si, n = no): ")
        respuesta = respuesta.strip().lower()
        if respuesta in ("s", "n"):
            return respuesta == "s"
        print("Respuesta no válida. Escriba s o n.")


def ejecutar_calculo():
    """Pide n y x, genera el polinomio y muestra los resultados."""
    exponente = leer_entero_no_negativo("Potencia n debe ser un entero: ")
    valor_x = leer_entero_no_negativo("Valor de x debe ser un entero no negativo: ")

    inicio = time.perf_counter()
    coeficientes = generar_fila_pascal(exponente)
    duracion = time.perf_counter() - inicio

    print("\nRESULTADOS EN PYTHON\n")
    print("Polinomio (x + 1)^" + str(exponente))
    print("Coeficientes: " + " ".join(str(c) for c in coeficientes))
    print("Polinomio: " + escribir_polinomio(coeficientes))
    print()

    print("Calculo por pasos con x = " + str(valor_x))
    resultado = calcular_por_pasos(coeficientes, valor_x)
    print("f(" + str(valor_x) + ") = " + str(resultado))
    print("Comprobacion directa: (" + str(valor_x) + " + 1)^"
          + str(exponente) + " = " + str((valor_x + 1) ** exponente))

    print("\nMedicion de tiempo")
    print("Tiempo para generar los coeficientes de n = " + str(exponente) + ": "
          + format(duracion, ".9f") + " segundos")


def main():
    try:
        while True:
            ejecutar_calculo()
            if not preguntar_otro_calculo():
                break
    except (EOFError, KeyboardInterrupt):
        print()
    print("\nPrograma finalizado.")


if __name__ == "__main__":
    main()
