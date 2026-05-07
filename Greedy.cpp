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

// Obtiene fecha y hora actual en formato "AAAA-MM-DD HH:MM:SS" para agregarlo al CSV.
string obtener_fecha_hora() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

// Verifica si el archivo para guardar datos existe.
bool archivo_existe(const string& nombre) {
    struct stat buffer;
    return (stat(nombre.c_str(), &buffer) == 0);
}

// Función heurística Greedy para el problema Max Cut
pair<int, vector<int>> max_cut_greedy(const vector<vector<int>>& matriz_adyacencia) {
    int n = matriz_adyacencia.size();
    vector<int> particion(n);

    // Genera una partición inicial aleatoria
    for (int i = 0; i < n; ++i) {
        particion[i] = rand() % 2;
    }

    bool hubo_mejora = true;

    // Continúa mientras existan mejoras en la solución
    while (hubo_mejora) {
        hubo_mejora = false;

        // Revisa cada nodo para evaluar si cambiarlo mejora el corte
        for (int i = 0; i < n; ++i) {
            int peso_mismo = 0;
            int peso_otro = 0;

            // Calcula pesos hacia nodos del mismo y del otro conjunto
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    if (particion[i] == particion[j])
                        peso_mismo += matriz_adyacencia[i][j];
                    else
                        peso_otro += matriz_adyacencia[i][j];
                }
            }

            // Si mover el nodo mejora el corte, se cambia de partición
            if (peso_mismo > peso_otro) {
                particion[i] = 1 - particion[i];
                hubo_mejora = true;
            }
        }
    }

    int peso_final = 0;

    // Calcula el peso total del corte final obtenido
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (particion[i] != particion[j]) {
                peso_final += matriz_adyacencia[i][j];
            }
        }
    }

    return {peso_final, particion};
}

// Función para leer grafo desde el archivo dado
vector<vector<int>> leer_grafo(const string& nombre_archivo) {
    ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir archivo: " << nombre_archivo << endl;
        exit(1);
    }

    int n, m;
    archivo >> n >> m;

    vector<vector<int>> matriz(n, vector<int>(n, 0));

    // Lee las aristas y sus pesos, ajustando índices para que comiencen en 0
    int u, v, w;
    while (archivo >> u >> v >> w) {
        u--; v--;
        matriz[u][v] = w;
        matriz[v][u] = w;
    }

    archivo.close();
    return matriz;
}

// Función principal para ejecutar la heurística Greedy
int main() {
    srand(2); // Semilla fija para obtener resultados reproducibles

    cout << "--- MAX CUT: HEURISTICA GREEDY ---\n\n";

    // Configuración para guardar resultados en CSV (algoritmo y lenguaje)
    string nombre_csv = "resultados_greedy_cpp.csv";
    bool existe = archivo_existe(nombre_csv);
    ofstream archivo_csv(nombre_csv, ios::app);
    
    if (!existe) {
        archivo_csv << "FechaHora,Algoritmo,Lenguaje,Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n";
    }

    string fecha = obtener_fecha_hora();
    string algoritmo = "Greedy";
    string lenguaje = "C++";

    // Ejecuta el algoritmo sobre las distintas instancias
    for (int i = 1; i <= 5; i++) {
        string nombre_archivo = "instancias/g" + to_string(i) + ".mc";
        cout << "===== INSTANCIA " << i << " =====\n";

        vector<vector<int>> grafo;

        // Intenta cargar el grafo desde archivo
        try {
            grafo = leer_grafo(nombre_archivo);
        } catch(...) {
            continue;
        }

        cout << "--- Ejecutando Búsqueda Local (Grafo completo) ---\n";

        // Se registra el tiempo de inicio
        auto inicio_g = chrono::high_resolution_clock::now();

        // Ejecuta la heurística Greedy
        auto resultado_g = max_cut_greedy(grafo);

        // Se registra el tiempo de término
        auto fin_g = chrono::high_resolution_clock::now();

        // Calcula tiempo total de ejecución
        double t_greedy = chrono::duration<double>(fin_g - inicio_g).count();

        cout << "Resultado: " << resultado_g.first << "\n";
        cout << "Tiempo: " << fixed << setprecision(6) << t_greedy << " s\n\n";

        // Guarda resultados en el archivo CSV
        archivo_csv << fecha << "," << algoritmo << "," << lenguaje << ","
                    << "Instancia " << i << ",Heuristica Greedy," << grafo.size() << ","
                    << resultado_g.first << "," << t_greedy << ",Semilla Determinista (2)\n";
    }

    archivo_csv.close();

    cout << "[+] Ejecucion finalizada. Datos guardados en '" << nombre_csv << "'\n";
    return 0;
}
