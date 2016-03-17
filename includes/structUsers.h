#include <stdlib.h>
#include <stdio.h>
#include "server.h"

struct Usuario {
	char* user;
	char* nick;
	pthread_t tid;
	int descriptor;
};


struct UsersList {
	struct Usuario u[MAX_USERS];
	int i;
};



void iniUsersList();
void setUser(pthread_t tid, int descriptor);

int getDescriptor(char* nick);

char** getUser(pthread_t tid);

char** getNick(pthread_t tid);