// #include <sys/types.h>
// #include <sys/socket.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <string.h>
// #include <json-c/json.h>
// #include <mysql/mysql.h>
#include "include/socket.h"
#include "include/database.h"

// extern int clientfd_set[MAX_CLIENTS];
// extern char buffer[BUFSIZE];
// extern int sockfd, clientfd, sockfd_len, client_len;
// extern unsigned short port;
// extern struct sockaddr_in server_addr, client_addr;
// extern pthread_t thread_id[MAX_CLIENTS];
// extern json_object *response;

// extern MYSQL *connection;
// extern MYSQL_RES *result;
// extern MYSQL_ROW *row;

// void *service_read(void*); // инициализация метода
// void *replyToClientWithMessage(void *data, json_object *request);
// void *replyToAllClientsWithIncomingMessage(void *data); // инициализация метода
// void dispatchAction(const char *action,  void *data);
// void signIn(void *data, const char *email, const char *password);
// void signUp(
// 	void *data,
// 	const char *email,
// 	const char *password,
// 	const char *first_name,
// 	const char *last_name,
// 	const char *gender,
// 	const char *phone,
// 	const char *birth_day,
// 	const char *passport_serial,
// 	const char *passport_number);

int main(int argc, char *argv[])
{
	create_socket_server(argc, argv);
	establish_connection_to_database();
	accept_connection_from_clients();
	return 0;
}

// void *replyToClientWithMessage(void *data, json_object *request)
// {
//   	int clientfd = (int)(long)data;
// 	char message[BUFSIZE];
// 	strcpy(message, json_object_to_json_string(request));
// 	printf("\n%s\n", message);
// 	if (clientfd > 0) {
// 		send(clientfd, message, strlen(message), 0);
// 		printf("Replied to client#%d with message: %s\n", clientfd, message);	
// 	}
// }

// void *replyToAllClientsWithIncomingMessage(void *data)
// {
// 	//pthread_mutex_lock(&mutex_buffer);
// 	for (int i = 0, fd = 0; i < 100; i++) { // проходится по массиву дескрипторов каждого клиента
// 		if ((fd = clientfd_set[i]) > 0) { // если таковой есть, то есть подключеный
// 			printf("send to %d\n", fd); // вывод
// 			send(fd, buffer, 98, 0); // отправляет последнее полученное сообщеение в буфере клиенту, то есть грубо говоря всем
// 		}
// 	}
// 	printf("Replied to all clients with incoming message: %s\n", buffer);
// 	//pthread_mutex_unlock(&mutex_buffer);
// 	return NULL;
// }

// void dispatchAction(const char *action, void *data)
// {
// 	json_object *payload = json_object_object_get(response, "payload");

// 	if (strcmp(action, "sign-in") == 0) {
// 		const char *email = json_object_get_string(json_object_object_get(payload, "email"));
// 		const char *password = json_object_get_string(json_object_object_get(payload, "password"));

// 		signIn((void *)(long)data, email, password);
// 	} else if (strcmp(action, "sign-up") == 0) {
// 		const char *email = json_object_get_string(json_object_object_get(payload, "email"));
// 		const char *password = json_object_get_string(json_object_object_get(payload, "password"));
// 		const char *first_name = json_object_get_string(json_object_object_get(payload, "first_name"));
// 		const char *last_name = json_object_get_string(json_object_object_get(payload, "last_name"));
// 		const char *gender = json_object_get_string(json_object_object_get(payload, "gender"));
// 		const char *phone = json_object_get_string(json_object_object_get(payload, "phone"));
// 		const char *birth_day = json_object_get_string(json_object_object_get(payload, "birth_day"));
// 		const char *passport_serial = json_object_get_string(json_object_object_get(payload, "passport_serial"));
// 		const char *passport_number = json_object_get_string(json_object_object_get(payload, "passport_number"));

// 		signUp((void *)(long)data, email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number);
// 	}
// }

// void signIn(void *data, const char *email, const char *password)
// {
// 	char test_query[256];
		
// 	sprintf(test_query, "select id from user where email='%s' and password='%s'", email, password); 
// 	printf("\n%s\n", test_query);
// 	mysql_query(connection, test_query);

// 	result = mysql_use_result(connection);

// 	if (result == NULL)
// 	{
// 		printf("error");
// 	}
// 	int result_id;
// 	while ((row = mysql_fetch_row(result)) != NULL){
// 		result_id = atoi(row[0]);
// 	}


// 	unsigned long int rows_num = mysql_num_rows(result);

// 	json_object *request = json_object_new_object();
// 	json_object_object_add(request, "action", json_object_new_string("SignIn"));
	
// 	if(rows_num==0)
// 	{
// 		printf("numrows0");
// 		json_object_object_add(request, "result", json_object_new_int(-1));
// 	}
// 	else
// 	{
// 		printf("numrows1");
// 		MYSQL_ROW row = mysql_fetch_row(result);
// 		json_object_object_add(request, "result", json_object_new_int(result_id));
		
// 	}
// 	replyToClientWithMessage((void *)(long)data, request);
// 	mysql_free_result(result);
// 	printf("%s, %s\n", email, password);
// }

// void signUp(
// 	void *data,
// 	const char *email,
// 	const char *password,
// 	const char *first_name,
// 	const char *last_name,
// 	const char *gender,
// 	const char *phone,
// 	const char *birth_day,
// 	const char *passport_serial,
// 	const char *passport_number) {

// 		char test_query[256];

// 		json_object *request = json_object_new_object();
// 		json_object_object_add(request, "action", json_object_new_string("SignUp"));	
// 		if(validation(email))
// 		{
// 			json_object_object_add(request, "result", json_object_new_int(-1));
// 		}	
// 		else
// 		{
// 			//mysql_stmt_prepare(statement, query, strlen(query));
// 			sprintf(test_query,"insert into user (email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number); 
// 			mysql_query(connection, test_query);
// 			signIn((void *)(long)data, email, password);
// 		}

// 		printf("%s, %s, %s, %s, %s, %s, %s, %s, %s",
// 		email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number);
// }

// int validation(char *email)
// {
// 	char test_query[256];
// 	int return_value;

// 	sprintf(test_query, "select id from user where email='%s'", email); 
// 	mysql_query(connection, test_query);

// 	result = mysql_use_result(connection);
// 	 if (result == NULL)
// 	{
// 		printf("error");
// 	}

// 	int rows_num = mysql_num_rows(result);

	
// 	if(rows_num==0)
// 	{
// 		return_value = 1;
// 	}
// 	else
// 	{
// 		return_value = 0;
		
// 	}
// 	mysql_free_result(result);
// 	return return_value;
// }