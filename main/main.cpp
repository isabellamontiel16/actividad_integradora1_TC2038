#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Lee el contenido del archivo ignorando saltos de linea. Complejidad: O(n).
string leerArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    string contenido;
    char caracter;

    if (!archivo.is_open()) {
        cerr << "Error al abrir " << nombreArchivo << endl;
        return "";
    }

    while (archivo.get(caracter)) {
        if (caracter != '\n' && caracter != '\r') {
            contenido += caracter;
        }
    }

    archivo.close();
    return contenido;
}

// PARTE 1: Busqueda de codigos maliciosos con KMP.

// Construye el arreglo LPS usado por KMP. Complejidad: O(m).
void construirLPS(const string& patron, int lps[]) {
    int longitud = 0;
    int i = 1;

    lps[0] = 0;

    while (i < static_cast<int>(patron.length())) {
        if (patron[i] == patron[longitud]) {
            longitud++;
            lps[i] = longitud;
            i++;
        }
        else {
            if (longitud != 0) {
                longitud = lps[longitud - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

/*Busca la primera aparicion de un patron dentro de un texto usando KMP.
Regresa la posicion iniciando en 1, o -1 si no se encuentra.
Complejidad: O(n + m).*/
int buscarKMP(const string& texto, const string& patron) {
    if (patron.empty()) {
        return -1;
    }

    int* lps = new int[patron.length()];
    construirLPS(patron, lps);

    int i = 0;
    int j = 0;

    while (i < static_cast<int>(texto.length())) {
        if (texto[i] == patron[j]) {
            i++;
            j++;
        }

        if (j == static_cast<int>(patron.length())) {
            int posicion = i - j;
            delete[] lps;
            return posicion + 1;
        }

        if (i < static_cast<int>(texto.length()) &&
            texto[i] != patron[j]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }

    delete[] lps;
    return -1;
}

// Imprime "true posicion" si encuentra el codigo, o "false" si no.
void mostrarBusqueda(const string& transmision, const string& codigo) {
    int posicion = buscarKMP(transmision, codigo);

    if (posicion != -1) {
        cout << "true " << posicion << endl;
    }
    else {
        cout << "false" << endl;
    }
}

// PARTE 2: Palindromo mas largo.

pair<int, int> palindromoMasLargo(const string& texto) {
    if (texto.empty()) {
        return {0, 0};
    }

    // Transformamos el texto para manejar de la misma forma
    // palindromos de longitud par e impar.
    string transformado = "^";

    for (char c : texto) {
        transformado += "#";
        transformado += c;
    }

    transformado += "#$";

    int n = static_cast<int>(transformado.length());
    int* p = new int[n]();

    int centro = 0;
    int derecha = 0;

    int mejorCentro = 0;
    int mejorLongitud = 0;

    for (int i = 1; i < n - 1; i++) {
        int espejo = 2 * centro - i;

        if (i < derecha) {
            p[i] = min(derecha - i, p[espejo]);
        }

        // Expande mientras los caracteres alrededor sean iguales.
        while (transformado[i + 1 + p[i]] ==
               transformado[i - 1 - p[i]]) {
            p[i]++;
        }

        // Actualiza el palindromo que llega mas a la derecha.
        if (i + p[i] > derecha) {
            centro = i;
            derecha = i + p[i];
        }

        // Guarda el palindromo mas largo encontrado.
        if (p[i] > mejorLongitud) {
            mejorLongitud = p[i];
            mejorCentro = i;
        }
    }

    int inicio = (mejorCentro - mejorLongitud) / 2;
    int fin = inicio + mejorLongitud - 1;

    delete[] p;

    // Se suma 1 porque la actividad pide posiciones desde 1.
    return {inicio + 1, fin + 1};
}

// PARTE 3: Substring comun mas largo.

/*
Usa programacion dinamica para encontrar el substring comun mas largo.
Solo guarda dos filas para reducir memoria.
Complejidad: O(n * m), memoria O(m).
*/
void substringComunMasLargo(const string& texto1, const string& texto2,
    int& inicio, int& fin) {
    int n = texto1.length();
    int m = texto2.length();

    int* anterior = new int[m + 1]();
    int* actual = new int[m + 1]();

    int longitudMaxima = 0;
    int posicionFinal = 0;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (texto1[i - 1] == texto2[j - 1]) {
                actual[j] = anterior[j - 1] + 1;

                if (actual[j] > longitudMaxima) {
                    longitudMaxima = actual[j];
                    posicionFinal = i;
                }
            }
            else {
                actual[j] = 0;
            }
        }

        int* temporal = anterior;
        anterior = actual;
        actual = temporal;

        for (int j = 0; j <= m; j++) {
            actual[j] = 0;
        }
    }

    inicio = posicionFinal - longitudMaxima + 1;
    fin = posicionFinal;

    delete[] anterior;
    delete[] actual;
}

int main() {
    // Los nombres son fijos porque no deben pedirse al usuario.
    string transmission1 = leerArchivo("transmission1.txt");
    string transmission2 = leerArchivo("transmission2.txt");
    string mcode1 = leerArchivo("mcode1.txt");
    string mcode2 = leerArchivo("mcode2.txt");
    string mcode3 = leerArchivo("mcode3.txt");

    // Parte 1: Busca los tres codigos en ambas transmisiones.
    mostrarBusqueda(transmission1, mcode1);
    mostrarBusqueda(transmission1, mcode2);
    mostrarBusqueda(transmission1, mcode3);
    mostrarBusqueda(transmission2, mcode1);
    mostrarBusqueda(transmission2, mcode2);
    mostrarBusqueda(transmission2, mcode3);

    int inicio;
    int fin;

   // Parte 2: Encuentra el palindromo mas largo de cada transmision.
    pair<int, int> pal1 = palindromoMasLargo(transmission1);
    pair<int, int> pal2 = palindromoMasLargo(transmission2);

    cout << pal1.first << " " << pal1.second << endl;
    cout << pal2.first << " " << pal2.second << endl;

    // Parte 3: Encuentra el substring comun mas largo.
    substringComunMasLargo(transmission1, transmission2, inicio, fin);
    cout << inicio << " " << fin << endl;

    return 0;
}