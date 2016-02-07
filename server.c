
int initiate_server(void){
	
	int sockval;
	struct sockaddr_in Direccion;

	syslog(LOG_INFO, "Creando socket");
	if((sockval = socket(AF_INET, SOCK_STREAM, 0)) <0){
		syslog(LOG_ERR, "Error creando socket");
		exit(EXIT_FAILURE);

	}

	Direccion.sin_family=AF_INET;  /* TCP/IP family */
	Direccion.sin_port=htons(NFC_SERVER_PORT); /*Asignando puerto*/
	Direccion.sin_addr.s_addr=htonl(INADDR_ANY); /* Aceptar todas las direcciones*/
	bzero((void*)&(Direccion.sin_zero),8);

	syslog(LOG_INFO "Binding socket");
	if(bind(sockval, (struct sockaddr *) & Direccion, sizeof(Direccion)) <0 ) {
		syslog(LOG_ERR, "Error binding socket");
		exit(EXIT_FAILURE);
	}


	syslog(LOG_INFO "Escuchando conexiones");
	if(listen(sockval, MAX_CONNECTIONS) <0 ) {
		syslog(LOG_ERR, "Error escuchando conexiones");
		exit(EXIT_FAILURE);
	}

	return sockval;
	
}

void launch_service(int connval){
	int pid;
	long type, aux;

	pid=fork();
	
	if(pid >0){ 			/* Error en el fork*/
		syslog(LOG_ERR "Acceso fallido");
		exit(EXIT_FAILURE);
	}
	if (pid==0){ 			/* Proceso padre */
		return;
	}

	/*Codigo a ejecutar por el hijo*/

	syslog(LOG_INFO "Nuevo acceso");
	recv(connval, &aux, sizeof(long), 0);
	type=ntohl(aux);

	database_access(connval, type, NULL);
	close(connval);
	syslog(LOG_INFO, "Cerrando servicio");
	exit(EXIT_SUCCESS);

}

void accept_connection(int sockval){
	
	int desc, len;
	struct sockaddr Conexion;

	len=sizeof(Conexion);

	if((desc= accept(sockval, &Conexion, &len)) <0){ /*Aceptar conexion*/
		syslog(LOG_ERR "Error aceptando conexion");
		exit(EXIT_FAILURE);
	}

	launch_service(desc); 
	wait_finished_services();

	return;
}