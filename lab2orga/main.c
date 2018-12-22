#include "cabeza.h"

int main()
{
	char* pathFile = (char*)malloc(sizeof(char)*256);
	char* finalPathFile = (char*)malloc(sizeof(char)*256);
	Programa* programa = (Programa*)malloc(sizeof(Programa));
	printf("%s\n","Ingrese la ruta de directorio del archivo, EJ: jugada.txt");
	fgets(pathFile,256,stdin);
	int a = strlen(pathFile);
	strncpy(finalPathFile,pathFile,a-1);
	programa = cargarPrograma(finalPathFile);
	if(programa != NULL)
	{
		pipeline(programa);
	}
	else
	{
		printf("%s\n","La ruta del archivo no es valida");
	}
	return 0;
}