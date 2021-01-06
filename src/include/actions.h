#if !defined(ACTIONS_H)
#define ACTIONS_H

#include <stdio.h>
#include <json-c/json.h>
#include <mysql/mysql.h>
#include "database.h"
#include "socket.h"
#include "../entities/User.h"
#include "validations.h"

extern MYSQL *connection;
extern MYSQL_RES *result;
extern MYSQL_ROW *row;

extern int clientfd_set[MAX_CLIENTS];
extern char buffer[BUFSIZE];

void signIn(void *data, const char *email, const char *password);
void signUp(
	void *data,
	const char *email,
	const char *password,
	const char *first_name,
	const char *last_name,
	const char *gender,
	const char *phone,
	const char *birth_date,
	const char *passport_serial,
	const char *passport_number);
int user_exists(const char *email);

void *replyToClientWithMessage(void *data, json_object *request)
{
  	int clientfd = (int)(long)data;
	char message[BUFSIZE];
	strcpy(message, json_object_to_json_string(request));
	if (clientfd > 0) {
		send(clientfd, message, strlen(message), 0);
	}
}

void *replyToAllClientsWithIncomingMessage(void *data)
{
	//pthread_mutex_lock(&mutex_buffer);
	for (int i = 0, fd = 0; i < 100; i++) { // проходится по массиву дескрипторов каждого клиента
		if ((fd = clientfd_set[i]) > 0) { // если таковой есть, то есть подключеный
			send(fd, buffer, 98, 0); // отправляет последнее полученное сообщеение в буфере клиенту, то есть грубо говоря всем
		}
	}
	//pthread_mutex_unlock(&mutex_buffer);
	return NULL;
}

void signIn(void *data, const char *email, const char *password)
{
	char test_query[256];
		
	sprintf(test_query, "select * from users where email='%s' and password='%s'", email, password); 

	mysql_query(connection, test_query);

	result = mysql_use_result(connection);

	if (result == NULL)
	{
		printf("error");
	}

	struct User *authenticatedUser = malloc(sizeof(struct User));

	while ((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL){
		// TODO: double-check
		authenticatedUser->id = (unsigned int)atoi((char *)(row[0]));
		authenticatedUser->email = (char *)row[1];
		authenticatedUser->password = (char *)row[2];
		break;
	}


	unsigned long int rows_num = mysql_num_rows(result);

	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("sing-in"));
	
	if (rows_num == 0)
	{
		// json_object_object_add(request, "payload", json_object_new_int(-1));
		json_object *payload = json_object_new_object();
		json_object_object_add(payload, "success", json_object_new_int(0));
		json_object_object_add(payload, "message", json_object_new_string("Wrong credentials"));
		json_object_object_add(request, "payload", payload);
	}
	else
	{
		json_object *payload = json_object_new_object();
		json_object_object_add(payload, "success", json_object_new_int(1));
		json_object *me = json_object_new_object();
		json_object_object_add(me, "id", json_object_new_int(authenticatedUser->id));
		json_object_object_add(me, "email", json_object_new_string(authenticatedUser->email));
		json_object_object_add(payload, "me", me);
		json_object_object_add(request, "payload", payload);
	}
	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
}

void signUp(
	void *data,
	const char *email,
	const char *password,
	const char *first_name,
	const char *last_name,
	const char *gender,
	const char *phone,
	const char *birth_date,
	const char *passport_serial,
	const char *passport_number) {

		char test_query[256];

		json_object *request = json_object_new_object();
		json_object_object_add(request, "action", json_object_new_string("sign-up"));
		if (user_exists(email)) {
			json_object *payload = json_object_new_object();
			json_object_object_add(payload, "success", json_object_new_int(0));
			json_object_object_add(payload, "message", json_object_new_string("User with such email already exists"));
			json_object_object_add(request, "payload", payload);

			replyToClientWithMessage((void *)(long)data, request);
		} else {
			sprintf(test_query,"insert into users (email, password, first_name, last_name, gender, phone, birth_date, passport_serial, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", email, password, first_name, last_name, gender, phone, birth_date, passport_serial, passport_number); 
			// signIn((void *)(long)data, email, password);
		}
}

#endif // ACTIONS_H
