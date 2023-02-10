#include <unistd.h>
#include <lib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/select.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

#define TRUE 1
#define FALSE !TRUE

static const char* SHELL_VERSION = "1.0";

/* definicion de las estructuras de datos de entrada de usuario */
#define BUFFER_MAX_LONG 50              // tamaño maximo del buffer (50 chars)
static char entradaUsuario = '\0';
static char buffer[BUFFER_MAX_LONG];    // buffer para la entrada del usuario
static int bufferChars = 0;

static char *comandoArgv[5];       // vector de cadenas que componen el comando dado
static int comandoArgc = 0;        // contador de las palabras del comando dado

/* shell pid, default terminal */
static pid_t SHELL_PID;
static int SHELL_TERMINAL, SHELL_ES_INTERACTIVO;

#define BASES_DEDATOS "/home/minixDB/src/basesdedatos.dat"
#define DATOS_USUARIOS "/home/minixDB/src/datosusuarios.dat" //ruta del archivo con usuarios para inicio de sesion

bool loggeado = false; //variable para saber si el usuario esta logeado.
					   //TRUE = tiene permisos para acceder a las opciones del gestor
					   //FALSE = no debe ser posible hacer nada

/*********************************************************************
 * PROMPT
 *********************************************************************/

/* despliega la pantalla de bienvenida */
void pantallaBienvenida();
/* despliega el prompt de nuestra terminal */
void shellPrompt();

/*********************************************************************
 * FUNCIONES DE ENTRADA DE USUARIO
 *********************************************************************/

/* obtiene una linea de texto digitada por el usuario */
void getLineaTexto();
/* rellena el arreglo comandoArgv[] con cada palabra que compone la linea de comandos */
void rellenarComando();
/* libera el buffer y comandoArgv[] */
void destruirComando();

/*********************************************************************
 * FUNCIONES DEL SHELL
 *********************************************************************/

/* maneja el comando dado: es un comando incorporado o un programa externo? */
void manejarComandoUsuario();
/* ejecuta un comando de sistema */
void ejecutarComando(char *comando[]);
/* crear un procesos hijo */
void crearProceso(char *comando[]);
/* inicializa el shell */
void init();
int verificarComandosBuiltIn();
void cambiarDirectorio();
void ShellPrincipal();
void limpiarPantalla(); //metodo para limpiar pantalla
void crearDirectorio(); //crea un nuevo directorio
//void CrearBaseDeDatos(); //crea base de datos
//int ValidarExistenciaBD(string nombreBD); //valida la existencia de una base de datos
void ListarBD(); //lista la base de datos
void MenuAyuda();
void MenuInicio();
void MenuAyudaBD();
void ListarTablas();
void MenuAyudaTablas();
void AgregarUsuario();
int ValidarCredenciales();
int ExisteUsuario(string usuario);
int ValidarExistenciaBD(string nombreBD);
void CrearBaseDeDatos(string nombreBDTmp);
void EliminarBD(string nombreBDTmp);
void ActualizarBD(string nombreBD, string nombreNuevo);
void CrearTabla(string nombreTabla);
void EliminarTabla(string nombreT);
void ActualizarTabla(string nombreT, string nombreTN);
bool vacio(ifstream& archivo);
int ValidarExistenciaTabla(string nombreBD, string nombreTabla);
void MostrarTabla(string nombreT, string nombreBD);
void IngresarRegistro(string nombreT);
void MostrarRegistros(string nombreT);
void MenuAyudaRegistros();
int ValidarId(string nombreT, string nombreBD, string id);
void MostrarRegistro(string nombreT, string nombreBD);