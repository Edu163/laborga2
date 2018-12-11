#ifndef __CABEZA__
#define __CABEZA__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Almacena todas las instrucciones de un programa
typedef struct Programa{
	char** matrizInstrucciones;
	int largo;
}Programa;
//Almacena una instruccion
typedef struct Instruccion
{
	char* op;
	char* rs;
	char* rt;
	char* rd;
	int  constante;
	struct Instruccion* siguiente;
}Instruccion;
//Almacena todas las instrucciones
typedef struct SetInstrucciones{
	Instruccion* cabeza;
	int largo;
	//int indice;
}SetInstrucciones;

//Almacena un registro
typedef struct Registro
{
	char* nombre;
	int numero;
	int valor;
	struct Registro* siguiente;
}Registro;

//Almacena la cabeza de los registros
typedef struct SetRegistros{
	Registro* cabeza;
	int largo;
	//int indice;
}SetRegistros;

void insertarInstruccion2(SetInstrucciones* inSet,char** linea);
void insertarInstruccion(SetInstrucciones* inSet,Instruccion* in);
Instruccion* obtenerInstruccion(int n,SetInstrucciones* inSet);
void imprimirInstrucciones(SetInstrucciones* inSet);
void imprimirInstrucciones2(SetInstrucciones* inSet);
void imprimirUnaInstruccion(Instruccion* instruccion);

SetRegistros* inicializarRegistros();
Registro* crearRegistro(char* nombreRegistro, int numero);
void insertFinal(SetRegistros* regSet, char* nombreRegistro, int numero);
void insertarRegistro(SetRegistros* regSet,char* nombreRegistro, int numero);
Registro* obtenerRegistro(int n,SetRegistros* regSet);
Registro* buscarRegistro(SetRegistros* regSet, char* nombreRegistro);
void imprimirRegistros(SetRegistros* registers);

Programa* elminarComentarios(Programa* inst);
Programa* cargarPrograma(char* pathFile);
SetInstrucciones* cargarProgramaAMemoria(Programa* programa);
Instruccion* crearInstruccion(char** programLine);

char** makeLine(char* instructionLine);
void removeChar(char *str, char garbage);
void concatChar(char* str, char c);
Instruccion* instructionFetch(SetInstrucciones* memoria, int direccionPC);
void avanzarInstrucciones(Instruccion* pipeline,Instruccion* instruccion);

void pipeline();
void printEtapasPL(Instruccion* pipeline);
int pipelineVacio(Instruccion* pipeline);
#endif

