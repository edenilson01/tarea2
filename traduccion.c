/*
 *	Traduccion Dirigida por Sintaxis
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *
 *	Descripcion: 
 *     Implementar un traductor para el lenguaje Json simplificado
 *
*/
#include "ansintac.c"

FILE *output;	  //archivo de salida
int espacios = 0; //espacios para lograr el formato de salida deseado

void element_trad();
void array_trad();
void arrayA_trad();
void element_list_trad();
void element_listA_trad();
void object_trad();
void objectA_trad();
void attribute_trad();
void attributes_list_trad();
void attributes_listA_trad();
void attribute_name_trad();
void attribute_value_trad();


void imprimir_esp(){
    int i=0;
    for (i=1;i<=espacios;i++){
        fprintf(output," ");
    }
}


//string sin comillas
void string_trad(char* cadena){
    char string[TAMLEX];
    strcpy(string,cadena);
    int i=1;
    while(string[i] != '"'){
        fprintf(output,"%c",string[i]);
        i++;
    }
}


//json_trad -> element_trad EOF 
void json_trad(){
    element_trad();
}


//element_trad -> object_trad | array_trad
void element_trad(){
    if(t.compLex == '{'){
        object_trad();
    }else if(t.compLex == '['){
        array_trad();
    }  
}


//array_trad -> [ arrayA_trad
void array_trad(){  
    if(t.compLex == '['){ 
        match('[');
        arrayA_trad();
    }
}


//arrayA_trad -> element-list ] | ]
void arrayA_trad(){
    if(t.compLex == '{' || t.compLex == '['){
        fprintf(output,"\n");
        imprimir_esp();
        fprintf(output,"<item>\n");
        espacios+=4;
        element_list_trad();
        match(']');
        imprimir_esp();
        fprintf(output,"</item>\n");
        espacios-=4;
        imprimir_esp();
    }else if(t.compLex == ']'){
        espacios-=4;
        match(']');
    }
}


//element-list_trad -> element_trad element-listA_trad
void element_list_trad(){
    if(t.compLex == '{' || t.compLex == '['){
        element_trad();
        element_listA_trad();    
    }
}


//element-listA_trad ->  ,element_trad element-listA_trad | ε
void element_listA_trad(){
    if(t.compLex == ']'){ 
        return;
    }

    if(t.compLex == ','){  
        match(',');
        imprimir_esp();
        fprintf(output,"</item>\n");
        imprimir_esp();
        fprintf(output,"<item>\n");
        espacios+=4;
        element_trad();
        element_listA_trad();   
    }
}


//object_trad -> { objectA_trad
void object_trad(){
    if(t.compLex == '{'){
        match('{');
        objectA_trad();
    }

}


//objectA_trad -> attributes-list_trad } | }
void objectA_trad(){
    if(t.compLex == STRING){
        attributes_list_trad();
        match('}');

    }else if(t.compLex == '}'){
        match('}');
    }
}


//attributes-list_trad -> attribute_trad attributes-listA_trad
void attributes_list_trad(){
    if(t.compLex == STRING){
        attribute_trad();
        attributes_listA_trad();
    }
}


//attributes-listA_trad -> ,attribute_trad attributes-listA_trad | ε
void attributes_listA_trad(){
    if (t.compLex == '}'){
        espacios-=4;
        return;
    }

    if(t.compLex == ','){
        match(',');
        attribute_trad();
        attributes_listA_trad();
    }
}


//attribute_trad -> attribute-name_trad : attribute-value_trad
void attribute_trad(){
    if(t.compLex == STRING){
        imprimir_esp();
        char lexema[TAMLEX];
        strcpy(lexema,t.pe->lexema);
        fprintf(output,"<");
        attribute_name_trad();
        fprintf(output,">");
        match(':');
        attribute_value_trad();
        fprintf(output,"</");
        string_trad(lexema);
        fprintf(output,">\n");    
    }
}


//attribute-name_trad -> string_trad
void attribute_name_trad(){
    if(t.compLex == STRING){    
        string_trad(t.pe->lexema);
        match(STRING);
    }
}


//attribute-value_trad -> element_trad | STRING | NUMBER | TRUE | FALSE | NULL
void attribute_value_trad(){
    if(t.compLex == '{' || t.compLex == '['){
        espacios +=4;
        element_trad();
    }else if(t.compLex == STRING){
        fprintf(output,"%s",t.pe->lexema);
        match(STRING);
    }else if(t.compLex == NUM){
        fprintf(output,"%s",t.pe->lexema);
        match(NUM);
    }else if(t.compLex == TRUE){
        fprintf(output,"%s",t.pe->lexema);
        match(TRUE);
    }else if(t.compLex == FALSE){
        fprintf(output,"%s",t.pe->lexema);
        match(FALSE);
    }else if(t.compLex == A_NULL){
        fprintf(output,"%s",t.pe->lexema);
        match(A_NULL);
    }
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
        if(correcto){
            output = fopen ("output.xml", "w");
            fclose(archivo);
            archivo=fopen(args[1],"rt");
            getToken();
            json_trad();
            printf("Archivo xml generado bajo el nombre de \"output.xml\"\n");
        }
    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }
    return 0;
}