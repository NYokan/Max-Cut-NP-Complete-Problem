import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;

public class Greedy {

    static class Resultado {
        int peso;
        int[] particion;

        Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    public static String obtenerFechaHora() {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        return sdf.format(new Date());
    }

    public static boolean archivoExiste(String nombre) {
        File f = new File(nombre);
        return f.exists();
    }

    public static Resultado maxCutGreedy(int[][] matriz, int semilla) {
        int n = matriz.length;
        int[] particion = new int[n];
        
        Random rand = new Random(semilla);

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

    public static void main(String[] args) {
        System.out.println("--- MAX CUT: HEURISTICA GREEDY ---\n");

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

            for (int i = 1; i <= 5; i++) {
                String archivo = "instancias/g" + i + ".mc";
                System.out.println("===== INSTANCIA " + i + " =====");

                int[][] grafo = leerGrafo(archivo);
                if (grafo == null) continue;

                System.out.println("--- Ejecutando Búsqueda Local (Grafo completo) ---");

                long inicioG = System.nanoTime();
                Resultado resG = maxCutGreedy(grafo, 2); 
                long finG = System.nanoTime();
                double tGreedy = (finG - inicioG) / 1e9;

                System.out.println("Resultado: " + resG.peso);
                System.out.printf("Tiempo: %.6f s\n\n", tGreedy);
                
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
