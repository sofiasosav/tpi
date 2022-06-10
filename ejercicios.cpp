#include "ejercicios.h"
#include "auxiliares.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

/******++++**************************** EJERCICIO tiempoTotal ***********+++***********************/
double tiempoMaximo(viaje v){
    double max = 0;
    for(int i = 0; i < v.size(); i++){
        if(obtenerTiempo(v[i]) >= max){
            max = obtenerTiempo(v[i]);
        }
    }
    return max;
}

double tiempoMinimo(viaje v){
    double min = obtenerTiempo(v[0]);
    for(int i = 0; i < v.size(); i++){
        if(obtenerTiempo(v[i]) <= min){
            min = obtenerTiempo(v[i]);
        }
    }
    return min;
}


tiempo tiempoTotal(viaje v) {
    tiempo t;
    t = tiempoMaximo(v) - tiempoMinimo(v);
    return t;
}

/************++*********************** EJERCICIO distanciaTotal ************++*********************/
void cambio(viaje &v, int i, int j){
    tuple <tiempo, gps> k;
    k = v[i];
    v[i] = v[j];
    v[j] = k;
}

void insertar(viaje &v, int i){
    while(i > 0 && obtenerTiempo(v[i]) < obtenerTiempo(v[i-1])){
        cambio(v,i,i-1);
        i--;
    }
}

void esViajeOrdenadoPorTiempo(viaje &v){
    for(int i = 0; i < v.size(); i++){
        insertar(v,i);
    }
}

distancia distanciaTotal(viaje v) {
    distancia d=0;
    double dist = 0;
    esViajeOrdenadoPorTiempo(v);
    for(int i = 1; i < v.size(); i++){
       dist = distEnKM(obtenerPosicion(v[i-1]), obtenerPosicion(v[i]));
       d = d + dist;
    }
    return d;
}

/*****************************+***** EJERCICIO excesoDeVelocidad **********************************/
float velocidad(viaje v, int i){
    int vel;
    vel = distEnKM(obtenerPosicion(v[i+1]), obtenerPosicion(v[i]))/((obtenerTiempo(v[i+1])- obtenerTiempo(v[i]))/3600);
    return vel;
}
// con q haya uno q sea mayor a 80 ya sale del for y te devuelve true
bool superoLaVelocidad(viaje v){
    for(int i = 0; i < v.size(); i++){
        if(velocidad(v,i) > 80){
            return true;
        }
    }
}

bool excesoDeVelocidad(viaje v) {
    bool resp;
    esViajeOrdenadoPorTiempo(v);
    resp = superoLaVelocidad(v);
    return resp;
}

/************************************ EJERCICIO recorridoCubierto *******************************/
bool estaCubierto(viaje v, recorrido r, distancia u, int i){
    int k = 0;
    for(int j = 0; j < v.size(); j++) {
        if (distEnKM(r[i], obtenerPosicion(v[j])) < u) {
            k = k+1;
        }
    }
    return k != 0;
}

vector<gps> recorridoNoCubierto(viaje v, recorrido r, distancia u) {
    vector<gps> resp;
    for(int i = 0; i < r.size(); i++){
        if(estaCubierto(v,r,u,i) == true){
            continue;
        } else {
            resp.push_back(r[i]);
        }
    }
    return resp;
}

/***************************************** EJERCICIO flota ***************************************/
int viajeEnFranjaHoraria(vector<viaje> f, tiempo t0, tiempo tf) {
    int viajeEnFranja = 0;
    for (int i = 0; i < f.size(); i++) {
        if (obtenerTiempo(f[i][f[i].size()-1]) < t0 || obtenerTiempo(f[i][0]) > tf){
            continue;
        } else {
            viajeEnFranja = viajeEnFranja + 1;
        }
    } return viajeEnFranja;
}


int flota(vector<viaje> f, tiempo t0, tiempo tf) {
    int resp = 0;
    resp = viajeEnFranjaHoraria(f,t0,tf);
    return resp;
}

/************************************** EJERCICIO construirGrilla *******************************/
double altoCelda(gps esq1, gps esq2, int k){
     double celdaAlto = (obtenerLatitud(esq1) - obtenerLatitud(esq2))/k;
     return celdaAlto;
}

double anchoCelda(gps esq1, gps esq2, int k){
    double celdaAncho = (obtenerLongitud(esq2)- obtenerLongitud(esq1))/k;
    return celdaAncho;
}

celda creoCelda(gps esq1, gps esq2, int n, int m, int i, int j){
    double latGPS1 = obtenerLatitud(esq1) - altoCelda(esq1,esq2,n)*(i-1);
    double latGPS2 = latGPS1 - altoCelda(esq1,esq2,n);
    double lonGPS1 = obtenerLongitud(esq1) + anchoCelda(esq1,esq2,m)*(j-1);
    double lonGPS2 = lonGPS1 + anchoCelda(esq1,esq2,m);
    gps gps1 = make_tuple(latGPS1,lonGPS1);
    gps gps2 = make_tuple(latGPS2,lonGPS2);
    nombre nombreCelda = make_tuple(i,j);

    celda celdaDeGrilla = make_tuple(gps1, gps2, nombreCelda);

}


grilla construirGrilla(gps esq1, gps esq2, int n, int m) {
    grilla resp = {};
    for(int i = n; i > 0; i--){ // recorre la fila
        for(int j = m; j > 0 ; j--){ // recorre columna
            resp.push_back(creoCelda(esq1,esq2,n,m,i,j));
        }

    }

    return resp;
}

/************************************* EJERCICIO cantidadDeSaltos ******************************/
nombre nombreDeLasCeldas(celda c){
    return get<2> (c);
}


int distanciaEntreCeldas(nombre n1, nombre n2){
    return sqrt(pow((obtenerLatitud(n1)- obtenerLatitud(n2)),2) + pow((obtenerLongitud(n1)- obtenerLongitud(n2)),2));
}


// fijas viaje y despues recorres la grilla para saber a q nombre pertenece
int cantidadDeSaltos(grilla g, viaje v) {
    int resp=0;
    esViajeOrdenadoPorTiempo(v);
    for(int i = 1; i < v.size(); i++){ // recorro viaje
        for(int j = 0; j < g.size(); j++){ // grilla para ver a q corresponde cada viaje
            if(distanciaEntreCeldas(nombreDeLasCeldas(g[j-1]), nombreDeLasCeldas(g[j])) > 1){
                resp += 1;
            }
        }
    }

    return resp;
}

/************************************* EJERCICIO corregirViaje ******************************/
bool gpsCorrecto(viaje v, vector<tiempo> errores, int i){
    bool resp = true;
    for(int j =0; j < errores.size(); j++){
        if(obtenerTiempo(v[i]) == errores[j]){
            resp = false;
            break; // prreguntar si rompe o no el for
        } // te da true si y solo si no esta en errores es decir si es correcto
    }
    return resp;
}

bool errorAlPrincipioDelViaje(viaje v, vector<tiempo> errores){
    for(int i = 0; i < errores.size(); i++){
        errores[i] = obtenerTiempo(v[0]);        // busco posicion q v[0] = errores en i
    }
    return true;
}

bool errorAlFinalDelViaje(viaje v, vector<tiempo> errores){
    for(int i = 0; i < errores.size(); i++){
        errores[i] = obtenerTiempo(v[v.size()]);        // busco posicion q v[0] = errores en i
    }
    return true;
}

void buscoPuntosMasCercanos(viaje v, int j, vector<tiempo> errores){
        if(errorAlPrincipioDelViaje(v, errores)){
            vector<tuple<tiempo, gps>> correcto1;
            vector<tuple<tiempo, gps>> correcto2;
            for(int j = 0; j < v.size(); j ++){
                if(gpsCorrecto(v,errores,j)){
                    
                }
            }
        }
        if(errorAlFinalDelViaje(v, errores)){

        }
    }



void corregirViaje(viaje& v, vector<tiempo> errores){
    esViajeOrdenadoPorTiempo(v);
    for(int j = 0; j < errores.size(); j++){
        for(int i = 0; v.size(); i++){
            if(errores[j] = obtenerTiempo(v[i])){ // si ese punto es un error entonces tenes q buscar el mas cercano

            }
        }
    }

    return;
}
