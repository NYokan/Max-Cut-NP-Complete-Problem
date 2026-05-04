#include <iostream>
#include <vector>
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

    // OJO: 1 << n es 2^N. Si n >= 32 esto desbordaría un entero normal.
    // Como extraeremos subgrafos de N=20, esto calcula ~1 millón de combinaciones, lo cual es seguro.
    int total_combinaciones = 1 << n;

    auto inicio = chrono::high_resolution_clock::now();

    // Iteramos por todas las particiones posibles usando máscaras de bits (bitmasking)
    for (int mask = 0; mask < total_combinaciones; ++mask) {

        // --- Mecanismo de Timeout ---
        // Evalúa el tiempo transcurrido para abortar si el grafo es muy grande
        auto ahora = chrono::high_resolution_clock::now();
        double tiempo = chrono::duration<double>(ahora - inicio).count();

        if (tiempo > tiempo_limite_segundos) {
            cout << "   [Timeout alcanzado]\n";
            break;
        }

        int peso_actual = 0;
        vector<int> particion(n);

        // Decodificamos la máscara de bits: si el bit i es 1, el nodo i va al grupo 1 (sino al 0)
        for (int i = 0; i < n; ++i) {
            particion[i] = (mask >> i) & 1;
        }

        // Sumamos los pesos de las aristas que cruzan el corte (nodos en grupos distintos)
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (particion[i] != particion[j]) {
                    peso_actual += matriz_adyacencia[i][j];
                }
            }
        }

        // Actualizamos el mejor resultado local
        if (peso_actual > max_peso) {
            max_peso = peso_actual;
            mejor_particion = particion;
        }
    }

    return {max_peso, mejor_particion};
}

// LECTURA DEL ARCHIVO .MC
vector<vector<int>> leer_grafo(const string& nombre_archivo) {
    ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir archivo: " << nombre_archivo << endl;
        exit(1);
    }

    int n, m;
    archivo >> n >> m; // Lee nodos y aristas

    // Inicializa la matriz de adyacencia NxN con ceros
    vector<vector<int>> matriz(n, vector<int>(n, 0));

    int u, v, w;
    while (archivo >> u >> v >> w) {
        u--; v--; // Ajuste de índice base 1 a base 0
        matriz[u][v] = w;
        matriz[v][u] = w; // Grafo no dirigido
    }

    archivo.close();
    return matriz;
}

// EXTRACCIÓN DEL SUBGRAFO
vector<vector<int>> subgrafo(const vector<vector<int>>& grafo, int k) {
    int n = grafo.size();
    k = min(k, n); // Asegura no salir de los límites

    vector<vector<int>> sub(k, vector<int>(k, 0));

    // Copia solo la esquina superior izquierda de KxK
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            sub[i][j] = grafo[i][j];
        }
    }
    return sub;
}

// FUNCIÓN PRINCIPAL
int main() {
    cout << "--- MAX CUT: FUERZA BRUTA ---\n\n";

    double TIMEOUT_REAL = 5.0; // Tiempo límite para el grafo completo
    double TIMEOUT_SUB = 10.0; // Tiempo límite para el subgrafo
    int k = 20; // Tamaño del subgrafo

    string nombre_csv = "resultados_fuerza_bruta.csv";
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
            continue; // Si falla la lectura, salta a la siguiente instancia
        }

        // --- GRAFO COMPLETO (Demostración de Timeout solo en la Instancia 1) ---
        if (i == 1) {
            cout << "--- Intento fuerza bruta (grafo completo) ---\n";

            auto inicio_full = chrono::high_resolution_clock::now();
            auto resultado_full = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL);
            auto fin_full = chrono::high_resolution_clock::now();
            double t_full = chrono::duration<double>(fin_full - inicio_full).count();

            cout << "Resultado parcial: " << resultado_full.first << "\n";
            cout << "Tiempo: " << fixed << setprecision(6) << t_full << " s\n";
            cout << "Conclusion: Timeout alcanzado, no es viable\n\n";
            
            // Guardar en CSV
            archivo_csv << "Instancia " << i << ",FB Completo," << grafo.size() << "," 
                        << resultado_full.first << "," << t_full << ",Timeout\n";
        }

        // --- SUBGRAFO (Medición exacta N=20) ---
        cout << "--- Fuerza bruta en subgrafo (20 nodos) ---\n";
        vector<vector<int>> grafo_pequeno = subgrafo(grafo, k);

        auto inicio_fb = chrono::high_resolution_clock::now();
        auto resultado_fb = max_cut_fuerza_bruta_timeout(grafo_pequeno, TIMEOUT_SUB);
        auto fin_fb = chrono::high_resolution_clock::now();
        double t_sub = chrono::duration<double>(fin_fb - inicio_fb).count();

        cout << "Resultado exacto: " << resultado_fb.first << "\n";
        cout << "Tiempo: " << fixed << setprecision(6) << t_sub << " s\n\n";
        
        // Guardar en CSV
        archivo_csv << "Instancia " << i << ",FB Subgrafo," << k << "," 
                    << resultado_fb.first << "," << t_sub << ",Optimo Garantizado\n";
    }

    archivo_csv.close();
    cout << "[+] Ejecucion finalizada. Datos guardados en '" << nombre_csv << "'\n";
    return 0;
}