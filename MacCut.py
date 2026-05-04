import itertools
import random
import time

# FUERZA BRUTA CON TIMEOUT 
def max_cut_fuerza_bruta_timeout(matriz, tiempo_limite):
    n = len(matriz)
    max_peso = float('-inf')
    mejor_particion = None

    inicio = time.time()

    for particion in itertools.product([0, 1], repeat=n):

        # timeout
        if time.time() - inicio > tiempo_limite:
            print("   [Timeout alcanzado]")
            break

        peso_actual = 0

        for i in range(n):
            for j in range(i + 1, n):
                if particion[i] != particion[j]:
                    peso_actual += matriz[i][j]

        if peso_actual > max_peso:
            max_peso = peso_actual
            mejor_particion = particion

    return max_peso, mejor_particion


# GREEDY 
def max_cut_greedy(matriz):
    n = len(matriz)
    particion = [random.choice([0, 1]) for _ in range(n)]

    hubo_mejora = True

    while hubo_mejora:
        hubo_mejora = False

        for i in range(n):
            peso_mismo = 0
            peso_otro = 0

            for j in range(n):
                if i != j:
                    if particion[i] == particion[j]:
                        peso_mismo += matriz[i][j]
                    else:
                        peso_otro += matriz[i][j]

            if peso_mismo > peso_otro:
                particion[i] = 1 - particion[i]
                hubo_mejora = True

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


# SUBGRAFO 
def subgrafo(grafo, k):
    k = min(k, len(grafo))
    return [fila[:k] for fila in grafo[:k]]


# MAIN 
if __name__ == "__main__":

    print("--- MAX CUT (5 INSTANCIAS) ---\n")

    TIMEOUT_REAL = 5
    TIMEOUT_SUB = 10
    k = 20

    for i in range(1, 6):

        nombre_archivo = f"instancias/g{i}.mc"

        print(f"===== INSTANCIA {i} =====")

        grafo = leer_grafo(nombre_archivo)

        # FUERZA BRUTA 
        if i == 1:
            print("--- Intento fuerza bruta (grafo completo) ---")

            inicio = time.time()
            res_full, _ = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL)
            fin = time.time()

            print(f"Resultado parcial: {res_full}")
            print(f"Tiempo: {fin - inicio:.6f} s")
            print("Conclusion: Timeout alcanzado, no es viable\n")

        # SUBGRAFO 
        print(f"--- Fuerza bruta en subgrafo ({k} nodos) ---")

        grafo_peq = subgrafo(grafo, k)

        inicio = time.time()
        res_fb, _ = max_cut_fuerza_bruta_timeout(grafo_peq, TIMEOUT_SUB)
        fin = time.time()

        print(f"Resultado exacto: {res_fb}")
        print(f"Tiempo: {fin - inicio:.6f} s\n")

        # GREEDY 
        print("--- Heuristica Greedy (grafo completo) ---")

        inicio = time.time()
        res_g, _ = max_cut_greedy(grafo)
        fin = time.time()

        print(f"Resultado: {res_g}")
        print(f"Tiempo: {fin - inicio:.6f} s\n")
