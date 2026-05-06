#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
#include <sys/stat.h>

using namespace std;

string obtener_fecha_hora() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

bool archivo_existe(const string& nombre) {
    struct stat buffer;
    return (stat(nombre.c_str(), &buffer) == 0);
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

int main() {
    srand(2); 

    cout << "--- MAX CUT: HEURISTICA GREEDY ---\n\n";

    string nombre_csv = "resultados_greedy_cpp.csv";
    bool existe = archivo_existe(nombre_csv);
    ofstream archivo_csv(nombre_csv, ios::app);
    
    if (!existe) {
        archivo_csv << "FechaHora,Algoritmo,Lenguaje,Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n";
    }

    string fecha = obtener_fecha_hora();
    string algoritmo = "Greedy";
    string lenguaje = "C++";

    for (int i = 1; i <= 5; i++) {
        string nombre_archivo = "instancias/g" + to_string(i) + ".mc";
        cout << "===== INSTANCIA " << i << " =====\n";

        vector<vector<int>> grafo;
        try {
            grafo = leer_grafo(nombre_archivo);
        } catch(...) {
            continue;
        }

        cout << "--- Ejecutando Búsqueda Local (Grafo completo) ---\n";

        auto inicio_g = chrono::high_resolution_clock::now();
        auto resultado_g = max_cut_greedy(grafo);
        auto fin_g = chrono::high_resolution_clock::now();
        double t_greedy = chrono::duration<double>(fin_g - inicio_g).count();

        cout << "Resultado: " << resultado_g.first << "\n";
        cout << "Tiempo: " << fixed << setprecision(6) << t_greedy << " s\n\n";

        archivo_csv << fecha << "," << algoritmo << "," << lenguaje << ","
                    << "Instancia " << i << ",Heuristica Greedy," << grafo.size() << ","
                    << resultado_g.first << "," << t_greedy << ",Semilla Determinista (2)\n";
    }

    archivo_csv.close();
    cout << "[+] Ejecucion finalizada. Datos guardados en '" << nombre_csv << "'\n";
    return 0;
}
