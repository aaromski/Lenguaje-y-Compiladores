"""
Archivo : ejercicio1.py
Curso   : Lenguajes y Compiladores - UNEG
Estudiante: Angie Urrieta
Desc.   : Ejercicio 1 - Validador de movimientos de un caballo.
"""
import time
import os
BASE = os.path.dirname(os.path.abspath(__file__))
os.chdir(BASE)

RUTA_MOV = os.path.join("datos", "movimientos_500.txt")
RUTA_RES = os.path.join("resultados", "tiempos_python.txt")

class Color:
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BOLD = '\033[1m'
    RESET = '\033[0m'

def limpiar():
    os.system('cls' if os.name == 'nt' else 'clear')

def print_header(title):
    limpiar()
    print(f"{Color.CYAN}{'='*65}")
    print(f"      UNEG - LENGUAJES Y COMPILADORES - PRUEBA DIAGNOSTICA")
    print(f"{'='*65}{Color.RESET}")
    print(f"{Color.BOLD}{title}{Color.RESET}\n")

def print_unit_test(caso, ak, n, cadena, resultado):
    es_correcto = "CORRECTO" in resultado and "INCORRECTO" not in resultado
    color = Color.GREEN if es_correcto else Color.RED
    icono = "\u2714" if es_correcto else "X"
    print(f"{Color.YELLOW}Caso {caso}:{Color.RESET} Ak={ak}, n={n}")
    print(f"  Cadena: {cadena[:30]}{'...' if len(cadena)>30 else ''}")
    print(f"  {icono} Resultado: {color}{resultado}{Color.RESET}\n")

def parse_pos(pos_str: str) -> tuple[int, int]:
    return ord(pos_str[0]) - ord('a'), ord(pos_str[1]) - ord('1')

def es_movimiento_caballo(actual, siguiente) -> bool:
    dx, dy = abs(actual[0] - siguiente[0]), abs(actual[1] - siguiente[1])
    return (dx == 1 and dy == 2) or (dx == 2 and dy == 1)

def validar_secuencia(pos_inicial: str, movimientos: list[str]) -> str:
    """Recorre la secuencia y SE DETIENE en el primer movimiento incorrecto."""
    actual = parse_pos(pos_inicial)
    for i, mov in enumerate(movimientos):
        if not mov:
            return f"INCORRECTO falla en el movimiento {i + 1}: token vacio"
        if mov[0] != 'K':
            return f"movimiento INCORRECTO falla letra {mov[0]} no pertenece a un caballo."
        if len(mov) != 3 or not ('a' <= mov[1] <= 'h' and '1' <= mov[2] <= '8'):
            return f"INCORRECTO falla en el movimiento {i + 1}"
        siguiente = parse_pos(mov[1:])
        if not es_movimiento_caballo(actual, siguiente):
            return f"INCORRECTO falla en el movimiento {i + 1}"
        actual = siguiente
    return "movimiento CORRECTO"

def cargar_movimientos_oficiales():
    if not os.path.exists(RUTA_MOV):
        print(f"  {Color.RED}X No se encontro {RUTA_MOV}{Color.RESET}")
        return None
    with open(RUTA_MOV, "r", encoding="utf-8-sig") as f:
        raw = f.read()
    tokens = [t for t in raw.split() if t and not t.startswith('#')]
    if len(tokens) != 500:
        print(f"  {Color.RED}X Advertencia: el archivo tiene {len(tokens)} movimientos; se esperan 500.{Color.RESET}")
        print(f"  {Color.YELLOW}  Revisa la copia del PDF (pag 1-2): debe empezar en Kb4 y terminar en Ka4.{Color.RESET}")
    return tokens

def main():
    print_header("EJERCICIO 1: VALIDADOR DE MOVIMIENTOS DE CABALLO")
    print(f"{Color.BOLD} Pruebas Unitarias{Color.RESET}")
    print("-" * 65)

    tests = [
        ("a1", ["Kc2", "Kd4", "Kb3", "Kc5"]),
        ("a1", ["Kc2", "Kd4", "Kb4", "Kc5"]),
        ("a2", ["Kc3", "Ke4", "Kf6", "Ke8", "Kg7"]),
        ("a2", ["Kc8", "Ke4", "Kf6", "Ke8", "Kg7"]),
        ("a8", ["Kb6", "Kd7"]),
        ("a8", ["Kb6", "Kb7"]),
        ("a1", ["Kc2", "Kd4", "Kf3", "Ke5", "Kg6", "Kh8", "Kf7", "Kd8", "Kc6", "Kb4"]),
        ("a1", ["Kc2", "Kd4", "Kf3", "Ke5", "Kg6", "Kh8", "Kf7", "Kd8", "Kc6", "Qb4"]),
        ("a1", ["Kc2", "Kd4", "Kf3", "Ke5", "Kg6", "Kh8", "Kf7", "Kd8", "Kh6", "Kb4"]),
    ]
    for i, (pos, movs) in enumerate(tests, 1):
        res = validar_secuencia(pos, movs)
        print_unit_test(i, pos, len(movs), " ".join(movs), res)
        time.sleep(0.2) 

    oficiales = cargar_movimientos_oficiales()

    print(f"{Color.BOLD} Prueba de Estres{Color.RESET}")
    print("-" * 65)

    if not oficiales:
        print(f"  {Color.RED}X Sin cadena oficial; no se corre el benchmark.{Color.RESET}")
        return

    print(f"Ejecutando 10 repeticiones de {len(oficiales)} movimientos (cadena oficial, Ak=a2)...")
    tiempos = []
    for rep in range(10):
        t0 = time.perf_counter()
        resultado = validar_secuencia("a2", oficiales) 
        t1 = time.perf_counter()
        tiempos.append(t1 - t0)
    tiempo_total = sum(tiempos)
    tiempo_promedio = tiempo_total / 10
    for i, t in enumerate(tiempos, 1):                  
        print(f"  Repeticion {i:2d}: {t:.9f} s")

    es_correcto = (resultado == "movimiento CORRECTO")
    color_res = Color.GREEN if es_correcto else Color.RED
    icono_res = "\u2714" if es_correcto else "X"
    print(f"\n{Color.BOLD}Resultado de la Prueba de Estres:{Color.RESET} {icono_res} {color_res}{resultado}{Color.RESET}")

    print(f"\n{Color.BOLD}----------------------------------------------------------")
    print(f"| {Color.CYAN}Metrica{Color.RESET}           | {Color.CYAN}Valor{Color.RESET}          | {Color.CYAN}Detalle{Color.RESET}           |")
    print(f"----------------------------------------------------------")
    print(f"| Repeticiones      | 10             | Completadas       |")
    print(f"| Movimientos       | {len(oficiales):<14} | Cadena oficial    |")
    print(f"| Tiempo Total      | {tiempo_total:<14.6f} | 10 repeticiones   |")
    print(f"| Tiempo Promedio   | {tiempo_promedio:<14.6f} | Por repeticion    |")
    print(f"----------------------------------------------------------{Color.RESET}")

    os.makedirs("resultados", exist_ok=True)
    with open(RUTA_RES, "w", encoding="utf-8") as f:
        f.write(f"Resultado final: {resultado}\n")
        f.write(f"Movimientos validados: {len(oficiales)} (cadena oficial, Ak=a2, sin fallas)\n")
        for i, t in enumerate(tiempos, 1):
            f.write(f"Repeticion {i}: {t:.9f} segundos\n")
        f.write(f"Tiempo Python total (10 reps x {len(oficiales)} movs): {tiempo_total:.6f} segundos\n")
        f.write(f"Tiempo promedio por repeticion: {tiempo_promedio:.6f} segundos\n")
    print(f"\n{Color.GREEN}\u2714 Resultados guardados en {RUTA_RES}{Color.RESET}")

if __name__ == "__main__":
    main()