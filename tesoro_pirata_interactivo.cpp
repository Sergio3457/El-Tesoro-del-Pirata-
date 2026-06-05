#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>

using namespace std;

// ============================================================
// UTILERIAS
// ============================================================
char* copiar_cadena(const char* origen) {
    if (origen == NULL) return NULL;
    int len = strlen(origen);
    char* dest = new char[len + 1];
    strcpy(dest, origen);
    return dest;
}

char* minusculas(const char* str) {
    char* copia = copiar_cadena(str);
    for (int i = 0; copia[i]; i++)
        copia[i] = tolower(copia[i]);
    return copia;
}

char* trim(const char* str) {
    if (str == NULL) return NULL;
    int inicio = 0, fin = strlen(str) - 1;
    while (str[inicio] == ' ' || str[inicio] == '\t' || str[inicio] == '\r') inicio++;
    while (fin >= inicio && (str[fin] == ' ' || str[fin] == '\t' || str[fin] == '\r' || str[fin] == '\n')) fin--;
    int len = fin - inicio + 1;
    char* res = new char[len + 1];
    strncpy(res, str + inicio, len);
    res[len] = '\0';
    return res;
}

int partir_cadena(const char* str, char delim, char*** resultado) {
    if (str == NULL) return 0;
    int cont = 1;
    for (int i = 0; str[i]; i++)
        if (str[i] == delim) cont++;
    *resultado = new char*[cont];
    int idx = 0, start = 0, len = strlen(str);
    for (int i = 0; i <= len; i++) {
        if (str[i] == delim || str[i] == '\0') {
            char* seg = new char[i - start + 1];
            strncpy(seg, str + start, i - start);
            seg[i - start] = '\0';
            (*resultado)[idx++] = seg;
            start = i + 1;
        }
    }
    return idx;
}

void pausa() {
    cout << "\n  Presiona ENTER para continuar...";
    cin.ignore(100, '\n');
    cin.get();
}

// ============================================================
// LISTA ENLAZADA (base para Pila y Cola)
// ============================================================
template <typename T>
struct NodoLista {
    T dato;
    NodoLista* siguiente;
    NodoLista(const T& d) : dato(d), siguiente(NULL) {}
};

// ============================================================
// LISTA DINAMICA
// ============================================================
template <typename T>
class Lista {
private:
    T* datos;
    int capacidad;
    int tamano;
    void redimensionar() {
        capacidad = (capacidad == 0) ? 4 : capacidad * 2;
        T* nuevo = new T[capacidad];
        for (int i = 0; i < tamano; i++) nuevo[i] = datos[i];
        if (datos != NULL) delete[] datos;
        datos = nuevo;
    }
public:
    Lista() : datos(NULL), capacidad(0), tamano(0) {}
    Lista(const Lista& otra) : datos(NULL), capacidad(0), tamano(0) {
        for (int i = 0; i < otra.tamano; i++) agregar(otra.datos[i]);
    }
    Lista& operator=(const Lista& otra) {
        if (this != &otra) {
            if (datos != NULL) delete[] datos;
            datos = NULL; capacidad = 0; tamano = 0;
            for (int i = 0; i < otra.tamano; i++) agregar(otra.datos[i]);
        }
        return *this;
    }
    ~Lista() { if (datos != NULL) delete[] datos; }
    void agregar(const T& val) {
        if (tamano >= capacidad) redimensionar();
        datos[tamano++] = val;
    }
    T& operator[](int idx) { return datos[idx]; }
    const T& operator[](int idx) const { return datos[idx]; }
    int obtener_tamano() const { return tamano; }
    bool vacia() const { return tamano == 0; }
    void limpiar() {
        if (datos != NULL) delete[] datos;
        datos = NULL; capacidad = 0; tamano = 0;
    }
    bool contiene_str(const char* val) const {
        for (int i = 0; i < tamano; i++)
            if (strcmp(datos[i], val) == 0) return true;
        return false;
    }
};

// ============================================================
// PILA (para DFS)
// ============================================================
template <typename T>
class Pila {
private:
    NodoLista<T>* tope_nodo;
public:
    Pila() : tope_nodo(NULL) {}
    ~Pila() { while (!vacia()) pop(); }
    void push(const T& val) {
        NodoLista<T>* nuevo = new NodoLista<T>(val);
        nuevo->siguiente = tope_nodo;
        tope_nodo = nuevo;
    }
    void pop() {
        if (tope_nodo == NULL) return;
        NodoLista<T>* temp = tope_nodo;
        tope_nodo = tope_nodo->siguiente;
        delete temp;
    }
    T top() const { return tope_nodo->dato; }
    bool vacia() const { return tope_nodo == NULL; }
};

// ============================================================
// COLA (para BFS)
// ============================================================
template <typename T>
class Cola {
private:
    NodoLista<T>* frente;
    NodoLista<T>* final_cola;
public:
    Cola() : frente(NULL), final_cola(NULL) {}
    ~Cola() { while (!vacia()) pop(); }
    void push(const T& val) {
        NodoLista<T>* nuevo = new NodoLista<T>(val);
        if (final_cola != NULL) final_cola->siguiente = nuevo;
        else frente = nuevo;
        final_cola = nuevo;
    }
    void pop() {
        if (frente == NULL) return;
        NodoLista<T>* temp = frente;
        frente = frente->siguiente;
        if (frente == NULL) final_cola = NULL;
        delete temp;
    }
    T front() const { return frente->dato; }
    bool vacia() const { return frente == NULL; }
};

// ============================================================
// TABLA HASH (diccionario de ubicaciones)
// ============================================================
struct ParHash {
    char* clave;
    char* descripcion;
    char* pista;
    bool visitado;
    ParHash* siguiente;
};

class TablaHash {
private:
    static const int TAM = 101;
    ParHash* tabla[TAM];
    int funcion_hash(const char* clave) const {
        int hash = 0;
        for (int i = 0; clave[i]; i++)
            hash = (hash * 31 + clave[i]) % TAM;
        return (hash < 0) ? -hash : hash;
    }
public:
    TablaHash() { for (int i = 0; i < TAM; i++) tabla[i] = NULL; }
    ~TablaHash() {
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                ParHash* temp = actual;
                actual = actual->siguiente;
                delete[] temp->clave;
                delete[] temp->descripcion;
                delete[] temp->pista;
                delete temp;
            }
        }
    }
    void insertar(const char* clave, const char* desc, const char* pst) {
        int idx = funcion_hash(clave);
        ParHash* nuevo = new ParHash;
        nuevo->clave = copiar_cadena(clave);
        nuevo->descripcion = copiar_cadena(desc);
        nuevo->pista = copiar_cadena(pst);
        nuevo->visitado = false;
        nuevo->siguiente = tabla[idx];
        tabla[idx] = nuevo;
    }
    ParHash* buscar(const char* clave) const {
        int idx = funcion_hash(clave);
        ParHash* actual = tabla[idx];
        while (actual != NULL) {
            if (strcmp(actual->clave, clave) == 0) return actual;
            actual = actual->siguiente;
        }
        return NULL;
    }
    bool existe(const char* clave) const { return buscar(clave) != NULL; }
    void marcar_visitado(const char* clave, bool v) {
        ParHash* par = buscar(clave);
        if (par != NULL) par->visitado = v;
    }
    bool obtener_visitado(const char* clave) const {
        ParHash* par = buscar(clave);
        return (par != NULL) ? par->visitado : false;
    }
    char* obtener_pista(const char* clave) const {
        ParHash* par = buscar(clave);
        return (par != NULL) ? par->pista : NULL;
    }
    char* obtener_descripcion(const char* clave) const {
        ParHash* par = buscar(clave);
        return (par != NULL) ? par->descripcion : NULL;
    }
    int contar_visitados() const {
        int count = 0;
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                if (actual->visitado) count++;
                actual = actual->siguiente;
            }
        }
        return count;
    }
    int contar_total() const {
        int count = 0;
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) { count++; actual = actual->siguiente; }
        }
        return count;
    }
    void listar_todo() const {
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                cout << "     " << (actual->visitado ? "[X]" : "[ ]")
                     << " " << actual->clave << ": " << actual->descripcion << endl;
                actual = actual->siguiente;
            }
        }
    }
    void obtener_nombres(const char*** nombres, int& count) const {
        count = contar_total();
        *nombres = new const char*[count];
        int idx = 0;
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                (*nombres)[idx++] = actual->clave;
                actual = actual->siguiente;
            }
        }
    }
};

// ============================================================
// ARBOL DE DECISION (preguntas SI/NO sobre la pista)
// ============================================================
struct NodoDecision {
    char** claves;
    int num_claves;
    char* ubicacion;
    bool es_hoja;
    NodoDecision* rama_si;
    NodoDecision* rama_no;
};

class ArbolDecision {
private:
    NodoDecision* raiz;
    int cont_nodos;

    bool es_linea_arbol_valida(const char* linea) const {
        if (linea == NULL || linea[0] == '\0') return false;
        const char* l = linea;
        while (*l == ' ' || *l == '\t') l++;
        if (*l == '#' || *l == '\0') return false;
        return (strncmp(l, "Q:", 2) == 0 ||
                strncmp(l, "Y:", 2) == 0 ||
                strncmp(l, "N:", 2) == 0);
    }

    bool leer_linea_arbol(ifstream& file, char* linea) const {
        while (file.getline(linea, 512)) {
            char* limpia = trim(linea);
            if (es_linea_arbol_valida(limpia)) {
                strcpy(linea, limpia);
                delete[] limpia;
                return true;
            }
            delete[] limpia;
        }
        return false;
    }

    void parsear_claves(const char* texto, char*** claves, int& num) const {
        char* copia = copiar_cadena(texto);
        char** partes = NULL;
        num = partir_cadena(copia, '|', &partes);
        *claves = new char*[num];
        for (int i = 0; i < num; i++) {
            char* clave_min = minusculas(trim(partes[i]));
            (*claves)[i] = clave_min;
            delete[] partes[i];
        }
        delete[] partes;
        delete[] copia;
    }

    NodoDecision* crear_hoja(const char* ubicacion) {
        NodoDecision* nodo = new NodoDecision;
        nodo->claves = NULL;
        nodo->num_claves = 0;
        nodo->ubicacion = (ubicacion != NULL && ubicacion[0] != '\0')
            ? copiar_cadena(ubicacion) : NULL;
        nodo->es_hoja = true;
        nodo->rama_si = NULL;
        nodo->rama_no = NULL;
        cont_nodos++;
        return nodo;
    }

    NodoDecision* crear_pregunta(const char* claves_texto) {
        NodoDecision* nodo = new NodoDecision;
        parsear_claves(claves_texto, &nodo->claves, nodo->num_claves);
        nodo->ubicacion = NULL;
        nodo->es_hoja = false;
        nodo->rama_si = NULL;
        nodo->rama_no = NULL;
        cont_nodos++;
        return nodo;
    }

    NodoDecision* parsear_rama(const char* linea, ifstream& file) {
        if (strncmp(linea, "Y:H:", 4) == 0 || strncmp(linea, "N:H:", 4) == 0)
            return crear_hoja(linea + 4);
        if (strncmp(linea, "Y:Q:", 4) == 0) {
            char pseudo[512];
            strcpy(pseudo, "Q:");
            strcat(pseudo, linea + 4);
            return parsear_nodo(pseudo, file);
        }
        if (strncmp(linea, "N:Q:", 4) == 0) {
            char pseudo[512];
            strcpy(pseudo, "Q:");
            strcat(pseudo, linea + 4);
            return parsear_nodo(pseudo, file);
        }
        return NULL;
    }

    NodoDecision* parsear_nodo(const char* linea, ifstream& file) {
        if (strncmp(linea, "Q:", 2) != 0) return NULL;

        NodoDecision* nodo = crear_pregunta(linea + 2);

        char rama_si_linea[512];
        if (!leer_linea_arbol(file, rama_si_linea)) return nodo;
        nodo->rama_si = parsear_rama(rama_si_linea, file);

        char rama_no_linea[512];
        if (!leer_linea_arbol(file, rama_no_linea)) return nodo;
        nodo->rama_no = parsear_rama(rama_no_linea, file);

        return nodo;
    }

    bool cumple_pregunta(const char* pista_min, NodoDecision* nodo) const {
        for (int i = 0; i < nodo->num_claves; i++) {
            if (strstr(pista_min, nodo->claves[i]) != NULL)
                return true;
        }
        return false;
    }

    char* interpretar_nodo(NodoDecision* nodo, const char* pista_min) const {
        if (nodo == NULL) return NULL;
        if (nodo->es_hoja) return nodo->ubicacion;
        if (cumple_pregunta(pista_min, nodo))
            return interpretar_nodo(nodo->rama_si, pista_min);
        return interpretar_nodo(nodo->rama_no, pista_min);
    }

    void liberar(NodoDecision* nodo) {
        if (nodo == NULL) return;
        if (nodo->claves != NULL) {
            for (int i = 0; i < nodo->num_claves; i++)
                delete[] nodo->claves[i];
            delete[] nodo->claves;
        }
        if (nodo->ubicacion != NULL) delete[] nodo->ubicacion;
        liberar(nodo->rama_si);
        liberar(nodo->rama_no);
        delete nodo;
    }

public:
    ArbolDecision() : raiz(NULL), cont_nodos(0) {}
    ~ArbolDecision() { liberar(raiz); }

    bool cargar(const char* archivo) {
        ifstream file(archivo);
        if (!file.is_open()) return false;

        char linea[512];
        while (leer_linea_arbol(file, linea)) {
            if (strncmp(linea, "Q:", 2) == 0) {
                raiz = parsear_nodo(linea, file);
                break;
            }
        }
        file.close();
        return raiz != NULL;
    }

    char* interpretar(const char* pista) const {
        if (raiz == NULL || pista == NULL) return NULL;
        char* pista_min = minusculas(pista);
        char* resultado = interpretar_nodo(raiz, pista_min);
        delete[] pista_min;
        return resultado;
    }

    int obtener_cont_nodos() const { return cont_nodos; }
};

// ============================================================
// GRAFO
// ============================================================
struct Arista {
    char* destino;
    int costo;
    Arista* siguiente;
};
struct NodoGrafo {
    char* nombre;
    Arista* aristas;
    NodoGrafo* siguiente;
};

class Grafo {
private:
    NodoGrafo* nodos;
    NodoGrafo* buscar_nodo(const char* nombre) const {
        NodoGrafo* actual = nodos;
        while (actual != NULL) {
            if (strcmp(actual->nombre, nombre) == 0) return actual;
            actual = actual->siguiente;
        }
        return NULL;
    }
public:
    Grafo() : nodos(NULL) {}
    ~Grafo() {
        while (nodos != NULL) {
            Arista* arista = nodos->aristas;
            while (arista != NULL) {
                Arista* temp = arista;
                arista = arista->siguiente;
                delete[] temp->destino;
                delete temp;
            }
            NodoGrafo* temp = nodos;
            nodos = nodos->siguiente;
            delete[] temp->nombre;
            delete temp;
        }
    }
    void agregar_nodo(const char* nombre) {
        if (buscar_nodo(nombre) != NULL) return;
        NodoGrafo* nuevo = new NodoGrafo;
        nuevo->nombre = copiar_cadena(nombre);
        nuevo->aristas = NULL;
        nuevo->siguiente = nodos;
        nodos = nuevo;
    }
    void agregar_arista(const char* origen, const char* destino, int costo) {
        agregar_nodo(origen);
        agregar_nodo(destino);
        NodoGrafo* no = buscar_nodo(origen);
        Arista* na = new Arista;
        na->destino = copiar_cadena(destino);
        na->costo = costo;
        na->siguiente = no->aristas;
        no->aristas = na;
        NodoGrafo* nd = buscar_nodo(destino);
        Arista* inv = new Arista;
        inv->destino = copiar_cadena(origen);
        inv->costo = costo;
        inv->siguiente = nd->aristas;
        nd->aristas = inv;
    }
    int obtener_costo(const char* origen, const char* destino) const {
        NodoGrafo* ng = buscar_nodo(origen);
        if (ng == NULL) return -1;
        Arista* actual = ng->aristas;
        while (actual != NULL) {
            if (strcmp(actual->destino, destino) == 0) return actual->costo;
            actual = actual->siguiente;
        }
        return -1;
    }
    int contar_nodos() const {
        int count = 0;
        NodoGrafo* actual = nodos;
        while (actual != NULL) { count++; actual = actual->siguiente; }
        return count;
    }
    void obtener_nombres_nodos(const char*** nombres, int& count) const {
        count = contar_nodos();
        *nombres = new const char*[count];
        NodoGrafo* actual = nodos;
        int idx = 0;
        while (actual != NULL) { (*nombres)[idx++] = actual->nombre; actual = actual->siguiente; }
    }
    void mostrar_vecinos(const char* nodo) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        if (ng == NULL) { cout << "     (sin conexiones)" << endl; return; }
        Arista* actual = ng->aristas;
        int idx = 1;
        cout << "     Conexiones disponibles:" << endl;
        while (actual != NULL) {
            cout << "     [" << idx << "] " << actual->destino << " (costo: " << actual->costo << ")" << endl;
            idx++;
            actual = actual->siguiente;
        }
    }
    int contar_vecinos(const char* nodo) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        if (ng == NULL) return 0;
        int count = 0;
        Arista* actual = ng->aristas;
        while (actual != NULL) { count++; actual = actual->siguiente; }
        return count;
    }
    const char* obtener_vecino_n(const char* nodo, int n) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        if (ng == NULL) return NULL;
        Arista* actual = ng->aristas;
        int idx = 0;
        while (actual != NULL) {
            if (idx == n) return actual->destino;
            idx++;
            actual = actual->siguiente;
        }
        return NULL;
    }
    int obtener_costo_vecino_n(const char* nodo, int n) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        if (ng == NULL) return -1;
        Arista* actual = ng->aristas;
        int idx = 0;
        while (actual != NULL) {
            if (idx == n) return actual->costo;
            idx++;
            actual = actual->siguiente;
        }
        return -1;
    }

    // Para BFS/DFS/Dijkstra (usando arreglos)
    void obtener_vecinos_arr(const char* nodo, const char*** destinos, int** costos, int& count) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        count = 0;
        if (ng == NULL) return;
        Arista* actual = ng->aristas;
        while (actual != NULL) { count++; actual = actual->siguiente; }
        *destinos = new const char*[count];
        if (costos != NULL) *costos = new int[count];
        actual = ng->aristas;
        int idx = 0;
        while (actual != NULL) {
            (*destinos)[idx] = actual->destino;
            if (costos != NULL) (*costos)[idx] = actual->costo;
            idx++; actual = actual->siguiente;
        }
    }
};

// ============================================================
// JUEGO INTERACTIVO
// ============================================================
class JuegoPirata {
private:
    Grafo grafo;
    TablaHash ubicaciones;
    ArbolDecision arbol;
    Lista<const char*> camino_recorrido;
    Lista<const char*> pistas_encontradas;
    int costo_total;
    bool tesoro_encontrado;

    char* ubicacion_actual;

    bool es_comentario_o_vacio(const char* linea) {
        if (linea == NULL || linea[0] == '\0') return true;
        const char* l = linea;
        while (*l == ' ' || *l == '\t') l++;
        return *l == '#' || *l == '\0';
    }

    void limpiar_pantalla() {
        // No limpiamos realmente, solo separamos
        cout << "\n\n\n\n";
    }

    void mostrar_titulo() {
        cout << "========================================================" << endl;
        cout << "                                                       " << endl;
        cout << "      ***  EL TESORO DEL PIRATA  ***" << endl;
        cout << "                                                       " << endl;
        cout << "   Un juego de exploracion y decision en una isla" << endl;
        cout << "                                                       " << endl;
        cout << "========================================================" << endl;
    }

    void mostrar_estado() {
        cout << "\n------------------------------------------------------" << endl;
        cout << "  LUGAR ACTUAL: " << ubicacion_actual << endl;
        cout << "------------------------------------------------------" << endl;

        char* desc = ubicaciones.obtener_descripcion(ubicacion_actual);
        if (desc != NULL) {
            cout << "  " << desc << endl;
        }

        char* pista = ubicaciones.obtener_pista(ubicacion_actual);
        if (pista != NULL) {
            cout << "\n  >> Pista encontrada: \"" << pista << "\"" << endl;
        }
    }

    void menu_destino() {
        cout << "\n  Costo acumulado: " << costo_total << " monedas de oro" << endl;
        cout << "\n  ---  MENU  ---" << endl;
        cout << "  [1-9] Ir a una ubicacion conectada" << endl;
        cout << "  [H]   Pedir pista (Arbol de decision)" << endl;
        cout << "  [M]   Ver mapa de la isla" << endl;
        cout << "  [R]   Ver ruta recorrida" << endl;
        cout << "  [B]   Auto-explorar (BFS)" << endl;
        cout << "  [D]   Auto-explorar (DFS)" << endl;
        cout << "  [O]   Ruta optima (Dijkstra)" << endl;
        cout << "  [Q]   Salir" << endl;
        cout << "\n  >> Elige una opcion: ";
    }

    void procesar_clue(const char* ubicacion) {
        char* pista = ubicaciones.obtener_pista(ubicacion);
        if (pista == NULL) return;

        bool ya_tenemos = false;
        for (int i = 0; i < pistas_encontradas.obtener_tamano(); i++) {
            if (strcmp(pistas_encontradas[i], pista) == 0) {
                ya_tenemos = true;
                break;
            }
        }
        if (!ya_tenemos) {
            pistas_encontradas.agregar(pista);
        }
    }

    void pedir_pista() {
        char* pista = ubicaciones.obtener_pista(ubicacion_actual);
        if (pista == NULL) {
            cout << "     No hay pista aqui." << endl;
            return;
        }
        cout << "\n     >> Analizando pista con el Arbol de Decision..." << endl;
        cout << "     Pista: \"" << pista << "\"" << endl;
        char* sugerencia = arbol.interpretar(pista);
        if (sugerencia != NULL) {
            cout << "     >> El arbol sugiere buscar en: " << sugerencia << endl;

            // Verificar si hay camino
            int num = grafo.contar_vecinos(ubicacion_actual);
            bool accesible = false;
            for (int i = 0; i < num; i++) {
                if (strcmp(grafo.obtener_vecino_n(ubicacion_actual, i), sugerencia) == 0) {
                    accesible = true;
                    break;
                }
            }
            if (!accesible) {
                cout << "     (!) Pero no hay un camino directo desde aqui." << endl;
            }
        } else {
            cout << "     >> El arbol no pudo interpretar la pista." << endl;
        }
    }

    void mostrar_mapa() {
        cout << "\n     == MAPA DE LA ISLA ==" << endl;
        cout << "     Visitados: " << ubicaciones.contar_visitados()
             << " / " << ubicaciones.contar_total() << endl;
        cout << "     [X] = Visitado   [ ] = No visitado" << endl << endl;
        ubicaciones.listar_todo();
    }

    void mostrar_ruta() {
        cout << "\n     == RUTA RECORRIDA ==" << endl;
        if (camino_recorrido.vacia()) {
            cout << "     Aun no has comenzado tu viaje." << endl;
            return;
        }
        cout << "     ";
        for (int i = 0; i < camino_recorrido.obtener_tamano(); i++) {
            if (i > 0) cout << " -> ";
            cout << camino_recorrido[i];
        }
        cout << "\n     Costo total: " << costo_total << " monedas de oro" << endl;
    }

    void ejecutar_bfs() {
        cout << "\n     == AUTO-EXPLORACION BFS == " << endl;
        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        const char** padres = new const char*[num_nodos];
        bool* visitados_arr = new bool[num_nodos];
        for (int i = 0; i < num_nodos; i++) { padres[i] = NULL; visitados_arr[i] = false; }

        Cola<const char*> cola;
        cola.push(ubicacion_actual);

        int idx_ini = -1;
        for (int i = 0; i < num_nodos; i++)
            if (strcmp(nombres[i], ubicacion_actual) == 0) { idx_ini = i; break; }
        if (idx_ini >= 0) visitados_arr[idx_ini] = true;

        bool encontrado = false;
        const char* destino = "Tesoro";
        while (!cola.vacia() && !encontrado) {
            const char* nodo = cola.front(); cola.pop();
            if (strcmp(nodo, destino) == 0) { encontrado = true; break; }
            const char** vecinos = NULL;
            int num_vec = 0;
            grafo.obtener_vecinos_arr(nodo, &vecinos, NULL, num_vec);
            for (int i = 0; i < num_vec && !encontrado; i++) {
                int idx_vec = -1;
                for (int j = 0; j < num_nodos; j++)
                    if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }
                if (idx_vec >= 0 && !visitados_arr[idx_vec]) {
                    visitados_arr[idx_vec] = true;
                    padres[idx_vec] = nodo;
                    cola.push(vecinos[i]);
                }
            }
            if (num_vec > 0) delete[] vecinos;
        }

        if (encontrado) {
            Lista<const char*> camino_rev;
            const char* act = destino;
            while (act != NULL) {
                camino_rev.agregar(act);
                if (strcmp(act, ubicacion_actual) == 0) break;
                int idx_act = -1;
                for (int i = 0; i < num_nodos; i++)
                    if (strcmp(nombres[i], act) == 0) { idx_act = i; break; }
                if (idx_act >= 0 && padres[idx_act] != NULL)
                    act = padres[idx_act];
                else break;
            }
            cout << "     BFS sugiere: ";
            for (int i = camino_rev.obtener_tamano() - 1; i >= 0; i--) {
                if (i < camino_rev.obtener_tamano() - 1) cout << " -> ";
                cout << camino_rev[i];
            }
            cout << endl;
        } else {
            cout << "     No se encontro camino al tesoro." << endl;
        }

        delete[] padres;
        delete[] visitados_arr;
        delete[] nombres;
    }

    void ejecutar_dfs() {
        cout << "\n     == AUTO-EXPLORACION DFS == " << endl;
        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        const char** padres = new const char*[num_nodos];
        bool* visitados_arr = new bool[num_nodos];
        for (int i = 0; i < num_nodos; i++) { padres[i] = NULL; visitados_arr[i] = false; }

        Pila<const char*> pila;
        pila.push(ubicacion_actual);

        bool encontrado = false;
        const char* destino = "Tesoro";
        while (!pila.vacia() && !encontrado) {
            const char* nodo = pila.top(); pila.pop();
            int idx_act = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], nodo) == 0) { idx_act = i; break; }
            if (idx_act >= 0 && !visitados_arr[idx_act]) {
                visitados_arr[idx_act] = true;
                if (strcmp(nodo, destino) == 0) { encontrado = true; break; }
                const char** vecinos = NULL;
                int num_vec = 0;
                grafo.obtener_vecinos_arr(nodo, &vecinos, NULL, num_vec);
                for (int i = 0; i < num_vec && !encontrado; i++) {
                    int idx_vec = -1;
                    for (int j = 0; j < num_nodos; j++)
                        if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }
                    if (idx_vec >= 0 && !visitados_arr[idx_vec]) {
                        padres[idx_vec] = nodo;
                        pila.push(vecinos[i]);
                    }
                }
                if (num_vec > 0) delete[] vecinos;
            }
        }

        if (encontrado) {
            Lista<const char*> camino_rev;
            const char* act = destino;
            while (act != NULL) {
                camino_rev.agregar(act);
                if (strcmp(act, ubicacion_actual) == 0) break;
                int idx_act = -1;
                for (int i = 0; i < num_nodos; i++)
                    if (strcmp(nombres[i], act) == 0) { idx_act = i; break; }
                if (idx_act >= 0 && padres[idx_act] != NULL)
                    act = padres[idx_act];
                else break;
            }
            cout << "     DFS sugiere: ";
            for (int i = camino_rev.obtener_tamano() - 1; i >= 0; i--) {
                if (i < camino_rev.obtener_tamano() - 1) cout << " -> ";
                cout << camino_rev[i];
            }
            cout << endl;
        } else {
            cout << "     No se encontro camino al tesoro." << endl;
        }
        delete[] padres;
        delete[] visitados_arr;
        delete[] nombres;
    }

    void ejecutar_dijkstra() {
        cout << "\n     == RUTA OPTIMA (Dijkstra) == " << endl;
        int num_nodos = grafo.contar_nodos();
        if (num_nodos == 0) return;
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        int* distancias = new int[num_nodos];
        const char** padres = new const char*[num_nodos];
        bool* visitados_arr = new bool[num_nodos];
        int* heap_dist = new int[num_nodos];
        const char** heap_nodo = new const char*[num_nodos];
        int heap_tam = 0;

        int idx_ini = -1, idx_dest = -1;
        for (int i = 0; i < num_nodos; i++) {
            distancias[i] = 2147483647;
            padres[i] = NULL;
            visitados_arr[i] = false;
            if (strcmp(nombres[i], ubicacion_actual) == 0) idx_ini = i;
            if (strcmp(nombres[i], "Tesoro") == 0) idx_dest = i;
        }
        if (idx_ini == -1 || idx_dest == -1) {
            delete[] nombres; delete[] distancias; delete[] padres;
            delete[] visitados_arr; delete[] heap_dist; delete[] heap_nodo;
            return;
        }

        distancias[idx_ini] = 0;
        heap_dist[heap_tam] = 0;
        heap_nodo[heap_tam] = nombres[idx_ini];
        heap_tam++;

        while (heap_tam > 0) {
            int min_idx = 0;
            for (int i = 1; i < heap_tam; i++)
                if (heap_dist[i] < heap_dist[min_idx]) min_idx = i;
            int dist_actual = heap_dist[min_idx];
            const char* nodo_actual = heap_nodo[min_idx];
            heap_dist[min_idx] = heap_dist[--heap_tam];
            heap_nodo[min_idx] = heap_nodo[heap_tam];

            int idx_act = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], nodo_actual) == 0) { idx_act = i; break; }
            if (visitados_arr[idx_act]) continue;
            visitados_arr[idx_act] = true;
            if (strcmp(nodo_actual, "Tesoro") == 0) break;

            const char** vecinos = NULL;
            int* costos_ptr = NULL;
            int num_vec = 0;
            grafo.obtener_vecinos_arr(nodo_actual, &vecinos, &costos_ptr, num_vec);
            for (int i = 0; i < num_vec; i++) {
                int idx_vec = -1;
                for (int j = 0; j < num_nodos; j++)
                    if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }
                int nueva_dist = dist_actual + costos_ptr[i];
                if (nueva_dist < distancias[idx_vec]) {
                    distancias[idx_vec] = nueva_dist;
                    padres[idx_vec] = nodo_actual;
                    heap_dist[heap_tam] = nueva_dist;
                    heap_nodo[heap_tam] = nombres[idx_vec];
                    heap_tam++;
                }
            }
            if (num_vec > 0) { delete[] vecinos; delete[] costos_ptr; }
        }

        if (distancias[idx_dest] < 2147483647) {
            Lista<const char*> camino_rev;
            const char* act = "Tesoro";
            while (act != NULL) {
                camino_rev.agregar(act);
                if (strcmp(act, ubicacion_actual) == 0) break;
                int idx_act = -1;
                for (int i = 0; i < num_nodos; i++)
                    if (strcmp(nombres[i], act) == 0) { idx_act = i; break; }
                if (idx_act >= 0 && padres[idx_act] != NULL)
                    act = padres[idx_act];
                else break;
            }
            cout << "     Ruta optima a Tesoro: ";
            for (int i = camino_rev.obtener_tamano() - 1; i >= 0; i--) {
                if (i < camino_rev.obtener_tamano() - 1) cout << " -> ";
                cout << camino_rev[i];
            }
            cout << "\n     Costo total: " << distancias[idx_dest] << " monedas" << endl;
        } else {
            cout << "     No se encontro camino al tesoro." << endl;
        }
        delete[] nombres; delete[] distancias; delete[] padres;
        delete[] visitados_arr; delete[] heap_dist; delete[] heap_nodo;
    }

    void victoria() {
        limpiar_pantalla();
        cout << "========================================================" << endl;
        cout << "                                                       " << endl;
        cout << "        ___  _  _  ___  _____  ___  ___  ___   " << endl;
        cout << "       |_ _|| \\| ||_ _||_   _|| __|| _ \\/ __|  " << endl;
        cout << "        | | | .` | | |   | |  | _| |   /\\__ \\  " << endl;
        cout << "       |___||_|\\_||___|  |_|  |___||_|_\\|___/  " << endl;
        cout << "                                                       " << endl;
        cout << "   ***  ENCONTRASTE EL TESORO DEL PIRATA!  ***" << endl;
        cout << "                                                       " << endl;
        cout << "========================================================" << endl;

        cout << "\n  Ruta completa:" << endl;
        cout << "  ";
        for (int i = 0; i < camino_recorrido.obtener_tamano(); i++) {
            if (i > 0) cout << " -> ";
            cout << camino_recorrido[i];
        }
        cout << "\n\n  Costo total: " << costo_total << " monedas de oro" << endl;
        cout << "\n  Pistas encontradas en el camino:" << endl;
        for (int i = 0; i < pistas_encontradas.obtener_tamano(); i++) {
            cout << "    " << (i+1) << ". \"" << pistas_encontradas[i] << "\"" << endl;
        }

        guardar_resultado();
        tesoro_encontrado = true;
    }

    void guardar_resultado() {
        ofstream file("ruta_tesoro_interactivo.txt");
        if (!file.is_open()) return;

        file << "==================================================" << endl;
        file << "  RUTA AL TESORO DEL PIRATA (MODO INTERACTIVO)" << endl;
        file << "==================================================" << endl << endl;
        file << "Ruta completa:" << endl;
        for (int i = 0; i < camino_recorrido.obtener_tamano(); i++) {
            if (i > 0) file << " -> ";
            file << camino_recorrido[i];
        }
        file << "\n\nCosto total: " << costo_total << " monedas de oro" << endl;
        file << "\nPistas encontradas:" << endl;
        for (int i = 0; i < pistas_encontradas.obtener_tamano(); i++) {
            file << "  " << (i+1) << ". " << pistas_encontradas[i] << endl;
        }
        file << "\nTodas las ubicaciones:" << endl;
        const char** nombres = NULL;
        int count = 0;
        ubicaciones.obtener_nombres(&nombres, count);
        for (int i = 0; i < count; i++) {
            file << "  " << (ubicaciones.obtener_visitado(nombres[i]) ? "[X]" : "[ ]")
                 << " " << nombres[i] << endl;
        }
        delete[] nombres;
        file << "\n==================================================" << endl;
        file.close();
        cout << "\n  >> Resultado guardado en 'ruta_tesoro_interactivo.txt'" << endl;
    }

public:
    JuegoPirata() : costo_total(0), tesoro_encontrado(false), ubicacion_actual(NULL) {}

    void cargar_datos() {
        // Cargar mapa
        ifstream file("mapa.txt");
        if (!file.is_open()) { cerr << "Error: no se encuentra mapa.txt" << endl; return; }
        char linea[256];
        while (file.getline(linea, 256)) {
            char* limpia = trim(linea);
            if (es_comentario_o_vacio(limpia)) { delete[] limpia; continue; }
            char** partes = NULL;
            int num = partir_cadena(limpia, ',', &partes);
            if (num == 3) {
                char* origen = trim(partes[0]);
                char* destino = trim(partes[1]);
                char* costo_str = trim(partes[2]);
                grafo.agregar_arista(origen, destino, atoi(costo_str));
                delete[] origen; delete[] destino; delete[] costo_str;
            }
            for (int i = 0; i < num; i++) delete[] partes[i];
            delete[] partes; delete[] limpia;
        }
        file.close();

        // Cargar ubicaciones con pistas (descripcion = pista por ahora)
        file.open("ubicaciones.txt");
        if (!file.is_open()) { cerr << "Error: no se encuentra ubicaciones.txt" << endl; return; }
        while (file.getline(linea, 256)) {
            char* limpia = trim(linea);
            if (es_comentario_o_vacio(limpia)) { delete[] limpia; continue; }
            size_t len = strlen(limpia);
            int pos_coma = -1;
            for (int i = 0; i < (int)len; i++) {
                if (limpia[i] == ',') { pos_coma = i; break; }
            }
            if (pos_coma == -1) { delete[] limpia; continue; }
            char* nombre = new char[pos_coma + 1];
            strncpy(nombre, limpia, pos_coma);
            nombre[pos_coma] = '\0';
            char* pista = new char[len - pos_coma];
            strcpy(pista, limpia + pos_coma + 1);
            char* nom_trim = trim(nombre);
            char* pista_trim = trim(pista);

            // Asignar descripcion segun el nombre
            const char* desc = "";
            if (strcmp(nom_trim, "Playa") == 0) desc = "Una playa de arena blanca con palmeras. El sol brilla intensamente.";
            else if (strcmp(nom_trim, "Bosque") == 0) desc = "Un bosque espeso y oscuro. Los arboles susurran secretos.";
            else if (strcmp(nom_trim, "Montana") == 0) desc = "Una montana imponente. La cima esta cubierta de niebla.";
            else if (strcmp(nom_trim, "Cueva") == 0) desc = "Una cueva humeda y oscura. El eco de gotas resuena.";
            else if (strcmp(nom_trim, "Lago") == 0) desc = "Un lago cristalino rodeado de vegetacion. El agua esta tranquila.";
            else if (strcmp(nom_trim, "Volcan") == 0) desc = "Un volcan humeante. El suelo tiembla suavemente.";
            else if (strcmp(nom_trim, "Ruinas") == 0) desc = "Antiguas ruinas de piedra. Jeroglificos cubren las paredes.";
            else if (strcmp(nom_trim, "Tesoro") == 0) desc = "Una camara oculta llena de oro y joyas. EL TESORO DEL PIRATA!";

            ubicaciones.insertar(nom_trim, desc, pista_trim);
            delete[] nombre; delete[] pista; delete[] nom_trim; delete[] pista_trim;
            delete[] limpia;
        }
        file.close();

        // Cargar arbol de decision
        if (!arbol.cargar("pistas.txt")) {
            cerr << "Error: no se pudo cargar pistas.txt" << endl;
            return;
        }

        // Inicializar ubicacion actual
        ubicacion_actual = copiar_cadena("Playa");
        ubicaciones.marcar_visitado("Playa", true);
        camino_recorrido.agregar("Playa");
        procesar_clue("Playa");
    }

    bool jugar() {
        limpiar_pantalla();
        mostrar_titulo();
        cout << "\n  Eres un pirata en busca del tesoro escondido." << endl;
        cout << "  Comienzas en la Playa. Cada lugar tiene una pista" << endl;
        cout << "  que te guiara. Usa el Arbol de Decision para" << endl;
        cout << "  interpretar las pistas. Llega al Tesoro para ganar!" << endl;
        pausa();

        while (!tesoro_encontrado) {
            limpiar_pantalla();
            mostrar_titulo();
            mostrar_estado();
            grafo.mostrar_vecinos(ubicacion_actual);
            menu_destino();

            char opcion[32];
            cin.getline(opcion, 32);

            if (strlen(opcion) == 0) continue;

            char cmd = tolower(opcion[0]);

            if (cmd >= '1' && cmd <= '9') {
                int idx = cmd - '1';
                int num_vecinos = grafo.contar_vecinos(ubicacion_actual);
                if (idx < 0 || idx >= num_vecinos) {
                    cout << "\n     Opcion invalida." << endl;
                    pausa();
                    continue;
                }
                const char* destino = grafo.obtener_vecino_n(ubicacion_actual, idx);
                int costo = grafo.obtener_costo_vecino_n(ubicacion_actual, idx);

                cout << "\n     Viajando a " << destino << "..." << endl;

                delete[] ubicacion_actual;
                ubicacion_actual = copiar_cadena(destino);
                costo_total += costo;
                ubicaciones.marcar_visitado(destino, true);
                camino_recorrido.agregar(destino);
                procesar_clue(destino);

                if (strcmp(destino, "Tesoro") == 0) {
                    pausa();
                    victoria();
                    return true;
                }
                pausa();

            } else if (cmd == 'h') {
                pedir_pista();
                pausa();

            } else if (cmd == 'm') {
                mostrar_mapa();
                pausa();

            } else if (cmd == 'r') {
                mostrar_ruta();
                pausa();

            } else if (cmd == 'b') {
                ejecutar_bfs();
                pausa();

            } else if (cmd == 'd') {
                ejecutar_dfs();
                pausa();

            } else if (cmd == 'o') {
                ejecutar_dijkstra();
                pausa();

            } else if (cmd == 'q') {
                cout << "\n  Saliendo del juego... Buena suerte, pirata!" << endl;
                delete[] ubicacion_actual;
                return false;

            } else {
                cout << "\n     Opcion no reconocida." << endl;
                pausa();
            }
        }
        delete[] ubicacion_actual;
        return true;
    }
};

// ============================================================
// MAIN
// ============================================================
int main() {
    JuegoPirata juego;
    juego.cargar_datos();
    juego.jugar();
    return 0;
}
