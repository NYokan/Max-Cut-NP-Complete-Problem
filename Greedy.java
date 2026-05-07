import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;

public class Greedy {

    // Clase para almacenar el resultado del corte máximo
    static class Resultado {
        int peso;
        int[] particion;

        Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    // Obtiene la fecha y hora actual en formato "AAAA-MM-DD HH:MM:SS"
    public static String obtenerFechaHora() {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        return sdf.format(new Date());
    }

    // Verifica si el archivo para guardar datos existe
    public static boolean archivoExiste(String nombre) {
        File f = new File(nombre);
        return f.exists();
    }

    // Función heurística Greedy para el problema Max Cut
    public static Resultado maxCutGreedy(int[][] matriz, int semilla) {
        int n = matriz.length;
        int[] particion = new int[n];
        
        // Generador aleatorio con semilla fija para resultados reproducibles
        Random rand = new Random(semilla);

        // Genera una partición inicial aleatoria
        for (int i = 0; i < n; i++) {
            particion[i] = rand.nextInt(2);
        }

        boolean huboMejora = true;

        // Continúa mientras existan mejoras en la solución
        while (huboMejora) {
            huboMejora = false;

            // Revisa cada nodo para evaluar si cambiarlo mejora el corte
            for (int i = 0; i < n; i++) {
                int mismo = 0;
                int otro = 0;

                // Calcula pesos hacia nodos del mismo y del otro conjunto
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        if (particion[i] == particion[j])
                            mismo += matriz[i][j];
                        else
                            otro += matriz[i][j];
                    }
                }

                // Si mover el nodo mejora el corte, se cambia de partición
                if (mismo > otro) {
                    particion[i] = 1 - particion[i];
                    huboMejora = true;
                }
            }
        }

        int pesoFinal = 0;

        // Calcula el peso total del corte final obtenido
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (particion[i] != particion[j]) {
                    pesoFinal += matriz[i][j];
                }
            }
        }

        return new Resultado(pesoFinal, particion);
    }

    // Lee un grafo desde un archivo y lo representa como una matriz de adyacencia
    public static int[][] leerGrafo(String nombreArchivo) {
        try {
            BufferedReader br = new BufferedReader(new FileReader(nombreArchivo));

            String[] primeraLinea = br.readLine().split(" ");
            int n = Integer.parseInt(primeraLinea[0]);

            int[][] matriz = new int[n][n];

            // Lee las aristas y sus pesos, ajustando índices para que comiencen en 0
            String linea;
            while ((linea = br.readLine()) != null) {
                String[] partes = linea.split(" ");
                int u = Integer.parseInt(partes[0]) - 1;
                int v = Integer.parseInt(partes[1]) - 1;
                int w = Integer.parseInt(partes[2]);

                matriz[u][v] = w;
                matriz[v][u] = w;
            }

            br.close();
            return matriz;

        } catch (Exception e) {
            System.out.println("Error al leer archivo: " + nombreArchivo);
            return null;
        }
    }

    // Función principal para ejecutar el programa
    public static void main(String[] args) {
        System.out.println("--- MAX CUT: HEURISTICA GREEDY ---\n");

        // Configuración para guardar resultados en CSV (algoritmo y lenguaje)
        String archivoCSV = "resultados_greedy_java.csv";
        boolean existe = archivoExiste(archivoCSV);

        try {
            FileWriter csvWriter = new FileWriter(archivoCSV, true);

            if (!existe) {
                csvWriter.append("FechaHora,Algoritmo,Lenguaje,Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n");
            }

            String fecha = obtenerFechaHora();
            String algoritmo = "Greedy";
            String lenguaje = "Java";

            // Ejecuta el algoritmo sobre las distintas instancias
            for (int i = 1; i <= 5; i++) {
                String archivo = "instancias/g" + i + ".mc";
                System.out.println("===== INSTANCIA " + i + " =====");

                int[][] grafo = leerGrafo(archivo);

                // Continúa con la siguiente instancia si hubo error al leer
                if (grafo == null) continue;

                System.out.println("--- Ejecutando Búsqueda Local (Grafo completo) ---");

                // Se registra el tiempo de inicio
                long inicioG = System.nanoTime();

                // Ejecuta la heurística Greedy
                Resultado resG = maxCutGreedy(grafo, 2); 

                // Se registra el tiempo de término
                long finG = System.nanoTime();

                // Calcula tiempo total de ejecución
                double tGreedy = (finG - inicioG) / 1e9;

                System.out.println("Resultado: " + resG.peso);
                System.out.printf("Tiempo: %.6f s\n\n", tGreedy);
                
                // Guarda resultados en el archivo CSV
                csvWriter.append(String.format(Locale.US, "%s,%s,%s,Instancia %d,Heuristica Greedy,%d,%d,%.6f,Semilla Determinista (2)\n", 
                                 fecha, algoritmo, lenguaje, i, grafo.length, resG.peso, tGreedy));
            }
            
            csvWriter.flush();
            csvWriter.close();

            System.out.println("[+] Resultados exportados a '" + archivoCSV + "'");

        } catch (IOException e) {
            System.out.println("Error al escribir el archivo CSV.");
            e.printStackTrace();
        }
    }
}
