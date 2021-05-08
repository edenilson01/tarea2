/*
 *	Analizador Lexico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Practica de Programacion Nro. 1
 *	
 *	Descripcion:
 *	Implementar un analizador léxico para el lenguaje Json simplificado.
 *	
 */

/*********** Inclusion de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/

token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
//FILE *output;
FILE *archivo;			// Fuente Json
char lexema[TAMLEX];	// Utilizado por el analizador lexico

int numLinea = 1;			// Numero de Linea
int correcto = 1;
/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje){
	printf("Linea %d: Error. %s.\n",numLinea,mensaje);
	correcto = 0;
}

void getToken(){
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF){	
		if (c==' ' || c=='\t'){
			continue;

		}else if(c=='\n'){
			numLinea++;
		}else if (tolower(c)=='t' || tolower(c)=='f' || tolower(c)=='n'){
			//es un boolean o null
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(archivo);
			}while(isalpha(c));
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			
			if (strcmp(lexema, "true")==0 || strcmp(lexema, "TRUE")==0){
				t.pe=buscar("true");
				t.compLex=TRUE;
			}else if (strcmp(lexema, "false")==0 || strcmp(lexema, "FALSE")==0){
				t.pe=buscar("false");
				t.compLex=FALSE;
			}else if (strcmp(lexema, "null")==0 || strcmp(lexema, "NULL")==0){
				t.pe=buscar("null");
				t.compLex=A_NULL;
			}else{
				strcpy(t.pe->componente,"");
				error("No se reconoce");
			}
			break;
		}else if (isdigit(c)){
			//es un numero
			i=0;
			estado=0;
			acepto=0;
			lexema[i]=c;
			
			while(!acepto){
				switch(estado){
				case 0: //una secuencia netamente de digitos, puede ocurrir . o e
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=0;
					}else if(c=='.'){
						lexema[++i]=c;
						estado=1;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else{
						estado=6;
					}
					break;
				case 1://un punto, debe seguir un digito
					c=fgetc(archivo);						
					if (isdigit(c)){
						lexema[++i]=c;
						estado=2;
					}else{
						if (c=='\n'){
							sprintf(msg,"No se esperaba un salto de linea");
						}else if (c=='\t' || c==' '){
							sprintf(msg,"No se esperaba un espacio");
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
						}
						estado=-1;
					}
					break;
				case 2://la fraccion decimal, pueden seguir los digitos o e
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=2;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else
						estado=6;
					
					break;
				case 3://una e, puede seguir +, - o una secuencia de digitos
					c=fgetc(archivo);
					if (c=='+' || c=='-'){
						lexema[++i]=c;
						estado=4;
					}else if(isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						if (c=='\n'){
							sprintf(msg,"No se esperaba un salto de linea");
						}else if (c=='\t' || c==' '){
							sprintf(msg,"No se esperaba un espacio");
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
						}
						estado=-1;
					}
					break;
				case 4://necesariamente debe venir por lo menos un digito
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						if (c=='\n'){
							sprintf(msg,"No se esperaba un salto de linea");
						}else if (c=='\t' || c==' '){
							sprintf(msg,"No se esperaba un espacio");
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
						}
						estado=-1;
					}
					break;
				case 5://una secuencia de digitos correspondiente al exponente
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						estado=6;
					}break;
				case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
					if (c!=EOF)
						ungetc(c,archivo);
					else
						c=0;
					
					lexema[++i]='\0';
					acepto=1;
					t.pe=buscar(lexema);
					if (t.pe->compLex==-1){
						strcpy(e.lexema,lexema);
						e.compLex=NUM;
						strcpy(e.componente,"NUMBER");
						insertar(e);
						t.pe=buscar(lexema);
					}
					t.compLex=NUM;
					break;
				case -1:
					if (c==EOF)
						error("No se esperaba el fin de archivo");
					else
						error(msg);
					strcpy(t.pe->componente,"");
					acepto=-1;
				}
			}
			break;
		}else if (c==':'){
			t.compLex=':';
			t.pe=buscar(":");
			break;
		}else if (c==','){
			t.compLex=',';
			t.pe=buscar(",");
			break;
		}else if (c=='['){
			t.compLex='[';
			t.pe=buscar("[");
			break;
		}else if (c==']'){
			t.compLex=']';
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"'){ 
			//un caracter o una cadena de caracteres
			i=0;
			lexema[i]=c;
			i++;
			int cantidadSalto = 0; //en caso de que la cadena cuente con saltos de linea
			do{
				c=fgetc(archivo);
				if (c=='\"'){
					c=fgetc(archivo);
					if (c=='\"'){
						lexema[i]=c;
						i++;
						lexema[i]=c;
						i++;
					}else{
						lexema[i]='\"';
						i++;
						break;
					}
				}else if(c==EOF){
					error("Se llego al fin de archivo sin finalizar la cadena");
					break;				
				}else{
					if (c=='\n')
						cantidadSalto++;
					
					lexema[i]=c;
					i++;
				}
			}while(c!=EOF);
			numLinea = numLinea + cantidadSalto;
			lexema[i]='\0';
			if (c!=EOF){
				ungetc(c,archivo);
			}else{
				c=0;
			}
			if (c==0){
				t.compLex=EOF;
				t.pe=NULL;
			}else{
				t.pe=buscar(lexema);
				t.compLex=STRING;
				if (t.pe->compLex==-1){
					strcpy(e.lexema,lexema);
					e.compLex=STRING;
					strcpy(e.componente,"STRING");
					insertar(e);
					t.pe=buscar(lexema);
					t.compLex=STRING;
				}
			}
			break;
		}else if (c=='{'){
			t.compLex='{';
			t.pe=buscar("{");
			break;
		}else if (c=='}'){
			t.compLex='}';
			t.pe=buscar("}");
			break;
		}else if (c!=EOF){
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}if (c==EOF){
		t.compLex=EOF;
		t.pe=NULL;
	}	
}
/*
int main(int argc,char* args[]){
	// inicializar analizador lexico
	initTabla();
	initTablaSimbolos();

	//output = fopen("salida.txt", "w");
	
	if(argc > 1){
		if (!(archivo=fopen(args[1],"rt"))){
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		
		while (t.compLex!=EOF){
			getToken();
			if (t.compLex!=EOF){
				//fprintf(output,"%s ", t.pe->componente);
				printf("%d ", t.pe->compLex);
				printf("%d/2 ", t.compLex);
			}
			
		}
		//fclose(output);
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}*/