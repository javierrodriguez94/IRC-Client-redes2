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
#include <time.h>
#include <arpa/inet.h>
#include <redes2/irc.h>
#include <netdb.h>
#include <pthread.h>

#define MAX_CONNECTIONS 2
#define TOPIC_MAXSIZE 100
#define NFC_SERVER_PORT 6667
#define TAM_BUFFER 8096
#define MAX_USERS 10


struct Usuario {
	char* nick;
	int socket;
};


struct UsersList {
	struct Usuario u[MAX_USERS];
	int i;
};

struct UsersList userslist;
int socket1;
int con;
long ERR = 0;
long nelements = 0;
char *nick;

char *prefix,*namelist, *msg, *serverPing, *serverPong, *mensaje, *string, *mode,*name, *server, *realname, *user, 
*password, *channel, *key, *usermode, *host, *target, *aux, *topic, *maskarray;


void iniUsersList(){
	userslist.i=0;
	return;
}

int introducirUsuario(char* nick, int socket){
	if (userslist.i>=MAX_USERS){
		return 0;
	}
	userslist.u[userslist.i].nick= (char*)malloc (sizeof(nick));
	strcpy(userslist.u[userslist.i].nick, nick);
	userslist.u[userslist.i].socket=socket;
	userslist.i++;
	return 1;
}

int getSocket(char* nick){
	int i;
	for(i=0; i<userslist.i; i++){
		fprintf(stderr, "GETSOCKET %s\n", userslist.u[i].nick);
		if(strcmp(userslist.u[i].nick, nick)==0){
			fprintf(stderr, "Encontradoooooooo\n");
			return userslist.u[i].socket;
		}
	}
	return 0;
}




/**
 * @brief Inicia un socket nuevo y devuelve su identificador
 * @return identificador del socket iniciado (int)
 */
int initiate_server(void){
	
	int sockval, uno=1;
	struct sockaddr_in Direccion;

	syslog(LOG_INFO, "Creando socket");
	if((sockval = socket(AF_INET, SOCK_STREAM, 0)) <0){
		syslog(LOG_ERR, "Error creando socket");
		exit(EXIT_FAILURE);

	}

	setsockopt(sockval, SOL_SOCKET, SO_REUSEADDR, &uno, sizeof(int));
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

void parsear_comandos(char* command, int connval){
		char **listchannels;
		char **nicklist;
		long numberOfChannels;
		time_t *tiempo = NULL;
		char **listusers; 
		long numberOfUsers, i;
		
		switch(IRC_CommandQuery(command)){
			case PASS:
				if(IRCParse_Pass(command, &prefix, &password)!=IRC_OK){
					fprintf(stderr, "Error en IRCParse_Pass command:%s", command);
					break;
				}
				if(IRCMsg_Pass(&mensaje, prefix, password)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_Pass");
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);

				free(prefix);
				prefix=NULL;
				free(msg);
				msg=NULL;
				free(mensaje);
				//mensaje=NULL;
				break;
			case NICK:
				/*if(IRCTADUser_SetNickByUser(nick, user)!= IRC_OK){
					fprintf(stderr, "Error en IRCTADUser_SetNickByUser, nick:%s user:%s\n", nick, user);
					break;
				}*/
				if(IRCParse_Nick (command, &prefix, &nick, &msg)!=IRC_OK){
					case IRCERR_INVALIDNICK:
						fprintf(stderr, "NICK---------INVALIDNICK");
						break;
					case IRCERR_INVALIDUSER:
						fprintf(stderr, "NICK---------INVALIDUSER");
						break;
				}

				switch(IRCTADUser_SetNickByUser(nick, user)!= IRC_OK){
					fprintf(stderr, "Error en IRCTADUser_SetNickByUser, nick:%s user:%s\n", nick, user);
					break;
				}
				
				if(IRCMsg_Nick (&mensaje, "prefix", NULL, nick)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_Nick");
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);
				break;
			case USER:
				if(IRCParse_User (command, &prefix, &user, &mode, &server, &realname)!=IRC_OK){
					fprintf(stderr, "Error en IRCParse_User:%s", mensaje);
					free(prefix);
					free(user);
					free(mode);
					free(server);
					free(realname);
					break;
				}
				if(IRCMsg_User (&mensaje, prefix, user, mode, realname)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_User");
					free(prefix);
					free(user);
					free(mode);
					free(server);
					free(realname);
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);
				if(!IRCTADUser_GetUserByNick (nick)){
					fprintf(stderr, "ADD USER\n");
					if(introducirUsuario(nick, connval)!=1){
						fprintf(stderr, "Error introduciendo usuario\n");
						break;
					}
					if(IRCTADUser_Add (user, nick, realname, NULL, "host", "ip")!=IRC_OK){
						fprintf(stderr, "Error en IRCTADUser_Add\n");
						break;
					}
					if(!IRCTADUser_GetUserByNick (nick)){
						fprintf(stderr, "USER MAL INTRODUCIDO\n");
					}
				}
				else{ //El usuario ya existe
						//mensaje de usuario
				}
				if(IRCMsg_RplWelcome (&mensaje, "host", nick, nick, user, "host")!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_RplWelcome");
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);
				break;
			case JOIN:
				if (IRCParse_Join(command, &prefix, &channel, &key, &msg) != IRC_OK){
					fprintf(stderr, "Error en IRCParse_Join ");
					free(prefix);
					free(channel);
					free(key);
					return;
				}
				switch(IRCTAD_JoinChannel(channel, user, "a", NULL)){
					case IRCERR_NAMEINUSE:
						fprintf(stderr, "\nERROR JOINCHANNEL 6");
						break;
					case IRCERR_NOVALIDCHANNEL:
						fprintf(stderr, "\nERROR JOINCHANNEL 2");
						break;
					case IRCERR_NOVALIDUSER:
							fprintf(stderr, "\nERROR JOINCHANNEL 1 user:%s", channel);
							break;
					case IRCERR_USERSLIMITEXCEEDED:
						fprintf(stderr, "\nERROR JOINCHANNEL 3");
						break;
					case IRCERR_NOENOUGHMEMORY:
						fprintf(stderr, "\nERROR JOINCHANNEL 4");
						break;
					case IRCERR_INVALIDCHANNELNAME:
						fprintf(stderr, "\nERROR JOINCHANNEL 5");
						break;
					case IRCERR_BANEDUSERONCHANNEL:
						fprintf(stderr, "\nERROR JOINCHANNEL 7");
						break;
					case IRCERR_NOINVITEDUSER:
						fprintf(stderr, "\nERROR JOINCHANNEL 8");
						break;
					//fprintf(stderr, " NADA");	
							
							

				}

				fprintf(stderr,"JOIN");
				if(IRC_Prefix(&prefix, nick, user, host, server)!=IRC_OK){
					fprintf(stderr, "Error en IRC_Prefix");
					break;
				}

				

				if(IRCMsg_Join(&mensaje, prefix, NULL, key, channel)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_Join");
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);
				fprintf(stderr, "SEND JOIN -->%s", mensaje);


				break;
			case LIST:
				time(tiempo);
				IRCParse_List (command, &prefix, &channel, &target);
				if(channel==NULL){
					IRCMsg_RplListStart (&mensaje, prefix, nick);
					send(connval, mensaje, strlen(mensaje), 0);
					fprintf(stderr, "\nStrt:%s", mensaje);
					IRCTADChan_GetList (&listchannels, &numberOfChannels, NULL);
					for (int i=0; i<numberOfChannels; i++) {
						aux=(char*)malloc(2*sizeof(char));
						topic=(char*)malloc(TOPIC_MAXSIZE+1*sizeof(char));
						sprintf(aux, "%lu", IRCTADChan_GetNumberOfUsers(listchannels[i]));
						topic=IRCTADChan_GetTopic (listchannels[i], tiempo);
						if(topic==NULL){
							if(IRCMsg_RplList (&mensaje, prefix, nick, listchannels[i], aux, "")==IRCERR_NOMESSAGE){
								fprintf(stderr, "Error en IRCMsg_RplList");
								break;
							}
						}else{
							if(IRCMsg_RplList (&mensaje, prefix, nick, listchannels[i], aux, topic)==IRCERR_NOMESSAGE){
								fprintf(stderr, "Error en IRCMsg_RplList");
								break;
							}
						}
						free(aux);
						free(topic);
						send(connval, mensaje, strlen(mensaje), 0);
						fprintf(stderr, "\nCANALES:%s", nick);
						fprintf(stderr, "\nList:%s", mensaje);
					}
					IRCMsg_RplListEnd (&mensaje, prefix, nick);
					send(connval, mensaje, strlen(mensaje), 0);
					fprintf(stderr, "\nENd:%s", mensaje);
					IRCTADChan_FreeList (listchannels, numberOfChannels);
				}else{
					topic=(char*)malloc(TOPIC_MAXSIZE+1*sizeof(char));
					topic=IRCTADChan_GetTopic (channel, tiempo);
					IRCMsg_RplTopic (&command, prefix, nick, channel, topic);
					send(connval, mensaje, strlen(mensaje), 0);
				}

					break;
			case WHOIS:
				fprintf(stderr,"WHOIS");
				char *cadenaC;

				if(IRCParse_Whois(command, &prefix, &target, &maskarray)!= IRC_OK){
					fprintf(stderr, "Error en IRCParse_Whois");
					break;
				}


				if(IRCTAD_ListChannelsOfUser (maskarray, &listchannels, &numberOfChannels)!=IRC_OK){
					fprintf(stderr, "Error en IRCTAD_ListChannelsOfUser");					
				}



				IRC_CreateSpaceList(&cadenaC, listchannels, numberOfChannels);

				char * s1;
				char * s2;
				s1=(char*)malloc(strlen(cadenaC)+(sizeof(char)*2*numberOfChannels));	
				s2=(char*)malloc(strlen(listchannels[0])+(sizeof(char)*2));	
				for(int i=0; i< numberOfChannels; i++){
					sprintf(s2, "@%s ", listchannels[i]);
					strcat(s1, s2);	
				}
				
				
				if(IRCMsg_RplWhoIsChannels(&mensaje, prefix, nick, maskarray, s1, NULL)!= IRC_OK){
				 	fprintf(stderr, "Error en IRCMsg_RplWhoIsChannels");
				    break;
				}
				
				send(connval, mensaje, strlen(mensaje), 0);		
				
				if(IRCMsg_RplEndOfWhoIs (&mensaje, prefix, nick, maskarray)!= IRC_OK){
				 	fprintf(stderr, "Error en IRCMsg_RplEndOfWhoIs");
				    break;
				}

				send(connval, mensaje, strlen(mensaje), 0);


				IRCTAD_FreeListChannelsOfUser(listchannels, numberOfChannels);
				break;

			case NAMES:				
				fprintf(stderr,"NAMES");
				char * cadenaN;
				if(IRCParse_Names (command, &prefix, &channel, &target)!= IRC_OK){
					fprintf(stderr, "Error en IRCParse_Names");
					break;
				}


				if(IRCTADUser_GetNickList (&nicklist, &nelements)!= IRC_OK){
					fprintf(stderr, "Error en IRCTADUser_GetNickList");
					break;
				}
				//IRCTADUser_GetUserList (char ***userlist, long *nelements)
				IRC_CreateSpaceList(&cadenaN, nicklist, nelements);

				IRCMsg_RplNamReply (&mensaje, "prefix", nick, "=", channel, cadenaN);
				send(connval, mensaje, strlen(mensaje), 0);
				/*for(int i=0; i< nelements; i++){
					send(connval, nicklist[i], strlen(nicklist[i]), 0);
					fprintf(stderr, "\n%s", nicklist[i]);
				}*/

				if(IRCMsg_RplEndOfNames (&mensaje, "prefix", nick, channel)!= IRC_OK){
					fprintf(stderr, "Error en IRCMsg_RplEndOfNames");
					break;
				}else{
					send(connval, mensaje, strlen(mensaje), 0);
					fprintf(stderr, "\n%s", mensaje);
				}


				break;

			case PING:
				fprintf(stderr,"Pass");
				if(IRCParse_Ping (command, &prefix, &serverPing, &serverPong, &msg)!= IRC_OK){
					fprintf(stderr, "Error en IRCParse_Names");
					break;
				}

				if(IRCMsg_Pong (&mensaje, serverPing, serverPing, NULL, serverPing)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_Pong");
					break;
				}
				send(connval, mensaje, strlen(mensaje), 0);
				fprintf(stderr, "\n%s", mensaje);
				break;

			case PRIVMSG:

				IRCParse_Privmsg (command, &prefix, &target, &msg);
				fprintf(stderr, "Target channel o user ------------------:%s\n", target);
				if(IRCTAD_ListUsersOnChannel (target, &listusers, &numberOfUsers)==IRCERR_NOVALIDCHANNEL){ //MENSAJE PRIVADO A USUARIO
					IRCMsg_Privmsg (&mensaje, "Prefix", target, msg);
					int targetSocket=getSocket(target);
					if(targetSocket==0){
						fprintf(stderr, "Error al obtener socket\n");
						break;
					}
					send(targetSocket, mensaje, strlen(mensaje),0);
				}else{ 																						//MENSAJE A UN GRUPO
					for(i=0; i<numberOfUsers; i++){
						IRCMsg_Privmsg (&mensaje, "Prefix", listusers[i], msg);
						int targetSocket=getSocket(listusers[i]);
						if(targetSocket==0){
							fprintf(stderr, "Error al obtener socket innnnnnn \n");
							break;
						}
						send(targetSocket, mensaje, strlen(mensaje),0);
					}
				}
				break;
			case PART:
				if(IRCParse_Part (command, &prefix, &channel, &msg)!=IRC_OK){
					fprintf(stderr, "Error en IRCParse_Part");
					break;
				}

				if(IRCTAD_PartChannel (channel, user)!=IRC_OK){
					fprintf(stderr, "Error en IRCTAD_PartChannel");
					break;
				}
				fprintf(stderr, "AQUI NOLLEGA: %s, %s\n", channel, user);
				free(mensaje);		
				mensaje = (char*)malloc(TAM_BUFFER);
				if(IRCMsg_Part (&mensaje, "prefix", channel, msg)!=IRC_OK){
					fprintf(stderr, "Error en IRCMsg_Part");
					break;
				}
				send(connval, mensaje, strlen(mensaje),0);

				break;
			case TOPIC:
			    time(tiempo);
				if(IRCParse_Topic (command, &prefix, &channel, &topic)!= IRC_OK){
					fprintf(stderr, "Error en IRCParse_Topic");
					break;
				}
				if(topic == NULL){
					IRCMsg_RplNoTopic(&mensaje, "prefix", nick, channel, "No topic is set");
					topic = IRCTADChan_GetTopic (channel, tiempo);
					IRCMsg_Topic (&mensaje, "prefix", channel, topic);
					send(connval, mensaje, strlen(mensaje),0);
				}else{
					/*IRC_Prefix (&prefix, NULL, user, host, server);
					fprintf(stderr, "Prefix = %s", server);*/
					//fprintf(stderr, "Prefix = %s", nick);
					if(IRCMsg_RplTopic (&mensaje, "prefix", nick, channel, topic)!= IRC_OK){
						fprintf(stderr, "Error en IRCMsg_RplTopic");
						break;
					}
					//send(connval, mensaje, strlen(mensaje),0);
					if(IRCTADChan_SetTopic (channel, topic)!=IRC_OK){
						fprintf(stderr, "Error en IRCTADChan_SetTopic");
						break;
					}
					fprintf(stderr, "END TOPIC");
					IRCMsg_Topic (&mensaje, "prefix", channel, topic);
					send(connval, mensaje, strlen(mensaje),0);
				}
			break;
			case KICK:

			break;
			default:
				//fprintf(stderr,"Error");
				break;
		}	
	
}


void salir(){
	fprintf(stderr,"salir");
	fprintf(stderr, "%d", socket1);
	int ret = close(socket1);
	fprintf(stderr, "%d", ret);
	close(con);
	pthread_exit("1");
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
	aux = (char*)malloc(TAM_BUFFER);
	mensaje = (char*)malloc(TAM_BUFFER);
	ret = (char*)malloc(TAM_BUFFER);
	
	
	signal(SIGINT, salir); 
	/*Codigo a ejecutar por el hijo*/
    	memset(aux, 0, TAM_BUFFER);
	syslog(LOG_INFO, "Nuevo acceso");
	while(true){

		recv(connval, mensaje, TAM_BUFFER, 0);
		
		
		//if(aux != NULL){
			
			strcpy(aux, mensaje);
			type=ntohl(*aux);
			ret=IRC_UnPipelineCommands(aux, &command, ret);
		    while(ret!=NULL){
				parsear_comandos(command, connval);
				ret=IRC_UnPipelineCommands(NULL, &command, ret);
			}

		    parsear_comandos(command, connval);
		//}
		//fprintf(stderr, "Mensaje: %s\n", command);
		
		memset(aux, 0, TAM_BUFFER);
		
	}
	
	close(connval);
	syslog(LOG_INFO, "Cerrando servicio");
	exit(EXIT_SUCCESS);
}


/**
 * @brief Lanza un nuevo servicio para un nuevo acceso
 * @param connval Socket en el que se lanza el servicio (int)
 */
void launch_service(int connval){

	int error;
	pthread_t idHilo; 

	
	error = pthread_create (&idHilo, NULL, recibir_mensajes, connval);
	
	//recibir_mensajes(connval);
	
	if (error != 0){
		syslog(LOG_ERR, "Error creando hilo");
		exit(EXIT_FAILURE);
	}
	



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
		con = desc;
		fprintf(stderr, "Ha llegado una conexion %d\n", con);
		launch_service(desc); 
	}

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

	
 	//fprintf(stderr, "%d", initiate_server());
 	accept_connection(initiate_server());
 	return 0;
 }
