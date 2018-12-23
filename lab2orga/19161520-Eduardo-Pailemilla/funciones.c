#ifndef funciones_c
#define funciones_c
#include "cabeza.h"
//PIPELINE
//Funcion encargada de ejecutar el pipeline de instrucciones
//ENTRADA: ruta: Ruta que contiene la ubicacion del archivo a ejecutar
void pipeline(Programa* programa)
{
	stackPointer = (int*)malloc(sizeof(int)*256);
	Riesgo* riesgo= (Riesgo*)malloc(sizeof(Riesgo));
	Riesgo* riesgoLoad = (Riesgo*)malloc(sizeof(Riesgo));
	SetRiesgos* riesgos = (SetRiesgos*)malloc(sizeof(SetRiesgos));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	riesgo->tipo = (char*)malloc(sizeof(char)*16);
	riesgoLoad->nombre = (char*)malloc(sizeof(char)*16);
	riesgoLoad->registro = (char*)malloc(sizeof(char)*16);
	riesgoLoad->tipo = (char*)malloc(sizeof(char)*16);
	riesgos->cabeza = NULL;
	riesgos->largo = 0;
	SetRegistros* regSet = (SetRegistros*)malloc(sizeof(SetRegistros));
	SetInstrucciones* inSet = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	//Programa* programa = (Programa*)malloc(sizeof(Programa));

	regSet = inicializarRegistros();
	inSet = cargarProgramaAMemoria(programa);

	Instruccion* pipeline = (Instruccion*)malloc(sizeof(Instruccion)*4);
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	instruccion->rs = (char*)malloc(sizeof(char)*8);
	instruccion->rt = (char*)malloc(sizeof(char)*8);
	instruccion->rd = (char*)malloc(sizeof(char)*8);
	instruccion->etiqueta = (char*)malloc(sizeof(char)*64);

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
	int* direccion;
	direccion = (int*)malloc(sizeof(int));
	*direccion = 0;
	int ciclosReloj = 1;
	strcpy(instruccion->op,"NOP");
	char* funct = (char*)malloc(sizeof(char)*8);
	char* functMEM = (char*)malloc(sizeof(char)*8);
	int resultado = 0;
	int jumpDireccion = 0;
	int branchDireccion = 0;
	while(pipelineVacio(pipeline) != 1|| *direccion<inSet->largo)
	{	
		//----------------------------------------------------
		if(strcmp(funct,"jump")==0)
		{
			jumpDireccion = alu(&pipeline[0],regSet,programa);
			*direccion = jumpDireccion;
			ponerInstruccionNopJump(pipeline);
			instruccion = instructionFetch(inSet,direccion);
		}
		else if(strcmp(functMEM,"branch")==0)
		{
			branchDireccion = alu(&pipeline[2],regSet,programa);
			if(branchDireccion != -1)
			{
				*direccion = branchDireccion;
				ponerInstruccionNopJump(pipeline);
				flush(pipeline);
				instruccion = instructionFetch(inSet,direccion);
			}
			else if(strcmp(riesgoLoad->nombre,"LOAD_HAZARD")==0 || strcmp(riesgoLoad->nombre,"LOAD_HAZARD_A")==0 || strcmp(riesgoLoad->nombre,"LOAD_HAZARD_B")==0)
			{

				ponerInstruccionNopJump(pipeline);
				*direccion = *direccion - 1;
				instruccion = instructionFetch(inSet,direccion);
				strcpy(riesgoLoad->nombre,"");
			}
			else
			{
				avanzarInstrucciones(pipeline,instruccion);

				instruccion = instructionFetch(inSet,direccion);
			}
		}
		else if(strcmp(riesgoLoad->nombre,"LOAD_HAZARD")==0 ||strcmp(riesgoLoad->nombre,"LOAD_HAZARD_A")==0 || strcmp(riesgoLoad->nombre,"LOAD_HAZARD_B")==0)
		{

			ponerInstruccionNopJump(pipeline);
			*direccion = *direccion - 1;
			instruccion = instructionFetch(inSet,direccion);
			strcpy(riesgoLoad->nombre,"");
		}
		else
		{
			avanzarInstrucciones(pipeline,instruccion);
			instruccion = instructionFetch(inSet,direccion);
		}
		//----------------------------------------------------
		riesgoLoad = LoadHazard(&pipeline[0],instruccion,regSet);
		deteccionRiesgosControl(programa,pipeline,riesgos,regSet,*direccion,ciclosReloj);
		riesgo = unidadDeteccionRiesgos(instruccion,riesgos,regSet,pipeline,ciclosReloj);
		funct = instructionDecode(&pipeline[0]);
		functMEM = instructionDecode(&pipeline[2]);
		forwarding(&pipeline,pipeline,regSet,programa,riesgo);
		writeBack(&pipeline[3], regSet);
		resultado = executeInstruction(&pipeline[1],regSet,programa);
		memoryAccess(&pipeline[2],regSet);
		if(ciclosReloj<10)
		{
			printf("%d)  ",ciclosReloj);
			printf("%d)  ",*direccion);
		}
		if(ciclosReloj>=10)
		{
			printf("%d) ",ciclosReloj);
			printf("%d)  ",*direccion);
		}
		printf("  %s |", funct);
		printf("%  d |",resultado);
		imprimirUnaInstruccion(instruccion);
		printf(" ");
		printEtapasPL(pipeline);
		printf("\n");
		*direccion = *direccion + 1;
		ciclosReloj++;
		//SALTOS DEL PIPELINE
	}
	
	imprimirRiesgosArchivo(riesgos);
	imprimirSoluciones(riesgos);
	imprimirRegistros(regSet);
	imprimirInstrucciones(inSet);

}
//FUNCION PARA IMPRIMIR LOS VALORES DEL STACK POINTER
void imprimirStack(FILE* f)
{
	fprintf(f,"%s ", "$sp");
	for(int i = 0; i < 10 ; i++)
	{
		fprintf(f,"%d ", stackPointer[i]);
		
	}
	printf("\n");
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
//Funcion que va hacia el set de instrucciones y busca una de ellas bajo el parametro de direccion
//ENTRADA: struct: SetInstruccones -> contiene las instrucciones cargadas, Int* direccionPC: Contiene la direccion de memoria
//SALIDA: Instruccion requerida, o un nop en caso de no encontrarla.
Instruccion* instructionFetch(SetInstrucciones* memoria, int* direccionPC)
{
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(instruccion));
	instruccion->op = (char*)malloc(sizeof(char)*8);
	if(*direccionPC<memoria->largo)
	{
		instruccion = obtenerInstruccion(*direccionPC,memoria);
		if(strcmp(instruccion->op,"ETIQUETA") == 0)
		{
			*direccionPC = *direccionPC + 1;
		}
	}
	if(*direccionPC>=memoria->largo)
	{
		strcpy(instruccion->op,"NOP");
		return instruccion;
	}
	return obtenerInstruccion(*direccionPC,memoria);
}
//Funcion para hacer avanzar las instrucciones en el pipeline
//Entrada: pipeline: Arreglo de instrucciones, instruccion: usada como la proveniente del fetch
void avanzarInstrucciones(Instruccion* pipeline,Instruccion* instruccion)
{
	pipeline[3] = pipeline[2];
	pipeline[2] = pipeline[1];
	pipeline[1] = pipeline[0];
	pipeline[0] = *instruccion;
}
//Funcion para poner una instruccion NOP en el campo ID del pipeline
//ENTRADA: pipeline: Arreglo de instrucciones
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
//Funcion para realizar un flush cuando se haga salto con un branch
//ENTRADA: pipeline: Arreglo de instrucciones
void flush(Instruccion* pipeline)
{
	Instruccion* instruccion1 = (Instruccion*)malloc(sizeof(Instruccion));
	Instruccion* instruccion2 = (Instruccion*)malloc(sizeof(Instruccion));
	Instruccion* instruccion3 = (Instruccion*)malloc(sizeof(Instruccion));
	Instruccion* instruccion4 = (Instruccion*)malloc(sizeof(Instruccion));

	instruccion1->op = (char*)malloc(sizeof(char)*8);
	instruccion1->rs = (char*)malloc(sizeof(char)*8);
	instruccion1->rt = (char*)malloc(sizeof(char)*8);
	instruccion1->rd = (char*)malloc(sizeof(char)*8);
	instruccion1->etiqueta = (char*)malloc(sizeof(char)*64);
	instruccion1->siguiente = NULL;

	instruccion2->op = (char*)malloc(sizeof(char)*8);
	instruccion2->rs = (char*)malloc(sizeof(char)*8);
	instruccion2->rt = (char*)malloc(sizeof(char)*8);
	instruccion2->rd = (char*)malloc(sizeof(char)*8);
	instruccion2->etiqueta = (char*)malloc(sizeof(char)*64);
	instruccion2->siguiente = NULL;

	instruccion3->op = (char*)malloc(sizeof(char)*8);
	instruccion3->rs = (char*)malloc(sizeof(char)*8);
	instruccion3->rt = (char*)malloc(sizeof(char)*8);
	instruccion3->rd = (char*)malloc(sizeof(char)*8);
	instruccion3->etiqueta = (char*)malloc(sizeof(char)*64);
	instruccion3->siguiente = NULL;

	instruccion4->op = (char*)malloc(sizeof(char)*8);
	instruccion4->rs = (char*)malloc(sizeof(char)*8);
	instruccion4->rt = (char*)malloc(sizeof(char)*8);
	instruccion4->rd = (char*)malloc(sizeof(char)*8);
	instruccion4->etiqueta = (char*)malloc(sizeof(char)*64);
	instruccion4->siguiente = NULL;

	strcpy(instruccion1->op,"NOP");
	strcpy(instruccion2->op,"NOP");
	strcpy(instruccion3->op,"NOP");
	strcpy(instruccion4->op,"NOP");

	pipeline[2] = *instruccion3;
	pipeline[1] = *instruccion2;
	pipeline[0] = *instruccion1;
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
		if(strcmp(pipeline[i].op,"addi")==0||strcmp(pipeline[i].op,"subi")==0||strcmp(pipeline[i].op,"addiu")==0)
		{
			printf("%s ",pipeline[i].op);
			//CAMBIO DE RT -> RD
			printf("%s ",pipeline[i].rd);
			printf("%s ",pipeline[i].rs);
			printf("%d   ",pipeline[i].constante);
		}
		if(strcmp(pipeline[i].op,"add")==0||strcmp(pipeline[i].op,"sub")==0||strcmp(pipeline[i].op,"mul")==0||strcmp(pipeline[i].op,"div")==0)
		{
			printf("%s ",pipeline[i].op);
			//CAMBIO DE RT -> RD
			printf("%s ",pipeline[i].rd);
			printf("%s ",pipeline[i].rt);
			printf("%s    ",pipeline[i].rs);
		}
		if(strcmp(pipeline[i].op,"j")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s          ",pipeline[i].etiqueta);
		}
		if(strcmp(pipeline[i].op,"jal")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s          ",pipeline[i].etiqueta);
		}
		if(strcmp(pipeline[i].op,"jr")==0)
		{
			printf("%s ",pipeline[i].op);
			printf("%s           ",pipeline[i].rs);
		}
		if(strcmp(pipeline[i].op,"beq")==0||strcmp(pipeline[i].op,"bne")==0||strcmp(pipeline[i].op,"blt")==0||strcmp(pipeline[i].op,"bgt")==0)
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
		if(strcmp(instruccion->op,"addi")==0||strcmp(instruccion->op,"subi")==0||strcmp(instruccion->op,"addiu")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rd);
			printf("%s ",instruccion->rs);
			printf("%d   ",instruccion->constante);
		}
		if(strcmp(instruccion->op,"add")==0||strcmp(instruccion->op,"sub")==0||strcmp(instruccion->op,"mul")==0||strcmp(instruccion->op,"div")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rd);
			printf("%s ",instruccion->rt);
			printf("%s    ",instruccion->rs);
		}
		if(strcmp(instruccion->op,"j")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s          ",instruccion->etiqueta);
		}
		if(strcmp(instruccion->op,"jal")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s          ",instruccion->etiqueta);
		}
		if(strcmp(instruccion->op,"jr")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s            ",instruccion->rs);
		}
		if(strcmp(instruccion->op,"beq")==0||strcmp(instruccion->op,"bne")==0||strcmp(instruccion->op,"blt")==0||strcmp(instruccion->op,"bgt")==0)
		{
			printf("%s ",instruccion->op);
			printf("%s ",instruccion->rt);
			printf("%s ",instruccion->rs);
			printf("%s ",instruccion->etiqueta);
		}
}
//Retorna 1 o 0 dependiendo si el pipeline esta vacio o no (sin instrucciones que ejecutar)
//ENTRADA: pipeline: Arreglo de instrucciones
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
//ALU unidad aritmetica logica
//Funcion que realiza los calculos a traves de una instruccion y los registros
//ENTRADA: in: instruccion a ejecutar, regSet: set de registros a utilziar, 
//programa: Necesario para saber a que linea saltar
//SALIDA: entero correspondiente a los calculos hechos en funcion a las instrucciones.
int alu(Instruccion* in,SetRegistros* regSet,Programa* programa){
 	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* ra = (Registro*)malloc(sizeof(Registro));
 	char* funct = (char*)malloc(sizeof(char)*8);
	int constante;
	int resultado;
	strcpy(funct, in->op);
 	if(strcmp(funct,"addi")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding + constante;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor + constante;
		return resultado;
	}
	if(strcmp(funct,"addiu")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding + constante;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor + constante;
		return abs(resultado);
	}
	else if(strcmp(funct,"subi")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding - constante;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor - constante;
		return resultado;
	}
	else if(strcmp(funct,"add")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor + in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding + rs->valor;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding + in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor + rt->valor;
		return resultado;
	}
	else if(strcmp(funct,"sub")==0)
	{	
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor - in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding - rs->valor;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding - in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rt->valor - rs->valor;
		return resultado;
	}
	else if(strcmp(funct,"mul")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor * in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding * rs->valor;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding * in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor * rt->valor;
		return resultado;
	}
	else if(strcmp(funct,"div")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor / in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding / rs->valor;
			strcpy(in->hazard,"");
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding / in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		resultado = rs->valor/rt->valor;
		return resultado;
	}
	//CONTROL SIGNAL EX AND MEM 
	else if(strcmp(funct,"sw")==0)
	{
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"lw")==0)
	{
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"j")==0)
	{
		resultado = buscarEtiqueta(in->etiqueta,programa);
		return resultado;
	}
	else if(strcmp(funct,"jal")==0)
	{
		ra = buscarRegistro(regSet, "$ra");
		resultado = buscarEtiqueta(in->etiqueta,programa);
		ra->valor = in->posicionReal;
		return resultado;
	}
	else if(strcmp(funct,"jr")==0)
	{
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding;
			strcpy(in->hazard,"");
			return resultado;
		}
		else
		{
			rs = buscarRegistro(regSet, in->rs);
			resultado = rs->valor;
			return resultado;
		}
		
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
	else if(strcmp(funct,"bne")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor != rs->valor)
		{
			resultado = buscarEtiqueta(in->etiqueta,programa);
		}
		else
		{
			resultado = -1;
		}
		return resultado;
	}
	else if(strcmp(funct,"blt")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor < rs->valor)
		{
			resultado = buscarEtiqueta(in->etiqueta,programa);
		}
		else
		{
			resultado = -1;
		}
		return resultado;
	}
	else if(strcmp(funct,"bgt")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor > rs->valor)
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
//Funcion que realiza los calculos a traves de una instruccion y los registros, Orientada al uso
//De forwardings.
//ENTRADA: in: instruccion a ejecutar, regSet: set de registros a utilziar, 
//programa: Necesario para saber a que linea saltar
//SALIDA: entero correspondiente a los calculos hechos en funcion a las instrucciones.
int aluForwarding(Instruccion* in,SetRegistros* regSet,Programa* programa){
 	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* ra = (Registro*)malloc(sizeof(Registro));
 	char* funct = (char*)malloc(sizeof(char)*8);
	int constante;
	int resultado;
	strcpy(funct, in->op);
 	if(strcmp(funct,"addi")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding + constante;
			return resultado;
		}
		resultado = rs->valor + constante;
		return resultado;
	}
	if(strcmp(funct,"addiu")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding + constante;
			return resultado;
		}
		resultado = rs->valor + constante;
		return abs(resultado);
	}
	else if(strcmp(funct,"subi")==0)
	{
		
		rs = buscarRegistro(regSet, in->rs);
		constante = in->constante;
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding - constante;
			return resultado;
		}
		resultado = rs->valor - constante;
		return resultado;
	}
	else if(strcmp(funct,"add")==0)
	{
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor + in->valorForwarding;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding + rs->valor;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding + in->valorForwarding;
			return resultado;
		}
		resultado = rs->valor + rt->valor;
		return resultado;
	}
	else if(strcmp(funct,"sub")==0)
	{
		
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor - in->valorForwarding;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding - rs->valor;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding - in->valorForwarding;
			return resultado;
		}
		resultado = rt->valor - rs->valor;
		return resultado;
	}
	else if(strcmp(funct,"mul")==0)
	{
		
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor * in->valorForwarding;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding * rs->valor;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding * in->valorForwarding;
			return resultado;
		}
		resultado = rs->valor * rt->valor;
		return resultado;
	}
	else if(strcmp(funct,"div")==0)
	{
		
		rs = buscarRegistro(regSet, in->rs);
		rt = buscarRegistro(regSet, in->rt);
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = rt->valor / in->valorForwarding;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD_B")==0)
		{
			resultado = in->valorForwarding / rs->valor;
			return resultado;
		}
		else if(strcmp(in->hazard,"EX_HAZARD")==0)
		{
			resultado = in->valorForwarding / in->valorForwarding;
			return resultado;
		}
		resultado = rs->valor/rt->valor;
		return resultado;
	}
	
	else if(strcmp(funct,"sw")==0)
	{
		
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"lw")==0)
	{
		
		resultado = in->constante;
		return resultado/4;
	}
	else if(strcmp(funct,"j")==0)
	{

		resultado = buscarEtiqueta(in->etiqueta,programa);

		return resultado;
	}
	else if(strcmp(funct,"jal")==0)
	{

		ra = buscarRegistro(regSet, "$ra");
		resultado = buscarEtiqueta(in->etiqueta,programa);
		ra->valor = in->posicionReal;

		return resultado;
	}
	else if(strcmp(funct,"jr")==0)
	{
		if(strcmp(in->hazard,"EX_HAZARD_A")==0)
		{
			resultado = in->valorForwarding;
			return resultado;
		}
		else
		{
			rs = buscarRegistro(regSet, in->rs);
			resultado = rs->valor;
			return resultado;
		}
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
	else if(strcmp(funct,"bne")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor != rs->valor)
		{
			resultado = buscarEtiqueta(in->etiqueta,programa);
			
		}
		else
		{
			resultado = -1;
		}
		return resultado;
	}
	else if(strcmp(funct,"blt")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor < rs->valor)
		{
			resultado = buscarEtiqueta(in->etiqueta,programa);
			
		}
		else
		{
			resultado = -1;
		}
		return resultado;
	}
	else if(strcmp(funct,"bgt")==0)
	{
		rt = buscarRegistro(regSet, in->rt);
		rs = buscarRegistro(regSet, in->rs);
		if(rt->valor > rs->valor)
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
//Funcion encargada de buscar un etiqueta para realizar un salto
//ENTRADA: etiqueta: String que contiene el nombre de la etiqueta.
//		   Programa: Structura que contiene el programa en una matriz de caracteres.
//Salida: Entero que representa la linea de codigo donde se ubica la etiqueta.
int buscarEtiqueta(char* etiqueta, Programa* programa)
{
	char* etiquitaAux = (char*)malloc(sizeof(char)*64);
	for(int i = 0; i < programa->largo; i++)
	{
		strcpy(etiquitaAux,programa->matrizInstrucciones[i]);
		removeChar(etiquitaAux, '\n');
		removeChar(etiquitaAux, ':');
		if(strcmp(etiqueta,etiquitaAux)==0)
		{
			return i;
		}
		strcpy(etiquitaAux,"");
	}
	return -1;

}

//------------------------------------------------------
//------------------------------------------------------
//ETAPAS DEL PIPELINE
//INSTRUCTION DECODE
//Funcion encargada de retornar a que tipo de instruccion pertenece la que se le pasa por parametro.
//ENTRADA: instruccion: Instruccion a ser decodificada
//Salida: tipo de instruccion
char* instructionDecode(Instruccion* instruccion)
{
	char* tipo = (char*)malloc(sizeof(char)*6);
	if(strcmp(instruccion->op,"addi")==0||strcmp(instruccion->op,"subi")==0||strcmp(instruccion->op,"addiu")==0)
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
	else if(strcmp(instruccion->op,"bne")==0||strcmp(instruccion->op,"bgt")==0||strcmp(instruccion->op,"beq")==0||strcmp(instruccion->op,"blt")==0)
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
//Funcion encargada de llamar a la alu para ejecutar la instruccion
//ENTRADA: instruccion: instruccion a ser ejecutada, regSet: set de registros a usar
//programa: Necesario para saber a que linea saltar en caso de un jump o branch
int executeInstruction(Instruccion* instruccion, SetRegistros* regSet,Programa* programa)
{
	int resultado = 0;
	resultado = alu(instruccion,regSet,programa);
	instruccion->valor = resultado;
	return resultado;
}
//Funcion encargada de realizar un forwarding de ser necesario
//ENTRADA: pipeline: Arreglo de instrucciones, regSet: set de registros a usar
//programa: Necesario para saber a que linea saltar en caso de un jump o branch, riesgo: para conocer si hay riesgos de datos.
void forwarding(Instruccion** pipeline3,Instruccion* pipeline, SetRegistros* regSet,Programa* programa, Riesgo* riesgo)
{
	int resultado = 0;
	if(strcmp(riesgo->nombre,"EX_HAZARD_A")==0 || strcmp(riesgo->nombre,"EX_HAZARD_B")==0)
	{
		resultado = aluForwarding(&pipeline[1],regSet,programa);
		pipeline[0].valorForwarding = resultado;
		strcpy(pipeline[0].hazard,riesgo->nombre);
	}
	else if(strcmp(riesgo->nombre,"MEM_HAZARD_A")==0 || strcmp(riesgo->nombre,"MEM_HAZARD_B")==0)
	{
		resultado = aluForwarding(&pipeline[1],regSet,programa);
		pipeline[0].valorForwarding = resultado;
		strcpy(pipeline[0].hazard,riesgo->nombre);

	}
	else if(strcmp(riesgo->nombre,"LOAD_HAZARD_A")==0 || strcmp(riesgo->nombre,"LOAD_HAZARD_B")==0)
	{
		resultado = aluForwarding(&pipeline[1],regSet,programa);
		pipeline[0].valorForwarding = resultado;
		strcpy(pipeline[0].hazard,riesgo->nombre);
	}

}
//MEM
//Funcion que permite el acceso a memoria para las instrucciones lw y sw
//Entrada: in: Instruccion que realiza un acceso a memoria si las condiciones se cumplen
//Salida: int: 1 o 0 dependiendo si se pudo o no utilizar.
int memoryAccess(Instruccion* in,SetRegistros* regSet)
{
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
 	char* funct = (char*)malloc(sizeof(char)*8);
	int posicion;
	strcpy(funct, in->op);
 	if(strcmp(funct,"sw")==0)
	{
		rt = buscarRegistro(regSet, in->rd);
		rs = buscarRegistro(regSet, in->rs);
		posicion = in->constante/4;
		stackPointer[posicion] = rt->valor;
		return 1;
	}
	else if(strcmp(funct,"lw")==0)
	{
		posicion = in->constante/4;
		in->valor = stackPointer[posicion];
		return 1;
	}
	return 0;
}
//WRITE BACK
//Funcion encargada para escribir en un registro el valor pasado a traves de los "Buffers".
//ENTRADA: instruccion con el registro a ser escrito, regSet: set de registros
void writeBack(Instruccion* instruccion, SetRegistros* regSet)
{
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	char* tipo = (char*)malloc(sizeof(char)*8);
	tipo = instructionDecode(instruccion);
	if(strcmp(tipo,"AI")==0)
	{
		rd = buscarRegistro(regSet, instruccion->rd);
		rd->valor = instruccion->valor;
	}
	else if(strcmp(tipo,"AR")==0)
	{
		rd = buscarRegistro(regSet, instruccion->rd);
		rd->valor = instruccion->valor;
	}
	else if(strcmp(tipo,"load")==0)
	{
		rd = buscarRegistro(regSet, instruccion->rd);
		rd->valor = instruccion->valor;
	}
}
//------------------------------------------------------
//------------------------------------------------------
//FUNCIONES DE DETECCION DE RIESGOS de control Y ESTRUCTURA RIESGO
//ENTDADA: programa: Estructura con una matriz de instrucciones, pipeline: Arrelgo de instrucciones, regSet: ser de registros.
void deteccionRiesgosControl(Programa* programa,Instruccion* pipeline,SetRiesgos* riesgos,SetRegistros* regSet, int linea, int CC)
{
	Riesgo* riesgoJump = (Riesgo*)malloc(sizeof(Riesgo));
	Riesgo* riesgoBranch = (Riesgo*)malloc(sizeof(Riesgo));
	riesgoJump->descripcion = (char*)malloc(sizeof(char)*32);
	riesgoBranch->descripcion = (char*)malloc(sizeof(char)*32);
	riesgoJump->nombre = (char*)malloc(sizeof(char)*16);
	riesgoJump->registro = (char*)malloc(sizeof(char)*16);
	riesgoJump->tipo = (char*)malloc(sizeof(char)*16);
	riesgoBranch->nombre = (char*)malloc(sizeof(char)*16);
	riesgoBranch->tipo = (char*)malloc(sizeof(char)*16);
	//Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	char* functJump = (char*)malloc(sizeof(char)*8);
	char* functBranch = (char*)malloc(sizeof(char)*8);
	functJump = instructionDecode(&pipeline[0]);
	functBranch = instructionDecode(&pipeline[2]);
	if(strcmp(functJump,"jump")==0)
	{
		strcpy(riesgoJump->tipo,"CONTROL");
		strcpy(riesgoJump->nombre,"JUMP");
		strcpy(riesgoJump->descripcion,"Hazard de control");
		riesgoJump->linea = pipeline[0].posicion;
		riesgoJump->ciclo = CC; 
		insertarRiesgo(riesgos, riesgoJump);
	}
	if(strcmp(functBranch,"branch")==0)
	{
		int resultado = alu(&pipeline[2],regSet,programa);
		if(resultado != -1)
		{
			strcpy(riesgoBranch->tipo,"CONTROL");
			strcpy(riesgoBranch->nombre,"BRANCH");
			strcpy(riesgoBranch->descripcion,"Hazard de control");
			riesgoBranch->linea = pipeline[2].posicion;
			riesgoBranch->ciclo = CC; 
			insertarRiesgo(riesgos, riesgoBranch);
		}
	}
}


//Funcion para detectar riesgos de datos:
//ENTDADA: programa: fetch: Instruccion de entrada, riesgos: Set de riesgos, pipeline: Arrelgo de instrucciones, cc: Cilo de reloj.
//SALIDA: riesgo: Contiene el riesgo encontrado.
Riesgo* unidadDeteccionRiesgos(Instruccion* fetch, SetRiesgos* riesgos,SetRegistros* regSet,Instruccion* pipeline, int CC)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	char* tipoEX_MEM = (char*)malloc(sizeof(char)*8);
	char* tipoMEM_WB = (char*)malloc(sizeof(char)*8);
	char* tipoID_EX = (char*)malloc(sizeof(char)*8);
	char* tipoIF_ID = (char*)malloc(sizeof(char)*8);
	tipoEX_MEM = instructionDecode(&pipeline[1]);
	tipoMEM_WB = instructionDecode(&pipeline[2]);
	tipoID_EX = instructionDecode(&pipeline[0]);
	tipoIF_ID = instructionDecode(fetch);
	if(strcmp(tipoMEM_WB,"AI")==0 || strcmp(tipoMEM_WB,"AR")==0)
	{
		riesgo = MEMHazard(&pipeline[2],&pipeline[0],regSet);
		if(strcmp(riesgo->nombre,"")!=0)
		{
			riesgo->ciclo = CC;
			riesgo->linea = pipeline[0].posicion;
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
			riesgo->linea = pipeline[0].posicion;
		}
		if(strcmp(riesgo->nombre,"")!=0)
		{
			insertarRiesgo(riesgos,riesgo);
		}
		
	}
	if(strcmp(tipoID_EX,"load")==0)
	{
		riesgo = LoadHazard(&pipeline[0],fetch,regSet);
		if(strcmp(riesgo->nombre,"")!=0)
		{
			riesgo->ciclo = CC + 1;
			riesgo->linea = fetch->posicion;

		}
		if(strcmp(riesgo->nombre,"")!=0)
		{
			insertarRiesgo(riesgos,riesgo);
		}
		
	}
	return riesgo;
}
//DETECCION FORWARDING
//Funcion par detectar un EXHAZARD:
//ENTRADA: Instruccion en la etapa ex_mem e id_ex del pipeline, regSet: set de registros.
//SALIDA: riesgo: Contiene el riesgo encontrado.
Riesgo* EXHazard(Instruccion* insEX_MEM, Instruccion* insID_EX,SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	riesgo->descripcion = (char*)malloc(sizeof(char)*32);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	if((strcmp(insEX_MEM->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
	{
		//printf("%s\n","--------------------------------------------------------" );
		rd = buscarRegistro(regSet,insEX_MEM->rd);
		rs = buscarRegistro(regSet,insID_EX->rs);
		rt = buscarRegistro(regSet,insID_EX->rt);
		if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre, rt->nombre) == 0 && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"EX_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
	
		}
		else if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"EX_HAZARD_A");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
	
		}
		else if((strcmp(rd->nombre, rt->nombre) == 0)&& strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"EX_HAZARD_B");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
		}
	}
	return riesgo;

}
//DETECCION FORWARDING
//Funcion par detectar un MEMHAZARD:
//ENTRADA: Instruccion en la etapa mem_wb e id_ex del pipeline, regSet: set de registros.
//SALIDA: riesgo: Contiene el riesgo encontrado.
Riesgo* MEMHazard(Instruccion* insMEM_WB, Instruccion* insID_EX,SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	riesgo->descripcion = (char*)malloc(sizeof(char)*32);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	if((strcmp(insMEM_WB->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
	{
		rd = buscarRegistro(regSet,insMEM_WB->rd);
		rs = buscarRegistro(regSet,insID_EX->rs);
		rt = buscarRegistro(regSet,insID_EX->rt);
		if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre, rt->nombre) == 0 && strcmp(rd->nombre,"")!=0)
		{
			strcpy(riesgo->nombre,"MEM_HAZARD");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
	
		}
		else if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0 && rd->valor != 0)
		{
			//printf("%s\n","-----------------------------------------------" );
			strcpy(riesgo->nombre,"MEM_HAZARD_A");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
	
		}
		else if((strcmp(rd->nombre, rt->nombre) == 0) && strcmp(rd->nombre,"")!=0 && rd->valor != 0)
		{
			strcpy(riesgo->nombre,"MEM_HAZARD_B");
			strcpy(riesgo->registro,rd->nombre);
			strcpy(riesgo->descripcion,"Hazard de datos");
		}
	}
	return riesgo;
}
//Funcion par detectar un EXHAZARD:
//ENTRADA: Instruccion en la etapa id_ex e if_id del pipeline, regSet: set de registros.
//SALIDA: riesgo: Contiene el riesgo encontrado.
Riesgo* LoadHazard(Instruccion* insID_EX, Instruccion* insIF_ID, SetRegistros* regSet)
{
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	riesgo->nombre = (char*)malloc(sizeof(char)*16);
	riesgo->registro = (char*)malloc(sizeof(char)*16);
	riesgo->descripcion = (char*)malloc(sizeof(char)*32);
	Registro* rs = (Registro*)malloc(sizeof(Registro));
	Registro* rt = (Registro*)malloc(sizeof(Registro));
	Registro* rd = (Registro*)malloc(sizeof(Registro));
	char* tipoID_EX = (char*)malloc(sizeof(char)*8);
	tipoID_EX = instructionDecode(insID_EX);
	if(strcmp(tipoID_EX,"load")==0)
	{
		if((strcmp(insIF_ID->op,"NOP")!=0)&&(strcmp(insID_EX->op,"NOP")!=0))
		{
			rd = buscarRegistro(regSet,insID_EX->rd);
			rs = buscarRegistro(regSet,insIF_ID->rs);
			rt = buscarRegistro(regSet,insIF_ID->rt);
			if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre, rt->nombre) == 0 && strcmp(rd->nombre,"")!=0)
			{
				strcpy(riesgo->nombre,"LOAD_HAZARD");
				strcpy(riesgo->registro,rd->nombre);
				strcpy(riesgo->descripcion,"Hazard de datos");
	
			}
			else if((strcmp(rd->nombre, rs->nombre) == 0) && strcmp(rd->nombre,"")!=0)
			{
				strcpy(riesgo->nombre,"LOAD_HAZARD_A");
				strcpy(riesgo->descripcion,"Hazard de datos");
				strcpy(riesgo->registro,rd->nombre);
				return riesgo;
		
			}
			else if((strcmp(rd->nombre, rt->nombre) == 0) && strcmp(rd->nombre,"")!=0)
			{
				strcpy(riesgo->nombre,"LOAD_HAZARD_B");
				strcpy(riesgo->descripcion,"Hazard de datos");
				strcpy(riesgo->registro,rd->nombre);
				return riesgo;
			}
			else
			{
				strcpy(riesgo->nombre,"");
			}
		}
	}
	return riesgo;
}

void imprimirRiesgoArchivo(Riesgo* riesgo, FILE* f)
{
	fprintf(f,"%s %s %d %s %d \n",riesgo->descripcion,"instruccion: ",riesgo->linea, "CC: ", riesgo->ciclo);
}
void imprimirRiesgosArchivo(SetRiesgos* riesgos)
{
	FILE* f;
	f = fopen("HAZARDS.txt","wb");
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	for(int i = 0; i < riesgos->largo; i++)
	{
		riesgo = obtenerRiesgo(riesgos,i);
		fprintf(f,"%d - ", i+1);
		imprimirRiesgoArchivo(riesgo,f);
	}
	fclose(f);
}
void imprimirSoluciones(SetRiesgos* riesgos)
{
	FILE* f;
	f = fopen("DETECCION.txt","wb");
	Riesgo* riesgo = (Riesgo*)malloc(sizeof(Riesgo));
	for(int i = 0; i < riesgos->largo; i++)
	{
		riesgo = obtenerRiesgo(riesgos,i);
		fprintf(f,"%d - ", i+1);
		imprimirSolucion(riesgo,f);
	}
}
void imprimirSolucion(Riesgo* riesgo,FILE* f)
{
	if(strcmp(riesgo->nombre,"LOAD_HAZARD_A") == 0 || strcmp(riesgo->nombre,"LOAD_HAZARD_B") == 0)
	{
		fprintf(f,"%s\n","Solucionable a traves de: NOP y Forwarding MEM/WB a ID/EX");
	}
	else if(strcmp(riesgo->nombre,"EX_HAZARD_A") == 0 || strcmp(riesgo->nombre,"EX_HAZARD_B") == 0)
	{
		fprintf(f,"%s\n","Solucionable a traves de: Forwarding EX/MEM a ID/EX");
	}
	else if(strcmp(riesgo->nombre,"MEM_HAZARD_A") == 0 || strcmp(riesgo->nombre,"MEM_HAZARD_B") == 0)
	{
		fprintf(f,"%s\n","Solucionable a traves de: Forwarding MEM/WB a ID/EX");
	}
	else if(strcmp(riesgo->nombre,"JUMP") == 0)
	{
		fprintf(f,"%s\n","Solucionable a traves de: FLUSH/NOP en IF/ID");
	}
	else if(strcmp(riesgo->nombre,"BRANCH") == 0)
	{
		fprintf(f,"%s\n","Solucionable a traves de: FLUSH/NOP en IF/ID, ID/EX y EX/MEM");
	}
}
//Funcion para insertar un riesgo en una lista de riesgos
//Entrada: riesgo: Riesgo, riesgos: Lista de riesgos.
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
//Funcion para obtener un riesgo de una lista de riesgos
//ENTRADA: riesgos: lista de riesgos, n: posicion del riesgo
//SALIDA: riesgo: Riesgo encontrado y obtenido.
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
//Salida: instruccion: Instruccion encontrada y obtenida.
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
		printf("NUMERO: %d | ",instruction->posicion);
		printf("POS %d | ",instruction->posicionReal);
		printf("Funct: %s ",instruction->op);
		printf("RT: %s ",instruction->rt);
		printf("RS: %s ",instruction->rs);
		printf("RD: %s ",instruction->rd);
		printf("Inmediate: %d ",instruction->constante);
		printf("Etiqueta: %s\n",instruction->etiqueta);
		//printf("FORWARDING: %d\n",instruction->valorForwarding);


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
//Inserta un registro a la lista de registros
//Entrada: regSet = lista de instrucciones, nombreRegistro: string "$s0", numero: numero del registro
void insertarRegistro(SetRegistros* regSet,char* nombreRegistro, int numero){
	Registro* nodo = crearRegistro(nombreRegistro,numero);
	nodo->siguiente = regSet->cabeza;
	regSet->cabeza = nodo;
	regSet->largo++;

}
//Obtiene un registro de una lista de registros
//Entrada: n = indice del registro que se quiere obtener, regSet = lista de registros
//Salida: Registro* = registro obtenido
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
	char** registersName = (char**)malloc(sizeof(char*)*32);
	for(int i = 0; i < 32; i++)
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
	strcpy(registersName[31],"$ra");

	for(int i = 0; i < 32; i++)
	{
		if(strcmp(registersName[31-i],"")!=0)
		{
			insertarRegistro(registers,registersName[i],i);
		}
	}
	return registers;
}
//IMPRIME LOS REGISTROS
void imprimirRegistros(SetRegistros* registers)
{	
	FILE* f;
	f = fopen("REGISTROS.txt","wb");
	Registro* rg = (Registro*)malloc(sizeof(Registro));
	for(int i = 0; i < 29; i++)
	{
		rg = obtenerRegistro(i,registers);
		if(strcmp(rg->nombre,"")!=0)
		{
			fprintf(f,"%s ",rg->nombre );
			fprintf(f,"%d\n",rg->valor );
		}
	}
	imprimirStack(f);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//CARGA DE ARCHIVOS
//Funcion encargada de cargar un programa desde un archivo de texto
//ENTRADA: un string que contiene el pathFile
//SALIDA: Retorna una matriz de caracteres con el programa cargado y sin comentarios
Programa* cargarPrograma(char* pathFile){
	FILE *instructionFile;
	instructionFile = fopen(pathFile,"rb");
	if(instructionFile != NULL)
	{
		Programa *instruction = (Programa*)malloc(sizeof(Programa));
		Programa *instructionClean = (Programa*)malloc(sizeof(Programa));
		instruction->largo = 0;
		instructionClean->largo = 0;  
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
			if(line[0] != '#' && strcmp(line,"")!=0)
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
			if(line[0] != '#' && strcmp(line,"")!=0)
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
//ENTRADA: Entra una estructura Program con el programa pre-cargado
//SALIDA: Retorna el programa sin comentarios adyacentes
Programa* elminarComentarios(Programa* inst)
{
	char delimit[] = "#";
	int lineCounter = inst->largo;
	Programa *instruction = (Programa*)malloc(sizeof(Programa));
	instruction->largo = 0;
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
//Funcion que retorna una lista con las instrucciones
//ENTRADA: programa: Contiene la matriz de caractres que almacena las instrucciones
//SALIDA: setInstrucciones que contiene todas las instrucciones leidas.
SetInstrucciones* cargarProgramaAMemoria(Programa* programa)
{
	SetInstrucciones* inSetInvertido = (SetInstrucciones*)malloc(sizeof(SetInstrucciones));
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	char** part = (char**)malloc(sizeof(char*)*5);
	for(int i = programa->largo-1; i >= 0; i--)
	{
		part = makeLine(programa->matrizInstrucciones[i]);
		instruccion = crearInstruccion(part);
		instruccion->posicionReal = i + 1;
		insertarInstruccion(inSetInvertido,instruccion);
	}
	enumerarInstrucciones(inSetInvertido);

	return inSetInvertido;
}
//Funcion para enumerar las instrucciones
//ENTRADA: instrucciones: Lista de instrucciones.
void enumerarInstrucciones(SetInstrucciones* instrucciones)
{
	Instruccion* instruccion = (Instruccion*)malloc(sizeof(Instruccion));
	int posicion = 1;
	for(int i = 0; i < instrucciones->largo; i++)
	{
		instruccion = obtenerInstruccion(i,instrucciones);
		if(strcmp(instruccion->op,"ETIQUETA")!=0)
		{
			instruccion->posicion = posicion;
			posicion++;
		}	
	}
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//Funcion encargada de separar y crear un nodo Instruction
//Entrada: una linea de instrucción
//Salida: Un nodo Instruction
char** makeLine(char* instructionLine)
{
	char** part = (char**)malloc(sizeof(char*)*4);
	for(int i = 0;i < 4; i++)
	{
		part[i] = (char*)malloc(sizeof(char)*8);
	}
	char* line = (char*)malloc(sizeof(char)*8);
	int largo = strlen(instructionLine);
	if(instructionLine[largo-2]==':')
	{
		strcpy(part[0],instructionLine);
		return part;
	}
	line = strtok(instructionLine," ");
	strcpy(part[0],line);
	int i = 1;
	while ((line = strtok(NULL," ")) != NULL)
	{
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
	instruction->hazard = (char*)malloc(sizeof(char)*16);
	instruction->siguiente = NULL;
	strcpy(funct,programLine[0]);
	int largo = strlen(funct);
	strcpy(instruction->hazard,"");
	if(strcmp(funct,"addi")==0||strcmp(funct,"subi")==0||strcmp(funct,"addiu")==0)
	{
		strcpy(instruction->op,funct);
		strcpy(instruction->rs,programLine[2]);
		strcpy(instruction->rd,programLine[1]);
		char *ptr;
		instruction->constante = strtol(programLine[3],&ptr,10);

	}
	else if(strcmp(funct,"add")==0||strcmp(funct,"sub")==0||strcmp(funct,"div")==0||strcmp(funct,"mul")==0)
	{
		strcpy(instruction->op,funct);
		strcpy(instruction->rt,programLine[2]);
		strcpy(instruction->rd,programLine[1]);
		removeChar(programLine[3], '\n');
		strcpy(instruction->rs,programLine[3]);


	}
	else if(strcmp(funct,"sw")==0)
	{
		strcpy(instruction->op, funct);
		
		strcpy(instruction->rd,programLine[1]);
		
		char *ptr;
		
		auxRegister = strtok(programLine[2],"(");
		instruction->constante = strtol(auxRegister,&ptr,10);
		auxRegister = strtok(NULL,")");
		strcpy(instruction->rs, auxRegister);
	}
	else if(strcmp(funct,"lw")==0)
	{
		strcpy(instruction->op, funct);
		
		strcpy(instruction->rd, programLine[1]);
		
		char *ptr;
		
		auxRegister = strtok(programLine[2],"(");
		instruction->constante = strtol(auxRegister,&ptr,10);
		auxRegister = strtok(NULL,")");
		strcpy(instruction->rs, auxRegister);
	}
	else if(strcmp(funct,"beq")==0||strcmp(funct,"bne")==0||strcmp(funct,"bgt")==0||strcmp(funct,"blt")==0)
	{
		strcpy(instruction->op, funct);
		strcpy(instruction->rs,programLine[2]);
		strcpy(instruction->rt,programLine[1]);
		removeChar(programLine[3], '\n');
		strcpy(instruction->etiqueta,programLine[3]);

	}
	else if(strcmp(funct,"j")==0)
	{
		strcpy(instruction->op, funct);
		removeChar(programLine[1], '\n');
		strcpy(instruction->etiqueta,programLine[1]);	

	}
	else if(strcmp(funct,"jal")==0)
	{
		strcpy(instruction->op, funct);
		removeChar(programLine[1], '\n');
		strcpy(instruction->etiqueta,programLine[1]);	
	}
	else if(strcmp(funct,"jr")==0)
	{
		strcpy(instruction->op, funct);
		removeChar(programLine[1], '\n');
		strcpy(instruction->rs,programLine[1]);	

	}
	else if(funct[largo-2]==':')
	{

		strcpy(instruction->op,"ETIQUETA");
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