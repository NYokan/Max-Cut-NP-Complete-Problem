import java.io.*;
import java.util.*;

public class Greedy {

    static class Resultado {
        int peso;
        int[] particion;

        Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    // HEURÍSTICA GREEDY (BÚSQUEDA LOCAL DETERMINISTA)
    public static Resultado maxCutGreedy(int[][] matriz, int semilla) {
        int n = matriz.length;
        int[] particion = new int[n];
        
        // Semilla fija para reproducibilidad exacta y justificar los resultados del informe
        Random rand = new Random(semilla);

        for (int i = 0; i < n; i++) {
            particion[i] = rand.nextInt(2);
        }

        boolean huboMejora = true;

        // El algoritmo itera mientras logre mejorar el peso moviendo algún nodo
        while (huboMejora) {
            huboMejora = false;

            for (int i = 0; i < n; i++) {
                int mismo = 0;
                int otro = 0;

                // Calculamos cuánto peso suma estando en su grupo actual (mismo)
                // y cuánto sumaría si cruza las aristas al irse al otro grupo (otro)
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        if (particion[i] == particion[j])
                            mismo += matriz[i][j];
                        else
                            otro += matriz[i][j];
                    }
                }

                // Si gana más peso (corta más aristas) al cambiarse de grupo, lo movemos
                if (mismo > otro) {
                    particion[i] = 1 - particion[i]; // Cambio 0->1 o 1->0
                    huboMejora = true;
                }
            }
        }

        // Calculamos el peso real de la configuración final estabilizada
        int pesoFinal = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (particion[i] != particion[j]) {
                    pesoFinal += matriz[i][j];
                }
            }
        }

        return new Resultado(pesoFinal, particion);
    }

    // LECTURA DEL ARCHIVO .MC
    public static int[][] leerGrafo(String nombreArchivo) {
        try {
            BufferedReader br = new BufferedReader(new FileReader(nombreArchivo));

            String[] primeraLinea = br.readLine().split(" ");
            int n = Integer.parseInt(primeraLinea[0]);

            int[][] matriz = new int[n][n];

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

    // MAIN
    public static void main(String[] args) {
        System.out.println("--- MAX CUT: HEURISTICA GREEDY ---\n");

        String archivoCSV = "resultados_greedy_java.csv";

        try {
            FileWriter csvWriter = new FileWriter(archivoCSV);
            csvWriter.append("Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n");

            for (int i = 1; i <= 5; i++) {
                String archivo = "instancias/g" + i + ".mc";
                System.out.println("===== INSTANCIA " + i + " =====");

                int[][] grafo = leerGrafo(archivo);
                if (grafo == null) continue;

                // --- GREEDY GRAFO COMPLETO ---
                System.out.println("--- Ejecutando Búsqueda Local (Grafo completo) ---");

                long inicioG = System.nanoTime();
                // Usamos la semilla '2' como se documentó en el informe
                Resultado resG = maxCutGreedy(grafo, 2); 
                long finG = System.nanoTime();
                double tGreedy = (finG - inicioG) / 1e9;

                System.out.println("Resultado: " + resG.peso);
                System.out.printf("Tiempo: %.6f s\n\n", tGreedy);
                
                csvWriter.append(String.format(Locale.US, "Instancia %d,Heuristica Greedy,%d,%d,%.6f,Semilla Determinista (2)\n", 
                                 i, grafo.length, resG.peso, tGreedy));
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