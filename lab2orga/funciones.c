#ifndef funciones_c
#define funciones_c
#include "cabeza.h"
//PIPELINE
void pipeline()
{
	//SetRegistros* regSet = (SetRegistros*)malloc(sizeof(SetRegistros));
	//regSet = inicializarRegistros();
	//imprimirRegistros(regSet);
	SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	Programa* programa = (Programa*)malloc(sizeof(Programa));
	programa = cargarPrograma("jugada1.txt");
	inSet = cargarProgramaAMemoria(programa);
	//imprimirInstrucciones(inSet);
	Instruccion* pipeline = (Instruccion*)malloc(sizeof(Instruccion)*4);
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	instruccion->rs = (char*)malloc(sizeof(char)*8);
	instruccion->rt = (char*)malloc(sizeof(char)*8);
	instruccion->rd = (char*)malloc(sizeof(char)*8);
	for(int i = 0; i<4; i++)
	{
			pipeline[i].op = (char*)malloc(sizeof(char)*8);
			pipeline[i].rs = (char*)malloc(sizeof(char)*8);
			pipeline[i].rt = (char*)malloc(sizeof(char)*8);
			pipeline[i].rd = (char*)malloc(sizeof(char)*8);
	}
	for(int i = 0; i<4; i++)
	{
		strcpy(pipeline[i].op,"NOP");
	}
	//-------------------------------------
	int direccion = 0;
	int ciclosReloj = 1;
	strcpy(instruccion->op,"NOP");
	while(pipelineVacio(pipeline) != 1|| direccion<inSet->largo)
	{
		avanzarInstrucciones(pipeline,instruccion);
		instruccion = instructionFetch(inSet,direccion);
		if(ciclosReloj<10)
		{
			printf("%d)  ",ciclosReloj);
		}
		if(ciclosReloj>=10)
		{
			printf("%d) ",ciclosReloj);
		}
		imprimirUnaInstruccion(instruccion);
		printf(" ");
		printEtapasPL(pipeline);
		printf("\n");
		direccion++;
		ciclosReloj++;
	}

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

void avanzarInstrucciones(Instruccion* pipeline,Instruccion* instruccion)
{
	pipeline[3] = pipeline[2];
	pipeline[2] = pipeline[1];
	pipeline[1] = pipeline[0];
	pipeline[0] = *instruccion;
}
//Imprimir las multiples etapas
void printEtapasPL(Instruccion* pipeline)
{
	for(int i = 0; i<4;i++)
	{
		if(strcmp(pipeline[i].op,"NOP")==0)
		{
			/*printf("%s ",pipeline->op);
			printf("%s ",pipeline->rt);
			printf("%d(",pipeline->constante);
			printf("%s)\n",pipeline->rs);*/
			printf("%s ","------ NOP -----");
		}
		if(strcmp(pipeline[i].op,"sw")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s ",pipeline[i].rt);
			printf("%d(",pipeline[i].constante);
			printf("%s)    ",pipeline[i].rs);
		}
		if(strcmp(pipeline[i].op,"addi")==0||strcmp(pipeline[i].op,"subi")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s ",pipeline[i].rt);
			printf("%s ",pipeline[i].rs);
			printf("%d ",pipeline[i].constante);
		}
	}
}
//Imprimir una instrucción simple
void imprimirUnaInstruccion(Instruccion* instruccion)
{
	if(strcmp(instruccion->op,"NOP")==0)
		{
			/*printf("%s ",instruccion->o->);
			printf("%s ",instruccion->r->);
			printf("%d(",instruccion->c->nstante);
			printf("%s)\n",instruccion->r->);*/
			printf("%s ","------ NOP -----");
		}
		if(strcmp(instruccion->op,"sw")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rt);
			printf("%d(",instruccion->constante);
			if(instruccion->constante>9){printf("%s)   ",instruccion->rs);}else{printf("%s)    ",instruccion->rs);}
		}
		if(strcmp(instruccion->op,"addi")==0||strcmp(instruccion->op,"subi")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rt);
			printf("%s ",instruccion->rs);
			printf("%d ",instruccion->constante);
		}
}
int pipelineVacio(Instruccion* pipeline)
{
	if(strcmp(pipeline[0].op,"NOP")==0 && strcmp(pipeline[1].op,"NOP")==0 && strcmp(pipeline[2].op,"NOP")==0 && strcmp(pipeline[3].op,"NOP")==0)
	{
		return 1;
	}
	return 0;
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
		printf("Inmediate %d\n",instruction->constante);

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
			printf("%s ",instruction->rt);
			printf("%d(",instruction->constante);
			printf("%s)\n",instruction->rs);
		}
		if(strcmp(instruction->op,"addi")==0||strcmp(instruction->op,"subi")==0)
		{
			printf("%s ",instruction->op);
			printf("%s ",instruction->rt);
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
/*
//Funcion para insertar al final de las instrucciones
void insertFinal(SetRegistros* regSet, char* nombreRegistro, int numero){
    Registro* nodo = crearRegistro(nombreRegistro, numero);
    if(regSet->cabeza==NULL){
        regSet->cabeza = nodo;
    }
    else{
        Registro* pointer = regSet->cabeza;
        while(pointer->siguiente){
            pointer = pointer->siguiente;
        }
        pointer->siguiente=nodo;
        regSet->largo++;
    }
    //ahora apunta al ultimo    
}
//Inserta un registro al set de registros
//Entrada: regSet = set de registros, nombreRegistro = nombre de registro, numero = numero de registro
*/
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
	SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	Instruccion* instruccionAux = (Instruccion*)malloc(sizeof(Instruccion));
	char** part = (char**)malloc(sizeof(char*)*5);
	for(int i = programa->largo-1; i >= 0; i--)
	{
		//FETCH
		part = makeLine(programa->matrizInstrucciones[i]);
		//ID
		instruccion = crearInstruccion(part);
		insertarInstruccion(inSetInvertido,instruccion);
		//EX MEM AND WB	
	}
	printf("%d\n",programa->largo);
	printf("%d\n",inSetInvertido->largo);
	/*for(int i = 0; i < inSetInvertido->largo -1; i++)
	{
		//FETCH				
		instruccionAux = obtenerInstruccion(i,inSetInvertido);
		//instruccionAux->siguiente = NULL;
		printf("Funct: %s ",instruccionAux->op);
		printf("RT: %s ",instruccionAux->rt);
		printf("RS: %s ",instruccionAux->rs);
		printf("RD: %s ",instruccionAux->rd);
		printf("Inmediate %d\n",instruccionAux->constante);
		//ID
		//printf("%s: %d\n", "ciclo", i+1);
		//insertarInstruccion(inSet,instruccionAux);										
		//EX MEM AND WB	
	}*/
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
	instruction->siguiente = NULL;
	strcpy(funct,programLine[0]);
	if(strcmp(funct,"addi")==0||strcmp(funct,"subi")==0)
	{
		strcpy(instruction->op,funct);
		strcpy(instruction->rs,programLine[2]);
		strcpy(instruction->rt,programLine[1]);
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
		strcpy(instruction->rt,programLine[1]);
		//Almacenar la  valor inmediato o constantee
		char *ptr;
		//Limpiar el registro, separar constantee y rs
		auxRegister = strtok(programLine[2],"(");
		instruction->constante = strtol(auxRegister,&ptr,10);
		auxRegister = strtok(NULL,")");
		strcpy(instruction->rs, auxRegister);
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