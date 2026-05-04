import java.io.*;
import java.util.*;

public class MaxCut {

    static class Resultado {
        int peso;
        int[] particion;

        Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    //FUERZA BRUTA CON TIMEOUT
    public static Resultado maxCutFuerzaBrutaTimeout(int[][] matriz, double tiempoLimiteSegundos) {
        int n = matriz.length;
        int maxPeso = Integer.MIN_VALUE;
        int[] mejorParticion = new int[n];

        long inicio = System.nanoTime();

        int total = 1 << n;

        for (int mask = 0; mask < total; mask++) {

            double tiempo = (System.nanoTime() - inicio) / 1e9;
            if (tiempo > tiempoLimiteSegundos) {
                System.out.println("   [Timeout alcanzado]");
                break;
            }

            int pesoActual = 0;
            int[] particion = new int[n];

            for (int i = 0; i < n; i++) {
                particion[i] = (mask >> i) & 1;
            }

            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (particion[i] != particion[j]) {
                        pesoActual += matriz[i][j];
                    }
                }
            }

            if (pesoActual > maxPeso) {
                maxPeso = pesoActual;
                mejorParticion = particion.clone();
            }
        }

        return new Resultado(maxPeso, mejorParticion);
    }

    // GREEDY 
    public static Resultado maxCutGreedy(int[][] matriz) {
        int n = matriz.length;
        int[] particion = new int[n];
        Random rand = new Random();

        for (int i = 0; i < n; i++) {
            particion[i] = rand.nextInt(2);
        }

        boolean huboMejora = true;

        while (huboMejora) {
            huboMejora = false;

            for (int i = 0; i < n; i++) {
                int mismo = 0;
                int otro = 0;

                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        if (particion[i] == particion[j])
                            mismo += matriz[i][j];
                        else
                            otro += matriz[i][j];
                    }
                }

                if (mismo > otro) {
                    particion[i] = 1 - particion[i];
                    huboMejora = true;
                }
            }
        }

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

    // LEER ARCHIVO
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
            e.printStackTrace();
            return null;
        }
    }

    // SUBGRAFO 
    public static int[][] subgrafo(int[][] grafo, int k) {
        int n = grafo.length;
        k = Math.min(k, n);

        int[][] sub = new int[k][k];

        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                sub[i][j] = grafo[i][j];
            }
        }

        return sub;
    }

    // MAIN 
    public static void main(String[] args) {

        System.out.println("--- MAX CUT (5 INSTANCIAS) ---\n");

        double TIMEOUT_REAL = 5;
        double TIMEOUT_SUB = 10;
        int k = 20;

        for (int i = 1; i <= 5; i++) {

            String archivo = "instancias/g" + i + ".mc";

            System.out.println("===== INSTANCIA " + i + " =====");

            int[][] grafo = leerGrafo(archivo);

            // BACKTRACKING
            if (i == 1) {
                System.out.println("--- Intento fuerza bruta (grafo completo) ---");

                long inicio = System.nanoTime();
                Resultado resFull = maxCutFuerzaBrutaTimeout(grafo, TIMEOUT_REAL);
                long fin = System.nanoTime();

                System.out.println("Resultado parcial: " + resFull.peso);
                System.out.printf("Tiempo: %.6f s\n", (fin - inicio) / 1e9);
                System.out.println("Conclusion: Timeout alcanzado, no es viable\n");
            }

            // SUBGRAFO
            System.out.println("--- Fuerza bruta en subgrafo (" + k + " nodos) ---");

            int[][] sub = subgrafo(grafo, k);

            long inicioFB = System.nanoTime();
            Resultado resFB = maxCutFuerzaBrutaTimeout(sub, TIMEOUT_SUB);
            long finFB = System.nanoTime();

            System.out.println("Resultado exacto: " + resFB.peso);
            System.out.printf("Tiempo: %.6f s\n\n", (finFB - inicioFB) / 1e9);

            // GREEDY 
            System.out.println("--- Heuristica Greedy (grafo completo) ---");

            long inicioG = System.nanoTime();
            Resultado resG = maxCutGreedy(grafo);
            long finG = System.nanoTime();

            System.out.println("Resultado: " + resG.peso);
            System.out.printf("Tiempo: %.6f s\n\n", (finG - inicioG) / 1e9);
        }
    }
}
