import itertools
import random
import time

def max_cut_fuerza_bruta(matriz_adyacencia):
    n = len(matriz_adyacencia)
    max_peso = 0
    mejor_particion = None
    
    todas_las_combinaciones = itertools.product([0, 1], repeat=n)
    
    for particion in todas_las_combinaciones:
        peso_actual = 0
        for i in range(n):
            for j in range(i + 1, n):
                if particion[i] != particion[j]:
                    peso_actual += matriz_adyacencia[i][j]
                    
        if peso_actual > max_peso:
            max_peso = peso_actual
            mejor_particion = particion
            
    return max_peso, mejor_particion

def max_cut_greedy(matriz_adyacencia):
    n = len(matriz_adyacencia)
    particion = [random.choice([0, 1]) for _ in range(n)]
    hubo_mejora = True
    
    while hubo_mejora:
        hubo_mejora = False
        
        for i in range(n):
            peso_mismo_conjunto = 0
            peso_otro_conjunto = 0
            
            for j in range(n):
                if i != j and matriz_adyacencia[i][j] > 0:
                    if particion[i] == particion[j]:
                        peso_mismo_conjunto += matriz_adyacencia[i][j]
                    else:
                        peso_otro_conjunto += matriz_adyacencia[i][j]
            
            if peso_mismo_conjunto > peso_otro_conjunto:
                particion[i] = 1 - particion[i]
                hubo_mejora = True
                
    peso_final = 0
    for i in range(n):
        for j in range(i + 1, n):
            if particion[i] != particion[j]:
                peso_final += matriz_adyacencia[i][j]
                
    return peso_final, particion

if __name__ == "__main__":
    grafo_prueba = [
        [0,5,0,4,1,2,0,3,1,2],
        [5,0,4,0,2,1,3,0,2,1],
        [0,4,0,5,3,2,1,2,0,3],
        [4,0,5,0,2,3,2,1,4,0],
        [1,2,3,2,0,4,0,3,2,1],
        [2,1,2,3,4,0,5,0,1,2],
        [0,3,1,2,0,5,0,4,2,3],
        [3,0,2,1,3,0,4,0,5,2],
        [1,2,0,4,2,1,2,5,0,3],
        [2,1,3,0,1,2,3,2,3,0]
    ]
    
    print("--- INICIANDO PRUEBAS MAX CUT EN PYTHON ---")
    
    inicio_fb = time.time()
    peso_fb, particion_fb = max_cut_fuerza_bruta(grafo_prueba)
    fin_fb = time.time()
    
    print(f"\n1. FUERZA BRUTA:")
    print(f"   Peso máximo del corte: {peso_fb}")
    print(f"   Partición óptima: {particion_fb}")
    print(f"   Tiempo de ejecución: {(fin_fb - inicio_fb):.6f} segundos")
    
    inicio_greedy = time.time()
    peso_greedy, particion_greedy = max_cut_greedy(grafo_prueba)
    fin_greedy = time.time()
    
    print(f"\n2. HEURÍSTICA (GREEDY / LOCAL SEARCH):")
    print(f"   Peso del corte encontrado: {peso_greedy}")
    print(f"   Partición encontrada: {particion_greedy}")
    print(f"   Tiempo de ejecución: {(fin_greedy - inicio_greedy):.6f} segundos")
