#include "cabeza.h"

int main()
{
	SetRegistros* regSet = (SetRegistros*)malloc(sizeof(SetRegistros));
	regSet = inicializarRegistros();
	//imprimirRegistros(regSet);
	//SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	//Programa* programa = (Programa*)malloc(sizeof(Programa));
	//programa = cargarPrograma("jugada1.txt");
	//inSet = cargarProgramaAMemoria(programa);
	//imprimirInstrucciones2(inSet);
	//Registro* reg = (Registro*)malloc(sizeof(Registro));
	//reg = crearRegistro("$t1",0);
	pipeline();
	//insertarRegistro(regSet,reg,0);
	return 0;
}