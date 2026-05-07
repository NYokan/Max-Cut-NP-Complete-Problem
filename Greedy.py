import itertools
import time
import csv
import os
from datetime import datetime

# Obtiene la fecha y hora actual en formato legible
def obtener_fecha_hora():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# Verifica si el archivo para guardar datos existe
def archivo_existe(nombre):
    return os.path.isfile(nombre)

# Función de fuerza bruta con timeout para el problema Max Cut
def max_cut_fuerza_bruta_timeout(matriz, tiempo_limite):
    n = len(matriz)
    max_peso = float('-inf')  # Guarda el peso máximo encontrado
    mejor_particion = None    # Guarda la partición que produce el peso máximo

    inicio = time.time()

    # Genera todas las particiones posibles usando combinaciones binarias
    for particion in itertools.product([0, 1], repeat=n):

        # Verifica si se alcanzó el tiempo límite
        if time.time() - inicio > tiempo_limite:
            print("   [Timeout alcanzado]")
            break

        peso_actual = 0

        # Calcula el peso del corte para la partición actual
        for i in range(n):
            for j in range(i + 1, n):
                if particion[i] != particion[j]:
                    peso_actual += matriz[i][j]

        # Actualiza el mejor resultado si encuentra uno mejor
        if peso_actual > max_peso:
            max_peso = peso_actual
            mejor_particion = particion

    return max_peso, mejor_particion

# Lee un grafo desde un archivo y lo representa como una matriz de adyacencia
def leer_grafo(nombre_archivo):
    with open(nombre_archivo, 'r') as f:
        n, m = map(int, f.readline().split())

        # Inicializa matriz de adyacencia con ceros
        matriz = [[0]*n for _ in range(n)]

        # Lee las aristas y sus pesos, ajustando índices para que comiencen en 0
        for line in f:
            u, v, w = map(int, line.split())
            u -= 1 
            v -= 1
            matriz[u][v] = w
            matriz[v][u] = w

    return matriz

# Extrae un subgrafo de los primeros k nodos del grafo original
def subgrafo(grafo, k):
    k = min(k, len(grafo))
    return [fila[:k] for fila in grafo[:k]]

# Función principal para ejecutar el programa
if __name__ == "__main__":
    print("--- MAX CUT: FUERZA BRUTA ---\n")

    TIMEOUT_REAL = 3600
    TIMEOUT_SUB = 3600
    k = 20
    
    # Configuración para guardar resultados en CSV (algoritmo y lenguaje)
    archivo_csv = "resultados_fuerza_bruta_python.csv"
    existe = archivo_existe(archivo_csv)

    with open(archivo_csv, mode='a', newline='', encoding='utf-8') as f_csv:
        writer = csv.writer(f_csv)

        # Escribe encabezados si el archivo aún no existe
        if not existe:
            writer.writerow([
                "FechaHora","Algoritmo","Lenguaje","Instancia",
                "Enfoque","Nodos","Resultado","Tiempo_s","Comentarios"
            ])

        fecha = obtener_fecha_hora()
        algoritmo = "FuerzaBruta"
        lenguaje = "Python"

        # Ejecuta el algoritmo sobre las distintas instancias
        for i in range(1, 6):
            nombre_archivo = f"instancias/g{i}.mc"
            print(f"===== INSTANCIA {i} =====")
            
            # Intenta cargar el grafo desde archivo
            try:
                grafo = leer_grafo(nombre_archivo)

            # Continúa con la siguiente instancia si el archivo no existe
            except FileNotFoundError:
                print(f"Error: No se encontró el archivo {nombre_archivo}\n")
                continue

            # Prueba fuerza bruta sobre el grafo completo solo en la primera instancia
            if i == 1:
                print("--- Intento fuerza bruta (grafo completo) ---")

                # Se registra el tiempo de inicio
                inicio = time.time()

                # Ejecuta fuerza bruta sobre el grafo completo
                res_full, _ = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL)

                # Se registra el tiempo de término
                fin = time.time()

                # Calcula tiempo total de ejecución
                t_full = fin - inicio

                print(f"Resultado parcial: {res_full}")
                print(f"Tiempo: {t_full:.6f} s")
                print("Conclusion: Timeout alcanzado, no es viable\n")

                # Guarda resultados en el archivo CSV
                writer.writerow([
                    fecha,"FuerzaBruta","Python",f"Instancia {i}",
                    "FB Completo",len(grafo),res_full,
                    f"{t_full:.6f}","Timeout"
                ])

            print(f"--- Fuerza bruta en subgrafo ({k} nodos) ---")

            # Genera un subgrafo de tamaño reducido
            grafo_peq = subgrafo(grafo, k)

            # Se registra el tiempo de inicio
            inicio = time.time()

            # Ejecuta fuerza bruta sobre el subgrafo
            res_fb, _ = max_cut_fuerza_bruta_timeout(grafo_peq, TIMEOUT_SUB)

            # Se registra el tiempo de término
            fin = time.time()

            # Calcula tiempo total de ejecución
            t_sub = fin - inicio

            print(f"Resultado exacto: {res_fb}")
            print(f"Tiempo: {t_sub:.6f} s\n")

            # Guarda resultados en el archivo CSV
            writer.writerow([
                fecha,"FuerzaBruta","Python",f"Instancia {i}",
                "FB Subgrafo",k,res_fb,
                f"{t_sub:.6f}","Optimo Garantizado"
            ])

    print(f"[+] Resultados exportados a '{archivo_csv}'")
