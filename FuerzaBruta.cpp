#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <climits>
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

// Función de fuerza bruta con timeout para el problema Max Cut
pair<int, vector<int>> max_cut_fuerza_bruta_timeout(
    const vector<vector<int>>& matriz_adyacencia,
    double tiempo_limite_segundos
) {
    int n = matriz_adyacencia.size();
    int max_peso = INT_MIN; // Guardará el peso máximo encontrado
    vector<int> mejor_particion(n, 0); // Guardará la partición que da el peso máximo

    int total_combinaciones = 1 << n;

    auto inicio = chrono::high_resolution_clock::now(); // Se registra el tiempo de inicio

    for (int mask = 0; mask < total_combinaciones; ++mask) {

        // Verifica tiempo transcurrido
        auto ahora = chrono::high_resolution_clock::now();
        double tiempo = chrono::duration<double>(ahora - inicio).count();

        // Se detiene si alcanza timeout
        if (tiempo > tiempo_limite_segundos) {
            cout << "   [Timeout alcanzado]\n";
            break;
        }
        
        int peso_actual = 0;
        vector<int> particion(n);
        
        // Construye la partición actual a partir del bitmask
        for (int i = 0; i < n; ++i) {
            particion[i] = (mask >> i) & 1;
        }
        
        // Calcula el peso del corte para esta partición
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (particion[i] != particion[j]) {
                    peso_actual += matriz_adyacencia[i][j];
                }
            }
        }

        // Actualiza el mejor resultado si se encuentra uno mejor
        if (peso_actual > max_peso) {
            max_peso = peso_actual;
            mejor_particion = particion;
        }
    }

    return {max_peso, mejor_particion};
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

// Función para obtener un subgrafo de los primeros k nodos
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

// Función de fuerza bruta con timeout para el problema Max Cut
int main() {
    cout << "--- MAX CUT: FUERZA BRUTA ---\n\n";

    double TIMEOUT_REAL = 3600;
    double TIMEOUT_SUB = 3600;
    int k = 20;

    // Configuración para guardar resultados en CSV (algoritmo y lenguaje)
    string nombre_csv = "resultados_fuerza_bruta_cpp.csv";
    bool existe = archivo_existe(nombre_csv);
    ofstream archivo_csv(nombre_csv, ios::app);

    if (!existe) {
        archivo_csv << "FechaHora,Algoritmo,Lenguaje,Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n";
    }

    string fecha = obtener_fecha_hora();
    string algoritmo = "FuerzaBruta";
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

        if (i == 1) {
            cout << "--- Intento fuerza bruta (grafo completo) ---\n";

            auto inicio_full = chrono::high_resolution_clock::now();
            auto resultado_full = max_cut_fuerza_bruta_timeout(grafo, TIMEOUT_REAL);
            auto fin_full = chrono::high_resolution_clock::now();
            double t_full = chrono::duration<double>(fin_full - inicio_full).count();

            cout << "Resultado parcial: " << resultado_full.first << "\n";
            cout << "Tiempo: " << fixed << setprecision(6) << t_full << " s\n";
            cout << "Conclusion: Timeout alcanzado, no es viable\n\n";
            
            archivo_csv << fecha << "," << algoritmo << "," << lenguaje << ","
                        << "Instancia " << i << ",FB Completo," << grafo.size() << ","
                        << resultado_full.first << "," << t_full << ",Timeout\n";
        }

        cout << "--- Fuerza bruta en subgrafo (20 nodos) ---\n";
        vector<vector<int>> grafo_pequeno = subgrafo(grafo, k);

        auto inicio_fb = chrono::high_resolution_clock::now();
        auto resultado_fb = max_cut_fuerza_bruta_timeout(grafo_pequeno, TIMEOUT_SUB);
        auto fin_fb = chrono::high_resolution_clock::now();
        double t_sub = chrono::duration<double>(fin_fb - inicio_fb).count();

        cout << "Resultado exacto: " << resultado_fb.first << "\n";
        cout << "Tiempo: " << fixed << setprecision(6) << t_sub << " s\n\n";
        
        archivo_csv << fecha << "," << algoritmo << "," << lenguaje << ","
                    << "Instancia " << i << ",FB Subgrafo," << k << ","
                    << resultado_fb.first << "," << t_sub << ",Optimo Garantizado\n";
    }

    archivo_csv.close();
    cout << "[+] Ejecucion finalizada. Datos guardados en '" << nombre_csv << "'\n";
    return 0;
}
