#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <redes2/irc.h>

#define MAX_CONNECTIONS 2
#define NFC_SERVER_PORT 8885

int socket1;
int con;
/**
 * @brief Inicia un socket nuevo y devuelve su identificador
 * @return identificador del socket iniciado (int)
 */
int initiate_server(void){
	
	int sockval;
	struct sockaddr_in Direccion;

	syslog(LOG_INFO, "Creando socket");
	if((sockval = socket(AF_INET, SOCK_STREAM, 0)) <0){
		syslog(LOG_ERR, "Error creando socket");
		exit(EXIT_FAILURE);

	}

	socket1 = sockval;

	Direccion.sin_family=AF_INET;  /* TCP/IP family */
	Direccion.sin_port=htons(NFC_SERVER_PORT); /*Asignando puerto*/
	Direccion.sin_addr.s_addr=htonl(INADDR_ANY); /* Aceptar todas las direcciones*/
	bzero((void*)&(Direccion.sin_zero),8);

	syslog(LOG_INFO, "Binding socket");
	if(bind(sockval, (struct sockaddr *) &Direccion, sizeof(Direccion)) <0 ) {
		syslog(LOG_ERR, "Error binding socket");
		exit(EXIT_FAILURE);
	}


	syslog(LOG_INFO, "Escuchando conexiones");
	if(listen(sockval, MAX_CONNECTIONS) <0 ) {
		syslog(LOG_ERR, "Error escuchando conexiones");
		exit(EXIT_FAILURE);
	}

	return sockval;
	
}

void parsear_comandos(char* command){
	
		switch(IRC_CommandQuery(command))
		{
			case PASS:
				fprintf(stderr,"Pass");
				break;
			case NICK:
				fprintf(stderr,"Pass");
				break;
			case USER:
				fprintf(stderr,"Pass");
				break;
			case JOIN:
				fprintf(stderr,"Pass");
				break;
			case LIST:
				fprintf(stderr,"Pass");
				break;
			case PING:
				fprintf(stderr,"PING");
				break;
			case PONG:
				fprintf(stderr,"Pass");
				break;
			default:
				fprintf(stderr,"Error");
				break;
		}	
	
}


void salir(){
	fprintf(stderr,"salir");
	close(socket1);
	close(con);
	exit(EXIT_SUCCESS);
	//close()
}

/**
 * @brief Conversion de un proceso en demonio
 */

void recibir_mensajes(int connval){
	char *aux;
	char * mensaje;
	char* command;
	char* ret=NULL;
	long type;
	aux = (char*)malloc(40);
	mensaje = (char*)malloc(40);
	ret = (char*)malloc(40);
	
	

	/*Codigo a ejecutar por el hijo*/
    	memset(aux, 0, 40);
	syslog(LOG_INFO, "Nuevo acceso");
	while(true){

		recv(connval, mensaje, 40, 0);
		
		
		//if(aux != NULL){
			fprintf(stderr, "hola:\n");
			strcpy(aux, mensaje);
			type=ntohl(*aux);
			ret=IRC_UnPipelineCommands(aux, &command, ret);
		    while(ret!=NULL){
				parsear_comandos(command);
				fprintf(stderr, "hola:\n");
				ret=IRC_UnPipelineCommands(NULL, &command, ret);
			}

		    parsear_comandos(command);
		//}
		fprintf(stderr, "Mensaje: %s\n", command);
		
		//memset(aux, 0, 20);
		
	}
	
	close(connval);
	syslog(LOG_INFO, "Cerrando servicio");
	exit(EXIT_SUCCESS);
}


/**
 * @brief Lanza un nueo servicio para un nuevo acceso
 * @param connval Socket en el que se lanza el servicio (int)
 */
void launch_service(int connval){
	
	
	

	fprintf(stderr, "launchservice");
	
	
	recibir_mensajes(connval);



}

/**
 * @brief Acepta una nueva conexion en un socket
 * @param sockval Socket en el que se realiza la conexion (int)
 */
void accept_connection(int sockval){
	
	int desc;
	socklen_t len;
	struct sockaddr Conexion;

	len=sizeof(Conexion);

	while(true){

		if((desc= accept(sockval, &Conexion, &len)) <0){ /*Aceptar conexion*/
			syslog(LOG_ERR, "Error aceptando conexion");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "Ha llegado una conexion\n");
		launch_service(desc); 
	}
		
		fprintf(stderr, "Voy a dormir\n");
		sleep(1);

	con = desc;

	return;
}





/**
 * @brief Conversion de un proceso en demonio
 */
 void do_daemon(void){
 	pid_t pid;

 	pid=fork();
 	if(pid >0){ 			/* Error en el fork */
		exit(EXIT_FAILURE);
	}
	if (pid==0){ 			/* Proceso padre */
		exit(EXIT_SUCCESS);
	}

	/*Codigo a ejecutar por el hijo*/

	umask(0); /*cambiamos el archivo a modo mascara*/
	setlogmask(LOG_UPTO(LOG_INFO)); /*Abrimos logs*/
	openlog("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);
	syslog (LOG_ERR, "Iniciando nuevo server.");

	if(setsid()<0){ /*Creamos un nuevo SID para el proceso hijo*/
		syslog(LOG_ERR, "Error creando un nuevo SID para el proceso hijo.");
		exit(EXIT_FAILURE);
	}

	if((chdir("/"))<0){ /*Cambiando el directorio de trabajo actual*/
		syslog(LOG_ERR, "Error cambiando el directorio de trabajo actual");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_ERR, "Cerrando descriptores de fichero estandar");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return;

 }


int main(){

	signal(SIGINT, salir); 
 	//fprintf(stderr, "%d", initiate_server());
 	accept_connection(initiate_server());
 	return 0;
 }
