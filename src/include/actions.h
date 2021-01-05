#if !defined(ACTIONS_H)
#define ACTIONS_H

#include <stdio.h>
#include <json-c/json.h>
#include <mysql/mysql.h>
#include "database.h"
#include "socket.h"
#include "../entities/User.h"

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
	const char *birth_day,
	const char *passport_serial,
	const char *passport_number);
int validation(const char *email);

void *replyToClientWithMessage(void *data, json_object *request)
{
  	int clientfd = (int)(long)data;
	char message[BUFSIZE];
	strcpy(message, json_object_to_json_string(request));
	printf("\n%s\n", message);
	if (clientfd > 0) {
		send(clientfd, message, strlen(message), 0);
		printf("Replied to client#%d with message: %s\n", clientfd, message);	
	}
}

void *replyToAllClientsWithIncomingMessage(void *data)
{
	//pthread_mutex_lock(&mutex_buffer);
	for (int i = 0, fd = 0; i < 100; i++) { // проходится по массиву дескрипторов каждого клиента
		if ((fd = clientfd_set[i]) > 0) { // если таковой есть, то есть подключеный
			printf("send to %d\n", fd); // вывод
			send(fd, buffer, 98, 0); // отправляет последнее полученное сообщеение в буфере клиенту, то есть грубо говоря всем
		}
	}
	printf("Replied to all clients with incoming message: %s\n", buffer);
	//pthread_mutex_unlock(&mutex_buffer);
	return NULL;
}

void signIn(void *data, const char *email, const char *password)
{
	char test_query[256];
		
	sprintf(test_query, "select * from users where email='%s' and password='%s'", email, password); 
	printf("\n%s\n", test_query);
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
		printf("authenticated email %s\n", authenticatedUser->email);
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
	printf("%s, %s\n", email, password);
}

void signUp(
	void *data,
	const char *email,
	const char *password,
	const char *first_name,
	const char *last_name,
	const char *gender,
	const char *phone,
	const char *birth_day,
	const char *passport_serial,
	const char *passport_number) {

		char test_query[256];

		json_object *request = json_object_new_object();
		json_object_object_add(request, "action", json_object_new_string("SignUp"));	
		if(validation(email))
		{
			json_object_object_add(request, "result", json_object_new_int(-1));
		}	
		else
		{
			//mysql_stmt_prepare(statement, query, strlen(query));
			sprintf(test_query,"insert into user (email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number); 
			mysql_query(connection, test_query);
			signIn((void *)(long)data, email, password);
		}

		printf("%s, %s, %s, %s, %s, %s, %s, %s, %s",
		email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number);
}

int validation(const char *email)
{
	char test_query[256];
	int return_value;

	sprintf(test_query, "select id from user where email='%s'", email); 
	mysql_query(connection, test_query);

	result = mysql_use_result(connection);
	 if (result == NULL)
	{
		printf("error");
	}

	int rows_num = mysql_num_rows(result);

	
	if(rows_num==0)
	{
		return_value = 1;
	} else {
		return_value = 0;
	}

	mysql_free_result(result);

	return return_value;
}

#endif // ACTIONS_H
