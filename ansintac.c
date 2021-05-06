/*
 *	Analizador Sintactico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *
 *	Descripcion: 
 *     Implementacion de un analizador sintactico descendente recursivo o LL(1) para el lenguaje JSON simplificado.	
 *     Estrategia de manejo de errores Panic Mode con sincronizacion
*/


#include "anlex.c"
//Prototipos de funciones
void element(int[]);
void array(int[]);
void arrayA(int[]);
void element_list(int[]);
void element_listA(int[]);
void object(int[]);
void objectA(int[]);
void attributes_list(int[]);
void attributes_listA(int[]);
void attribute(int[]);
void attribute_name(int[]);
void attribute_value(int[]);
int accept=1;


void match(int expectedToken){
    if(t.compLex == expectedToken){
        getToken();
    } 
}

void error_sint(int sincronizacion []){
    accept=0;
   	printf("Error sintactico en linea: %d. No se esperaba %s.\n",numLinea,t.pe->componente);
	int i = 0;
    while(t.compLex != sincronizacion[i] && t.compLex != EOF){   
        if (sincronizacion[i] == '\0'){
            getToken();
            i=0;        
        }
        i++;
    }
    getToken();   	   
}

void check_input(int primero[], int siguiente[]){
    int syncset[2000];
    int i=0;
    if(t.compLex == EOF) return;    
    while(primero[i] != '\0'){
        if(t.compLex == primero[i]){
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

//json -> element EOF 
void json(){
    int primero[] = {'{','[','\0'};
    int siguiente[] = {EOF, '\0'};
    element(siguiente);
}


//element -> object | array
void element(int syncset[]){
    int primero[] = {'{','[','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '{'){
        object(siguiente);
    }
    else if(t.compLex == '['){
        array(siguiente);
    }
    check_input(siguiente,primero);
}


//array -> [ arrayA
void array(int syncset[]){
    int primero[] = {'[','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '['){
        match('[');
        arrayA(siguiente);
    }
    check_input(siguiente,primero);
}


//array' -> element-list ] | ]
void arrayA(int syncset[]){
    int primero[]={'{','[',']','\0'};
    int siguiente[] = {'[',',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '{' || t.compLex == '['){
        element_list(siguiente);
        match(']');
    }
    else if(t.compLex == ']'){
        match(']');
    }
    check_input(siguiente,primero);
}

//element-list -> element element-listA
void element_list(int syncset[]){
    int primero[]={'{','[','\0'};
    int siguiente[] = {']', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '{' || t.compLex == '['){
        element(siguiente);
        element_listA(siguiente);
    }
    check_input(siguiente,primero);
}


//element-list' ->  ,element element-list' | ε
void element_listA(int syncset[]){
    if(t.compLex == ']'){ 
        return;
    }
    int primero[]={',','\0'};
    int siguiente[] = {']', '\0'};
    check_input(primero,syncset);
    if(t.compLex == ','){
        match(',');
        element(siguiente);
        element_listA(siguiente);
    }
    check_input(siguiente,primero);
}


//object -> { objectA
void object(int syncset[]){
    int primero[]={'{','\0'};
    int siguiente[] = {',',']','}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '{'){
        match('{');
        objectA(siguiente);
    }
    check_input(siguiente,primero);
}


//object' -> attributes-list} | }
void objectA(int syncset[]){
    int primero[]={STRING,'}','\0'};
    int siguiente[] = {'{',',',']','}','\0'};
    check_input(primero,syncset);
    if(t.compLex == STRING){
        attributes_list(siguiente);
        match('}');
    }else if(t.compLex == '}'){
        match('}');
    }
    check_input(siguiente,primero);
}


//attributes-list -> attribute attributes-listA
void attributes_list(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {'}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == STRING){
        attribute(siguiente);
        attributes_listA(siguiente);
    }
    check_input(siguiente,primero);
}


//attributes-list' -> ,attribute attributes-listA | ε
void attributes_listA(int syncset[]){
    if (t.compLex == '}'){
        return;
    }
    int primero[]={',','\0'};
    int siguiente[] = {'}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == ','){
        match(',');
        attribute(siguiente);
        attributes_listA(siguiente);
    }
    check_input(siguiente,primero);
}


//attribute -> attribute-name : attribute-value
void attribute(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {',','}', '\0'};
    check_input(primero,siguiente);
    if(t.compLex == STRING){
        attribute_name(siguiente);
        match(':');
        attribute_value(siguiente);
    }
    check_input(siguiente,primero);
}

//attribute-name -> string
void attribute_name(int syncset[]){
    int primero[]={STRING,'\0'};
    int siguiente[] = {':', '\0'};
    check_input(primero,syncset);
    if(t.compLex == STRING){
        match(STRING);
    }
    check_input(siguiente,primero);
}


//attribute-value -> element | STRING | NUMBER | TRUE | FALSE | NULL
void attribute_value(int syncset[]){
    int primero[]={'{','[',STRING, NUM, TRUE, FALSE, A_NULL,'\0'};
    int siguiente[] = {',','}', '\0'};
    check_input(primero,syncset);
    if(t.compLex == '{' || t.compLex == '['){
        element(siguiente);
    }
    else if(t.compLex == STRING){
        match(STRING);
    }
    else if(t.compLex == NUM){
        match(NUM);
    }
    else if(t.compLex == TRUE){
        match(TRUE);
    }
    else if(t.compLex == FALSE){
        match(FALSE);
    }
    else if(t.compLex == A_NULL){
        match(A_NULL);
    }
    check_input(siguiente,primero);
}


int main (int argc,char* args[]){
    initTabla();
	initTablaSimbolos();
    if(argc > 1){
        if (!(archivo=fopen(args[1],"rt"))){
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        getToken();
        json();
        if(accept){
            printf("Correctamente sintactico \n");
        }
        fclose(archivo);
    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }
    return 0;
}
