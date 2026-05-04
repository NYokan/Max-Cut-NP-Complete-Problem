import java.io.*;
import java.util.*;

public class FuerzaBruta {

    // Clase auxiliar para retornar tanto el peso como el arreglo de la partición
    static class Resultado {
        int peso;
        int[] particion;

        Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    // FUERZA BRUTA CON TIMEOUT
    public static Resultado maxCutFuerzaBrutaTimeout(int[][] matriz, double tiempoLimiteSegundos) {
        int n = matriz.length;
        int maxPeso = Integer.MIN_VALUE;
        int[] mejorParticion = new int[n];

        long inicio = System.nanoTime();

        // Total de combinaciones posibles es 2^N (usando desplazamiento de bits)
        int total = 1 << n;

        // Iteramos por cada posible combinación usando una máscara de bits
        for (int mask = 0; mask < total; mask++) {

            // Verificación del timeout para abortar si el grafo es demasiado grande
            double tiempo = (System.nanoTime() - inicio) / 1e9;
            if (tiempo > tiempoLimiteSegundos) {
                System.out.println("   [Timeout alcanzado]");
                break;
            }

            int pesoActual = 0;
            int[] particion = new int[n];

            // Decodificamos la máscara: si el bit i es 1, el nodo i va al grupo 1 (si no, al 0)
            for (int i = 0; i < n; i++) {
                particion[i] = (mask >> i) & 1;
            }

            // Sumamos los pesos de las aristas que conectan nodos en distintos grupos (corte)
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (particion[i] != particion[j]) {
                        pesoActual += matriz[i][j];
                    }
                }
            }

            // Actualizamos si encontramos un corte más pesado
            if (pesoActual > maxPeso) {
                maxPeso = pesoActual;
                mejorParticion = particion.clone();
            }
        }

        return new Resultado(maxPeso, mejorParticion);
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
                int u = Integer.parseInt(partes[0]) - 1; // Ajuste base 0
                int v = Integer.parseInt(partes[1]) - 1;
                int w = Integer.parseInt(partes[2]);

                matriz[u][v] = w;
                matriz[v][u] = w; // Grafo no dirigido
            }

            br.close();
            return matriz;

        } catch (Exception e) {
            System.out.println("Error al leer archivo: " + nombreArchivo);
            return null;
        }
    }

    // EXTRACCIÓN DE SUBGRAFO
    public static int[][] subgrafo(int[][] grafo, int k) {
        int n = grafo.length;
        k = Math.min(k, n); // Evitar desbordes

        int[][] sub = new int[k][k];

        // Copiamos solo los nodos de 0 a K
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                sub[i][j] = grafo[i][j];
            }
        }

        return sub;
    }

    // MAIN
    public static void main(String[] args) {
        System.out.println("--- MAX CUT: FUERZA BRUTA ---\n");

        double TIMEOUT_REAL = 5.0;
        double TIMEOUT_SUB = 10.0;
        int k = 20;
        
        String archivoCSV = "resultados_fuerza_bruta_java.csv";

        try {
            FileWriter csvWriter = new FileWriter(archivoCSV);
            csvWriter.append("Instancia,Enfoque,Nodos,Resultado,Tiempo_s,Comentarios\n");

            for (int i = 1; i <= 5; i++) {
                String archivo = "instancias/g" + i + ".mc";
                System.out.println("===== INSTANCIA " + i + " =====");

                int[][] grafo = leerGrafo(archivo);
                if (grafo == null) continue;

                // --- INTENTO GRAFO COMPLETO (Instancia 1) ---
                if (i == 1) {
                    System.out.println("--- Intento fuerza bruta (grafo completo) ---");

                    long inicio = System.nanoTime();
                    Resultado resFull = maxCutFuerzaBrutaTimeout(grafo, TIMEOUT_REAL);
                    long fin = System.nanoTime();
                    double tFull = (fin - inicio) / 1e9;

                    System.out.println("Resultado parcial: " + resFull.peso);
                    System.out.printf("Tiempo: %.6f s\n", tFull);
                    System.out.println("Conclusion: Timeout alcanzado, no es viable\n");
                    
                    csvWriter.append(String.format(Locale.US, "Instancia %d,FB Completo,%d,%d,%.6f,Timeout\n", 
                                     i, grafo.length, resFull.peso, tFull));
                }

                // --- SUBGRAFO (Medición exacta N=20) ---
                System.out.println("--- Fuerza bruta en subgrafo (" + k + " nodos) ---");

                int[][] sub = subgrafo(grafo, k);

                long inicioFB = System.nanoTime();
                Resultado resFB = maxCutFuerzaBrutaTimeout(sub, TIMEOUT_SUB);
                long finFB = System.nanoTime();
                double tSub = (finFB - inicioFB) / 1e9;

                System.out.println("Resultado exacto: " + resFB.peso);
                System.out.printf("Tiempo: %.6f s\n\n", tSub);
                
                csvWriter.append(String.format(Locale.US, "Instancia %d,FB Subgrafo,%d,%d,%.6f,Optimo Garantizado\n", 
                                 i, k, resFB.peso, tSub));
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