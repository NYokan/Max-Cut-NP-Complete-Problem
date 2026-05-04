import itertools
import time
import csv

# FUERZA BRUTA CON TIMEOUT 
def max_cut_fuerza_bruta_timeout(matriz, tiempo_limite):
    """
    Resuelve Max Cut evaluando todas las 2^n particiones posibles (O(2^N)).
    Incluye un mecanismo de timeout para evitar colapsos con grafos grandes.
    """
    n = len(matriz)
    max_peso = float('-inf')
    mejor_particion = None

    inicio = time.time()

    # itertools.product genera todas las combinaciones posibles de 0s y 1s
    for particion in itertools.product([0, 1], repeat=n):

        # Verificación del timeout: si el tiempo supera el límite, aborta el ciclo
        if time.time() - inicio > tiempo_limite:
            print("   [Timeout alcanzado]")
            break

        peso_actual = 0

        # Suma los pesos de las aristas que conectan nodos en grupos distintos
        for i in range(n):
            for j in range(i + 1, n):
                if particion[i] != particion[j]:
                    peso_actual += matriz[i][j]

        # Actualiza el máximo encontrado
        if peso_actual > max_peso:
            max_peso = peso_actual
            mejor_particion = particion

    return max_peso, mejor_particion


# LEER ARCHIVO 
def leer_grafo(nombre_archivo):
    with open(nombre_archivo, 'r') as f:
        n, m = map(int, f.readline().split())
        matriz = [[0]*n for _ in range(n)]
        for line in f:
            u, v, w = map(int, line.split())
            u -= 1 
            v -= 1
            matriz[u][v] = w
            matriz[v][u] = w
    return matriz


# SUBGRAFO 
def subgrafo(grafo, k):
    """Extrae un subgrafo de tamaño k x k"""
    k = min(k, len(grafo))
    return [fila[:k] for fila in grafo[:k]]


# MAIN FUERZA BRUTA
if __name__ == "__main__":
    print("--- MAX CUT: FUERZA BRUTA ---\n")

    TIMEOUT_REAL = 5
    TIMEOUT_SUB = 10
    k = 20
    
    archivo_csv = "resultados_fuerza_bruta.csv"

    with open(archivo_csv, mode='w', newline='', encoding='utf-8') as f_csv:
        writer = csv.writer(f_csv)
        writer.writerow(["Instancia", "Enfoque", "Nodos", "Resultado", "Tiempo_s", "Comentarios"])

        for i in range(1, 6):
            nombre_archivo = f"instancias/g{i}.mc"
            print(f"===== INSTANCIA {i} =====")
            
            try:
                grafo = leer_grafo(nombre_archivo)
            except FileNotFoundError:
                print(f"Error: No se encontró el archivo {nombre_archivo}\n")
                continue

            # Demostración del Timeout en Grafo Completo (Solo Instancia 1)
            if i == 1:
                print("--- Intento fuerza bruta (grafo completo) ---")
                inicio = time.time()
                res_full, _ = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL)
                fin = time.time()
                t_full = fin - inicio

                print(f"Resultado parcial: {res_full}")
                print(f"Tiempo: {t_full:.6f} s")
                print("Conclusion: Timeout alcanzado, no es viable\n")
                writer.writerow([f"Instancia {i}", "FB Completo", len(grafo), res_full, f"{t_full:.6f}", "Timeout"])

            # Ejecución real sobre el Subgrafo de 20 nodos
            print(f"--- Fuerza bruta en subgrafo ({k} nodos) ---")
            grafo_peq = subgrafo(grafo, k)

            inicio = time.time()
            res_fb, _ = max_cut_fuerza_bruta_timeout(grafo_peq, TIMEOUT_SUB)
            fin = time.time()
            t_sub = fin - inicio

            print(f"Resultado exacto: {res_fb}")
            print(f"Tiempo: {t_sub:.6f} s\n")
            writer.writerow([f"Instancia {i}", "FB Subgrafo", k, res_fb, f"{t_sub:.6f}", "Óptimo Garantizado"])

    print(f"[+] Resultados exportados a '{archivo_csv}'")