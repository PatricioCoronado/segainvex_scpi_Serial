/**********************************************************************************
	DEPARTAMENTO DE ELECTRÓNICA DE SEGAINVEX. UNIVERSIDAD AUTONOMA DE MADRID				
	LIBRERIA PARA ARDUINO segainvex_scpi_Serial SERIAL V2.2
	SISTEMA PARA COMUNICAR UNA COMPUTADORA CON ARDUINO MEDIANTE PUERTO SERIE 
	Fichero de código Segainvex_scpi_Serial.cpp
***********************************************************************************/
/**********************************************************************************
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
    along with segainvex_scpi_Serial.  If not, see <http://www.gnu.org/licenses/>
************************************************************************************/
/*********************************************************************************** 
	Funciona con todos los puertos Serial tipo HardwareSeria no con SerialUSB.
 ***********************************************************************************/
/************************************************************************************
  ESTE FICHERO NO HA DE SER EDITADO. CONTIENE LAS FUNCIONES QUE IMPLEMENTAN 
  segainvex_scpi_Serial. 
**************************************************************************************/
#include <Arduino.h>
#include "segainvex_scpi_Serial.h"
/************************************************************************************
                  Variables privadas para implementar SEGAINVEX-SCPI
**************************************************************************************/
char buffCom[BUFFCOM_SIZE]; // Buffer leido del puerto serie con el comando
int locCom; // Cantidad de caracteres en el buffer
unsigned char indComRd = 0;
HardwareSerial * MiPuerto=&Serial;
//Cadena donde el usuario puede escribir el nombre de su sistema.
String IdentificacionDelSistema="identificacion del sistema";
/************************************************************************************
                  Variables públicas para implementar SEGAINVEX-SCPI
**************************************************************************************/
char *FinComando;   // Puntero al final del comando para leer parámetros
/***************************************************************************************
							FUNCIONES QUE IMPLEMENTAN SEGAINVEX-SCPI
****************************************************************************************/
/***************************************************************************************
    Lee caracteres del buffer buffCom[]
****************************************************************************************/
char lee_caracter()  {
  unsigned char carRecv;
  if (locCom == 0) return (0x00); //Si no hay caracteres en buffer salimos con caracter nulo 0
  carRecv = buffCom[indComRd];  //Si lo hay,leemos el "char" correspondiente del buffer
  indComRd++;                   //Apuntamos el indice de lectura al siguiente caracter de buffer
  locCom--;                     // Descontamos contador de "caracteres pendientes" en buffCom
  if (indComRd == BUFFCOM_SIZE) indComRd = 0; // Si se sale del buffCom lo ponemos al pricipio
  return (carRecv);             // Salimos devolviendo el caracter leido
}
/***************************************************************************************
  Busca un caracter separador : ; \n \r
***************************************************************************************/
unsigned char separador(char a) {
  if (a == ':' || a == ';' || a == '\n' || a == '\r') return (1);
  return (0);
}
/**************************************************************************************
  Busca otros caracteres válidos . ? * - + '
***************************************************************************************/
unsigned char valido(char a) {
  if (a == '?' || a == '.' ||  a == '*'  || a == '-' || a == '+' || a == ',' ) return (1);
  return (0);
}
/****************************************************************************************
  Verifica si el carácter es válido. (alfanamerico, separador ó puntuación)
****************************************************************************************/
char CaracterBueno(char caracter) {
  if (!isalnum(caracter))       //Si no es alfanumerico...
    if (!isspace(caracter))    // ni un espacio...
      if (!separador(caracter)) // ni un separador admitido...
        if (!valido(caracter)) // ni un caracter especial...
          return (0);// no es un caracter bueno.
  return (toupper(caracter));   //Si es un caracter bueno lo pasa a mayuscula y lo devuelve
}
/********************************************************************************************
  Gestiona la pila de errores de segainvex_scpi_Serial
********************************************************************************************/
void errorscpi(int codigo) {
  static unsigned char errores[MAXERR];
  static int numero_errores = 0;
  // Si el parámetro es -1, limpia la pila de errores 
  if (codigo == -1) {numero_errores = 0; return;}
  if (codigo == 0) //Si el parámetro es 0..
    if (numero_errores)	 
    {  //...y hay errores.
      numero_errores--;  // Retrocede la pila de errores apuntando al siguiete error
      ESCRIBE_PUERTO_SERIE(CodigoError[errores[numero_errores]])
    } //if (numero_errores)
	  else // Si no hay erroes se comunica.
	  {
      ESCRIBE_PUERTO_SERIE("0 No hay errores")
    }
  else {//Si el parametro no es ni -1 ni 0 se almacena un error en la pila
    errores[numero_errores] = codigo;
    if (numero_errores < MAXERR - 1) numero_errores++;
  }
}
/*******************************************************************************************
  Lee comandos. Se pasa como parametro de entrada una cadena con un comando.
  Si encuentra un comando valido de un submenu, apunta a ese menu y si es
  un comando con puntero a funcíon, la ejecuta. Si no encuentra ni una cosa 
  ni otra, apunta al menú raiz. Y si en este tampoco esta el comando, sale 
  con error
********************************************************************************************/
void LeeComandos(char *cadena)
{
  unsigned char a, n, b/* ,NumComandos*/;
  static /*struct*/ tipoNivel  *PNivel = Raiz, *PNivelSup = Raiz;
  while (*cadena == ' ')cadena++; //Limpiamos los blancos del comienzo
  // Si comienza la cadena con ':' hay que rastrear en el raiz 
  if (cadena[0] == ':') {PNivel = Raiz;   return;}// Apunta al raiz y sale
  n = strcspn(cadena, "? :"); // Busca si existen parametros despues de la funcion y 
  if (n < strlen(cadena))    
	FinComando = cadena + n;// si existen apunta FinComando a la cadena que los contiene
	b = PNivel->NumNivInf; // b contiene el numero de comandos en este nivel
  // Compara el comando con cada uno de los que hay en el nivel
  for (a = 0; a < b; a++) {  
    if (
		(n == strlen(PNivel->sub[a].corto) && !strncmp(cadena, PNivel->sub[a].corto, n))
		 ||
        (n == strlen(PNivel->sub[a].largo) && !strncmp(cadena, PNivel->sub[a].largo, n))
	   ) 
		{
		// Si encuentra el comando en el nivel apuntado
		if (PNivel->sub) {//Si hay un nivel debajo...
			PNivelSup = PNivel;//..guarda el nivel actual..        
			PNivel = &(PNivel->sub[a]);//.. y lo apunta bajando un nivel.
		} //Pero si en vez de un nivel inferior hay una función:
		if (PNivel->pf!=NULL) {  // Si hay una funcion a ejecutar...
			PNivel->pf(); // ...la ejecuto. AQUÍ SE EJECUTA LA FUNCIÓN
			PNivel = PNivelSup;// Restauro el puntero un nivel por encima
		}   
      return;// Tanto si ejecuta el comando como si cambia el nivel apuntado, sale
    }//if ((n == strlen(....
  }//for (a = 0; a < b; a++) 
  // Busqueda del comando en el raiz, después de no encontrarlo en el nivel actual
  if (PNivel != Raiz) { // Si el comando no existe en este nivel...
    PNivelSup = PNivel; // ...guarda el nivel actual...
    PNivel = Raiz;// ...y voy al raiz y..
	LeeComandos(cadena);// busco llamando de forma recursiva a "LeeComando"
    if (PNivel == Raiz) PNivel = PNivelSup; // Si tampoco lo encontró en el raiz restauro el nivel
  } //if (PNivel != Raiz)                                   
  else errorscpi(2);     // Si ya estaba en el raiz el comando no existe
}
/**************************************************************************************************
			FUNCION DE ENTRADA A segainvex_scpi_Serial
			segainvex_scpi_Serial busca comandos en la cadena de entrada.Las envía a 
      analizar a LeeComandos. El parámetro de entrada es el puerto a utilizar
**************************************************************************************************/
void scpi(HardwareSerial *PuertoSerie)
{
	MiPuerto=PuertoSerie;
  locCom=0; // Pone a 0 el índice de la cadena por la que recibe del puerto serie
	indComRd = 0; // Pone a cero el indice de la cadena de comando que va a leer
	// Lee la cadena del comando y el nº de caracteres.
	LEE_PUERTO_SERIE //Macro que lee el puerto pasado como argumento
	buffCom[locCom]='\r'; // Añade un terminador porque readBytesUntil se lo quita
	buffCom[locCom+1]='\0'; // Añade un fin de cadena
	locCom++;// Ajusta la longitud de la cadena porque le he añadido un caracter más
	//
	// Cuando se envia un comando de un submenu SUBMENU:COMANDO [lista de parametros], en una
	// pasada del bucle while se procesa SUBMENU: y en otra :COMANDO dejando "FinComando" 
	// apuntando a la "lista de parámetros" 
	while(locCom)// Mientras haya caracteres en el buffer, sigo ejecutando buscando comandos
	{ 
		char serie[LONG_SCPI];//En "serie" se va concatenando los caracteres leidos
		char c; //en "c" se copia el caracter leido desde buffCom a la cadena serie[]
		unsigned char n = 0;//n es el indice de la cadena serie
		do{// Lee caracteres hasta que encuentra un caracter separador 
			c=CaracterBueno(lee_caracter());//verifica si el carácter apuntado en buffCom es valido  
			if (!c){errorscpi(1);return;}//Si no es valido da un error y sale
			serie[n] = c; //Si el caracter es valido lo mete en la cadena "serie"
			if (n < LONG_SCPI - 1){n++;}//si no estamos al final físico de la cadena avanzamos el indice
			else{errorscpi(3);return;} //Si estamos al final de la cadena da un error y sale
		 }while (!separador(c));//Mientras no encontremos un separador seguimos recorriendo la cadena
			if (c != ':') n--; // En caso de ser un serapador distinto de : lo machaco con un 0x00
				serie[n] = 0;// Los : son transmitidos a EjecutaComandos. otros separadores no
			LeeComandos(serie);//Una vez encontrada una cadena valida la envia a ejecutar
	}//while(locCom)
}//scpi(void)
/*********************************************************************
 * Funcion para copiar el nombre del sistema en IdentificacionDelSistema
**********************************************************************/
void scpiIDN(String identificacion)
{
  IdentificacionDelSistema=identificacion;
}
/**********************************************************************
	FUNCIONES públicas segainvex_scpi_Serial COMUNES A TODOS LOS SISTEMAS 
***********************************************************************/
/**********************************************************************
    Comando: *IDN"
    Función:240
  Envia por el puerto una cadena que identifica al sistema
 *********************************************************************/
//extern String IdentificacionDelSistema;
void scpi_idn(void)
{
    ESCRIBE_PUERTO_SERIE(IdentificacionDelSistema)
}	
/**********************************************************************
    Comando: ERROR ó ERR
    Función:243
    Envia por el puerto el último error registrado por SEGAINVEX-SCPI
 *********************************************************************/
void scpi_error(void)
{
  if (FinComando[0] == '?')errorscpi(0);
	else errorscpi(4);
}
 /**********************************************************************
  Comando:*OPC
  Función:248
  Envia por el puerto un uno
 *********************************************************************/
void scpi_opc(void)
{
	ESCRIBE_PUERTO_SERIE("1")
}
/**********************************************************************
    Comando: CLS
    Función:255
    Limpia la pila de errores de SEGAINVEX-SCPI
 *********************************************************************/
void scpi_cls(void){errorscpi(-1);}
/*********************************************************************/


/************************************************************************************************
		FUNCIONES AUXILIARES PARA CAMBIAR VALORES ENTEROS, DOBLES Y BOOLEANOS EN EL SISTEMA
 
 Estas funciones hay que ejecutarlas dentro de las funciones que responden a comandos 
 scpi y sabemos que hay UN parámetro en el buffer "FinComando"
 Leen un parámetro del array tipo char "FinComando[]".
 Si el carácter FinComando[0] es '?' devuelve el valor actual del parámetro. Si es un espacio,
 lee el parámetro que tiene que estar a continuación, si no está da un error. Si no es ninguno
 de los anteriores da un error 6. 
 *************************************************************************************************/
/**************************************************************************************************
   Cambia la variable del sistema tipo double cuya dirección se pasa como argumento. Además hay que 
   pasarle el valor máximo, el mínimo. Devuelve 1 si cambió la variable. 0, si no cambió la variable
   por errores y 2 si devolvió el valor de la variable.
 **************************************************************************************************/
int cambia_variable_double_del_sistema(double *pVariableDelSistema,double Maximo, double Minimo)
{
  double Var; //variable intermedia
  // Si solo pregunta por el dato, se responde y sale
  if(FinComando[0]=='?')  
  {
	  ESCRIBE_PUERTO_SERIE(*pVariableDelSistema) 
    return 2;
  }      
  if(FinComando[0]==' ')// Si el primer carácter de FinComado es 'espacio' lee el parámetro
  {   
    Var=strtod(FinComando,NULL);//Lee un double de la cadena FinComando
  }   
  else {errorscpi(4);return 0;} // Si FinComando no empieza por 'espacio'¡Error!Parámetros inexistentes.
// Si el número de parámetros leidos es correcto comprueba los rangos
  if ((Var <= Maximo && Var >= Minimo)){*pVariableDelSistema=Var;return 1;}//si esta en rango,cambia la variable
  else  {errorscpi(6);return 0;}// si no está en rango da el error correspondiente y sale
}
/************************************************************************************************
   Cambia la variable del sistema tipo int cuya dirección se pasa como argumento. Además hay que
   pasarle el valor máximo y el mínimo.
   Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
   Y 2 si devolvió el valor de la variable.
 ***********************************************************************************************/
int cambia_variable_int_del_sistema(int *pVariableDelSistema,int Maximo, int Minimo)
{
  unsigned int np; // número de parámetros leido por sscanf
  int Var; //variable intermedia
  // Si solo pregunta por el dato, se responde y sale
  if(FinComando[0]=='?')  
	{
	  ESCRIBE_PUERTO_SERIE(*pVariableDelSistema) 
    return 2;
  }      
   // Si el primer carácter de FinComado es 'espacio' lee los parámetros
    if(FinComando[0]==' ')  
  {   
    np = sscanf(FinComando,"%u",&Var);// Lee la cadena de parámetros
    if(np != 1){errorscpi(5);return 0;}// Si no lee 1 parámetro¡Error!:Formato de parámetro no válido
  }   
  else {errorscpi(4);return 0;} // Si el comando no empieza por 'espacio'¡Error!Parámetros inexistentes.
// Si el número de parámetros leidos es correcto comprueba los rangos
  if ((Var<=Maximo && Var>=Minimo)){*pVariableDelSistema=Var;return 1;} //si esta en rango, cambia la variable
  else  errorscpi(6);return 0;// si no está en rango da el error "parámetro fuera de rango"
}
/*********************************************************************************************
 Cambia una variable tipo int que puede tener un conjunto discreto de valores
 Se pasa como argumento un array con los posibles valores y un entero con 
 la cantidad de posibles valores.
 Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
 Y 2 si devolvió el valor de la variable.
 *********************************************************************************************/
int cambia_variable_int_discreta_del_sistema
(int *pVariableDelSistema,int ValoresPosibles[],int NumeroValores)// 
{
  unsigned int np; // número de parámetros leido por sscanf
  int i, Var; //variable intermedia
  // Si solo pregunta por el dato, se responde y sale
  if(FinComando[0]=='?')  
  {
    ESCRIBE_PUERTO_SERIE(*pVariableDelSistema) 
    return 2;
  }      
   // Si el primer carácter de FinComado es 'espacio' lee los parámetros
    if(FinComando[0]==' ')  
  {   
    np = sscanf(FinComando,"%u",&Var);// Lee el nuevo valor
    if(np != 1){errorscpi(5);return 0;}// Si no lee 1 parámetro¡Error!:Formato de parámetro no válido
  }   
  else {errorscpi(4);return 0;} // Si el comando no empieza por 'espacio'¡Error!Parámetro inexistente.
// Si se ha leido un parámetro se testea su valor
  for(i=0;i<NumeroValores;i++)
  {
  if (Var == ValoresPosibles[i]) // Si encuentra una coincidencia cambia el valor y sale
  {*pVariableDelSistema=Var;return 1;}
  }
  errorscpi(6);// si no está en la lista de valores posible da el error "parámetro fuera de rango"
  return 0;
}
/************************************************************************************************
   Cambia la variable del sistema tipo booleana cuya dirección se pasa como argumento
 ***********************************************************************************************/
int cambia_variable_bool_del_sistema(bool *pVariableDelSistema)
{
  unsigned int np; // número de parámetros leido por sscanf
  int Var; //variable intermedia
  // Si solo pregunta por el dato, se responde y sale
  if(FinComando[0]=='?')  
	  {
       ESCRIBE_PUERTO_SERIE(*pVariableDelSistema)  
       return 2;
	  }      
   // Si el primer carácter de FinComado es 'espacio' lee los parámetros
    if(FinComando[0]==' ')  
  {   
    np = sscanf(FinComando,"%u",&Var);// Lee la cadena de parámetros
    if(np != 1){errorscpi(5);return 0;}// Si no lee 1 parámetro¡Error!:Formato de parámetro no válido
  }   
  else {errorscpi(4);return 0;} // Si el comando no empieza por 'espacio'¡Error!Parámetros inexistentes.
// Si el número de parámetros leidos es correcto comprueba los rangos
  if ((Var == 0 || Var == 1)){*pVariableDelSistema=Var;return 1;} // si vale 0 ó 1, cambia_la variable
  else  {errorscpi(6); return 0;}// si no está en rango da el error de "parámetro fuera de rango"
} 
//****************************************** FIN ************************************************/
