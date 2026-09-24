/*
 * Actividad Integradora 1
 *
 * Descripcion: Programa que analiza archivos de transmision utilizando
 * los algoritmos KMP, Manacher y programacion dinamica.
 * 
 * Autores
 * Isabella Montiel Reyes | A01278286
 * Gerardo Martinez Carbajal | A01713474
 * Cristhian Viery Maida Suarez | A01668790
 *
 * Compilacion para debug:
 *   clang++ -std=c++17 -Wall -g *.cpp -o main
 *
 * Compilacion para ejecucion:
 *   clang++ -std=c++17 -O3 *.cpp -o main
 *
 * Ejecucion:
 *   ./main
 */


#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::min;
using std::pair;
using std::string;
using std::vector;


// Junta todo el archivo en una sola cadena y se salta los saltos de linea.
// Si el archivo no existe avisa por cerr y regresa la cadena vacia.
// Complejidad: O(n)
string leerSinSaltos(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    string contenido = "";
    char caracter = '\0';
    if (archivo.is_open()) {
        while (archivo.get(caracter)) {
            if (caracter != '\n' && caracter != '\r') {
                contenido += caracter;
            }
        }
        archivo.close();
    } else {
        cerr << "No se encontro el archivo " << nombreArchivo << endl;
    }
    return contenido;
}

// Parte 1: busqueda del codigo malicioso con KMP
 
// Para cada posicion del patron guarda el largo del prefijo que tambien
// termina ahi como sufijo. Es lo que le dice a KMP a donde regresar.
// Complejidad: O(m)
vector<int> armarTablaLPS(const string &patron) {
    int m = static_cast<int>(patron.length());
    vector<int> lps(m, 0);
    int largo = 0;
    int i = 1;
    while (i < m) {
        if (patron[i] == patron[largo]) {
            largo++;
            lps[i] = largo;
            i++;
        } else if (largo != 0) {
            largo = lps[largo - 1];
        } else {
            lps[i] = 0;
            i++;
        }
    }
    return lps;
}
 
// Busca la primera vez que aparece el codigo en la transmision. Regresa la
// posicion donde empieza contando desde 1, o -1 si no aparece.
// Complejidad: O(n + m)
int localizarCodigo(const string &transmision, const string &codigo) {
    int n = static_cast<int>(transmision.length());
    int m = static_cast<int>(codigo.length());
    int posicion = -1;
    if (m > 0) {
        vector<int> lps = armarTablaLPS(codigo);
        int i = 0;
        int j = 0;
        while (i < n && posicion == -1) {
            if (transmision[i] == codigo[j]) {
                i++;
                j++;
                if (j == m) {
                    posicion = i - m + 1;
                }
            } else if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return posicion;
}
 
// Imprime "true" con la posicion si el codigo esta en la transmision, o "false".
// Complejidad: O(n + m)
void reportarCodigo(const string &transmision, const string &codigo) {
    int posicion = localizarCodigo(transmision, codigo);
    if (posicion != -1) {
        cout << "true " << posicion << endl;
    } else {
        cout << "false" << endl;
    }
}
 
// Parte 2: codigo espejeado mas largo con Manacher
 
// Mete un separador entre cada caracter para que los palindromos pares e
// impares se traten igual, y aprovecha la posicion espejo para no volver a
// comparar lo que ya se sabe. Regresa inicio y fin contando desde 1.
// Complejidad: O(n)
pair<int, int> ubicarEspejeado(const string &texto) {
    pair<int, int> resultado(0, 0);
    if (!texto.empty()) {
        string separado = "^";
        for (char caracter : texto) {
            separado += "#";
            separado += caracter;
        }
        separado += "#$";
        int n = static_cast<int>(separado.length());
        vector<int> radios(n, 0);
        int centro = 0;
        int derecha = 0;
        int mejorCentro = 0;
        int mejorRadio = 0;
        for (int i = 1; i < n - 1; i++) {
            if (i < derecha) {
                radios[i] = min(derecha - i, radios[2 * centro - i]);
            }
            while (separado[i + 1 + radios[i]] == separado[i - 1 - radios[i]]) {
                radios[i]++;
            }
            if (i + radios[i] > derecha) {
                centro = i;
                derecha = i + radios[i];
            }
            if (radios[i] > mejorRadio) {
                mejorRadio = radios[i];
                mejorCentro = i;
            }
        }
        int inicio = (mejorCentro - mejorRadio) / 2;
        resultado.first = inicio + 1;
        resultado.second = inicio + mejorRadio;
    }
    return resultado;
}
 
// Parte 3: substring comun mas largo con programacion dinamica
 
// En lugar de la matriz completa solo se guardan dos filas que se van
// intercambiando. Regresa inicio y fin dentro de la primera transmision,
// o 0 0 si no comparten ningun caracter.
// Complejidad: O(n * m) en tiempo y O(m) en memoria
pair<int, int> ubicarTramoComun(const string &texto1, const string &texto2) {
    int n = static_cast<int>(texto1.length());
    int m = static_cast<int>(texto2.length());
    vector<int> anterior(m + 1, 0);
    vector<int> actual(m + 1, 0);
    int largoMaximo = 0;
    int finEnTexto1 = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (texto1[i - 1] == texto2[j - 1]) {
                actual[j] = anterior[j - 1] + 1;
                if (actual[j] > largoMaximo) {
                    largoMaximo = actual[j];
                    finEnTexto1 = i;
                }
            } else {
                actual[j] = 0;
            }
        }
        anterior.swap(actual);
    }
    pair<int, int> resultado(0, 0);
    if (largoMaximo > 0) {
        resultado.first = finEnTexto1 - largoMaximo + 1;
        resultado.second = finEnTexto1;
    }
    return resultado;
}
 
// Carga los cinco archivos y saca las tres partes en el orden que pide la actividad.
// Complejidad: O(n * m), la tercera parte es la que domina
int main() {
    string transmision1 = leerSinSaltos("transmission1.txt");
    string transmision2 = leerSinSaltos("transmission2.txt");
    string codigo1 = leerSinSaltos("mcode1.txt");
    string codigo2 = leerSinSaltos("mcode2.txt");
    string codigo3 = leerSinSaltos("mcode3.txt");
 
    reportarCodigo(transmision1, codigo1);
    reportarCodigo(transmision1, codigo2);
    reportarCodigo(transmision1, codigo3);
    reportarCodigo(transmision2, codigo1);
    reportarCodigo(transmision2, codigo2);
    reportarCodigo(transmision2, codigo3);
 
    pair<int, int> espejo1 = ubicarEspejeado(transmision1);
    pair<int, int> espejo2 = ubicarEspejeado(transmision2);
    cout << espejo1.first << " " << espejo1.second << endl;
    cout << espejo2.first << " " << espejo2.second << endl;
 
    pair<int, int> tramo = ubicarTramoComun(transmision1, transmision2);
    cout << tramo.first << " " << tramo.second << endl;
 
    return 0;
}