import random
import time
import csv
import os
from datetime import datetime

def obtener_fecha_hora():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

def archivo_existe(nombre):
    return os.path.isfile(nombre)

def max_cut_greedy(matriz, semilla=2):
    n = len(matriz)
    
    random.seed(semilla)
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

if __name__ == "__main__":
    print("--- MAX CUT: HEURÍSTICA GREEDY ---\n")
    
    archivo_csv = "resultados_greedy_python.csv"
    existe = archivo_existe(archivo_csv)

    with open(archivo_csv, mode='a', newline='', encoding='utf-8') as f_csv:
        writer = csv.writer(f_csv)

        if not existe:
            writer.writerow(["FechaHora","Algoritmo","Lenguaje","Instancia","Enfoque","Nodos","Resultado","Tiempo_s","Comentarios"])

        fecha = obtener_fecha_hora()
        algoritmo = "Greedy"
        lenguaje = "Python"

        for i in range(1, 6):
            nombre_archivo = f"instancias/g{i}.mc"
            print(f"===== INSTANCIA {i} =====")
            
            try:
                grafo = leer_grafo(nombre_archivo)
            except FileNotFoundError:
                print(f"Error: No se encontró el archivo {nombre_archivo}\n")
                continue

            print("--- Ejecutando Búsqueda Local (Grafo completo) ---")

            inicio = time.time()
            res_g, _ = max_cut_greedy(grafo, semilla=2) 
            fin = time.time()
            t_greedy = fin - inicio

            print(f"Resultado: {res_g}")
            print(f"Tiempo: {t_greedy:.6f} s\n")
            
            writer.writerow([fecha,"Greedy","Python",f"Instancia {i}","Heuristica Greedy",len(grafo),res_g,f"{t_greedy:.6f}","Semilla Determinista (2)"])

    print(f"[+] Resultados exportados a '{archivo_csv}'")
