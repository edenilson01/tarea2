/*
 *	Analizador Sintactico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *
 *	Descripcion: 
 *     Implementacion de un analizador sintactico descendente recursivo LL1 para el lenguaje JSON simplificado.	
 */


#include "anlex.c"
#define CANT_COMP_LEX 10
/**Prototipos de funciones**/
int accept=1;


void element(int[]);
void array(int[]);
void arrayB(int[]);
void element_list(int[]);
void element_listB(int[]);
void object(int[]);
void objectB(int[]);
void attributes_list(int[]);
void attributes_listB(int[]);
void attribute(int[]);
void attribute_name(int[]);
void attribute_value(int[]);

/***/
void match(int expectedToken){
    if(t.pe->compLex == expectedToken){
        getToken();

    }/*else{
        error("error en match");
    }*/
  
}

void error(char mensaje[]){
    printf("%s", mensaje);
}

void error_sint(int sincronizacion [])
{
    accept=0;
   	printf("\nError sintactico en linea: %d. No se esperaba %s.\n",numLinea,t.pe->componente);
	/*int i = 0;
    while(t.compLex != sincronizacion[i] && t.compLex != EOF){   
        if (sincronizacion[i] == '\0'){
            getToken();
            i=0;        
        }
        i++;
    }
    getToken();
    return;*/	   
}

void check_input(int primero[], int siguiente[])
{
    int syncset[]={CANT_COMP_LEX};
    int i=0;
    if(t.compLex == EOF) return;  
    while(primero[i] != '\0') 
    {
        if(t.pe->compLex == primero[i])
        {
            return;
        }
        i++;
    }
    i=0;    
    while(primero[i] != '\0'){ 
        syncset[i] = primero[i];
        i++;
    }
    int j=0;
    while(siguiente[j] != '\0'){
        syncset[i] = siguiente[j];
        i++;
        j++;
    }
    error_sint(syncset);

}
 
void json(){
    int primero[] = {'{','[','\0'};
    int siguiente[] = {EOF, '\0'}; //PROBAR
    //int siguiente[] = {',',']','}', '\0'}; //PROBAR
    element(siguiente);
}

void element(int syncset[]){

    int primero[] = {'{','[','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);

    if(t.pe->compLex == '{'){
        object(siguiente);
    }
    else if(t.pe->compLex == '['){
        array(siguiente);
    }else{
        error("error en element");
    }
    check_input(siguiente,primero);
}


/*array -> [ arrayB*/
void array(int syncset[]){
    int primero[] = {'[','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);
   
    if(t.pe->compLex == '['){
        match('[');
        arrayB(siguiente);
    }
    check_input(siguiente,primero);
}

/*array' -> element-list ] | ]*/
void arrayB(int syncset[]){
   
    int primero[]={'{','[',']','\0'};
    int siguiente[] = {'[',',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == '{' || t.pe->compLex == '['){
        element_list(siguiente);
        match(']');
    }
    else if(t.pe->compLex == ']'){
        match(']');
    }
    check_input(siguiente,primero);

}

/*element-list -> element element-list'*/
void element_list(int syncset[]){
    
    int primero[]={'{','[','\0'};
    int siguiente[] = {']', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == '{' || t.pe->compLex == '['){
        element(siguiente);
        element_listB(siguiente);
    }
    check_input(siguiente,primero);
}


/*element-list' ->  ,element element-list' | ε*/
void element_listB(int syncset[]){
   
    if(t.pe->compLex == ']'){ 
        return;
    }
    int primero[]={',','\0'};
    int siguiente[] = {']', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == ','){
        match(',');
        element(siguiente);
        element_listB(siguiente);
    }
    
    check_input(siguiente,primero);
}


/*  object -> { object'*/
void object(int syncset[]){
 
    int primero[]={'{','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == '{'){
        match('{');
        objectB(siguiente);
    }
    check_input(siguiente,primero);

}

/*   object' -> attributes-list} | }*/
void objectB(int syncset[]){
  
    int primero[]={STRING,'}','\0'};
    int siguiente[] = {'{',',',']','}','\0'};
    //int siguiente[] = {STRING,',',']','}','\0'};
    check_input(primero,syncset);
    
    if(t.pe->compLex == STRING){
        
        attributes_list(siguiente);
        match('}');
    }
    else if(t.pe->compLex == '}'){
        match('}');
    }
    check_input(siguiente,primero);
}

/*   attributes-list -> attribute attributes-list'*/
void attributes_list(int syncset[]){

    int primero[]={STRING,'\0'};
    int siguiente[] = {'}', '\0'};
    check_input(primero,syncset);

    if(t.pe->compLex == STRING){
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    check_input(siguiente,primero);
}

/*   attributes-list' -> ,attribute attributes-list' | ε*/
void attributes_listB(int syncset[]){
 
    if (t.pe->compLex == '}'){
        return;
    }
    int primero[]={',','\0'};
    int siguiente[] = {'}', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == ','){
        match(',');
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    
    check_input(siguiente,primero);
}

/*   attribute -> attribute-name : attribute-value*/
void attribute(int syncset[]){

    int primero[]={STRING,'\0'};
    int siguiente[] = {',','}', '\0'};
    check_input(primero,siguiente);
    if(t.pe->compLex == STRING){
        attribute_name(siguiente);
        match(':');
        attribute_value(siguiente);
    }
    check_input(siguiente,primero);
}

/*   attribute-name -> string*/
void attribute_name(int syncset[]){
   
    int primero[]={STRING,'\0'};
    int siguiente[] = {':', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == STRING){
        match(STRING);
    }
    check_input(siguiente,primero);
}

/*attribute-value -> element | string | number | true | false | null*/
void attribute_value(int syncset[]){

    int primero[]={'{','[',STRING, NUM, TRUE, FALSE, A_NULL,'\0'};
    int siguiente[] = {',','}', '\0'};
    check_input(primero,syncset);
    if(t.pe->compLex == '{' || t.pe->compLex == '['){
        element(siguiente);
    }
    else if(t.pe->compLex == STRING){
        match(STRING);
    }
    else if(t.pe->compLex == NUM){
        match(NUM);
    }
    else if(t.pe->compLex == TRUE){
        match(TRUE);
    }
    else if(t.pe->compLex == FALSE){
        match(FALSE);
    }
    else if(t.pe->compLex == A_NULL){
        match(A_NULL);
    }
    check_input(siguiente,primero);

}



/***** M A I N ******/
int main (int argc,char* args[]){
    // inicializar analizador lexico
	initTabla();
	initTablaSimbolos();

    if(argc > 1)
    {
        if (!(archivo=fopen(args[1],"rt")))
        {
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        getToken();
        json();
        if(accept) printf("Sintacticamente correcto \n");
        fclose(archivo);
    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}