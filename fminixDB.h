/*********************************************************************
 * PROMPT
 *********************************************************************/

void pantallaBienvenida()
{
        cout << "\n\t*************************************************************\n";
        cout << "\t\tBIENVENIDO AL GESTOR DE BASE DE DATOS MINIXDB\n";
        cout << "\t*************************************************************\n";
        cout << "\n";
}

/**
 *	despliega el prompt de nuestro shell
 */
void shellPrompt()
{
	cout << " MinixDB >> ";
}

void init()
{
        SHELL_PID = getpid();    // retorna el pid del shell
        SHELL_TERMINAL = STDIN_FILENO;     // terminal = STDIN
        SHELL_ES_INTERACTIVO = isatty(SHELL_TERMINAL); // el shell es interactivo si STDIN es el terminal
        //isatty - testea si el descriptor de archivo refiere a el terminal
        /*STDIN_FILENO es una macro definida en <unistd.h>.
         * Al menos para sistemas compatibles con POSIX,  es requerido que sea 0
        /STDIN_FILENO es el descriptor de archivo de entrada estandar por defecto el cual es 0
         * es esencialmente una directiva definida como cero*/
        if (SHELL_ES_INTERACTIVO) {     // es el shell interactivo?
        } else {
        		cout << "No es posible hacer el shell interactivo. Saliendo..\n";
                exit(EXIT_FAILURE);
        }
}

/*********************************************************************
 * FUNCIONES DE ENTRADA DE USUARIO
 *********************************************************************/

/**
 *	obtiene una linea de texto del teclado
 */
void getLineaTexto()
{
		destruirComando();    // limpia el buffer y la linea de comandos
		while ((entradaUsuario != '\n') && (bufferChars < BUFFER_MAX_LONG)) {
                buffer[bufferChars++] = entradaUsuario;
                entradaUsuario = getchar();
        }
        buffer[bufferChars] = 0x00;    // null para terminar el buffer
        rellenarComando();
}

/**
 * rellena el array comandoArgv con todas las palabras
 * de la linea de comandos (un espacio  " " es usado como separador)
 */
void rellenarComando()
{
        char* pbuffer;            // un puntero al buffer
        pbuffer = strtok(buffer, " ");
        /*La funcion de la libreria de C char *strtok(char *str, const char *delim)
        * divide la cadena str en una serie de tokens usando el delimitador delim*/

        while (pbuffer != NULL) {   // mientras el puntero no sea NULL
                comandoArgv[comandoArgc] = pbuffer;   // rellenar el array comandoArgv
                pbuffer = strtok(NULL, " "); // Posteriores llamadas
                comandoArgc++;
        }
}

/**
 *	destruye el array de chars, dejando que el usuario introduzca otra linea de comandos
 */
void destruirComando()
{
        while (comandoArgc != 0) {
                comandoArgv[comandoArgc] = NULL;  // elimina el puntero a la cadena
                comandoArgc--;        // decrementa el numero de las palabras en la linea
        }
        bufferChars = 0;              // reinicia el conteo de chars en el buffer
}

/*********************************************************************
 * FUNCIONES DEL SHELL
 *********************************************************************/

/**
 * ejecucion generica void
 */
void manejarComandoUsuario()
{
	if (verificarComandosBuiltIn()) {
        crearProceso(comandoArgv);
	}
}

/**
 * comandos incorporados:
 * retorna 1 si un comando built-in es ejecutado de lo contrario 0
 */
int verificarComandosBuiltIn()
{

        if (strcmp("cd", comandoArgv[0]) == 0) {
                cambiarDirectorio();
                return 1;
        }
        //salir
        if (strcmp("salir", comandoArgv[0]) == 0) {
        		cout << "abandonando programa...\n";
        		exit(0);
        		return 1;
        }
        //limpiar pantalla
        if (strcmp("limpiar", comandoArgv[0]) == 0) {
        	limpiarPantalla();
        	return 1;
        }
        //muestra un menu de ayuda
        if (strcmp("ayuda", comandoArgv[0]) == 0) {
        	MenuAyuda();
            return 1;
        }
        //inicia sesion
        if (strcmp("iniciar", comandoArgv[0]) == 0 && strcmp("sesion", comandoArgv[1]) == 0) {
        	 if(loggeado){
        	     cout << "\n\t\tYa existe una sesion activa" << endl;
        	 }else{
        		 if(ValidarCredenciales()==1){
        			 loggeado = true; //si se loggea significa que si posee permisos
        			 cout << endl;
        			 cout << "\n\t\tInicio de sesion exitoso" << endl << endl;
        			 return 1;
        		  }else{
        			  cout << endl;
        		      cout << "\n\t\tCredenciales incorrectas..." << endl << endl;
        		  }
        	 }
         }
        if (strcmp("cerrar", comandoArgv[0]) == 0 && strcmp("sesion", comandoArgv[1]) == 0) {
        		if(!loggeado){
        			cout << "\n\t\tNo existe sesion activa..." << endl;
                return 1;
        		}else{
                    loggeado = false; //la variable regresa a falso
        			cout << "\n\t\tAdios... :'(" << endl;
        		}
         }
        //crea cuenta para el usuario
        if (strcmp("crear", comandoArgv[0]) == 0 && strcmp("cuenta", comandoArgv[1]) == 0) {
            AgregarUsuario();
            return 1;
        }
        //muestra la sintaxis que se debe seguir para manejar las bases de datos (CREAR, ELIMINAR, RENOMBRAR)
        if (strcmp("opciones", comandoArgv[0]) == 0 && strcmp("bd", comandoArgv[1]) == 0) {
        	MenuAyudaBD();
        	return 1;
        }
        if (strcmp("opciones", comandoArgv[0]) == 0 && strcmp("tablas", comandoArgv[1]) == 0) {
        	MenuAyudaTablas();
        	return 1;
        }
        if (strcmp("opciones", comandoArgv[0]) == 0 && strcmp("registros", comandoArgv[1]) == 0) {
        	MenuAyudaRegistros();
        	return 1;
        }
        //crea base de datos: crear basededatos NOMBRE
        if (strcmp("crear", comandoArgv[0]) == 0 && strcmp("basededatos", comandoArgv[1]) == 0 ) {
        	if(loggeado){
        		string nombreBDTmp(comandoArgv[2]);
        		CrearBaseDeDatos(nombreBDTmp);
        		return 1;
        	}else{
        		cout << "\n\t\tNo posee los permisos necesarios" << endl;
        	}

         }
        //renombrar base de datos: renombrar basededatos NOMBREEXISTENTE NOMBRENUEVO
        if (strcmp("renombrar", comandoArgv[0]) == 0 && strcmp("basededatos", comandoArgv[1]) == 0 ) {
        	if(loggeado){
        		string nombreBD(comandoArgv[2]);
        		string nombreNuevo(comandoArgv[3]);
        		ActualizarBD(nombreBD, nombreNuevo);
        		return 1;
        	}else{
        		cout << "\n\t\tNo posee los permisos necesarios" << endl;
        	}

        }
        //eliminar base de datos: eliminar basededatos NOMBREEXISTENTE
        if (strcmp("eliminar", comandoArgv[0]) == 0 && strcmp("basededatos", comandoArgv[1]) == 0) {

              if(loggeado){
                  string nombreBDTmp(comandoArgv[2]);
                  EliminarBD(nombreBDTmp);
                  return 1;
               }else{
            	  cout << "\n\t\tNo posee los permisos necesarios" << endl;
               }

       }
       //mostrar bases de datos existentes: mostrar basesdedatos
        if (strcmp("mostrar", comandoArgv[0]) == 0  && strcmp("basesdedatos", comandoArgv[1]) == 0 ) {

        	if(loggeado){
        	  ListarBD();
        	  return 1;
        	}else{
        	   cout << "\n\t\tNo posee los permisos necesarios" << endl;
        	}

         }
        if (strcmp("crear", comandoArgv[0]) == 0 && strcmp("tabla", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreTabla(comandoArgv[2]);
                	CrearTabla(nombreTabla);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }

        }
        if (strcmp("actualizar", comandoArgv[0]) == 0 && strcmp("tabla", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
					string nombreTN(comandoArgv[3]);
					ActualizarTabla(nombreT, nombreTN);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }

        }
        if (strcmp("eliminar", comandoArgv[0]) == 0 && strcmp("tabla", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
                	EliminarTabla(nombreT);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }

        }
        if (strcmp("mostrar", comandoArgv[0]) == 0 && strcmp("tablas", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	ListarTablas();
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }

        }
        if (strcmp("mostrar", comandoArgv[0]) == 0 && strcmp("tabla", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
                	string nombreBD(comandoArgv[3]);
                	MostrarTabla(nombreT, nombreBD);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }
        }
        if (strcmp("insertar", comandoArgv[0]) == 0 && strcmp("registro", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
                	IngresarRegistro(nombreT);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }

        }
        if (strcmp("mostrar", comandoArgv[0]) == 0 && strcmp("registros", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
                	MostrarRegistros(nombreT);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }
        }
        if (strcmp("mostrar", comandoArgv[0]) == 0 && strcmp("registro", comandoArgv[1]) == 0 ) {
                if(loggeado){
                	string nombreT(comandoArgv[2]);
                	string nombreBD(comandoArgv[3]);
                	MostrarRegistro(nombreT,nombreBD);
                	return 1;
                }else{
                	cout << "\n\t\tNo posee los permisos necesarios" << endl;
                }
        }

        return 0;
}

/**
 * cambia el directorio actual
 */
void cambiarDirectorio()
{
	//chdir -> Cambia el directorio actual de trabajo
	//getenv -> Retorna una cadena conteniendo el valor de la variable de entorno cuyo nombre es
	//especificado como argumento
        if (comandoArgv[1] == NULL) {
                chdir(getenv("HOME"));            // simula un comando "cd"
        } else {
                if (chdir(comandoArgv[1]) == -1) {     // ir al nuevo directorio

                	cout << "no such directory\n";
                	//cout << comandoArgv[1];
                }
        }
}
/**
 * ejecuta un programa
 */
void ejecutarComando(char *comando[])
{
        if (execvp(*comando, comando) == -1)
                perror("error en execvp");
        	perror("");
}

/**
 * crear un proceso con fork y ejecuta un programa hijo
 */
void crearProceso(char *comando[])
{
        pid_t pid;
        pid = fork();
        switch (pid) {
        case -1:
                perror("error en fork");
                exit(EXIT_FAILURE);
                break;
        case 0:
             //   ejecutarComando(comando);
                exit(EXIT_SUCCESS);
                break;
        default:
                break;
        }
}

void ShellPrincipal(){
	 init();
	 pantallaBienvenida();
	 MenuAyuda();
	 shellPrompt();

	 while (TRUE) {
	  entradaUsuario = getchar();
	  switch (entradaUsuario) {
	   case '\n':
	   // shellPrompt();
	    break;
	   default:
	    getLineaTexto();
	    manejarComandoUsuario();
	    shellPrompt();
	    break;

	  }

	 }
	 printf("\n");

}
//metodo para limpiar pantalla
void limpiarPantalla(){
	system("clear");
}

void MenuAyuda(){
	cout << "\n\t\tComandos: " << endl;
	cout << "\n\t\tIniciar sesion -> iniciar sesion" << endl;
	cout << "\n\t\tCrear una cuenta -> crear cuenta" << endl;
	cout << "\n\t\tCerrar sesion -> cerrar sesion" << endl;
	cout << "\n\t\tVer opciones para bases de datos -> opciones bd" << endl;
	cout << "\n\t\tVer opciones para tablas -> opciones tablas" << endl;
	cout << "\n\t\tVer opciones para registros -> opciones registros" << endl;
	cout << "\n\t\tVer nuevamente estas opciones -> ayuda" << endl << endl;

}
void MenuAyudaBD(){
	cout << "\n\tSintaxis para: " << endl;
	cout << "\n\tcrear BD -> crear basededatos nombredelabase" << endl;
	cout << "\n\trenombrar BD -> renombrar basededatos NombreExistente NombreNuevo" << endl;
	cout << "\n\teliminar BD -> eliminar basededatos nombredelabase" << endl;
	cout << "\n\tlistar BD -> mostrar basesdedatos" << endl;
}
void MenuAyudaTablas(){
	cout << "\n\tSintaxis para: " << endl;
	cout << "\n\tcrear tabla -> crear tabla nombredelatabla" << endl;
	cout << "\n\trenombrar tabla -> actualizar tabla NombreExistente NombreNuevo" << endl;
	cout << "\n\teliminar tabla -> eliminar tabla nombredelatabla" << endl;
	cout << "\n\tlistar todas las tablas -> mostrar tablas" << endl;
	cout << "\n\tmostrar una tabla -> mostrar tabla nombretabla basededatos" << endl;
}
void MenuAyudaRegistros(){
	cout << "\n\tSintaxis para: " << endl;
	cout << "\n insertar registro a una tabla -> insertar registro nombretabla" << endl;
	cout << "\n mostrar todos los registros de una tabla -> mostrar registros nombretabla" << endl;
	cout << "\n mostrar un registro de una tabla -> mostrar registro nombretabla nombreBD" << endl <<endl;
}
int ExisteUsuario(string usuario){
	ifstream archivo(DATOS_USUARIOS); //variable para leer archivo
	string usuarioAux; //variable auxiliar que almacenará el usuario digitado
	string usuarioTmp; //variable temporal que retornará el archivo
	int encontrado = 0; //variable para determinar si se encontró usuario

	usuarioAux = "Usuario: "+usuario; //concatenamos Usuario: con la variable usuario para comparar
									  //la linea que retornará el archivo

	if(archivo.fail()){ //si el archivo no se puede abrir
		cout << "\t\tno se pudo abrir el archivo\n";
	}else{
		while(!archivo.eof() && encontrado == 0){
			getline(archivo, usuarioTmp);
			if(usuarioTmp == usuarioAux){
				encontrado = 1; //ya existe un usuario
			}else{
				encontrado = 0;
			}
		}
	}
	archivo.close();
	return encontrado;
}

void AgregarUsuario(){
	ofstream archivo; //variable para manejar nuestro archivo y escribir en él
	string usuario;
	string clave;
	string puerto;
	int longitud; //guardara la longitud de la clave a encriptar

	archivo.open(DATOS_USUARIOS,ios::app); //ruta del archivo y abrimos el archivo en modo añadir

	if(archivo.fail()){ //si el archivo no se puede abrir
		cout << "\t\tno se pudo abrir el archivo\n";
	}else{
		cout << endl;
		cout << "\t\tDigite el usuario:";
		cin >> usuario;
		cout << "\t\tDigite la clave: ";
		cin >> clave;
		cout << "\t\tDigite el puerto de escucha: ";
		cin >> puerto;
		cout << endl;
		if(ExisteUsuario(usuario) == 1){
			cout << "\t\tYa existe un usuario con ese nombre" << endl;
			//exit(1);
		}else{
			longitud = clave.size(); //captura la longitud de la clave digitada
			string claveCifrada;
			for(int i=0;i<longitud;i++){ //evalua caracter por caracter

				claveCifrada += (char)(clave[i]+10*2-5);
			}

			//escribe en nuestro archivo datosusuarios.dat
			archivo << "Usuario: " << usuario << endl;
			archivo << "Clave: " << claveCifrada << endl;
			archivo << "Puerto: " << puerto << endl;

			cout << "\t\tUsuario agregado con exito" << endl;
		}

	}

	archivo.close(); //cerramos el archivo
}

int ValidarCredenciales(){
	ifstream ArchivoUsuariosLec(DATOS_USUARIOS); //variable para manejar el archivo en modo lectura
	string usuario; //variable para usuario digitado
	string clave; //variable para clave digitada
	string puerto; //variable para puerto digitado
	string usuarioTmp; //almacena temporalmente el usuario digitado
	string claveTmp; //almacena temporalmente la clave digitada
	string puertoTmp; //almacena temporalmente el puerto digitado
	string usuarioAux; //auxiliar de usuario para comparar variable de archivo con variable digitada
	string claveAux; //auxiliar de clave para comparar variable de archivo con variable digitada
	string puertoAux; //auxiliar de puerto para comparar variable de archivo con variable digitada
	int  encontrado = 0; //sirve para verificar si se encontraron los registros. Se retorna en la funcion
	int longitud;
	//ArchivoUsuariosLec.open(DATOS_USUARIOS,ios::in); //abrimos el archivo en modo lectura

	if(ArchivoUsuariosLec.fail()){ //Si el archivo no se pudo abrir
		cout << "\t\tNo se pudo abrir el archivo";
		encontrado = 0;
	}else{
		cout << endl;
		cout << "\t\tUsuario: ";
		cin >> usuarioTmp;
		cout << "\t\tClave: ";
		cin >> claveTmp;
		cout << "\t\tPuerto de escucha: ";
		cin >> puertoTmp;
		cout << endl;

		longitud = claveTmp.size();

		string claveCifrada;

		for(int i=0;i<longitud;i++){ //evalua caracter por caracter

			claveCifrada += (char)(claveTmp[i]+10*2-5);
		}
		//variables sirven para poder comparar en la validacion y mantiene la estructura de
		//guardado que se ha manejado en el metodo AgregarUsuario(). Sirve para no ir leyendo
		//caracter por caracter y facilitar la busqueda
		usuario = "Usuario: "+ usuarioTmp;
		clave = "Clave: "+ claveCifrada;
		puerto = "Puerto: "+puertoTmp;
		//mientras archivo no llegue a su fin y variable encontrado valga 0
		while(!ArchivoUsuariosLec.eof() && encontrado == 0){
			//lee las lineas del archivo y las almacena en variables auxiliares para poder comparar
			getline(ArchivoUsuariosLec, usuarioAux);
			getline(ArchivoUsuariosLec, claveAux);
			getline(ArchivoUsuariosLec, puertoAux);
			//evaluo si lo que hay en mi archivo es igual a lo que digitó el usuario
			if(usuarioAux == usuario && claveAux == clave && puertoAux == puerto){
				encontrado = 1; //los datos coinciden
			}else{
				encontrado = 0; //los datos no coinciden
			}
		}
	}
	ArchivoUsuariosLec.close(); //cerramos el archivo
	return encontrado;
}
int ValidarExistenciaBD(string nombreBD){
	ifstream archivoLectura(BASES_DEDATOS); //variable para manejar archivo de modo lectura para validar que la BD ya existe
	string nombreBaseTmp; //variable temporal que retornará el archivo
	int encontrado = 0; //variable para determinar si se encontró la base en el archivo


	if(archivoLectura.fail()){ //si el archivo no se puede abrir
		cout << "\n\t\tno se pudo abrir el archivo\n";
	}else{
		while(!archivoLectura.eof() && encontrado == 0){
			getline(archivoLectura, nombreBaseTmp); //lee linea por linea y la almacena en la variable temporal
			if(nombreBaseTmp == nombreBD){
				encontrado = 1; //ya existe la base de datos
			}else{
				encontrado = 0;
			}
		}
	}
	archivoLectura.close();
	return encontrado;
}
void ListarBD(){
	ifstream archivo(BASES_DEDATOS);
	string DBTmp; //variable que guardara el nombre de la base de datos de manera temporal
	string BD[100]; //array para almacenar los nombres de las bases de datos
	int i=0;

	if(archivo.fail()){
		cout << "\n\t\tNo se pudo abrir el archivo" << endl;
	}else{
		if (vacio(archivo)){
		    //archivo vacio
			cout << "\n\t\tNo hay bases de datos que mostrar" << endl;
		}else{
			cout << "\n\t\t****BASES DE DATOS****" << endl;
			string prueba;
			while(!archivo.eof()){
					getline(archivo,DBTmp);
					BD[i] = DBTmp; //llenado del array con los nombres de las bases de datos
					i++;
			}
			//mostramos el array
			cout << endl;
			for(int j=0;j<i-1;j++){
			    cout << "\t\tBase de datos " << j+1 << " ------ " << BD[j] << endl;
			}
		}

	}
}

void CrearBaseDeDatos(string nombreBDTmp){ //crea la base como un directorio
	string rutaBase;//variable que contendrá la ruta donde se creará directorio con la base de datos
	string nombreBase; //variable para almacenar el nombre de la base de datos
	string rutaTabla;
	ofstream archivo; //variable para escribir sobre el archivo que contiene el nombre de las DB
	ofstream archivoTablas;
	nombreBase = nombreBDTmp;

	if(ValidarExistenciaBD(nombreBase) == 1){
		cout << "\n\t\tYa existe una base de datos con ese nombre" << endl << endl;

	}else{
		rutaBase = "mkdir /home/minixDB/src/BD/" + nombreBase;
		system(rutaBase.c_str()); //se crea el directorio
		//lectura, escritura, ejecucion (user, group, other) S_IRWXU S_IRWXG S_IRWXO
		//status = mkdir(rutaBase.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); //devuelve -1 si ocurrio un error

		archivo.open(BASES_DEDATOS,ios::app); //se va guardando la nueva base de datos en el archivo que
			  						                 //contendrá todos los nombres de las bases de datos creadas
		if(archivo.fail()){
				cout << "\n\t\tNo se pudo abrir el archivo para guardar la base de datos" << endl;
		}else{
			archivo << nombreBase << endl; //se escribe el nombre al final del archivo
			cout << "\n\t\tBase de datos creada con exito :D " << endl;

		}
	}
	//cuando se crea una base de datos tambien se crea un archivo que contendrá los nombres
	//de las tablas para esa base de datos
		rutaTabla = "/home/minixDB/src/BD/" + nombreBase + "/tablas.dat";
		archivoTablas.open(rutaTabla.c_str(),ios::out);

		if(archivoTablas.fail()){
			cout << "\n\t\tNo se pudo crear la basededatos" << endl;

		}else{
			archivo.close();
			archivoTablas.close();
		}
}

void EliminarBD(string nombreBDTmp){
	string nombreBD;
	string rutaBD; //variable que contendrá la ruta de la base a eliminar
	string auxnombreBD;
	string rutaArchivoTmp;
	ifstream archivoLectura("/home/minixDB/src/basesdedatos.dat");
	ofstream archivoTmp; //archivo para escribir
	int longitud = 0;
	nombreBD = nombreBDTmp;

	//rutaBD = "/home/user88/src/BD/" + nombreBD + ".dat";
	rutaBD = "rm -r /home/minixDB/src/BD/" + nombreBD; //ruta del directorio

	if(ValidarExistenciaBD(nombreBD)==1){
		//base de datos existe. Se procede a eliminar
		rutaArchivoTmp = "/home/minixDB/src/basesdedatostmp.dat";
		archivoTmp.open(rutaArchivoTmp.c_str(),ios::out);
		longitud = nombreBD.length();
		while(!archivoLectura.eof()){
			getline(archivoLectura, auxnombreBD); //lee linea por linea y la almacena en la variable temporal
			if(auxnombreBD==nombreBD){
			  auxnombreBD.replace(auxnombreBD.find(nombreBD),longitud,""); //elimina la linea del
			  	  	  	  	  	  	  	  	  	  	  	  	  	  //archivo q contiene los nombres de las BD
			}else if(!auxnombreBD.empty()){
				archivoTmp << auxnombreBD << endl;
			}
		}

		archivoLectura.close();
		archivoTmp.close();

		remove("/home/minixDB/src/basesdedatos.dat");
		rename("/home/minixDB/src/basesdedatostmp.dat","/home/minixDB/src/basesdedatos.dat");
		//remove(rutaBD.c_str());
		system(rutaBD.c_str());

		cout << "\n\t\tBase de datos eliminada con exito :D" << endl << endl;
	}else{
		//base de datos no existe
		cout << "\n\t\tNo existe una base de datos con ese nombre" << endl << endl;
	}
}

void ActualizarBD(string nombreBD, string nombreNuevo){
	string auxnombreBD = nombreBD;
	string auxnombreNuevo = nombreNuevo;
	ifstream archivoLectura("/home/minixDB/src/basesdedatos.dat"); //variable para archivo q contiene
	       	   	   	   	   	   	   	   	   	   	   	   	   	   	  //los nombres de las BD
	ofstream archivoTmp; //archivo para escribir
	string rutaArchivoTmp;
	string aux;

	int longitud = 1;

	if(ValidarExistenciaBD(auxnombreBD)==1){
		//base de datos existe. Debe existir para poder renombrarla
		string rutaExistente;
		//rutaExistente = "/home/user88/src/BD/"+auxnombreBD+".dat";
		rutaExistente = "/home/minixDB/src/BD/"+auxnombreBD;
		if (ValidarExistenciaBD(auxnombreNuevo)==1){
			//ya existe base de datos con el nuevo nombre
			cout  << "\n\t\t" << auxnombreNuevo << " Ya existe. Digite otro nombre" << endl << endl;
		}else{
			//se procede a renombrar
			string rutaNueva;
			rutaNueva = "/home/minixDB/src/BD/"+auxnombreNuevo;

			rutaArchivoTmp = "/home/minixDB/src/basesdedatostmp.dat";
			archivoTmp.open(rutaArchivoTmp.c_str(),ios::out); //archivo para copiar las BD

			longitud = nombreBD.length();

			while(!archivoLectura.eof()){ //se lee el archivo original
				getline(archivoLectura, aux); //lee linea por linea y la almacena en la variable temporal
				  if(aux==nombreBD){
					 aux.replace(aux.find(nombreBD),longitud,"");
				  }else if(!aux.empty()){
					 archivoTmp << aux << endl;
				  }

			}
			archivoTmp << auxnombreNuevo << endl; //se agrega la BD renombrada al final del
												//archivo

			archivoLectura.close();
			archivoTmp.close();

			remove("/home/minixDB/src/basesdedatos.dat");
			//se renombra el archivo
			rename("/home/minixDB/src/basesdedatostmp.dat","/home/minixDB/src/basesdedatos.dat");

			rename(rutaExistente.c_str(),rutaNueva.c_str()); //se actualiza el nombre de la BD
			cout << "\n\t\tBase de datos actualizada con exito :D" << endl << endl;
		}
	}else{
		//base de datos no existe
		cout << "\n\t\tNo existe la base de datos: " << auxnombreBD << endl << endl;
	}
}
int ValidarExistenciaTabla(string nombreBD, string nombreTabla){
	//ifstream archivoLectura; //variable para manejar archivo de modo lectura del archivo de las tablas
	string nombreBase; //nombre de la base de datos
	string nombreT; //nombre de la tabla
	int encontrado = 0; //variable para determinar si se encontró la tabla en el archivo
	string ruta;
	string temporal; //variable que almacenará de manera temporal lo que trae cada linea del archivo

	nombreBase = nombreBD;
	nombreT = nombreTabla;

	ruta = "/home/minixDB/src/BD/"+nombreBase+"/tablas.dat";

	ifstream archivoLectura(ruta.c_str());

	while(!archivoLectura.eof() && encontrado == 0){
		getline(archivoLectura, temporal);
		if(temporal==nombreT){
			//el nombre ya existe
			encontrado = 1;
		}else{
			//el nombre no existe
			encontrado = 0;
		}
	}

	return encontrado;
}
void CrearTabla(string nombreTabla){
	string nombreBD; //nombre de la base de datos
	string tabla; //nombre de la tabla
	ofstream archivoTabla, archivoTablaData, archivoNombresTablas; //archivo que contendrá la estructura de la tabla
	int variablesInt;
	int variablesDouble;
	int variablesString;

	string enteros[50]; //array para nombre de variables enteras
	string dobles[50]; //array para nombre de variables dobles
	string cadenas[50]; //array para nombre de variables string

	//se pide el nombre de la base de datos en la que se desea crear la tabla
	cout << "\nDigite el nombre de la base de datos en la que desea crear su tabla: ";
	cin >> nombreBD;

	tabla = nombreTabla;

	if(ValidarExistenciaBD(nombreBD) == 1){//si BD existe
		if(ValidarExistenciaTabla(nombreBD,tabla)==1){ //Si ya existe una tabla con ese nombre en esa BD
			cout << "\n\t\tYa existe una tabla con ese nombre en: " << nombreBD << endl;
		}else{
			//se crea la tabla

			//variables para almacenar las rutas
			string rutaTabla;
			string rutaTablaData;
			string rutaNombreTablas;
			//rutas de los archivos a manejar
			rutaTabla = "/home/minixDB/src/BD/"+nombreBD+"/"+tabla+".dat"; //ruta del archivo para almacenar la tabla
			rutaTablaData = "/home/minixDB/src/BD/"+nombreBD+"/DATA"+tabla+".dat"; //ruta del archivo para almacenar la DATA de la tabla
			rutaNombreTablas = "/home/minixDB/src/BD/"+nombreBD+"/tablas.dat";

			archivoTabla.open(rutaTabla.c_str(),ios::out); //se crea archivo con nombre de la tabla
			archivoTablaData.open(rutaTablaData.c_str(),ios::out); //se crea un archivo con el nombre de la tabla
																		//pero aquí se almacenarán los registros
			archivoNombresTablas.open(rutaNombreTablas.c_str(),ios::app); //se abre el archivo q contiene los
																				//nombres de las tablas para escribir

			if(archivoTabla.fail()){
				cout << "\n\t\tNo se puede crear un archivo para esa tabla" << endl;
			}else{
				if(archivoNombresTablas.fail()){
					cout << "\n\t\tNo se pudo abrir el archivo para guardar esa tabla" << endl;
				}else{
					archivoNombresTablas << tabla << endl;
				}

			cout << "\nDigite la cantidad de variables de tipo int para su tabla: ";
			cin >> variablesInt;
			cout << "\nDigite la cantidad de variables de tipo double para su tabla: ";
			cin >> variablesDouble;
			cout << "\nDigite la cantidad de variables de tipo string para su tabla: ";
			cin >> variablesString;
			//si la cantidad de variables a digitar por el usuario es mayor a cero debe pedirse
			//los nombres de las variables al usuario
			if(variablesInt>0){
				cout << "\n\t\t****VARIABLES DE TIPO INT****" << endl;
				for(int i=1;i<=variablesInt;i++){
					cout << "\nDigite el nombre de la variable " << i << " : ";
					cin >> enteros[i];
				}
			}
			if(variablesDouble>0){
				cout << "\n\t\t****VARIABLES DE TIPO DOUBLE****" << endl;
				for(int i=1;i<=variablesDouble;i++){
					cout << "\nDigite el nombre de la variable " << i << " : ";
					cin >> dobles[i];
				}
			}
			if(variablesString>0){
				cout << "\n\t\t****VARIABLES DE TIPO STRING****" << endl;
				for(int i=1;i<=variablesString;i++){
					cout << "\nDigite el nombre de la variable " << i << " : ";
					cin >> cadenas[i];
				}
			 }
			//luego de almacenados los datos se proceden a escribir en el archivo
			archivoTabla << "tabla " << tabla << " {" << endl;
			archivoTabla << "int " << "id" << endl;

			if(variablesInt>0){
				for(int i=1;i<=variablesInt;i++){
					archivoTabla << "int " << enteros[i] << endl;
				}
			}
			if(variablesDouble>0){
				for(int i=1;i<=variablesDouble;i++){
					archivoTabla << "double " << dobles[i] << endl;
				}
			}
			if(variablesString>0){
				for(int i=1;i<=variablesString;i++){
					archivoTabla << "string " << cadenas[i] << endl;
				}
			}
			archivoTabla << "}" << endl;

		  //Si la tabla se creo con exito se debe mostrar al usuario

			cout << "\n\t\tTabla creada con exito: " << endl;
			cout << "tabla " << tabla << " {" << endl;
			cout  << "int " << "id" << endl;

			if(variablesInt>0){
				for(int i=1;i<=variablesInt;i++){
					cout << "int " << enteros[i] << endl;
				}
			}
			if(variablesDouble>0){
				for(int i=1;i<=variablesDouble;i++){
					cout << "double " << dobles[i] << endl;
				}
			}
			if(variablesString>0){
				for(int i=1;i<=variablesString;i++){
					cout << "string " << cadenas[i] << endl;
				}
			}
			cout << "}" << endl;


		  }
	   }
    }else{
		cout << "\n\t\tNo existe base de datos con ese nombre" << endl;
  }
}
void EliminarTabla(string nombreT){
	string nombreBD;
	string nombreTabla;
	string rutaTablasExistentes; //ruta que contiene las tablas existentes
	string rutaArchivoTmp; //ruta para el archivo temporal
	string rutaNombreTabla; //ruta del archivo con el nombre de la tabla
	string rutaDataTabla; //ruta del archivo que contiene la DATA de la tabla
	string temporal; //variable para almacenar de manera temporal lo que se lee del archivo de las tablas
	ofstream archivoTmp; //archivo temporal donde se sobreescribiran los nombres de las tablas

	cout << "\nDigite el nombre de la base de datos en la que desea eliminar la tabla: ";
	cin >> nombreBD;
	nombreTabla = nombreT;
	//rutas de los archivos a manejar
	rutaTablasExistentes = "/home/minixDB/src/BD/"+nombreBD+"/tablas.dat"; //ruta de las tablas existentes
	rutaArchivoTmp = "/home/minixDB/src/BD/"+nombreBD+"/tablasTmp.dat"; //ruta del archivo temporal
	rutaNombreTabla	= "/home/minixDB/src/BD/"+nombreBD+"/"+nombreTabla+".dat"; //ruta de la estructura de la tabla
	rutaDataTabla = "/home/minixDB/src/BD/"+nombreBD+"/DATA"+nombreTabla+".dat"; //ruta de los datos de la tabla

	if(ValidarExistenciaBD(nombreBD)==1){
		if(ValidarExistenciaTabla(nombreBD,nombreTabla)==1){
			//la tabla si existe. Se procede a eliminar

			ifstream archivoLectura(rutaTablasExistentes.c_str()); //archivo con las tablas existentes
			archivoTmp.open(rutaArchivoTmp.c_str(), ios::out);

			if(archivoTmp.fail()){
				cout << "\n\t\tEl archivo a escribir no se puede abrir" << endl;
			}else{
				ifstream archivoDATA(rutaDataTabla.c_str()); //archivo que contiene la DATA de la tabla
				if(vacio(archivoDATA)){
					//no contiene data, si se puede elminiar
					while(!archivoLectura.eof()){
						getline(archivoLectura,temporal);
						if(temporal==nombreTabla){
							//temporal.replace(nombreTabla,longitud,"");
							temporal.replace(temporal.find(nombreTabla),nombreTabla.length(),"");

						}else if(!temporal.empty()){
							archivoTmp << temporal << endl;
						}
					}

					archivoLectura.close();
					archivoTmp.close();

					remove(rutaTablasExistentes.c_str()); //se elimina el archivo original
					rename(rutaArchivoTmp.c_str(),rutaTablasExistentes.c_str()); //se renombra el archivo que se sobreescribio como el original
					remove(rutaNombreTabla.c_str()); //se elimina el archivo con el nombre y estructura de la tabla
					remove(rutaDataTabla.c_str()); //se elimina el archivo con la DATA de la tabla

					cout << "\n\t\tTabla eliminada con exito :D" << endl << endl;
				}else{
					//contiene data. no se puede eliminar
					cout << "\n\t\tNo se puede eliminar esa tabla porque contiene datos" << endl;
				}

			}

		}else{
			//la tabla no existe
			cout << "\n\t\tNo existe una tabla con ese nombre :( " << endl;
		}

	}else{
		cout << "\n\t\tNo existe una base de datos con ese nombre :(" << endl;
	}
}
void ActualizarTabla(string nombreT, string nombreTN){ //actualiza el nombre de la tabla
	string nombreBD;
	string nombreTabla;
	string nombreNuevo;
	string rutaTablasExistentes; //ruta que contiene las tablas existentes
	string rutaArchivoTmp; //ruta para el archivo temporal
	string rutaNombreTabla; //ruta del archivo con el nombre de la tabla
	string rutaDataTabla; //ruta del archivo que contiene la DATA de la tabla
	string rutaNuevaTabla;
	string rutaNuevaDataTabla;
	string temporal; //variable para almacenar de manera temporal lo que se lee del archivo de las tablas
	ofstream archivoTmp; //archivo temporal donde se sobreescribiran los nombres de las tablas

	cout << "\nDigite el nombre de la base de datos en la que desea eliminar la tabla: ";
	cin >> nombreBD;
	nombreTabla = nombreT;
	nombreNuevo = nombreTN;
	//rutas de los archivos a manejar
	rutaTablasExistentes = "/home/minixDB/src/BD/"+nombreBD+"/tablas.dat"; //ruta de las tablas existentes
	rutaArchivoTmp = "/home/minixDB/src/BD/"+nombreBD+"/tablasTmp.dat"; //ruta del archivo temporal
	rutaNombreTabla	= "/home/minixDB/src/BD/"+nombreBD+"/"+nombreTabla+".dat"; //ruta de la estructura de la tabla
	rutaDataTabla = "/home/minixDB/src/BD/"+nombreBD+"/DATA"+nombreTabla+".dat"; //ruta de los datos de la tabla
	rutaNuevaTabla = "/home/minixDB/src/BD/"+nombreBD+"/"+nombreNuevo+".dat";
	rutaNuevaDataTabla	= "/home/minixDB/src/BD/"+nombreBD+"/DATA"+nombreNuevo+".dat";

	if(ValidarExistenciaBD(nombreBD)==1){
		if(ValidarExistenciaTabla(nombreBD,nombreTabla)==1){
			//la tabla que se desea renombrar si existe. Se procede a eliminar
			if(ValidarExistenciaTabla(nombreBD,nombreNuevo)==1){
				//el nuevo nombre de la tabla ya existe
				cout << "\n\t\t"<< nombreNuevo << " Ya existe :( " << endl;
			}else{
				//no existe el nombre nuevo. Se puede actualizar
				ifstream archivoLectura(rutaTablasExistentes.c_str()); //archivo con las tablas existentes
				archivoTmp.open(rutaArchivoTmp.c_str(), ios::out);

				if(archivoTmp.fail()){
					cout << "\n\t\tEl archivo a escribir no se puede abrir" << endl;
				}else{
					while(!archivoLectura.eof()){
						getline(archivoLectura,temporal);
						if(temporal==nombreTabla){
							//temporal.replace(nombreTabla,longitud,"");
							temporal.replace(temporal.find(nombreTabla),nombreTabla.length(),"");
						}else if(!temporal.empty()){
							archivoTmp << temporal << endl;
						}
					}
					archivoTmp << nombreNuevo << endl; //se agrega el nombre de la tabla actualizado
												       //al final del archivo

					archivoLectura.close();
					archivoTmp.close();

					remove(rutaTablasExistentes.c_str()); //se elimina el archivo original
					rename(rutaArchivoTmp.c_str(),rutaTablasExistentes.c_str()); //se renombra el archivo que se sobreescribio como el original
					rename(rutaNombreTabla.c_str(),rutaNuevaTabla.c_str()); //se renombra el archivo con el nombre y estructura de la tabla
					rename(rutaDataTabla.c_str(),rutaNuevaDataTabla.c_str()); //se renombra el archivo con la DATA de la tabla

					//se sobreescribe el archivo que contiene la estructura de la tabla
					string nuevaRutaTmp;

					nuevaRutaTmp = "/home/minixDB/src/BD/"+nombreBD+"/archivoTablaTMP.dat"; //ruta del nuevo
																						//archivo temporal

					ifstream archivoLectura(rutaNuevaTabla.c_str());
					ofstream archivoTmpTabla;
					archivoTmpTabla.open(nuevaRutaTmp.c_str(), ios::out);

					if(archivoTmpTabla.fail()){
						cout << "\nNo se puede abrir el archivo para sobreescribir la estructura de la tabla";
					}else{
						string auxTabla;
						string linea;
						linea = "tabla "+nombreTabla+" {";//linea a sustituir (trae el nombre de la tabla)
						archivoTmpTabla << "tabla " << nombreNuevo << " {" << endl;
						while(!archivoLectura.eof()){
							getline(archivoLectura, auxTabla);
							if(auxTabla==linea){
								auxTabla.replace(auxTabla.find(linea),linea.length(),"");
							}else if(!auxTabla.empty()){
								archivoTmpTabla << auxTabla << endl;
							}
						}
						remove(rutaNuevaTabla.c_str());
						rename(nuevaRutaTmp.c_str(),rutaNuevaTabla.c_str());
					}

					cout << "\n\t\tTabla actualizada con exito :D" << endl << endl;
				}
			}
		}else{
			//la tabla que se desea actualizar no existe
			cout << "\n\t\tNo existe la tabla " << nombreT << " :( " << endl;
		}

	}else{
		cout << "\n\t\tNo existe una base de datos con ese nombre :(" << endl;
	}
}
//valida si un archivo esta vacio
bool vacio(ifstream& archivo){
    return archivo.peek() == ifstream::traits_type::eof();
}

void ListarTablas(){
	string BD;
	string rutaArchivoLectura; //ruta del archivo donde estan los nombres de las tablas
	string tmp;
	string array[50]; //array que contendra los nombres de las tablas
	int i=0; //contador para determinar cuantas tablas existen
	cout << "\nDigite el nombre de la base de datos de la que desea ver las tablas: ";
	cin >> BD;

	if(ValidarExistenciaBD(BD)==1){
		//la base de datos si existe. Se proceden a listar las tablas

		rutaArchivoLectura = "/home/minixDB/src/BD/"+BD+"/tablas.dat";
		ifstream archivoLectura(rutaArchivoLectura.c_str()); //archivo modo lectura con el nombre de las tablas

		if (vacio(archivoLectura)){
		    //archivo vacio
			cout << "\n\t\tNo hay tablas que mostrar" << endl;
		}else{
			//Si el archivo no esta vacio se muestran las tablas
			while (!archivoLectura.eof()){
				getline(archivoLectura,tmp);
				array[i] = tmp; //se almacenan los nombres en el array
				i++;
			}

			i=i-1; //se le quita el salto de linea final que viene al final del archivo

			string rutas[50]; //array para las rutas

			for(int j=0;j<i;j++){
				string ruta; //ruta de la tabla a mostrar
				string aux; //almacena el nombre de la tabla
				ruta = "/home/minixDB/src/BD/"+BD+"/"+array[j]+".dat";
				ifstream archivo(ruta.c_str());
				cout << "\nTabla " << j+1 << ": " << endl;
				while(!archivo.eof()){
					getline(archivo,aux);
					cout << aux << endl;
				}
			}
			cout << "\n\t\tCantidad de tablas encontradas: " << i << endl << endl;
		}


	}else{
		cout << "\n\t\tNo existe una base de datos con ese nombre :( " << endl;
	}

}

void MostrarTabla(string nombreT, string nombreBD){
	string nombreTabla;
	string nombreBase;
	string ruta;
	string tmp;
	string array[100];
	int contador=0;

	nombreTabla = nombreT;
	nombreBase = nombreBD;

	if(ValidarExistenciaBD(nombreBase)==1){
		if(ValidarExistenciaTabla(nombreBase,nombreTabla)==1){
			ruta = "/home/minixDB/src/BD/"+nombreBase+"/"+nombreTabla+".dat";
			ifstream archivoLectura(ruta.c_str());
			while(!archivoLectura.eof()){
				getline(archivoLectura,tmp);
				array[contador]=tmp;
				contador++;
			}
			cout << "\n\t******************* TABLA " << nombreTabla << " *******************" << endl;
			for(int i=0;i<contador-1;i++){
				cout << array[i] << endl;
			}

		}else{
			cout << "\n\t\tNo existe la tabla " << nombreTabla << " en " << nombreBase << endl;
		}

	}else{
		cout << "\n\t\tNo existe la base de datos " << nombreBase << endl;
	}

}
void IngresarRegistro(string nombreT){
	ofstream archivoData;
	string nombreTabla;
	string nombreBD;
	string rutaArchivoLectura; //variable que almacena la ruta de la estructura de la tabla
	string rutaArchivoDATA; //varibale que almacena la ruta del archivo para guardar la DATA
	string tmp;
	string aux;
	string palabra;
	string palabras[200]; //array para guardar cada palabra del archivo
	string arrayLineas[100]; //ira almacenando cada linea del archivo lectura
	string tipos[200]; //array para almacenar los tipos de variables
	string variables[200]; //array para almacenar los nombres de variables
	int contadorEnteros = 0; //cuenta cuantas variables de tipo entero hay en la tabla
	int contadorDobles = 0;//cuenta cuantas variables de tipo doble hay en la tabla
	int contadorCadenas = 0;//cuenta cuantas variables de tipo string hay en la tabla
	int contTipos=0;
	int contVariables=0;
	int contPalabras=0;
	int contador = 0;


	nombreTabla = nombreT;

	cout << "\n\tDigite la base de datos a la que pertenece la tabla " << nombreTabla << ": ";
	cin >> nombreBD;

	rutaArchivoLectura = "/home/minixDB/src/BD/"+nombreBD+"/"+nombreTabla+".dat";
	rutaArchivoDATA = "/home/minixDB/src/BD/"+nombreBD+"/DATA"+nombreTabla+".dat";

	ifstream archivoLectura(rutaArchivoLectura.c_str());

	if(ValidarExistenciaBD(nombreBD)==1){
		//si existe la base de datos
		if(ValidarExistenciaTabla(nombreBD, nombreTabla)==1){
			//si existe la tabla
			while(!archivoLectura.eof()){
				getline(archivoLectura,tmp);
				arrayLineas[contador] = tmp;
				contador++;
			}
			//pos = 0: nombre de la tabla, pos = contador: salto de linea (ultima posicion)
			//pos = contador-2: ultima variable (se resta el salto de linea y la llave de cierre
			for(int i=1;i<contador-2;i++){
				aux = arrayLineas[i];
				stringstream input_stringstream(aux);
				while (getline(input_stringstream, palabra, ' ')){ //separa palabra por palabra
						palabras[contPalabras] = palabra; //se almacena la palabra en un array
						contPalabras++;
				}
			}
			//se separan las palabras y se almacenan en arrays.
			//1 array para el tipo de datos y 1 array para almacenar los nombres de variables
			for(int i=0;i<contPalabras;i++){
				aux = palabras[i];
				if(strcmp("int",aux.c_str())==0 || strcmp("double",aux.c_str())==0 || strcmp("string",aux.c_str())==0){
					tipos[contTipos] = aux;
					contTipos++;
				}else{
					//si no es un tipo de dato entonces es el nombre de la variable
					variables[contVariables] = aux;
					contVariables++;
				}
			}
			//clasificamos los tipos de datos en enteros, dobles o strings
			for(int i=0;i<contTipos;i++){
				aux = tipos[i];
				if(strcmp("int",aux.c_str())==0){
					contadorEnteros++;
				}else if(strcmp("double",aux.c_str())==0){
					contadorDobles++;
				}else{
					contadorCadenas++;
				}
			}

			archivoData.open(rutaArchivoDATA.c_str(),ios::app);

			if(archivoData.fail()){
				cout << "\n\t\tNo se puede abrir el archivo para guardar la data" << endl;
			}else{
				int variableEntero;
				double variableDoble;
				string variableString;
				string id;
				int contAux = 0;

				cout << "\n\t\tENTEROS" << endl;
				cout << "Digite la variable id: "; //siempre pedira el id
				cin >> id;
				if(ValidarId(nombreTabla,nombreBD,id)==1){
					//ya existe ese ID
					cout << "Ya existe ese ID. Intente con otro" << endl;
				}else{
					archivoData << "id: " << id << endl;
					if(contadorEnteros>1){ //el id ya se pidio
						for(int i=1;i<contadorEnteros;i++){ //inicia en la posicion siguiente al id
							contAux = i; //contador auxiliar para imprimir el nombre de la variable en
										//la posicion adecuada
							aux = variables[contAux];
							cout << "Digite la variable " << aux << ": ";
							cin >> variableEntero;
							archivoData << aux << ": " << variableEntero << endl;
						}
					}
					//si el contador de dobles es mayor a cero si deben pedirse los datos
					if(contadorDobles>0){
						cout << "\n\t\tDOBLES" << endl;
						for(int i=0;i<contadorDobles;i++){
							contAux = contAux+1; //lo que trae el contador solo se le debe sumar una posicion
							aux = variables[contAux];
							cout << "Digite la variable " << aux << ": ";
							cin >> variableDoble;
							archivoData << aux << ": " << variableDoble << endl;
						}
					}
					//si el contador de strings es mayor a cero si deben pedirse los datos
					if(contadorCadenas>0){
						cout << "\n\t\tSTRINGS" << endl;
						for(int i=0;i<contadorCadenas;i++){
							contAux = contAux+1; //lo que trae el contador solo se le debe sumar una posicion
							aux = variables[contAux];
							cout << "Digite la variable " << aux << ": ";
							cin >> variableString;
							archivoData << aux << ": " << variableString << endl;
						}
					}
				}
			}
			archivoLectura.close();
			archivoData.close();

		}else{
			cout << "\n\t\tNo existe la tabla " << nombreTabla << " en " << nombreBD << endl;
		}
	}else{
		cout << "\n\t\tNo existe la base de datos " << nombreBD << endl;
	}
}
int ValidarId(string nombreT, string nombreBD, string id){
	string tabla;
	string nombreBase;
	string rutaData;
	string aux;
	string idTmp;
	int existe=0;

	tabla = nombreT;
	nombreBase = nombreBD;
	idTmp = id;

	rutaData = "/home/minixDB/src/BD/"+nombreBase+"/DATA"+tabla+".dat";

	ifstream archivo(rutaData.c_str());

	string identificador = "id: "+idTmp;

	while(!archivo.eof() && existe == 0){
		getline(archivo,aux);
		if(aux == identificador){
			//se encontro
			existe = 1;
		}else{
			existe = 0;
		}
	}
	return existe;
}
void MostrarRegistros(string nombreT){
	string nombreBD;
	string nombreTabla;
	string rutaArchivo;
	string tmp;
	string aux;
	string palabra;
	string arrayLineas[150]; //array  almacena lineas
	int contLineas = 0; //contador para las lineas del archivo


	nombreTabla = nombreT;

	cout << "\n\tDigite la base de datos a la que pertenece la tabla " << nombreTabla << ": ";
	cin >> nombreBD;

	rutaArchivo = "/home/minixDB/src/BD/"+nombreBD+"/DATA"+nombreTabla+".dat";

	ifstream archivo(rutaArchivo.c_str());

	if(ValidarExistenciaBD(nombreBD)==1){
		//si existe la BD
		if(ValidarExistenciaTabla(nombreBD, nombreTabla)==1){
			//si existe la tabla
			if(vacio(archivo)){
				//no contiene data
				cout << "\n\tLa tabla " << nombreTabla << " no contiene datos a mostrar" << endl;

			}else{
				//si contiene data a mostrar
				while(!archivo.eof()){
					getline(archivo,tmp);
					arrayLineas[contLineas]=tmp;
					contLineas++;
				}

				archivo.close();

				//se imprimen los datos
				cout << "\t\t-------------------------" << endl;
				for(int i=0;i<contLineas-1;i++){
					cout <<"\t\t|" << arrayLineas[i]  << endl;
					cout << "\t\t-------------------------" << endl;
				}
			}

		}else{
			cout << "\n\t\tNo existe la tabla " << nombreTabla << " en " << nombreBD << endl;
		}

	}else{
		cout << "\n\t\tNo existe la base de datos " << nombreBD << endl;
	}
}
void MostrarRegistro(string nombreT, string nombreBD){
	string tabla;
	string BD;
	string id;
	string idTmp;
	string rutaData;
	string rutaEstructura;
	string tmp;
	string arrayLineas[100];
	string arrayDatos[100];
	int contVariables=0; //almacena la cantidad de variables dela estructura
	int contLineas=0; //almacena la cantidad de lineas al leer el archivo de la DATA
	int contDatos = 0;
	bool encontrado = false;

	tabla = nombreT;
	BD = nombreBD;



	cout << "\n\t\tDigite el id del registro que desea ver: ";
	cin >> idTmp;
	id = "id: "+idTmp;
	if(ValidarExistenciaBD(BD)==1){
		//si existe la BD
		if(ValidarExistenciaTabla(BD,tabla)==1){
			//si existe la tabla
			if(ValidarId(tabla,BD,idTmp)==1){
				//ID correcto
				rutaEstructura = "/home/minixDB/src/BD/"+BD+"/"+tabla+".dat";
				rutaData = "/home/minixDB/src/BD/"+BD+"/DATA"+tabla+".dat";

				ifstream archivoEstructura(rutaEstructura.c_str());
				ifstream archivoData(rutaData.c_str());

				//se lee el archivo estructura para saber cuantas variables hay
				while(!archivoEstructura.eof()){
					getline(archivoEstructura,tmp);
					contVariables++;
				}
				///le restamos 3 al contador por el nombre de la tabla, llave final y salto de linea
				contVariables = contVariables-3; //cantidad de variables que hay en la tabla

				while(!archivoData.eof()){
					getline(archivoData,tmp);
					arrayLineas[contLineas] = tmp;
					contLineas++;
				}

				for(int i=0;i<contLineas;i++){
					tmp = arrayLineas[i];
					if(strcmp(id.c_str(),tmp.c_str())==0){
						//encontro el id
						encontrado = true;
				}
					if(encontrado){
						arrayDatos[contDatos] = tmp;
						contDatos++;
					}
				}
				cout << "\t\t-------------------------" << endl;
				cout << "\t\t------ REGISTRO " << idTmp << " ------" << endl;
				cout << "\t\t-------------------------" << endl;
				for(int i=0;i<contVariables;i++){
					cout << "\t\t|" << arrayDatos[i] << endl;
					cout << "\t\t-------------------------" << endl;
				}

			}else{
				cout << "\n\t\tNo existe un registro con ese ID" << endl;
			}
		}else{
			cout << "\n\t\tNo existe esa tabla"<<endl;
		}
	}else{
		cout << "\n\t\tNo existe esa base de datos"<<endl;
	}

}