/*******************************************************************************
	DEPARTAMENTO DE ELECTRÓNICA DE SEGAINVEX. UNIVERSIDA AUTONOMA DE MADRID				
				LIBRERIA PARA ARDUINO Segainvex_scpi_Serial V2.2
 SISTEMA PARA COMUNICAR UNA COMPUTADORA CON ARDUINO MEDIANTE PUERTO SERIE 
 Fichero de cabecera segainvex_scpi_Serial.h
******************************************************************************/
/*
	Copyright © 2017 Mariano Cuenca, Patricio Coronado
	
	This file is part of segainvex_scpi_Serial.

    segainvex_scpi_Serial is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    segainvex_scpi_Serial is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with segainvex_scpi_Serial.  If not, see <http://www.gnu.org/licenses/>.

*/
/* 
		Mejoras futuras:
		1)Poner el menú de segainvex_scpi_Serial en flash para no consumir RAM
		2)Mejorar la función int cambia_variable_double_del_sistema(double *,double,double);
 */
 /* 
		Funciona con todos los puertos Serial tipo HardwareSeria no con SerialUSB.
		
		Para enviar datos por el Serial si sabes por que puerto has recibido
     el comando escribes directamente en el.  Pero si no sabes por que
    puerto has recibido el comando  Se puede utilizar la macro 
    ESCRIBE_PUERTO_SERIE(X).
    Ejemplo: para enviar 2 float Temperatura y Humedad:
		String Datos=String(Temperatura)+" "+String(Humedad);
    ESCRIBE_PUERTO_SERIE(Datos)

 */
#ifndef SEGAINVEX_SCPI_SERIAL_H_INCLUDED
#define SEGAINVEX_SCPI_SERIAL_H_INCLUDED
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif
/*******************************************************************************
							CONSTANTES EXPORTADAS
Hay que tener en cuenta que el buffer de puerto serie de Arduino es 64 bytes							
Por lo que BUFFCOM_SIZE y LONG_SCPI no pueden ser mayores de 64 bytes.
Estas constantes pueden ser redefinidas.
********************************************************************************/
// ESTAS 3 CONSTANTES SON LAS ÚNICAS QUE SE PUEDEN EDITAR
#define BUFFCOM_SIZE 64 //Longitud del buffer de lectura del comando recibido
#define LONG_SCPI 32 // Longitud máxima del comando sin contar parámetros
#define MAXERR 3   // Profundidad de la pila de errores
//Puerto serie a utilizar
#define PSERIAL  0
#define PSERIAL1 1
#define PSERIAL2 2
#define PSERIAL3 3
#define PSERIALUSB 4
/*******************************************************************************
						DEFINICION DE TIPOS EXPORTADOS
*******************************************************************************/            
//TIPOS PARA LA ESTRUCTURA DEL MENU DE SEGAINVEX-SCPI
//typedef void (*tpf)();// tpf es ahora puntero a función
// Declaración de tipo estructura para menús de comandos SEGAINVEX-SCPI
/*typedef*/ struct tipoNivel
{             
  int   NumNivInf;// Número de niveles por debajo de este
  const char *largo, *corto;// Nombre largo y nombre corto del comando
  void (*pf)();// Puntero a función a ejecutar
  tipoNivel *sub;// Puntero a la estructura de niveles inferiores
};
// TIPO PARA EL ARRAY DE ERRORES
typedef String tipoCodigoError[]; //Array de Strings
/********************************************************************************
      PROTOTIPOS DE FUNCIONES DE SEGAINVEX-SCPI QUE TIENEN QUE SER VISIBLES
*********************************************************************************/
/*********************************************************************************
	Funcion que gestiona la pila de errores
	El parámetro de entrada es el número del error a poner el la pila de errores
**********************************************************************************/
extern void errorscpi(int); // 
/*********************************************************************************
	Funcion principal de SEGAINVEX-SCPI
	Se debe ejecutar cuando se detecten datos en el puerto serie
**********************************************************************************/
extern void scpi(HardwareSerial *);  
/*********************************************************************************
	* Funcion para copiar el nombre del sistema en IdentificacionDelSistema
**********************************************************************************/
extern void scpiIDN(String);
/**********************************************************************************
 Funciones auxiliares que se pueden utilizar para leer los parámetros
 que van con el comando. Si el comando va seguido de ? devuelve el valor
 de las variables.
 Leen el/los parámetros del array de char FinComando[]. El carácter FinComando[0] 
 se espera que sea un espacio. A continuación tiene que estar el parámetro a leer.
*********************************************************************************/
/*******************************************************************************
	Cambia la variable entera cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera, el valor máximo, el valor mínimo.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
*******************************************************************************/
extern int cambia_variable_int_del_sistema(int *,int,int);//
/*******************************************************************************
	Cambia la variable entera discreta cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera,array de entero con los valores posibles,
	tamaño del array.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
*******************************************************************************/
extern int cambia_variable_int_discreta_del_sistema(int *,int*,int);//
/*******************************************************************************
	Cambia la variable booleana cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera, el valor máximo, el valor mínimo y el
	número de error a anotar si la variable está fuera de rango.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
********************************************************************************/	
extern int cambia_variable_bool_del_sistema(bool *);
/*******************************************************************************
	Cambia la variable double cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable float, el valor máximo, el valor mínimo.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable. 
*******************************************************************************/
extern int cambia_variable_double_del_sistema(double *,double,double);
/******************************************************************************
 Funciones SCPI del menú principal
 *****************************************************************************/
void scpi_error(void);// Envía el ultimo error
void scpi_idn(void);// Identifica el instrumento
void scpi_opc(void);// Devuelve un 1 al PC
void scpi_cls(void);// Borra la pila de errore
/********************************************************************************
   VARIABLES GLOBALES EXPORTADAS DE SEGAINVEX-SCPI QUE TIENEN QUE SER VISIBLES
  Son globales todos los modulos con el include a segainvex_scpi_Serial.h  						
********************************************************************************/
//Puerto serie a utilizar
extern HardwareSerial *MiPuerto;
//Array que contiene los parámetros del comando
extern char *FinComando;// Puntero al final del comando para leer parámetros
//Estas 2 variable tienen que estar definida en el código del usuario.
extern tipoNivel Raiz[]; //Array de la estructura raiz de comandos
extern tipoCodigoError CodigoError;//Puntero al array de cadenas explicativas del error
/********************************************************************************
 * Macros para generar los menús de comandos
*********************************************************************************/      
// Para definir submenús 
#define SCPI_SUBMENU(X,Y) sizeof(X)/sizeof(*X), #X,#Y,NULL,X,  
// Para definir comandos
#define SCPI_COMANDO(X,Y,Z) 0, #X,#Y,Z,NULL, //Para definir comandos 
//Para definir el nivel raiz de comandos
#define SCPI_NIVEL_RAIZ tipoNivel Raiz[]={sizeof(NivelDos)/sizeof(*NivelDos),"","",NULL,NivelDos};
/************************************************************************************
  Macros para leer y escribir el pueto serie actual
**************************************************************************************/
#define LEE_PUERTO_SERIE if(MiPuerto)\
 locCom=MiPuerto->readBytesUntil('\r',buffCom, BUFFCOM_SIZE);
//Macro para enviar al pueto serie
#define ESCRIBE_PUERTO_SERIE(X) if (MiPuerto) MiPuerto->println(X);
/**************************************************************************/
/*
NOTAS:
1)  El buffer de recepción de Arduino es de 64 bytes, por lo que hay que dimensionar
	la cadena de recepción como mucho a 64 bytes. BUFFCOM_SIZE=<64 

2)  Si se lee el puerto serie desde Arduino con: 	
        Serial.readBytesUntil('r',buffer,tamaño_buffer);
	  hay que	asegurarse de que el terminador con que el PC cierre las cadenas a enviar
	  sea '\r'.
3)	Si Arduino lee con Serial.readBytesUntil('\r',buffer,tamaño_buffer);.
	Esta función no mete	el terminador '\r' en la cadena que devuelve, por lo que 
	hay que ponerselo, ya que segainvex_scpi_Serial necesita un terminador para dar 
  por buena una cadena. Al hacerlo hay que incrementar la variable que indica
	la longitud del comando "locCom" en 1. 
						
  Se ha definido un tipo de estructura denominado "tipoNivel". Las variables de este 
  tipo puede contener un nivel de comandos o un comando.
  
  Si contiene un nivel, el formato es:
  
  int número_de_comandos_del_nivel,
  char[] Nombre_largo_del_nivel,  
  char[] Nombre_corto_del_nivel,
  (*tpf)() NULL;                    //puntero a función   
  tipoNivel* Nombre_del_nivel,		//puntero a tipoNivel
  
  Y si contiene un comando, el formato es:
  
  int 0,
  char[] Nombre_largo_del_comando,
  char[] Nombre_corto_del_comando,
  (*tpf)() funcion_a_ejecutar,       //puntero a función
  tipoNivel* NULL,					//puntero a tipoNivel  
  
  Como se vé la diferencia es que en un nivel, el puntero a función está a NULL, ya que
  al no ser un comando, no ejecuta ninguna función. Si es un comando, su puntero a función
  apunta a la función que se quiere ejecutar con ese comando. Pero su puntero a
  "tipoNivel" está a NULL, ya que de el no cuelga un nivel, porque es solo un comando.
  
  Se declaran array de variables tipoNivel. Siempre ha de existir este array:
  
  tipoNivel Raiz[] = {sizeof(NivelDos)/(5*sizeof(int)),"","",NULL, NivelDos};
	  
  Que tiene una única estructura "tipoNivel" y siempre el mismo formanto. 
  Es el nivel raiz del menú de submenús y comandos.
  
  Otro array que debe existir siempre es el denominado "NivelDos" (como se ve, 
  último miembro de la única estructura del "Raiz") 
	  
  El formato del array de estructuras "tipoNivel" "NivelDos" es:
  tipoNivel NivelDos[] ={
	  estructura_nivel_1,estructura_nivel_2,...,estructura_nivel_n,
	  estructura_comandos_1,estructura_comandos_2,..,estructura_comandos_n,
  };
  Cada nivel_estructura_i y nivel_comando_i tiene los formatos vistos arriba.
  
      El formato de cada array con un nivel de comandos es:
	tipoNivel NOMBRE_SUBNIVEL1[]={ 
		estructura_comandos_1,estructura_comandos_2,..,estructura_comandos_n,
  };
  
  Hay que añadir tantos arrays  "tipoNivel" como se necesiten. Y a su vez, en los
  arrays incluir tantos comandos como se necesiten. 
  
  Todo esto se puede declara en un fichero .h o dentro del .cpp, lo que resulte
  más apropiado en cada caso.
*/

#ifdef __cplusplus
    }
#endif

/**************************************************************************/
#endif // SEGAINVEX_SCPI_SERIAL_H_INCLUDED
/***********************************FIN************************************/
