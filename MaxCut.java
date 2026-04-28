import java.util.Arrays;
import java.util.Random;

public class MaxCut {

    public static class Resultado {
        int peso;
        int[] particion;

        public Resultado(int peso, int[] particion) {
            this.peso = peso;
            this.particion = particion;
        }
    }

    public static Resultado maxCutFuerzaBruta(int[][] matrizAdyacencia) {
        int n = matrizAdyacencia.length;
        int maxPeso = 0;
        int[] mejorParticion = new int[n];
        int totalCombinaciones = 1 << n;

        for (int mask = 0; mask < totalCombinaciones; mask++) {
            int pesoActual = 0;
            int[] particion = new int[n];
            
            for (int i = 0; i < n; i++) {
                particion[i] = (mask >> i) & 1;
            }

            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (particion[i] != particion[j]) {
                        pesoActual += matrizAdyacencia[i][j];
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

    public static Resultado maxCutGreedy(int[][] matrizAdyacencia) {
        int n = matrizAdyacencia.length;
        int[] particion = new int[n];
        Random rand = new Random();

        for (int i = 0; i < n; i++) {
            particion[i] = rand.nextInt(2);
        }

        boolean huboMejora = true;

        while (huboMejora) {
            huboMejora = false;

            for (int i = 0; i < n; i++) {
                int pesoMismoConjunto = 0;
                int pesoOtroConjunto = 0;

                for (int j = 0; j < n; j++) {
                    if (i != j && matrizAdyacencia[i][j] > 0) {
                        if (particion[i] == particion[j]) {
                            pesoMismoConjunto += matrizAdyacencia[i][j];
                        } else {
                            pesoOtroConjunto += matrizAdyacencia[i][j];
                        }
                    }
                }

                if (pesoMismoConjunto > pesoOtroConjunto) {
                    particion[i] = 1 - particion[i];
                    huboMejora = true;
                }
            }
        }

        int pesoFinal = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (particion[i] != particion[j]) {
                    pesoFinal += matrizAdyacencia[i][j];
                }
            }
        }

        return new Resultado(pesoFinal, particion);
    }

    public static void main(String[] args) {
        int[][] grafoPrueba = {
            {0,5,0,4,1,2,0,3,1,2},
            {5,0,4,0,2,1,3,0,2,1},
            {0,4,0,5,3,2,1,2,0,3},
            {4,0,5,0,2,3,2,1,4,0},
            {1,2,3,2,0,4,0,3,2,1},
            {2,1,2,3,4,0,5,0,1,2},
            {0,3,1,2,0,5,0,4,2,3},
            {3,0,2,1,3,0,4,0,5,2},
            {1,2,0,4,2,1,2,5,0,3},
            {2,1,3,0,1,2,3,2,3,0}
        };

        System.out.println("--- INICIANDO PRUEBAS MAX CUT EN JAVA ---\n");

        long inicioFB = System.nanoTime();
        Resultado resultadoFB = maxCutFuerzaBruta(grafoPrueba);
        long finFB = System.nanoTime();
        double duracionFB = (finFB - inicioFB) / 1e9;

        System.out.println("1. FUERZA BRUTA:");
        System.out.println("   Peso maximo: " + resultadoFB.peso);
        System.out.println("   Particion: " + Arrays.toString(resultadoFB.particion));
        System.out.printf("   Tiempo: %.6f segundos\n\n", duracionFB);

        long inicioGreedy = System.nanoTime();
        Resultado resultadoGreedy = maxCutGreedy(grafoPrueba);
        long finGreedy = System.nanoTime();
        double duracionGreedy = (finGreedy - inicioGreedy) / 1e9;

        System.out.println("2. HEURISTICA (GREEDY):");
        System.out.println("   Peso maximo: " + resultadoGreedy.peso);
        System.out.println("   Particion: " + Arrays.toString(resultadoGreedy.particion));
        System.out.printf("   Tiempo: %.6f segundos\n", duracionGreedy);
    }
}
