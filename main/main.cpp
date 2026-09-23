/*Descripcion: Programa que analiza archivos de transmision utilizando
los algoritmos KMP, Manacher y programacion dinamica.

Autores: 
- Isabella Montiel - A01278286
- Gerardo Martínez Carbajal - A01713474
- Cristhian Viery Maida Suarez - A01668790

Fecha de modificacion: 23/09/2026. */

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

/*Lee el contenido de un archivo e ignora los saltos de linea.
Recibe el nombre del archivo que se desea leer.
Retorna una cadena con el contenido del archivo.
Complejidad: O(n).*/
string leerArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    string contenido = "";
    char caracter = '\0';

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

/*Construye el arreglo LPS utilizado por el algoritmo KMP.
Recibe el patron que se desea buscar y el arreglo donde se
almacenan las longitudes de prefijos y sufijos.
No retorna ningun valor.
Complejidad: O(m).*/
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

/*Busca la primera aparicion de un patron dentro de un texto utilizando KMP.
Recibe el texto donde se realiza la busqueda y el patron que se desea buscar.
Retorna la posicion inicial comenzando en 1 o -1 si el patron no se encuentra.
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

/*Busca un codigo dentro de una transmision y muestra el resultado.
Recibe la transmision donde se realiza la busqueda y el codigo a buscar.
No retorna ningun valor.
Complejidad: O(n + m).*/
void mostrarBusqueda(const string& transmision, const string& codigo) {
    int posicion = buscarKMP(transmision, codigo);

    if (posicion != -1) {
        cout << "true " << posicion << endl;
    }
    else {
        cout << "false" << endl;
    }
}

// PARTE 2: Palindromo mas largo con Manacher.

/*Encuentra el palindromo mas largo utilizando el algoritmo de Manacher.
Recibe el texto en el que se desea buscar el palindromo.
Retorna un par con las posiciones inicial y final comenzando en 1.
Complejidad: O(n).*/
pair<int, int> palindromoMasLargo(const string& texto) {
    if (texto.empty()) {
        return {0, 0};
    }

    string transformado = "^";

    for (char caracter : texto) {
        transformado += "#";
        transformado += caracter;
    }

    transformado += "#$";

    int n = static_cast<int>(transformado.length());
    int* radios = new int[n]();

    int centro = 0;
    int derecha = 0;
    int mejorCentro = 0;
    int mejorLongitud = 0;

    for (int i = 1; i < n - 1; i++) {
        int espejo = 2 * centro - i;

        if (i < derecha) {
            radios[i] = min(derecha - i, radios[espejo]);
        }

        while (transformado[i + 1 + radios[i]] ==
               transformado[i - 1 - radios[i]]) {
            radios[i]++;
        }

        if (i + radios[i] > derecha) {
            centro = i;
            derecha = i + radios[i];
        }

        if (radios[i] > mejorLongitud) {
            mejorLongitud = radios[i];
            mejorCentro = i;
        }
    }

    int inicio = (mejorCentro - mejorLongitud) / 2;
    int fin = inicio + mejorLongitud - 1;

    delete[] radios;

    return {inicio + 1, fin + 1};
}

// PARTE 3: Substring comun mas largo.

/*Encuentra el substring comun mas largo utilizando programacion dinamica.
Recibe los dos textos que se desean comparar y las variables donde se
guardan las posiciones inicial y final del substring en el primer texto.
No retorna ningun valor.
Complejidad: O(n * m) en tiempo y O(m) en memoria.*/
void substringComunMasLargo(const string& texto1, const string& texto2,
    int& inicio, int& fin) {
    int n = static_cast<int>(texto1.length());
    int m = static_cast<int>(texto2.length());

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

/*Ejecuta las tres partes de la actividad utilizando los cinco archivos
de entrada y muestra los resultados requeridos.
No recibe parametros.
Retorna 0 cuando el programa termina correctamente.*/
int main() {
    string transmission1 = leerArchivo("transmission1.txt");
    string transmission2 = leerArchivo("transmission2.txt");
    string mcode1 = leerArchivo("mcode1.txt");
    string mcode2 = leerArchivo("mcode2.txt");
    string mcode3 = leerArchivo("mcode3.txt");

    mostrarBusqueda(transmission1, mcode1);
    mostrarBusqueda(transmission1, mcode2);
    mostrarBusqueda(transmission1, mcode3);
    mostrarBusqueda(transmission2, mcode1);
    mostrarBusqueda(transmission2, mcode2);
    mostrarBusqueda(transmission2, mcode3);

    pair<int, int> palindromo1 = palindromoMasLargo(transmission1);
    pair<int, int> palindromo2 = palindromoMasLargo(transmission2);

    cout << palindromo1.first << " " << palindromo1.second << endl;
    cout << palindromo2.first << " " << palindromo2.second << endl;

    int inicio = 0;
    int fin = 0;

    substringComunMasLargo(transmission1, transmission2, inicio, fin);
    cout << inicio << " " << fin << endl;

    return 0;
}