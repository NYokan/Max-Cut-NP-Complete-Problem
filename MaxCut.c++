#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <climits>

using namespace std;

// FUERZA BRUTA CON TIMEOUT
pair<int, vector<int>> max_cut_fuerza_bruta_timeout(
    const vector<vector<int>>& matriz_adyacencia,
    double tiempo_limite_segundos
) {
    int n = matriz_adyacencia.size();
    int max_peso = INT_MIN;
    vector<int> mejor_particion(n, 0);

    int total_combinaciones = 1 << n;

    auto inicio = chrono::high_resolution_clock::now();

    for (int mask = 0; mask < total_combinaciones; ++mask) {

        // verificar tiempo
        auto ahora = chrono::high_resolution_clock::now();
        double tiempo = chrono::duration<double>(ahora - inicio).count();

        if (tiempo > tiempo_limite_segundos) {
            cout << "   [Timeout alcanzado]\n";
            break;
        }

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

// GREEDY 
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
            int peso_mismo = 0;
            int peso_otro = 0;

            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    if (particion[i] == particion[j])
                        peso_mismo += matriz_adyacencia[i][j];
                    else
                        peso_otro += matriz_adyacencia[i][j];
                }
            }

            if (peso_mismo > peso_otro) {
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

// LEER ARCHIVO
vector<vector<int>> leer_grafo(const string& nombre_archivo) {
    ifstream archivo(nombre_archivo);

    if (!archivo.is_open()) {
        cout << "Error al abrir archivo: " << nombre_archivo << endl;
        exit(1);
    }

    int n, m;
    archivo >> n >> m;

    vector<vector<int>> matriz(n, vector<int>(n, 0));

    int u, v, w;
    while (archivo >> u >> v >> w) {
        u--; v--;
        matriz[u][v] = w;
        matriz[v][u] = w;
    }

    archivo.close();
    return matriz;
}

// CREAR SUBGRAFO 
vector<vector<int>> subgrafo(const vector<vector<int>>& grafo, int k) {
    int n = grafo.size();
    k = min(k, n);

    vector<vector<int>> sub(k, vector<int>(k, 0));

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            sub[i][j] = grafo[i][j];
        }
    }

    return sub;
}

int main() {
    srand(time(0));

    cout << "--- MAX CUT (5 INSTANCIAS) ---\n\n";

    double TIMEOUT_REAL = 5;
    double TIMEOUT_SUB = 10;  
    int k = 20;

    for (int i = 1; i <= 5; i++) {

        string nombre_archivo = "instancias/g" + to_string(i) + ".mc";

        cout << "===== INSTANCIA " << i << " =====\n";

        vector<vector<int>> grafo = leer_grafo(nombre_archivo);

    // BACKTRACKING INSTANCIA 1

    if (i == 1) {
        cout << "--- Intento fuerza bruta (grafo completo) ---\n";

        auto inicio_full = chrono::high_resolution_clock::now();
        auto resultado_full = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL);
        auto fin_full = chrono::high_resolution_clock::now();

        cout << "Resultado parcial: " << resultado_full.first << "\n";
        cout << "Tiempo: "
            << chrono::duration<double>(fin_full - inicio_full).count()
            << " s\n";

        cout << "Conclusion: Timeout alcanzado, por lo que no es viable\n\n";
}

        // BACKTRACKING CON SUBGRAFO

        cout << "--- Fuerza bruta en subgrafo (20 nodos) ---\n";

        vector<vector<int>> grafo_pequeno = subgrafo(grafo, k);

        auto inicio_fb = chrono::high_resolution_clock::now();
        auto resultado_fb = max_cut_fuerza_bruta_timeout(grafo_pequeno, TIMEOUT_SUB);
        auto fin_fb = chrono::high_resolution_clock::now();

        cout << "Resultado exacto: " << resultado_fb.first << "\n";
        cout << "Tiempo: "
             << chrono::duration<double>(fin_fb - inicio_fb).count()
             << " s\n\n";

        // GREEDY

        cout << "--- Heuristica Greedy (grafo completo) ---\n";

        auto inicio_g = chrono::high_resolution_clock::now();
        auto resultado_g = max_cut_greedy(grafo);
        auto fin_g = chrono::high_resolution_clock::now();

        cout << "Resultado: " << resultado_g.first << "\n";
        cout << "Tiempo: "
             << chrono::duration<double>(fin_g - inicio_g).count()
             << " s\n\n";
    }

    return 0;
}
