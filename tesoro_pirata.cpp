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
    int idx = 0;
    int len = strlen(str);
    int start = 0;
    for (int i = 0; i <= len; i++) {
        if (str[i] == delim || str[i] == '\0') {
            int seg_len = i - start;
            char* seg = new char[seg_len + 1];
            strncpy(seg, str + start, seg_len);
            seg[seg_len] = '\0';
            (*resultado)[idx++] = seg;
            start = i + 1;
        }
    }
    return idx;
}

// ============================================================
// 1. LISTA ENLAZADA (usada internamente)
// ============================================================
template <typename T>
struct NodoLista {
    T dato;
    NodoLista* siguiente;
    NodoLista(const T& d) : dato(d), siguiente(NULL) {}
};

// ============================================================
// 2. LISTA DINAMICA (para camino optimo)
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
        for (int i = 0; i < tamano; i++)
            nuevo[i] = datos[i];
        if (datos != NULL) delete[] datos;
        datos = nuevo;
    }

public:
    Lista() : datos(NULL), capacidad(0), tamano(0) {}

    Lista(const Lista& otra) : datos(NULL), capacidad(0), tamano(0) {
        for (int i = 0; i < otra.tamano; i++)
            agregar(otra.datos[i]);
    }

    Lista& operator=(const Lista& otra) {
        if (this != &otra) {
            if (datos != NULL) delete[] datos;
            datos = NULL;
            capacidad = 0;
            tamano = 0;
            for (int i = 0; i < otra.tamano; i++)
                agregar(otra.datos[i]);
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
        datos = NULL;
        capacidad = 0;
        tamano = 0;
    }

    bool contiene(const T& val) const {
        for (int i = 0; i < tamano; i++)
            if (datos[i] == val) return true;
        return false;
    }
};

// ============================================================
// 3. PILA (para DFS)
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
// 4. COLA (para BFS)
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
        if (final_cola != NULL)
            final_cola->siguiente = nuevo;
        else
            frente = nuevo;
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
// 5. TABLA HASH (diccionario de ubicaciones)
// ============================================================
struct ParHash {
    char* clave;
    char* valor1;   // nombre
    char* valor2;   // pista
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
    TablaHash() {
        for (int i = 0; i < TAM; i++)
            tabla[i] = NULL;
    }

    ~TablaHash() {
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                ParHash* temp = actual;
                actual = actual->siguiente;
                delete[] temp->clave;
                delete[] temp->valor1;
                delete[] temp->valor2;
                delete temp;
            }
        }
    }

    void insertar(const char* clave, const char* val1, const char* val2) {
        int idx = funcion_hash(clave);
        ParHash* nuevo = new ParHash;
        nuevo->clave = copiar_cadena(clave);
        nuevo->valor1 = copiar_cadena(val1);
        nuevo->valor2 = copiar_cadena(val2);
        nuevo->visitado = false;
        nuevo->siguiente = tabla[idx];
        tabla[idx] = nuevo;
    }

    ParHash* buscar(const char* clave) const {
        int idx = funcion_hash(clave);
        ParHash* actual = tabla[idx];
        while (actual != NULL) {
            if (strcmp(actual->clave, clave) == 0)
                return actual;
            actual = actual->siguiente;
        }
        return NULL;
    }

    bool existe(const char* clave) const {
        return buscar(clave) != NULL;
    }

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
        return (par != NULL) ? par->valor2 : NULL;
    }

    void obtener_todas(const char*** claves, const char*** pistas, bool** visitados, int& count) const {
        count = 0;
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                count++;
                actual = actual->siguiente;
            }
        }
        *claves = new const char*[count];
        *pistas = new const char*[count];
        *visitados = new bool[count];
        int idx = 0;
        for (int i = 0; i < TAM; i++) {
            ParHash* actual = tabla[i];
            while (actual != NULL) {
                (*claves)[idx] = actual->clave;
                (*pistas)[idx] = actual->valor2;
                (*visitados)[idx] = actual->visitado;
                idx++;
                actual = actual->siguiente;
            }
        }
    }
};

// ============================================================
// 6. ARBOL DE DECISION (preguntas SI/NO sobre la pista)
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

    void mostrar_nodo(NodoDecision* nodo, int nivel, const char* etiqueta) const {
        if (nodo == NULL) return;
        for (int i = 0; i < nivel; i++) cout << "  ";
        if (etiqueta != NULL) cout << etiqueta << " ";

        if (nodo->es_hoja) {
            cout << "-> " << (nodo->ubicacion ? nodo->ubicacion : "(sin resultado)") << endl;
            return;
        }

        cout << "Pregunta: ";
        for (int i = 0; i < nodo->num_claves; i++) {
            if (i > 0) cout << " / ";
            cout << "\"" << nodo->claves[i] << "\"";
        }
        cout << endl;

        mostrar_nodo(nodo->rama_si, nivel + 1, "SI:");
        mostrar_nodo(nodo->rama_no, nivel + 1, "NO:");
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

    void mostrar() const {
        if (raiz == NULL) {
            cout << "     (arbol vacio)" << endl;
            return;
        }
        mostrar_nodo(raiz, 1, "RAIZ:");
    }
};

// ============================================================
// 7. GRAFO
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
            if (strcmp(actual->nombre, nombre) == 0)
                return actual;
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

        NodoGrafo* nodo_origen = buscar_nodo(origen);
        Arista* nueva = new Arista;
        nueva->destino = copiar_cadena(destino);
        nueva->costo = costo;
        nueva->siguiente = nodo_origen->aristas;
        nodo_origen->aristas = nueva;

        NodoGrafo* nodo_destino = buscar_nodo(destino);
        Arista* inversa = new Arista;
        inversa->destino = copiar_cadena(origen);
        inversa->costo = costo;
        inversa->siguiente = nodo_destino->aristas;
        nodo_destino->aristas = inversa;
    }

    void obtener_vecinos(const char* nodo, const char*** destinos, int** costos, int& count) const {
        NodoGrafo* ng = buscar_nodo(nodo);
        count = 0;
        if (ng == NULL) return;

        Arista* actual = ng->aristas;
        while (actual != NULL) {
            count++;
            actual = actual->siguiente;
        }

        *destinos = new const char*[count];
        if (costos != NULL) *costos = new int[count];
        actual = ng->aristas;
        int idx = 0;
        while (actual != NULL) {
            ((const char**)(*destinos))[idx] = actual->destino;
            if (costos != NULL) (*costos)[idx] = actual->costo;
            idx++;
            actual = actual->siguiente;
        }
    }

    int obtener_costo(const char* origen, const char* destino) const {
        NodoGrafo* ng = buscar_nodo(origen);
        if (ng == NULL) return -1;
        Arista* actual = ng->aristas;
        while (actual != NULL) {
            if (strcmp(actual->destino, destino) == 0)
                return actual->costo;
            actual = actual->siguiente;
        }
        return -1;
    }

    int contar_nodos() const {
        int count = 0;
        NodoGrafo* actual = nodos;
        while (actual != NULL) {
            count++;
            actual = actual->siguiente;
        }
        return count;
    }

    void obtener_nombres_nodos(const char*** nombres, int& count) const {
        count = contar_nodos();
        *nombres = new const char*[count];
        NodoGrafo* actual = nodos;
        int idx = 0;
        while (actual != NULL) {
            (*nombres)[idx++] = actual->nombre;
            actual = actual->siguiente;
        }
    }

    bool existe_nodo(const char* nombre) const {
        return buscar_nodo(nombre) != NULL;
    }
};

// ============================================================
// 8. PROGRAMA PRINCIPAL
// ============================================================
class MapaPirata {
private:
    Grafo grafo;
    TablaHash ubicaciones;
    ArbolDecision arbol;
    Lista<const char*> camino_optimo;

    bool es_comentario_o_vacio(const char* linea) {
        if (linea == NULL || linea[0] == '\0') return true;
        const char* l = linea;
        while (*l == ' ' || *l == '\t') l++;
        return *l == '#' || *l == '\0';
    }

public:
    void cargar_mapa(const char* archivo) {
        ifstream file(archivo);
        if (!file.is_open()) {
            cerr << "  [ERROR] No se pudo abrir " << archivo << endl;
            return;
        }
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
                int costo = atoi(costo_str);
                grafo.agregar_arista(origen, destino, costo);
                delete[] origen;
                delete[] destino;
                delete[] costo_str;
            }
            for (int i = 0; i < num; i++) delete[] partes[i];
            delete[] partes;
            delete[] limpia;
        }
        file.close();
        cout << "  [OK] Grafo cargado: " << archivo << endl;
    }

    void cargar_ubicaciones(const char* archivo) {
        ifstream file(archivo);
        if (!file.is_open()) {
            cerr << "  [ERROR] No se pudo abrir " << archivo << endl;
            return;
        }
        char linea[256];
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

            ubicaciones.insertar(nom_trim, nom_trim, pista_trim);

            delete[] nombre;
            delete[] pista;
            delete[] nom_trim;
            delete[] pista_trim;
            delete[] limpia;
        }
        file.close();
        cout << "  [OK] Ubicaciones cargadas: " << archivo << endl;
    }

    void cargar_arbol_decision(const char* archivo) {
        if (!arbol.cargar(archivo)) {
            cerr << "  [ERROR] No se pudo cargar el arbol de decision: " << archivo << endl;
            return;
        }
        cout << "  [OK] Arbol de decision cargado: " << archivo << endl;
    }

    Lista<const char*> reconstruir_camino(const char* inicio, const char* destino,
                                          const char** padres, const char** nombres, int num_nodos) const {
        Lista<const char*> resultado_rev;
        const char* actual = destino;
        while (actual != NULL) {
            resultado_rev.agregar(actual);
            if (strcmp(actual, inicio) == 0) break;
            int idx = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], actual) == 0) { idx = i; break; }
            if (idx >= 0 && padres[idx] != NULL)
                actual = padres[idx];
            else
                break;
        }
        Lista<const char*> resultado;
        for (int i = resultado_rev.obtener_tamano() - 1; i >= 0; i--)
            resultado.agregar(resultado_rev[i]);
        return resultado;
    }

    // --- BFS (usa COLA) ---
    Lista<const char*> bfs(const char* inicio, const char* destino) {
        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        const char** padres = new const char*[num_nodos];
        bool* visitados_arr = new bool[num_nodos];
        for (int i = 0; i < num_nodos; i++) {
            padres[i] = NULL;
            visitados_arr[i] = false;
        }

        Cola<const char*> cola;
        cola.push(inicio);

        int idx_inicio = -1;
        for (int i = 0; i < num_nodos; i++)
            if (strcmp(nombres[i], inicio) == 0) { idx_inicio = i; break; }
        if (idx_inicio >= 0) visitados_arr[idx_inicio] = true;

        bool encontrado = false;
        while (!cola.vacia() && !encontrado) {
            const char* nodo = cola.front(); cola.pop();

            if (strcmp(nodo, destino) == 0) { encontrado = true; break; }

            const char** vecinos = NULL;
            int num_vecinos = 0;
            grafo.obtener_vecinos(nodo, &vecinos, NULL, num_vecinos);

            for (int i = 0; i < num_vecinos && !encontrado; i++) {
                int idx_vec = -1;
                for (int j = 0; j < num_nodos; j++)
                    if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }

                if (idx_vec >= 0 && !visitados_arr[idx_vec]) {
                    visitados_arr[idx_vec] = true;
                    padres[idx_vec] = nodo;
                    cola.push(vecinos[i]);
                }
            }
            if (num_vecinos > 0) delete[] vecinos;
        }

        Lista<const char*> resultado;
        if (encontrado)
            resultado = reconstruir_camino(inicio, destino, padres, nombres, num_nodos);

        delete[] padres;
        delete[] visitados_arr;
        delete[] nombres;
        return resultado;
    }

    // --- DFS (usa PILA) ---
    Lista<const char*> dfs(const char* inicio, const char* destino) {
        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        const char** padres = new const char*[num_nodos];
        bool* visitados_arr = new bool[num_nodos];
        for (int i = 0; i < num_nodos; i++) {
            padres[i] = NULL;
            visitados_arr[i] = false;
        }

        Pila<const char*> pila;
        pila.push(inicio);

        bool encontrado = false;
        while (!pila.vacia() && !encontrado) {
            const char* nodo = pila.top(); pila.pop();

            int idx_act = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], nodo) == 0) { idx_act = i; break; }

            if (idx_act >= 0 && !visitados_arr[idx_act]) {
                visitados_arr[idx_act] = true;

                if (strcmp(nodo, destino) == 0) { encontrado = true; break; }

                const char** vecinos = NULL;
                int num_vecinos = 0;
                grafo.obtener_vecinos(nodo, &vecinos, NULL, num_vecinos);

                for (int i = 0; i < num_vecinos && !encontrado; i++) {
                    int idx_vec = -1;
                    for (int j = 0; j < num_nodos; j++)
                        if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }

                    if (idx_vec >= 0 && !visitados_arr[idx_vec]) {
                        padres[idx_vec] = nodo;
                        pila.push(vecinos[i]);
                    }
                }
                if (num_vecinos > 0) delete[] vecinos;
            }
        }

        Lista<const char*> resultado;
        if (encontrado)
            resultado = reconstruir_camino(inicio, destino, padres, nombres, num_nodos);

        delete[] padres;
        delete[] visitados_arr;
        delete[] nombres;
        return resultado;
    }

    // --- DIJKSTRA (camino de menor costo) ---
    Lista<const char*> dijkstra(const char* inicio, const char* destino) {
        Lista<const char*> resultado;
        int num_nodos = grafo.contar_nodos();
        if (num_nodos == 0) return resultado;

        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        int* distancias = new int[num_nodos];
        const char** padres = new const char*[num_nodos];
        bool* visitados = new bool[num_nodos];
        int* heap_dist = new int[num_nodos];
        const char** heap_nodo = new const char*[num_nodos];
        int heap_tam = 0;

        int idx_inicio = -1, idx_destino = -1;
        for (int i = 0; i < num_nodos; i++) {
            distancias[i] = 2147483647;
            padres[i] = NULL;
            visitados[i] = false;
            if (strcmp(nombres[i], inicio) == 0) idx_inicio = i;
            if (strcmp(nombres[i], destino) == 0) idx_destino = i;
        }

        if (idx_inicio == -1 || idx_destino == -1) {
            delete[] nombres;
            delete[] distancias;
            delete[] padres;
            delete[] visitados;
            delete[] heap_dist;
            delete[] heap_nodo;
            return resultado;
        }

        distancias[idx_inicio] = 0;
        heap_dist[heap_tam] = 0;
        heap_nodo[heap_tam] = nombres[idx_inicio];
        heap_tam++;

        while (heap_tam > 0) {
            int min_idx = 0;
            for (int i = 1; i < heap_tam; i++)
                if (heap_dist[i] < heap_dist[min_idx]) min_idx = i;

            int dist_actual = heap_dist[min_idx];
            const char* nodo_actual = heap_nodo[min_idx];
            heap_dist[min_idx] = heap_dist[--heap_tam];
            heap_nodo[min_idx] = heap_nodo[heap_tam];

            int idx_actual = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], nodo_actual) == 0) { idx_actual = i; break; }

            if (visitados[idx_actual]) continue;
            visitados[idx_actual] = true;

            if (strcmp(nodo_actual, destino) == 0) break;

            const char** vecinos = NULL;
            int* costos_ptr = NULL;
            int num_vecinos = 0;
            grafo.obtener_vecinos(nodo_actual, &vecinos, &costos_ptr, num_vecinos);

            for (int i = 0; i < num_vecinos; i++) {
                int idx_vecino = -1;
                for (int j = 0; j < num_nodos; j++)
                    if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vecino = j; break; }

                int nueva_dist = dist_actual + costos_ptr[i];
                if (nueva_dist < distancias[idx_vecino]) {
                    distancias[idx_vecino] = nueva_dist;
                    padres[idx_vecino] = nodo_actual;
                    heap_dist[heap_tam] = nueva_dist;
                    heap_nodo[heap_tam] = nombres[idx_vecino];
                    heap_tam++;
                }
            }
            if (num_vecinos > 0) { delete[] vecinos; delete[] costos_ptr; }
        }

        if (distancias[idx_destino] == 2147483647) {
            delete[] nombres;
            delete[] distancias;
            delete[] padres;
            delete[] visitados;
            delete[] heap_dist;
            delete[] heap_nodo;
            return resultado;
        }

        Lista<const char*> camino_rev;
        const char* actual = destino;
        while (actual != NULL) {
            camino_rev.agregar(actual);
            int idx_act = -1;
            for (int i = 0; i < num_nodos; i++)
                if (strcmp(nombres[i], actual) == 0) { idx_act = i; break; }
            if (idx_act >= 0 && padres[idx_act] != NULL && strcmp(padres[idx_act], actual) != 0)
                actual = padres[idx_act];
            else
                break;
        }

        for (int i = camino_rev.obtener_tamano() - 1; i >= 0; i--)
            resultado.agregar(camino_rev[i]);

        delete[] nombres;
        delete[] distancias;
        delete[] padres;
        delete[] visitados;
        delete[] heap_dist;
        delete[] heap_nodo;
        return resultado;
    }

    void explorar_con_pistas(const char* inicio, const char* destino) {
        cout << "\n==================================================" << endl;
        cout << "  EXPLORANDO LA ISLA CON PISTAS (BFS)" << endl;
        cout << "==================================================" << endl;
        cout << "  Inicio: " << inicio << endl;
        cout << "  Tesoro: " << destino << endl;

        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);
        bool* visitados_arr = new bool[num_nodos];
        for (int i = 0; i < num_nodos; i++) visitados_arr[i] = false;

        int idx_ini = -1;
        for (int i = 0; i < num_nodos; i++)
            if (strcmp(nombres[i], inicio) == 0) { idx_ini = i; break; }
        if (idx_ini >= 0) visitados_arr[idx_ini] = true;

        Cola<const char*> cola;
        cola.push(inicio);

        while (!cola.vacia()) {
            const char* actual = cola.front(); cola.pop();

            if (ubicaciones.existe(actual)) {
                ubicaciones.marcar_visitado(actual, true);
                char* pista = ubicaciones.obtener_pista(actual);
                cout << "\n  >> Llegaste a: " << actual << endl;
                cout << "     Pista: \"" << (pista ? pista : "?") << "\"" << endl;

                char* sugerencia = NULL;
                if (pista != NULL) sugerencia = arbol.interpretar(pista);

                if (sugerencia != NULL) {
                    cout << "     >> La pista sugiere ir a: " << sugerencia << endl;
                } else {
                    cout << "     >> No se pudo interpretar la pista" << endl;
                }

                if (strcmp(actual, destino) == 0) {
                    cout << "\n  ================================================" << endl;
                    cout << "  *** TESORO ENCONTRADO en " << actual << "! ***" << endl;
                    cout << "  ================================================" << endl;
                    break;
                }
            }

            const char** vecinos = NULL;
            int num_vecinos = 0;
            grafo.obtener_vecinos(actual, &vecinos, NULL, num_vecinos);

            for (int i = 0; i < num_vecinos; i++) {
                int idx_vec = -1;
                for (int j = 0; j < num_nodos; j++)
                    if (strcmp(nombres[j], vecinos[i]) == 0) { idx_vec = j; break; }
                if (idx_vec >= 0 && !visitados_arr[idx_vec]) {
                    visitados_arr[idx_vec] = true;
                    cola.push(vecinos[i]);
                }
            }
            if (num_vecinos > 0) delete[] vecinos;
        }
        delete[] visitados_arr;
        delete[] nombres;
    }

    void guardar_resultado(const char* inicio, const char* destino,
                           const Lista<const char*>& camino_bfs,
                           const Lista<const char*>& camino_dijkstra) {
        ofstream file("ruta_tesoro.txt");
        if (!file.is_open()) {
            cerr << "  [ERROR] No se pudo crear ruta_tesoro.txt" << endl;
            return;
        }

        file << "==================================================" << endl;
        file << "  RUTA AL TESORO DEL PIRATA" << endl;
        file << "==================================================" << endl << endl;
        file << "Isla: El Tesoro del Pirata" << endl;
        file << "Inicio: " << inicio << endl;
        file << "Tesoro: " << destino << endl << endl;

        file << "UBICACIONES Y PISTAS:" << endl;
        file << "----------------------------------------" << endl;

        const char** claves = NULL;
        const char** pistas_arr = NULL;
        bool* visitados_arr = NULL;
        int count = 0;
        ubicaciones.obtener_todas(&claves, &pistas_arr, &visitados_arr, count);

        for (int i = 0; i < count; i++) {
            file << "  " << claves[i] << ": \"" << pistas_arr[i] << "\" "
                 << (visitados_arr[i] ? "[Visitado]" : "[No visitado]") << endl;
        }
        delete[] claves;
        delete[] pistas_arr;
        delete[] visitados_arr;

        file << "\nCAMINO MAS CORTO (BFS):" << endl;
        file << "----------------------------------------" << endl;
        if (!camino_bfs.vacia()) {
            for (int i = 0; i < camino_bfs.obtener_tamano(); i++) {
                if (i > 0) file << " -> ";
                file << camino_bfs[i];
            }
            file << endl;
        } else {
            file << "  No encontrado" << endl;
        }

        file << "\nCAMINO OPTIMO (Dijkstra):" << endl;
        file << "----------------------------------------" << endl;
        if (!camino_dijkstra.vacia()) {
            for (int i = 0; i < camino_dijkstra.obtener_tamano(); i++) {
                if (i > 0) file << " -> ";
                file << camino_dijkstra[i];
            }
            file << endl;

            file << "\n  Detalle del costo:" << endl;
            int costo_total = 0;
            for (int i = 0; i < camino_dijkstra.obtener_tamano() - 1; i++) {
                int c = grafo.obtener_costo(camino_dijkstra[i], camino_dijkstra[i + 1]);
                file << "    " << camino_dijkstra[i] << " -> " << camino_dijkstra[i + 1] << ": " << c << endl;
                costo_total += c;
            }
            file << "  Costo total: " << costo_total << endl;
        } else {
            file << "  No encontrado" << endl;
        }

        file << "\nSECUENCIA DE PISTAS:" << endl;
        file << "----------------------------------------" << endl;
        if (!camino_dijkstra.vacia()) {
            for (int i = 0; i < camino_dijkstra.obtener_tamano(); i++) {
                const char* pista = ubicaciones.obtener_pista(camino_dijkstra[i]);
                file << "  " << (i + 1) << ". " << camino_dijkstra[i] << ": \""
                     << (pista ? pista : "?") << "\"" << endl;
            }
        }

        file << "\n==================================================" << endl;
        file.close();
        cout << "\n  >> Resultado guardado en 'ruta_tesoro.txt'" << endl;
    }

    void ejecutar() {
        cout << "=======================================================" << endl;
        cout << "   ***  EL TESORO DEL PIRATA  ***" << endl;
        cout << "=======================================================" << endl;

        cargar_mapa("mapa.txt");
        cargar_ubicaciones("ubicaciones.txt");
        cargar_arbol_decision("pistas.txt");

        int num_nodos = grafo.contar_nodos();
        const char** nombres = NULL;
        grafo.obtener_nombres_nodos(&nombres, num_nodos);

        cout << "\n  Nodos del grafo (" << num_nodos << "): ";
        for (int i = 0; i < num_nodos; i++) {
            if (i > 0) cout << ", ";
            cout << nombres[i];
        }
        cout << endl;
        delete[] nombres;

        cout << "  Arbol de decision (" << arbol.obtener_cont_nodos() << " nodos):" << endl;
        arbol.mostrar();

        const char* inicio = "Playa";
        const char* destino = "Tesoro";

        cout << "\n==================================================" << endl;
        cout << "  RECORRIDOS DEL GRAFO" << endl;
        cout << "==================================================" << endl;

        Lista<const char*> camino_bfs = bfs(inicio, destino);
        cout << "\n  BFS (Cola) - Camino: ";
        if (!camino_bfs.vacia()) {
            for (int i = 0; i < camino_bfs.obtener_tamano(); i++) {
                if (i > 0) cout << " -> ";
                cout << camino_bfs[i];
            }
            cout << endl;
        } else {
            cout << "No encontrado" << endl;
        }

        Lista<const char*> camino_dfs = dfs(inicio, destino);
        cout << "  DFS (Pila) - Camino: ";
        if (!camino_dfs.vacia()) {
            for (int i = 0; i < camino_dfs.obtener_tamano(); i++) {
                if (i > 0) cout << " -> ";
                cout << camino_dfs[i];
            }
            cout << endl;
        } else {
            cout << "No encontrado" << endl;
        }

        Lista<const char*> camino_dijkstra = dijkstra(inicio, destino);
        cout << "  Dijkstra - Camino optimo: ";
        if (!camino_dijkstra.vacia()) {
            for (int i = 0; i < camino_dijkstra.obtener_tamano(); i++) {
                if (i > 0) cout << " -> ";
                cout << camino_dijkstra[i];
            }
            cout << endl;

            int costo_total = 0;
            for (int i = 0; i < camino_dijkstra.obtener_tamano() - 1; i++) {
                costo_total += grafo.obtener_costo(camino_dijkstra[i], camino_dijkstra[i + 1]);
            }
            cout << "  Costo total del camino optimo: " << costo_total << endl;
            camino_optimo = camino_dijkstra;
        } else {
            cout << "No encontrado" << endl;
        }

        explorar_con_pistas(inicio, destino);
        guardar_resultado(inicio, destino, camino_bfs, camino_dijkstra);
    }
};

int main() {
    MapaPirata mapa;
    mapa.ejecutar();
    return 0;
}
