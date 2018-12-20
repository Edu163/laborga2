#ifndef __CABEZA__
#define __CABEZA__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int* stackPointer;
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
	char* etiqueta;
	int  constante;
	int valor;
	int posicion;
	int posicionReal;
	struct Instruccion* siguiente;
}Instruccion;
typedef struct Buffer
{
	int valor;
}Buffer;
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
typedef struct Riesgo
{
	char* nombre;
	char* descripcion;
	char* tipo; //dato o control
	char* registro;
	int linea;
	int ciclo;
	struct Riesgo* siguiente;
}Riesgo;

//Almacena la cabeza de los registros
typedef struct SetRiesgos{
	Riesgo* cabeza;
	int largo;
	//int indice;
}SetRiesgos;


void insertarInstruccion2(SetInstrucciones* inSet,char** linea);
void insertarInstruccion(SetInstrucciones* inSet,Instruccion* in);
Instruccion* obtenerInstruccion(int n,SetInstrucciones* inSet);
void imprimirInstrucciones(SetInstrucciones* inSet);
void imprimirInstrucciones2(SetInstrucciones* inSet);
void imprimirUnaInstruccion(Instruccion* instruccion);
void enumerarInstrucciones(SetInstrucciones* instrucciones);

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
void avanzarInstrucciones(Instruccion* pipeline,Instruccion* instruccion);
void avanzarBuffer(Buffer* buffer, int valor);

void pipeline();
void printEtapasPL(Instruccion* pipeline);
int pipelineVacio(Instruccion* pipeline);
void ponerInstruccionNopJump(Instruccion* pipeline);
void flush(Instruccion* pipeline);
int condicionSaltoJump(Instruccion* pipeline);
//ETAPAS
//ALU
int alu(Instruccion* in,SetRegistros* regSet,Programa* programa);
int buscarEtiqueta(char* etiqueta, Programa* programa);
//DATA HAZARD
Riesgo* unidadDeteccionRiesgos(Instruccion* fetch,SetRiesgos* riesgos, SetRegistros* regSet, Instruccion* pipeline, int CC);
void deteccionRiesgosControl(Programa* programa,Instruccion* pipeline,SetRiesgos* riesgos,SetRegistros* regSet, int linea, int CC);
Riesgo* EXHazard(Instruccion* insEX_MEM, Instruccion* insID_EX,SetRegistros* regSet);
Riesgo* MEMHazard(Instruccion* insMEM_WB, Instruccion* insID_EX,SetRegistros* regSet);
Riesgo* LoadHazard(Instruccion* insID_EX, Instruccion* insIF_ID, SetRegistros* regSet);
//RIESGOS ESTRUCTURA
void imprimirRiesgo(Riesgo* riesgo);
void insertarRiesgo(SetRiesgos* riesgos, Riesgo* riesgo);
Riesgo* obtenerRiesgo(SetRiesgos* riesgos,int n);
void imprimirRiesgos(SetRiesgos* riesgos);
void imprimirSoluciones(SetRiesgos* riesgos);
void imprimirSolucion(Riesgo* riesgo);
//ETAPAS
Instruccion* instructionFetch(SetInstrucciones* memoria, int* direccionPC);
char* instructionDecode(Instruccion* instruccion);
int executeInstruction(Instruccion* instruccion, SetRegistros* regSet,Programa* programa);
void writeBack(Instruccion* instruccion, SetRegistros* regSet);
int memoryAccess(Instruccion* instruccion,SetRegistros* regSet);

#endif

