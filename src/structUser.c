#include "../includes/structUsers.h"

struct UsersList userslist;

void iniUsersList(){
	userslist.i=0;
	return;
}
void setUser(pthread_t tid, int descriptor){
	userslist.u[userslist.i].tid=tid;
	userslist.u[userslist.i].descriptor=descriptor;
	userslist.i++;
}

int getDescriptor(char* nick){
	int i;
	for(i=0; i<userslist.i; i++){
		if(strcmp(userslist.u[i].nick, nick)==0){
			return userslist.u[i].descriptor;
		}
	}
	return 0;
}

char** getUser(pthread_t tid){
	int i;
	for(i=0; i<userslist.i; i++){
		if(userslist.u[i].tid==tid){
			return &userslist.u[i].user;
		}
	}
	fprintf(stderr, "getUser tid no encontrado NULL\n");
	return NULL;
}

char** getNick(pthread_t tid){
	int i;
	for(i=0; i<userslist.i; i++){
		if(userslist.u[i].tid==tid){
			return &userslist.u[i].nick;
		}
	}
	fprintf(stderr, "getNick tid no encontrado NULL\n");
	return NULL;
}