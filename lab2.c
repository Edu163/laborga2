#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void pipeline();
void printEtapasPL(char* etapas);
void avanzarInstrucciones(char* etapas, char instruccion);
//PIPELINE
char instructionFetch(char memoria[], int direccionPC);
void instructionDecode(char instruccion);
void executeInstruction(char instruccion);
void MEM(char instruccion);
void writeBack(char instruccion);
int pipelineVacio(char* pipeline);
//REGISTROS
int main()
{


	pipeline();
	return 0;
}

void pipeline()
{
	char instruccion = '-';
	char* pipeline = (char*)malloc(sizeof(char)*4);
	for(int i = 0; i<4;i++)
	{
		pipeline[i] = '-';
	}
	//printEtapasPL(pipeline);
	char memoria[10] = {'a','b','c','d','f','g','h','i','j','k'};
	int direccion = 0;
	int ciclosReloj = 1;
	printf("%s","    IF ID EX MM WB" );
	printf("\n");
	while(pipelineVacio(pipeline) != 1 || direccion < 10)
	{
		avanzarInstrucciones(pipeline,instruccion);
		instruccion = instructionFetch(memoria,direccion);
		instructionDecode(pipeline[0]);
		executeInstruction(pipeline[1]);
		MEM(pipeline[2]);
		writeBack(pipeline[3]);
		//SOLO PRINTS
		if(ciclosReloj<10)
		{
			printf("%d)  ",ciclosReloj);
		}
		if(ciclosReloj>=10)
		{
			printf("%d) ",ciclosReloj);
		}
		printf("%c  ",instruccion);
		printEtapasPL(pipeline);
		printf("\n");
		ciclosReloj++;
		direccion++;
	}
	

}
int pipelineVacio(char* pipeline)
{
	if(pipeline[0] == '-' && pipeline[1] == '-' && pipeline[2] == '-' && pipeline[3] == '-')
	{
		return 1;
	}
	return 0;
}
void printEtapasPL(char* pipeline)
{
	for(int i = 0; i<4;i++)
	{
		printf("%c  ", pipeline[i]);
	}
}
void avanzarInstrucciones(char* pipeline,char instruccion)
{
	pipeline[3] = pipeline[2];
	pipeline[2] = pipeline[1];
	pipeline[1] = pipeline[0];
	pipeline[0] = instruccion;
}
char instructionFetch(char memoria[], int direccionPC)
{
	if(direccionPC>=10)
	{
		return '-';
	}
	return memoria[direccionPC];
}
void instructionDecode(char instruccion)
{
	//printf("%s %c\n","ID: ", instruccion);
}
void executeInstruction(char instruccion)
{
	//printf("%s %c\n","EX: ", instruccion);
}
void MEM(char instruccion)
{
	//printf("%s %c\n","MEM: ", instruccion);
}
void writeBack(char instruccion)
{
	//printf("%s %c\n","WB: ", instruccion);
}


//Funcion para iniciar los registros
//Retorna una lista enlazada de registros iniciados con valor 0
RegisterSet* initializeRegisters()
{
	RegisterSet* registers = (RegisterSet*)malloc(sizeof(RegisterSet));
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
			insert(registers,registersName[i],i);
		}
	}
	return registers;
}
//Inserta un registro al set de registros
//Entrada: regSet = set de registros, registerName = nombre de registro, number = numero de registro
void insert(RegisterSet* regSet,char* registerName, int number){
	Register* nodo = makeRegister(registerName,number);
	nodo->next = regSet->head;
	regSet->head = nodo;
	regSet->size++;

}