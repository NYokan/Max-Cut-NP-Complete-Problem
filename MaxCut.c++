#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;

pair<int, vector<int>> max_cut_fuerza_bruta(const vector<vector<int>>& matriz_adyacencia) {
    int n = matriz_adyacencia.size();
    int max_peso = 0;
    vector<int> mejor_particion(n, 0);
    int total_combinaciones = 1 << n;

    for (int mask = 0; mask < total_combinaciones; ++mask) {
        int peso_actual = 0;
        vector<int> particion(n);
        for (int i = 0; i < n; ++i) {
            particion[i] = (mask >> i) & 1;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (particion[i] != particion[j]) {
                    peso_actual += matriz_adyacencia[i][j];
                }
            }
        }

        if (peso_actual > max_peso) {
            max_peso = peso_actual;
            mejor_particion = particion;
        }
    }
    return {max_peso, mejor_particion};
}

pair<int, vector<int>> max_cut_greedy(const vector<vector<int>>& matriz_adyacencia) {
    int n = matriz_adyacencia.size();
    vector<int> particion(n);
    for (int i = 0; i < n; ++i) {
        particion[i] = rand() % 2;
    }

    bool hubo_mejora = true;
    while (hubo_mejora) {
        hubo_mejora = false;
        for (int i = 0; i < n; ++i) {
            int peso_mismo_conjunto = 0;
            int peso_otro_conjunto = 0;

            for (int j = 0; j < n; ++j) {
                if (i != j && matriz_adyacencia[i][j] > 0) {
                    if (particion[i] == particion[j]) {
                        peso_mismo_conjunto += matriz_adyacencia[i][j];
                    } else {
                        peso_otro_conjunto += matriz_adyacencia[i][j];
                    }
                }
            }

            if (peso_mismo_conjunto > peso_otro_conjunto) {
                particion[i] = 1 - particion[i];
                hubo_mejora = true;
            }
        }
    }

    int peso_final = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (particion[i] != particion[j]) {
                peso_final += matriz_adyacencia[i][j];
            }
        }
    }
    return {peso_final, particion};
}

int main() {
    srand(time(0));
    vector<vector<int>> grafo_prueba = {
        {0,5,0,4,1,2,0,3,1,2},
        {5,0,4,0,2,1,3,0,2,1},
        {0,4,0,5,3,2,1,2,0,3},
        {4,0,5,0,2,3,2,1,4,0},
        {1,2,3,2,0,4,0,3,2,1},
        {2,1,2,3,4,0,5,0,1,2},
        {0,3,1,2,0,5,0,4,2,3},
        {3,0,2,1,3,0,4,0,5,2},
        {1,2,0,4,2,1,2,5,0,3},
        {2,1,3,0,1,2,3,2,3,0}
    };


    cout << "--- INICIANDO PRUEBAS MAX CUT EN C++ ---\n\n";

    auto inicio_fb = chrono::high_resolution_clock::now();
    auto resultado_fb = max_cut_fuerza_bruta(grafo_prueba);
    auto fin_fb = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion_fb = fin_fb - inicio_fb;

    cout << fixed << setprecision(9);

    cout << "1. FUERZA BRUTA:\n";
    cout << "   Peso maximo: " << resultado_fb.first << "\n";
    cout << "   Tiempo: " << duracion_fb.count() << " segundos\n\n";

    auto inicio_greedy = chrono::high_resolution_clock::now();
    auto resultado_greedy = max_cut_greedy(grafo_prueba);
    auto fin_greedy = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion_greedy = fin_greedy - inicio_greedy;

    cout << "2. HEURISTICA (GREEDY):\n";
    cout << "   Peso maximo: " << resultado_greedy.first << "\n";
    cout << "   Tiempo: " << duracion_greedy.count() << " segundos\n";

    return 0;
}
