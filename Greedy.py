import random
import time
import csv

# GREEDY (Determinista)
def max_cut_greedy(matriz, semilla=2):
    """
    Heurística Greedy de búsqueda local.
    Utiliza una semilla para inicializar la partición de forma determinista.
    """
    n = len(matriz)
    
    # Fijamos la semilla para que el estado inicial sea reproducible
    random.seed(semilla)
    particion = [random.choice([0, 1]) for _ in range(n)]

    hubo_mejora = True

    # El ciclo continúa mientras mover un nodo genere una mejora
    while hubo_mejora:
        hubo_mejora = False

        for i in range(n):
            peso_mismo = 0
            peso_otro = 0

            # Evaluamos la relación del nodo i con el resto de los nodos j
            for j in range(n):
                if i != j:
                    if particion[i] == particion[j]:
                        peso_mismo += matriz[i][j] 
                    else:
                        peso_otro += matriz[i][j]  

            # Si el nodo suma más peso en su mismo grupo que en el otro, conviene cambiarlo.
            if peso_mismo > peso_otro:
                particion[i] = 1 - particion[i]
                hubo_mejora = True

    # Cálculo del peso final exacto del corte
    peso_final = 0
    for i in range(n):
        for j in range(i + 1, n):
            if particion[i] != particion[j]:
                peso_final += matriz[i][j]

    return peso_final, particion


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


# MAIN GREEDY
if __name__ == "__main__":
    print("--- MAX CUT: HEURÍSTICA GREEDY ---\n")
    
    archivo_csv = "resultados_greedy.csv"

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

            # GREEDY en Grafo Completo
            print("--- Ejecutando Búsqueda Local (Grafo completo) ---")

            inicio = time.time()
            # Usamos la semilla 2 como documentamos en el informe
            res_g, _ = max_cut_greedy(grafo, semilla=2) 
            fin = time.time()
            t_greedy = fin - inicio

            print(f"Resultado: {res_g}")
            print(f"Tiempo: {t_greedy:.6f} s\n")
            
            writer.writerow([f"Instancia {i}", "Heurística Greedy", len(grafo), res_g, f"{t_greedy:.6f}", "Semilla Determinista"])

    print(f"[+] Resultados exportados a '{archivo_csv}'")