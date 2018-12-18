#ifndef funciones_c
#define funciones_c
#include "cabeza.h"
//PIPELINE
void pipeline()
{
	stackPointer = (int*)malloc(sizeof(int)*256);
	Riesgo* riesgo= (Riesgo*)malloc(sizeof(Riesgo));
	SetRiesgos* riesgos = (SetRiesgos*)malloc(sizeof(SetRiesgos));
	riesgos->cabeza = NULL;
	riesgos->largo = 0;
	SetRegistros* regSet = (SetRegistros*)malloc(sizeof(SetRegistros));
	regSet = inicializarRegistros();
	//imprimirRegistros(regSet);
	SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	Programa* programa = (Programa*)malloc(sizeof(Programa));
	programa = cargarPrograma("jugada3.txt");
	/*for(int i = 0; i < programa->largo; i++)
	{
		printf("%s",programa->matrizInstrucciones[i]);
	}*/
	printf("\n");
	inSet = cargarProgramaAMemoria(programa);
	//printf("\n");
	imprimirInstrucciones(inSet);
	printf("ETIQ: %d\n", buscarEtiqueta("label",programa));
	//int dir = alu(obtenerInstruccion(10,inSet),regSet,programa);
	//imprimirUnaInstruccion(obtenerInstruccion(10,inSet));
	//printf("ETIQ: %d\n", dir);
	//imprimirInstrucciones(inSet);
	Instruccion* pipeline = (Instruccion*)malloc(sizeof(Instruccion)*4);
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	instruccion->rs = (char*)malloc(sizeof(char)*8);
	instruccion->rt = (char*)malloc(sizeof(char)*8);
	instruccion->rd = (char*)malloc(sizeof(char)*8);
	instruccion->etiqueta = (char*)malloc(sizeof(char)*64);
	//Buffer* buffer = (Buffer*)malloc(sizeof(Buffer)*4);
	for(int i = 0; i<4; i++)
	{
			pipeline[i].op = (char*)malloc(sizeof(char)*8);
			pipeline[i].rs = (char*)malloc(sizeof(char)*8);
			pipeline[i].rt = (char*)malloc(sizeof(char)*8);
			pipeline[i].rd = (char*)malloc(sizeof(char)*8);
			pipeline[i].etiqueta = (char*)malloc(sizeof(char)*64);
	}
	for(int i = 0; i<4; i++)
	{
		strcpy(pipeline[i].op,"NOP");
	}
	//----------------------------------------------------
	//----------------------------------------------------
	int direccion = 0;
	int ciclosReloj = 1;
	int cndJump = -1;
	strcpy(instruccion->op,"NOP");
	char* funct = (char*)malloc(sizeof(char)*8);
	int resultado = 0;
	int jumpDireccion = 0;
	printf("%s\n","                    IF                 ID                EX                MM                WB" );
	/*while(pipelineVacio(pipeline) != 1|| direccion<inSet->largo)
	{	
		//----------------------------------------------------
		if(strcmp(funct,"jump")==0)
		{
			jumpDireccion = alu(&pipeline[0],regSet,programa);
			direccion = jumpDireccion;
			ponerInstruccionNopJump(pipeline);
			instruccion = instructionFetch(inSet,jumpDireccion);
			//printf("DIRECCION %d\n", jumpDireccion );
		}
		else
		{
			avanzarInstrucciones(pipeline,instruccion);
			instruccion = instructionFetch(inSet,direccion);
		}
		//----------------------------------------------------
		riesgo = unidadDeteccionRiesgos(riesgos,regSet,pipeline,direccion,ciclosReloj);
		funct = instructionDecode(&pipeline[0]);
		resultado = executeInstruction(&pipeline[1],regSet,programa);
		memoryAccess(&pipeline[2],regSet);
		writeBack(&pipeline[3], regSet);
		//IMPRIMIR
		//----------------------------------------------------
		//----------------------------------------------------
		if(ciclosReloj<10)
		{
			printf("%d)  ",ciclosReloj);
			printf("%d)  ",direccion);
		}
		if(ciclosReloj>=10)
		{
			printf("%d) ",ciclosReloj);
			printf("%d)  ",direccion);
		}
		printf("  %s |", funct);
		printf("%  d |",resultado);
		imprimirUnaInstruccion(instruccion);
		printf(" ");
		printEtapasPL(pipeline);
		printf("\n");
		//----------------------------------------------------
		//----------------------------------------------------
		//FIN DE LA IMPRESION
		direccion++;
		ciclosReloj++;
		//SALTOS DEL PIPELINE
	}*/
	//imprimirRegistros(regSet);
	//imprimirRiesgos(riesgos);
	//printf("%d\n",riesgos->largo);

}
//VERIFICAR INSTRUCCION JUMP EN PIPELINE
int condicionSaltoJump(Instruccion* pipeline)
{
	if(strcmp(pipeline[0].op,"j")==0)
	{
		return 1;
	}
	if(strcmp(pipeline[1].op,"j")==0)
	{
		return 2;
	}
	if(strcmp(pipeline[2].op,"j")==0)
	{
		return 3;
	}
	if(strcmp(pipeline[3].op,"j")==0)
	{
		return 3;
	}
	return -1;
		
}
//PUEDE GENERAR ERROR AL NO DAR ESPACIO PARA LAS DEMAS VARIABLES
Instruccion* instructionFetch(SetInstrucciones* memoria, int direccionPC)
{
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	if(direccionPC>=memoria->largo)
	{
		strcpy(instruccion->op,"NOP");
		return instruccion;
	}
	return obtenerInstruccion(direccionPC,memoria);
}
//Se hace avanzar el pipeline de instrucciones
void avanzarInstrucciones(Instruccion* pipeline,Instruccion* instruccion)
{
	pipeline[3] = pipeline[2];
	pipeline[2] = pipeline[1];
	pipeline[1] = pipeline[0];
	pipeline[0] = *instruccion;
}
void ponerInstruccionNopJump(Instruccion* pipeline)
{
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	instruccion->rs = (char*)malloc(sizeof(char)*8);
	instruccion->rt = (char*)malloc(sizeof(char)*8);
	instruccion->rd = (char*)malloc(sizeof(char)*8);
	instruccion->etiqueta = (char*)malloc(sizeof(char)*64);
	instruccion->siguiente = NULL;
	strcpy(instruccion->op,"NOP");
	pipeline[3] = pipeline[2];
	pipeline[2] = pipeline[1];
	pipeline[1] = pipeline[0];
	pipeline[0] = *instruccion;
}
//Se hace avanzar el buffer con los datos
void avanzarBuffer(Buffer* buffer, int valor)
{
	buffer[3].valor = buffer[2].valor;
	buffer[2].valor = buffer[1].valor;
	buffer[1].valor = buffer[0].valor;
	buffer[0].valor = valor;
}
//Imprimir las multiples etapas
void printEtapasPL(Instruccion* pipeline)
{
	for(int i = 0; i<4;i++)
	{
		if(strcmp(pipeline[i].op,"NOP")==0)
		{
			printf("%s ","------ NOP -----");
		}
		if(strcmp(pipeline[i].op,"sw")==0)
		{
			printf("%s ",pipeline[i].op);
			//CAMBIO DE RT -> RD
			printf("%s ",pipeline[i].rd);
			printf("%d(",pipeline[i].constante);
			printf("%s)    ",pipeline[i].rs);
		}
		if(strcmp(pipeline[i].op,"lw")==0)
		{
			printf("%s ",pipeline[i].op);
			//CAMBIO DE RT -> RD
			printf("%s ",pipeline[i].rd);
			printf("%d(",pipeline[i].constante);
			printf("%s)    ",pipeline[i].rs);
		}
		if(strcmp(pipeline[i].op,"addi")==0||strcmp(pipeline[i].op,"subi")==0)
		{
			printf("%s ",pipeline[i].op);
			//CAMBIO DE RT -> RD
			printf("%s ",pipeline[i].rd);
			printf("%s ",pipeline[i].rs);
			printf("%d ",pipeline[i].constante);
		}
		if(strcmp(pipeline[i].op,"j")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s          ",pipeline[i].etiqueta);
		}
		if(strcmp(pipeline[i].op,"beq")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s ",pipeline[i].rt);
			printf("%s ",pipeline[i].rs);
			printf("%s ",pipeline[i].etiqueta);
		}
	}
}
//Imprimir una instrucción simple
void imprimirUnaInstruccion(Instruccion* instruccion)
{
	if(strcmp(instruccion->op,"NOP")==0)
		{
			printf("%s ","------ NOP -----");
		}
		if(strcmp(instruccion->op,"sw")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rd);
			printf("%d(",instruccion->constante);
			if(instruccion->constante>9){printf("%s)   ",instruccion->rs);}else{printf("%s)    ",instruccion->rs);}
		}
		if(strcmp(instruccion->op,"lw")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rd);
			printf("%d(",instruccion->constante);
			if(instruccion->constante>9){printf("%s)   ",instruccion->rs);}else{printf("%s)    ",instruccion->rs);}
		}
		if(strcmp(instruccion->op,"addi")==0||strcmp(instruccion->op,"subi")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rd);
			printf("%s ",instruccion->rs);
			printf("%d ",instruccion->constante);
		}
		if(strcmp(instruccion->op,"j")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s          ",instruccion->etiqueta);
		}
		if(strcmp(instruccion->op,"beq")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rt);
			printf("%s ",instruccion->rs);
			printf("%s ",instruccion->etiqueta);
		}
}
//Retorna 1 o 0 dependiendo si el pipeline esta vacio o no (sin instrucciones que ejecutar)
int pipelineVacio(Instruccion* pipeline)
{
	if(strcmp(pipeline[0].op,"NOP")==0 && strcmp(pipeline[1].op,"NOP")==0 && strcmp(pipeline[2].op,"NOP")==0 && strcmp(pipeline[3].op,"NOP")==0)
	{
		return 1;
	}
	return 0;
}
//------------------------------------------------------
//------------------------------------------------------
//FUNCIONES PARA ALU
//ALU unidad de calculo aritmetico
int alu(Instruccion* in,SetRegistros* regSet,Programa* programa){
 	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
 	char* funct = (char*)malloc(sizeof(char)*8);
	int constante;
	int resultado;
	strcpy(funct, in->op);
 	if(strcmp(funct,"addi")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		resultado = rs->valor + constante;
		return resultado;
	}
	//CONTROL SIGNAL EX AND WB
	else if(strcmp(funct,"subi")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		resultado = rs->valor - constante;
		return resultado;
	}
	//CONTROL SIGNAL EX AND MEM 
	else if(strcmp(funct,"sw")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"lw")==0)
	{
		//rt = buscarRegistro(regSet, in->rt);
		//rs = buscarRegistro(regSet, in->rs);
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"j")==0)
	{
		//rt = buscarRegistro(regSet, in->rt);
		//rs = buscarRegistro(regSet, in->rs);
		resultado = buscarEtiqueta(in->etiqueta,programa);
		//printf("ETIQUETA: %d\n", resultado);
		//printf("ETIQUETA: %s\n", in->etiqueta);
		return resultado;
	}
	else if(strcmp(funct,"beq")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor == rs->valor)
		{
			resultado = buscarEtiqueta(in->etiqueta,programa);
		}
		else
		{
			resultado = -1;
		}
		return resultado;
	}
	return 0;
}
int buscarEtiqueta(char* etiqueta, Programa* programa)
{
	for(int i = 0; i < programa->largo; i++)
	{
		int largo = strlen(programa->matrizInstrucciones[i]);
		if(strncmp(etiqueta,programa->matrizInstrucciones[i],largo-2)==0)
		{
			//printf("ET: %s\n", etiqueta );
			//printf("PROGRAMA: %s\n",programa->matrizInstrucciones[i] );
			return i;
		}
	}
	return -1;

}








//------------------------------------------------------
//------------------------------------------------------
//ETAPAS
//INSTRUCTION DECODE
char* instructionDecode(Instruccion* instruccion)
{
	char* tipo = (char*)malloc(sizeof(char)*6);
	if(strcmp(instruccion->op,"addi")==0||strcmp(instruccion->op,"subi")==0||strcmp(instruccion->op,"addiu")==0
		||strcmp(instruccion->op,"add")==0||strcmp(instruccion->op,"sub")==0||strcmp(instruccion->op,"mul")==0
		||strcmp(instruccion->op,"div")==0)
	{
		strcpy(tipo,"AI");
		return tipo;
	}
	if(strcmp(instruccion->op,"add")==0||strcmp(instruccion->op,"sub")==0||strcmp(instruccion->op,"mul")==0
		||strcmp(instruccion->op,"div")==0)
	{
		strcpy(tipo,"AR");
		return tipo;
	}
	else if(strcmp(instruccion->op,"bgt")==0||strcmp(instruccion->op,"beq")==0||strcmp(instruccion->op,"blt")==0)
	{
		strcpy(tipo,"branch");
		return tipo;
	}
	else if(strcmp(instruccion->op,"j")==0||strcmp(instruccion->op,"jal")==0||strcmp(instruccion->op,"jr")==0)
	{
		strcpy(tipo,"jump");
		return tipo;
	}
	else if(strcmp(instruccion->op,"sw")==0)
	{
		strcpy(tipo,"store");
		return tipo;
	}
	else if(strcmp(instruccion->op,"lw")==0)
	{
		strcpy(tipo,"load");
		return tipo;
	}
	else
	{
		strcpy(tipo,"NOP");
		return tipo;
	}
}
//EXECUTE INSTRUCTION
int executeInstruction(Instruccion* instruccion, SetRegistros* regSet,Programa* programa)
{
	int resultado;
	resultado = alu(instruccion,regSet,programa);
	instruccion->valor = resultado;
	return resultado;
}
//MEM
int memoryAccess(Instruccion* in,SetRegistros* regSet)
{
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
 	char* funct = (char*)malloc(sizeof(char)*8);
	//int constante;
	int posicion;
	strcpy(funct, in->op);
 	if(strcmp(funct,"sw")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		posicion = in->constante/4;
		stackPointer[posicion] = rt->valor;
		return 1;
	}
	else if(strcmp(funct,"lw")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		posicion = in->constante/4;
		rt->valor = stackPointer[posicion];
		return 1;
	}
	return 0;
}
//WRITE BACK
void writeBack(Instruccion* instruccion, SetRegistros* regSet)
{
	//Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	char* tipo = (char*)malloc(sizeof(char)*8);
	tipo = instructionDecode(instruccion);
	if(strcmp(tipo,"AI")==0)
	{
		rt = buscarRegistro(regSet, instruccion->rt);
		rt->valor = instruccion->valor;
	}
	else if(strcmp(tipo,"AR")==0)
	{
		rd = buscarRegistro(regSet, instruccion->rd);
		rd->valor = instruccion->valor;
	}
}
//------------------------------------------------------
//------------------------------------------------------
//FUNCIONES DE DETECCION DE RIESGOS Y ESTRUCTURA RIESGO
//HAZARD DETECT UNIT
Riesgo* unidadDeteccionRiesgos(SetRiesgos* riesgos,SetRegistros* regSet,Instruccion* pipeline, int linea, int CC)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	char* tipoEX_MEM = (char*)malloc(sizeof(char)*8);
	char* tipoMEM_WB = (char*)malloc(sizeof(char)*8);
	char* tipoID_EX = (char*)malloc(sizeof(char)*8);
	tipoEX_MEM = instructionDecode(&pipeline[1]);
	tipoMEM_WB = instructionDecode(&pipeline[2]);
	tipoID_EX = instructionDecode(&pipeline[0]);
	if(strcmp(tipoID_EX,"AI")==0 || strcmp(tipoID_EX,"AR")==0)
	{
		riesgo = MEMHazard(&pipeline[2],&pipeline[0],regSet);
		if(strcmp(riesgo->nombre,"")!=0)
		{
			riesgo->ciclo = CC;
			riesgo->linea = linea;
		}
		if(strcmp(riesgo->nombre,"")!=0)
		{
			insertarRiesgo(riesgos,riesgo);
		}
	}
	if(strcmp(tipoEX_MEM,"AI")==0 || strcmp(tipoEX_MEM,"AR")==0)
	{
		riesgo = EXHazard(&pipeline[1],&pipeline[0],regSet);
		if(strcmp(riesgo->nombre,"")!=0)
		{
			riesgo->ciclo = CC;
			riesgo->linea = linea;
		}
		if(strcmp(riesgo->nombre,"")!=0)
		{
			insertarRiesgo(riesgos,riesgo);
		}
		
	}
	return riesgo;
}
//DETECCION FORWARDING
Riesgo* EXHazard(Instruccion* insEX_MEM, Instruccion* insID_EX,SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	if((strcmp(insEX_MEM->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
	{
		//printf("%s\n","--------------------------------------------------------" );
		rd = buscarRegistro(regSet,insEX_MEM->rd);
		rs = buscarRegistro(regSet,insID_EX->rs);
		rt = buscarRegistro(regSet,insID_EX->rt);
		if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"EX_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
	
		}
		else if((strcmp(rd->nombre, rt->nombre) == 0)&& strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"EX_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
		}
	}
	return riesgo;

}
Riesgo* MEMHazard(Instruccion* insMEM_WB, Instruccion* insID_EX,SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	rd = buscarRegistro(regSet,insMEM_WB->rd);
	rs = buscarRegistro(regSet,insID_EX->rs);
	rt = buscarRegistro(regSet,insID_EX->rt);
	if((strcmp(insMEM_WB->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
	{
		if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			//printf("%s\n","-----------------------------------------------" );
			strcpy(riesgo->nombre,"MEM_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
	
		}
		else if((strcmp(rd->nombre, rt->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"MEM_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
		}
	}
	return riesgo;
}
Riesgo* LoadHazard(Instruccion* insID_EX, Instruccion* insIF_ID, SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	rd = buscarRegistro(regSet,insID_EX->rd);
	rs = buscarRegistro(regSet,insIF_ID->rs);
	rt = buscarRegistro(regSet,insIF_ID->rt);
	if((strcmp(insIF_ID->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
	{
		if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"MEM_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
	
		}
		else if((strcmp(rd->nombre, rt->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"MEM_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
		}
	}
	return riesgo;
}
void imprimirRiesgo(Riesgo* riesgo)
{
	printf("%s Registro: %s %s %d %s %d \n",riesgo->nombre,riesgo->registro,"linea: ",riesgo->linea, "ciclo: ", riesgo->ciclo);
}
void imprimirRiesgos(SetRiesgos* riesgos)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	for(int i = 0; i < riesgos->largo; i++)
	{
		riesgo = obtenerRiesgo(riesgos,i);
		imprimirRiesgo(riesgo);
	}
}
void insertarRiesgo(SetRiesgos* riesgos, Riesgo* riesgo)
{
	if(riesgos->cabeza==NULL){
		riesgos->cabeza = riesgo;
		riesgos->largo++;
	}
	else{
		Riesgo* puntero = riesgos->cabeza;
		while(puntero->siguiente){
			puntero = puntero->siguiente;
		}
		puntero->siguiente=riesgo;
		riesgos->largo++;
	}
}
Riesgo* obtenerRiesgo(SetRiesgos* riesgos,int n){
	if(riesgos->cabeza==NULL){
		return NULL;
	}else{
		Riesgo* pointer = riesgos->cabeza;
		int position = 0;
		while(position < n && pointer->siguiente){
			pointer = pointer->siguiente;
			position++;
		}
		if(position!=n){
			return NULL;//no tengo tantas cartas
		}
		return pointer;
	}
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//FUNCIONES PARA LA ESTRUCTURA INSTRUCTION
//Inserta una instruccion a la lista de instrucciones
//Entrada: inSet = lista de instrucciones, in = instruccion
//Salida:
void insertarInstruccion(SetInstrucciones* inSet,Instruccion* in)
{
	in->siguiente = inSet->cabeza;
	inSet->cabeza = in;
	inSet->largo++;
}
void insertarInstruccion2(SetInstrucciones* inSet,char** linea)
{
	Instruccion* in  = crearInstruccion(linea);
	in->siguiente = inSet->cabeza;
	inSet->cabeza = in;
	inSet->largo++;
}
//Obtiene una instruccion de una lista de instrucciones
//Entrada: n = indice del valor que se quiere obtener, inSet = lista de instrucciones
//Salida: pointer = instruccion
Instruccion* obtenerInstruccion(int n,SetInstrucciones* inSet)
{
	if(inSet->cabeza==NULL){
		return NULL;
	}else{
		Instruccion* pointer = inSet->cabeza;
		int position = 0;
		while(position < n && pointer->siguiente){
			pointer = pointer->siguiente;
			position++;
		}
		if(position!=n){
			return NULL;//no tengo tantas cartas
		}
		return pointer;
	}
}
//IMPRIME LAS INSTRUCCIONES
void imprimirInstrucciones(SetInstrucciones* inSet)
{
	Instruccion* instruction = (Instruccion*)malloc(sizeof(Instruccion));
	for(int i = 0; i < inSet->largo; i++)
	{
		instruction = obtenerInstruccion(i,inSet);
		printf("Funct: %s ",instruction->op);
		printf("RT: %s ",instruction->rt);
		printf("RS: %s ",instruction->rs);
		printf("RD: %s ",instruction->rd);
		printf("Inmediate: %d ",instruction->constante);
		printf("Etiqueta: %s\n",instruction->etiqueta);

	}	
}
void imprimirInstrucciones2(SetInstrucciones* inSet)
{
	Instruccion* instruction = (Instruccion*)malloc(sizeof(Instruccion));
	for(int i = 0; i < inSet->largo; i++)
	{
		instruction = obtenerInstruccion(i,inSet);
		if(strcmp(instruction->op,"sw")==0)
		{
			printf("%s ",instruction->op);
			//Cambio de rt -> rd
			printf("%s ",instruction->rd);
			printf("%d(",instruction->constante);
			printf("%s)\n",instruction->rs);
		}
		if(strcmp(instruction->op,"addi")==0||strcmp(instruction->op,"subi")==0)
		{
			printf("%s ",instruction->op);
			//Cambio de rt -> rd
			printf("%s ",instruction->rd);
			printf("%s ",instruction->rs);
			printf("%d\n",instruction->constante);
		}

	}	
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//FUNCIONES PARA LA ESTRUCTURA REGISTRO
//Crea un registro a partir del numbre y numero
//Entrada: nombreRegistro = string con el nombre del registro, numero = numero de registro
//Salida: reg = registro
Registro* crearRegistro(char* nombreRegistro, int numero){
	Registro* reg = (Registro*)malloc(sizeof(Registro));
	reg->nombre = (char*)malloc(sizeof(char)*5);
	strcpy(reg->nombre,nombreRegistro);
	reg->numero = numero;
	reg->valor = 0;
	reg->siguiente = NULL;
	return reg;
}

void insertarRegistro(SetRegistros* regSet,char* nombreRegistro, int numero){
	Registro* nodo = crearRegistro(nombreRegistro,numero);
	nodo->siguiente = regSet->cabeza;
	regSet->cabeza = nodo;
	regSet->largo++;

}
//Obtiene un registro de una lista de registros
//Entrada: n = indice del registro que se quiere obtener, regSet = lista de registros
//Salida: Registro* pointer = registro obtenido
Registro* obtenerRegistro(int n,SetRegistros* regSet){
	if(regSet->cabeza==NULL){
		return NULL;
	}else{
		Registro* pointer = regSet->cabeza;
		int position = 0;
		while(position < n && pointer->siguiente){
			pointer = pointer->siguiente;
			position++;
		}
		if(position!=n){
			return NULL;//no tengo tantas cartas
		}
		return pointer;
	}
}

//Busca un registro por nombre en una lista de registros
//Entrada: regSet = lista de registro, nombreRegistro = nombre del registro a buscar
//Salida: Registro* reg = reg
Registro* buscarRegistro(SetRegistros* regSet, char* nombreRegistro)
{
	Registro* reg = (Registro*)malloc(sizeof(Registro));
	for(int i = 0; i < regSet->largo; i++)
	{
		reg = obtenerRegistro(i,regSet);
		if(strcmp(reg->nombre,nombreRegistro)==0)
		{
			return reg;
		}
	}
	return NULL;
}
//Funcion para iniciar los registros
//Retorna una lista enlazada de registros iniciados con valor 0
SetRegistros* inicializarRegistros()
{
	SetRegistros* registers = (SetRegistros*)malloc(sizeof(SetRegistros));
	char** registersName = (char**)malloc(sizeof(char*)*31);
	for(int i = 0; i < 31; i++)
	{
		registersName[i] = (char*)malloc(sizeof(char*)*5);
	}
	strcpy(registersName[0],"$zero");
	strcpy(registersName[1],"$at");
	strcpy(registersName[2],"$v0");
	strcpy(registersName[3],"$v1");
	strcpy(registersName[4],"$a0");
	strcpy(registersName[5],"$a1");
	strcpy(registersName[6],"$a2");
	strcpy(registersName[7],"$a3");
	strcpy(registersName[8],"$t0");
	strcpy(registersName[9],"$t1");
	strcpy(registersName[10],"$t2");
	strcpy(registersName[11],"$t3");
	strcpy(registersName[12],"$t4");
	strcpy(registersName[13],"$t5");
	strcpy(registersName[14],"$t6");
	strcpy(registersName[15],"$t7");
	strcpy(registersName[16],"$s0");
	strcpy(registersName[17],"$s1");
	strcpy(registersName[18],"$s2");
	strcpy(registersName[19],"$s3");
	strcpy(registersName[20],"$s4");
	strcpy(registersName[21],"$s5");
	strcpy(registersName[22],"$s6");
	strcpy(registersName[23],"$s7");
	strcpy(registersName[24],"$t8");
	strcpy(registersName[25],"$t9");
	strcpy(registersName[26],"$k0");
	strcpy(registersName[27],"$k1");
	strcpy(registersName[29],"$sp");

	for(int i = 0; i < 30; i++)
	{
		if(strcmp(registersName[29-i],"")!=0)
		{
			insertarRegistro(registers,registersName[i],i);
		}
	}
	return registers;
}
//IMPRIME LOS REGISTROS
void imprimirRegistros(SetRegistros* registers)
{	
	Registro* rg = (Registro*)malloc(sizeof(Registro));
	for(int i = 0; i < 29; i++)
	{
		rg = obtenerRegistro(i,registers);
		printf("%s ",rg->nombre );
		printf("%d\n",rg->valor );
	}
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//CARGA DE ARCHIVOS
//Funcion encargada de cargar un programa desde un archivo de texto
//Parametros: un string que contiene el pathFile
//Retorna una matriz de caracteres con el programa cargado y sin comentarios
Programa* cargarPrograma(char* pathFile){
	FILE *instructionFile;
	instructionFile = fopen(pathFile,"rb");
	if(instructionFile != NULL)
	{
		Programa *instruction = (Programa*)malloc(sizeof(Programa));
		Programa *instructionClean = (Programa*)malloc(sizeof(Programa));  
		//Abrir archivos
		if(instructionFile == NULL)
		{
			printf("\nError al abrir el archivo");
		}
		//Variables para contar el numero de lineas de un archivo
		char line[200];
		int lineCounter = 0;
		while (fgets(line, 200, instructionFile) != NULL)
		{
			if(line[0] != '#')
			{
				lineCounter++;
			}
			
		}
		//MATRIZ ALMACENADORA de lineas de instruccion
		instruction->matrizInstrucciones = (char**)malloc(sizeof(char*)*lineCounter);
		instruction->largo = lineCounter;
		for(int i = 0; i < lineCounter; i++)
		{
			instruction->matrizInstrucciones[i] = (char*)malloc(sizeof(char)*200);
		}
		rewind(instructionFile);
	
		int a = 0;
		while (fgets(line, 200, instructionFile) != NULL)
		{	
			if(line[0] != '#')
			{
				strcpy(instruction->matrizInstrucciones[a],line);
				a++;
			}
   			
	
		}
		//Borrar comentarios de columna
		instructionClean = elminarComentarios(instruction);
		fclose(instructionFile);
		return instruction;
	}
	return NULL;
}
//Funcion encargada de eliminar los comentarios adyacentes en un programa
//Entra una estructura Program con el programa pre-cargado
//Retorna el programa sin comentarios adyacentes
Programa* elminarComentarios(Programa* inst)
{
	char delimit[] = "#";
	int lineCounter = inst->largo;
	Programa *instruction = (Programa*)malloc(sizeof(Programa));
	instruction->matrizInstrucciones = (char**)malloc(sizeof(char*)*lineCounter);
	instruction->largo = lineCounter;
	for(int i = 0; i < lineCounter; i++)
	{
		instruction->matrizInstrucciones[i] = (char*)malloc(sizeof(char)*200);
	}
	char* word = (char*)malloc(sizeof(char)*200);
	for(int i = 0; i < lineCounter; i++)
	{
		word = strtok(inst->matrizInstrucciones[i],delimit);
		strcpy(instruction->matrizInstrucciones[i],word);
	}
	return instruction;
}
SetInstrucciones* cargarProgramaAMemoria(Programa* programa)
{
	SetInstrucciones* inSetInvertido = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	//SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	//Instruccion* instruccionAux = (Instruccion*)malloc(sizeof(Instruccion));
	char** part = (char**)malloc(sizeof(char*)*5);
	for(int i = programa->largo-1; i >= 0; i--)
	{
		//FETCH
		part = makeLine(programa->matrizInstrucciones[i]);
		//ID
		instruccion = crearInstruccion(part);
		if(strcmp(instruccion->op,"Etiqueta")!=0)
		{
			insertarInstruccion(inSetInvertido,instruccion);
		}
		//EX MEM AND WB	
	}
	printf("%d\n",programa->largo);
	printf("%d\n",inSetInvertido->largo);

	return inSetInvertido;
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//Funcion encargada de separar y crear un nodo Instruction
//Entrada: una linea de instrucción
//Salida: Un nodo Instruction
char** makeLine(char* instructionLine)
{
	//Contador de las señales de control
	//Asignaciones de memoria
	char** part = (char**)malloc(sizeof(char*)*4);
	for(int i = 0;i < 4; i++)
	{
		part[i] = (char*)malloc(sizeof(char)*8);
	}
	char* line = (char*)malloc(sizeof(char)*8);
	//Procesamiento de string (separar instrucciones)
	int largo = strlen(instructionLine);
	//printf("%d\n", largo);
	if(instructionLine[largo-2]==':')
	{
		//printf("%s\n", "HERERERERERERERER");
		strcpy(part[0],instructionLine);
		return part;
	}
	line = strtok(instructionLine," ");
	strcpy(part[0],line);
	int i = 1;
	while ((line = strtok(NULL," ")) != NULL)
	{
		//Limpiando el string
		removeChar(line,',');
		part[i] = line;
		i++;				
	}
    return part;
}
//Funcion que resive un arreglo de strings con el propósito de retornar un puntero a Instruction con el formato
//de cada instruccion
//ENTRADA: programLine -> arreglo de strings que contiene una instruccion, ctrlSignal -> para sumar 1 al instruction ID
//SALIDA: puntero a Instruction en un formato de estructura con la instruccion ingresada por programLine.
Instruccion* crearInstruccion(char** programLine)
{
	//Contador de las señales de control
	//Asignaciones de memoria
	Instruccion* instruction = (Instruccion*)malloc(sizeof(Instruccion));
	char* funct = (char*)malloc(sizeof(char)*8);
	char* auxRegister = (char*)malloc(sizeof(char)*8);
	instruction->op = (char*)malloc(sizeof(char)*8);
	instruction->rs = (char*)malloc(sizeof(char)*8);
	instruction->rt = (char*)malloc(sizeof(char)*8);
	instruction->rd = (char*)malloc(sizeof(char)*8);
	instruction->etiqueta = (char*)malloc(sizeof(char)*64);
	instruction->siguiente = NULL;
	strcpy(funct,programLine[0]);
	int largo = strlen(funct);
	int largo2;
	//printf("%s %c\n","Ultimo caracter: ",funct[largo-1]);
	if(strcmp(funct,"addi")==0||strcmp(funct,"subi")==0)
	{
		strcpy(instruction->op,funct);
		strcpy(instruction->rs,programLine[2]);
		//cambio de rt-> rd
		strcpy(instruction->rd,programLine[1]);
		char *ptr;
		instruction->constante = strtol(programLine[3],&ptr,10);

	}
	else if(strcmp(funct,"add")==0||strcmp(funct,"sub")==0)
	{
		strcpy(instruction->op,funct);
		strcpy(instruction->rs,programLine[2]);
		strcpy(instruction->rt,programLine[1]);
		strcpy(instruction->rd,programLine[3]);
	}
	else if(strcmp(funct,"sw")==0)
	{
		strcpy(instruction->op, funct);
		//Cambio de rt-> rd
		strcpy(instruction->rd,programLine[1]);
		//Almacenar la  valor inmediato o constantee
		char *ptr;
		//Limpiar el registro, separar constantee y rs
		auxRegister = strtok(programLine[2],"(");
		instruction->constante = strtol(auxRegister,&ptr,10);
		auxRegister = strtok(NULL,")");
		strcpy(instruction->rs, auxRegister);
	}
	else if(strcmp(funct,"lw")==0)
	{
		strcpy(instruction->op, funct);
		//Cambio de rt-> rd
		strcpy(instruction->rd, programLine[1]);
		//Almacenar la  valor inmediato o constantee
		char *ptr;
		//Limpiar el registro, separar constantee y rs
		auxRegister = strtok(programLine[2],"(");
		instruction->constante = strtol(auxRegister,&ptr,10);
		auxRegister = strtok(NULL,")");
		strcpy(instruction->rs, auxRegister);
	}
	else if(strcmp(funct,"beq")==0)
	{
		strcpy(instruction->op, funct);
		strcpy(instruction->rs,programLine[2]);
		strcpy(instruction->rt,programLine[1]);
		strcpy(instruction->etiqueta,programLine[3]);	
		/*largo2 = strlen(programLine[1]);
		printf("LARGO: %d\n",largo2);
		//VERSION FINAL PUEDE QUE NO SE DEJe usar strcpy
		strncpy(instruction->etiqueta, programLine[1],largo2-1);*/
	}
	else if(strcmp(funct,"j")==0)
	{
		strcpy(instruction->op, funct);	
		largo2 = strlen(programLine[1]);
		printf("LARGO: %d\n",largo2);
		//VERSION FINAL PUEDE QUE NO SE DEJe usar strcpy
		strncpy(instruction->etiqueta, programLine[1],largo2-1);
	}
	else if(funct[largo-2]==':')
	{
		//printf("%s\n","DDDDDDDDD" );
		strcpy(instruction->op,"Etiqueta");
		strcpy(instruction->etiqueta, funct);
	}
	return instruction;
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//FUNCIONES PARA MANEJO DE STRINGS
//Funcion encargada de eliminar un caracter de un string
//ENTRADA: str -> string, garbage -> caracter a ser eliminado
void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
//Funcion para concatenar un caracter a un string
//ENTRADA: str -> string, c -> caracter a ser concatenado a str
void concatChar(char* str, char c)
{
	size_t len = strlen(str);
	str[len] = c;
    str[len + 1] = '\0';
}
#endif