"""
============================================================================
UNEG - Lenguajes y Compiladores (2026-II)
Profesor: Msc. Félix Márquez
Estudiante: Lisbelis del Valle Yemes Espinoza
Sección: 3
Grupo: Sintaxis
Actividad: Prueba Diagnóstica - Ejercicio 1: Validación de movimientos de un caballo de ajedrez
============================================================================
"""

import argparse
import time
from pathlib import Path

REPETICIONES = 10


def salto_caballo(col_actual, fil_actual, col_nueva, fil_nueva):
    diferencia_col = abs(col_nueva - col_actual)
    diferencia_fil = abs(fil_nueva - fil_actual)
    return (diferencia_col, diferencia_fil) in ((1, 2), (2, 1))


def posicion_valida(posicion):
    posicion = posicion.lower()
    return (
        len(posicion) == 2
        and "a" <= posicion[0] <= "h"
        and "1" <= posicion[1] <= "8"
    )


def verificar(inicio, movimientos, mostrar_detalle=False):
    inicio = inicio.lower()
    if not posicion_valida(inicio):
        return "Posicion inicial invalida"

    col = ord(inicio[0]) - ord("a") + 1  # codigo numerico de un caracter
    fil = int(inicio[1])  # convierte texto a numero entero

    for numero, mov in enumerate(movimientos, start=1):
        if mov:
            mov = mov[0].upper() + mov[1:].lower()
        if not mov or mov[0] != "K":
            letra = mov[0] if mov else "vacia"
            resultado = (
                f"Falla en el movimiento {numero}: la pieza {letra} "
                "no es un caballo; todo movimiento debe comenzar con K"
            )
            return resultado

        if (
            len(mov) != 3
            or not ("a" <= mov[1] <= "h")
            or not ("1" <= mov[2] <= "8")
        ):
            resultado = (
                f"Falla en el movimiento {numero}: formato de movimiento "
                f"invalido ({mov})"
            )
            return resultado

        col_nueva = ord(mov[1]) - ord("a") + 1
        fil_nueva = int(mov[2])
        if not salto_caballo(col, fil, col_nueva, fil_nueva):
            resultado = (
                f"Falla en el movimiento {numero}: {mov} no es un salto "
                "valido de caballo"
            )
            return resultado

        col, fil = col_nueva, fil_nueva
        if mostrar_detalle:
            print(f"{mov} -> movimiento correcto")

    return "movimiento correcto"


def movimientos_desde_texto(texto):
    return texto.split()


def cargar_movimientos(archivo):
    try:
        texto = archivo.read_text(encoding="utf-8-sig")
    except OSError as error:
        raise RuntimeError(
            f"No se pudo leer {archivo}: {error}"
        ) from error

    return movimientos_desde_texto(texto)


def solicitar_datos_manuales():
    inicio = input("Ingrese la posicion inicial del caballo (por ejemplo, a2): ").strip()
    movimientos = []
    print("Escriba uno o varios movimientos por linea. Escriba FIN para validarlos.")
    while True:
        texto = input("Movimiento(s): ").strip()
        if texto.upper() == "FIN":
            if not movimientos:
                print("Debe introducir al menos un movimiento.")
                continue
            return inicio, movimientos
        if not texto:
            print("Escriba movimientos o FIN.")
            continue
        movimientos.extend(movimientos_desde_texto(texto))


def medir_movimientos(inicio, movimientos, archivo):
    lineas = [
        "RESULTADOS DE LA MEDICION DE TIEMPO",
        "",
        "Medicion de tiempo",
        f"Posicion inicial: {inicio}",
        f"Secuencia leida desde {archivo.name}: {len(movimientos)} movimientos",
        f"Resultado: {verificar(inicio, movimientos)}",
    ]

    tiempos = []
    for repeticion in range(1, REPETICIONES + 1):
        inicio_t = time.perf_counter()
        verificar(inicio, movimientos)
        fin_t = time.perf_counter()
        duracion = fin_t - inicio_t
        tiempos.append(duracion)
        lineas.append(f"Repeticion {repeticion}: {duracion:.9f} segundos")

    total = sum(tiempos)
    lineas.append(f"\nTiempo total (10 repeticiones): {total:.9f} segundos")

    salida = "\n".join(lineas) + "\n"
    print(salida, end="")


def ejecutar_menu():
    while True:
        print("\n\nMENU PRINCIPAL")
        print("1. Validar movimientos manuales")
        print("2. Cargar movimientos desde un archivo y medir tiempo")
        print("3. Salir")
        opcion = input("Seleccione una opcion: ").strip()

        if opcion == "1":
            inicio, movimientos = solicitar_datos_manuales()
            print("Resultado final:")
            print(verificar(inicio, movimientos, mostrar_detalle=True))
        elif opcion == "2":
            inicio = input("Ingrese la posicion inicial del caballo: ").strip()
            nombre_archivo = input(
                "Ingrese la ruta del archivo de movimientos: "
            ).strip()
            if not nombre_archivo:
                print("Debe indicar la ruta de un archivo.")
                continue
            archivo = Path(nombre_archivo).expanduser()
            try:
                movimientos = cargar_movimientos(archivo)
            except RuntimeError as error:
                print(f"ERROR: {error}")
                continue
            medir_movimientos(inicio, movimientos, archivo)
        elif opcion == "3":
            print("Programa finalizado.")
            return
        else:
            print("Opcion invalida. Seleccione 1, 2 o 3.")


def ejecutar():
    parser = argparse.ArgumentParser(
        description="Valida movimientos de un caballo de ajedrez."
    )
    parser.add_argument("--inicio", help="Posicion inicial, por ejemplo a1")
    parser.add_argument(
        "--movimientos",
        help="Movimientos separados por espacios, por ejemplo 'Kc2 Kd4 Kb3'",
    )
    parser.add_argument(
        "--archivo",
        type=Path,
        help="Archivo de texto con movimientos para medir el tiempo",
    )
    parser.add_argument(
        "--solo-validar",
        action="store_true",
        help="Solo valida la entrada indicada y no ejecuta la medicion",
    )
    args = parser.parse_args()

    if args.archivo:
        if not args.inicio or args.movimientos or args.solo_validar:
            parser.error(
                "--archivo requiere --inicio, no puede combinarse con "
                "--movimientos ni --solo-validar"
            )
        try:
            movimientos = cargar_movimientos(args.archivo)
        except RuntimeError as error:
            parser.error(str(error))
        medir_movimientos(args.inicio, movimientos, args.archivo)
    elif args.inicio or args.movimientos:
        if not args.inicio or not args.movimientos:
            parser.error("--inicio y --movimientos deben usarse juntos")
        inicio = args.inicio
        movimientos = movimientos_desde_texto(args.movimientos)
    elif args.solo_validar:
        parser.error("--solo-validar requiere --inicio y --movimientos")
    else:
        ejecutar_menu()
        return

    if args.solo_validar:
        print(verificar(inicio, movimientos))
        return


if __name__ == "__main__":
    ejecutar()
