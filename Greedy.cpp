#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

// HEURÍSTICA GREEDY (BÚSQUEDA LOCAL DETERMINISTA)
pair<int, vector<int>> max_cut_greedy(const vector<vector<int>>& matriz_adyacencia) {
    int n = matriz_adyacencia.size();
    vector<int> particion(n);

    // Asignación inicial (determinista, ya que el srand() se definió en el main)
    for (int i = 0; i < n; ++i) {
        particion[i] = rand() % 2;
    }

    bool hubo_mejora = true;

    // Iteramos mientras logremos mejorar el peso del corte intercambiando nodos
    while (hubo_mejora) {
        hubo_mejora = false;

        for (int i = 0; i < n; ++i) {
            int peso_mismo = 0;
            int peso_otro = 0;

            // Comparamos el peso de mantener el nodo en su grupo vs enviarlo al contrario
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    if (particion[i] == particion[j])
                        peso_mismo += matriz_adyacencia[i][j]; // Conexiones perdidas
                    else
                        peso_otro += matriz_adyacencia[i][j];  // Conexiones ganadas (cortadas)
                }
            }

            // Si mantenerlo en su grupo suma más, entonces lo cambiamos de bando para cortar esas aristas
            if (peso_mismo > peso_otro) {
                particion[i] = 1 - particion[i]; // Invierte el grupo (0->1 o 1->0)
                hubo_mejora = true;
            }
        }
    }

    // Calculamos el peso final real tras alcanzar el óptimo local
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

// LECTURA DEL ARCHIVO .MC
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

// FUNCIÓN PRINCIPAL
int main() {
    // --- SEMILLA DETERMINISTA ---
    // En vez de time(0), usamos una semilla fija (ej. 2) para que el estado inicial 
    // sea siempre idéntico y dependa puramente de la estructura iterativa de C++.
    srand(2); 

    cout << "--- MAX CUT: HEURISTICA GREEDY ---\n\n";

    string nombre_csv = "resultados_greedy.csv";
    ofstream archivo_csv(nombre_csv);
    
    // Escribimos los encabezados en el CSV
    archivo_csv << "Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n";

    for (int i = 1; i <= 5; i++) {
        string nombre_archivo = "instancias/g" + to_string(i) + ".mc";
        cout << "===== INSTANCIA " << i << " =====\n";

        vector<vector<int>> grafo;
        try {
            grafo = leer_grafo(nombre_archivo);
        } catch(...) {
            continue;
        }

        // --- GREEDY GRAFO COMPLETO ---
        cout << "--- Ejecutando Búsqueda Local (Grafo completo) ---\n";

        auto inicio_g = chrono::high_resolution_clock::now();
        auto resultado_g = max_cut_greedy(grafo);
        auto fin_g = chrono::high_resolution_clock::now();
        double t_greedy = chrono::duration<double>(fin_g - inicio_g).count();

        cout << "Resultado: " << resultado_g.first << "\n";
        cout << "Tiempo: " << fixed << setprecision(6) << t_greedy << " s\n\n";

        // Guardar en CSV
        archivo_csv << "Instancia " << i << ",Heuristica Greedy," << grafo.size() << "," 
                    << resultado_g.first << "," << t_greedy << ",Semilla Determinista (2)\n";
    }

    archivo_csv.close();
    cout << "[+] Ejecucion finalizada. Datos guardados en '" << nombre_csv << "'\n";
    return 0;
}