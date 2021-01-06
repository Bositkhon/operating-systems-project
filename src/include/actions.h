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




//
//Actions with Bokings
//

json_object* bookTicket(void *data, int user_id, int schedule_id){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("booked-ticked"));	
	
	sprintf(test_query, 
			"insert into bookings (user_id, schedul_id) values ('%d', '%d')", 
			user_id, schedule_id); 

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "schedule_id", json_object_new_int(schedule_id));
	replyToAllClientsWithIncomingMessage(request);
	json_object_object_add(request, "user_id", json_object_new_int(user_id));	
	
	//replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;

}


json_object* getAllBookings(void *data){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("get-all-bookings"));	
	json_object *list_of_bookings = json_object_new_array();
	sprintf(test_query, 
			"select * from bookings natural join users join schedules where bookings.schedule_id = schedule.id"); 

	mysql_query(connection, test_query);// Querry execution
	while ((row = mysql_fetch_row(result)) != NULL){ 
		json_object *bookings = json_object_new_object();
		json_object_object_add(bookings, "booking_id", json_object_new_int(atoi(row[0])));
		json_object_object_add(bookings, "user_id", json_object_new_int(atoi(row[1])));
		json_object_object_add(bookings, "schedule_id", json_object_new_int(atoi(row[2])));
		json_object_object_add(bookings, "email", json_object_new_string(row[3]));
		json_object_object_add(bookings, "first_name", json_object_new_string(row[5]));
		json_object_object_add(bookings, "last_name", json_object_new_string(row[6]));
		json_object_object_add(bookings, "passport_serie", json_object_new_string(row[11]));
		json_object_object_add(bookings, "passport_number", json_object_new_string(row[12]));
		 json_object_array_add(list_of_bookings, bookings);
	}
	json_object_object_add(request, "result", list_of_bookings);
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;

}

json_object* deleteBookings(void *data, int user_id, int schedule_id){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("delete-bookings"));	
	
	if(user_id != 0 && schedule_id == 0)
		sprintf(test_query, 
				"delete from bookings where user_id='%d", 
				user_id); 
	
	else if(user_id == 0 && schedule_id != 0)
		sprintf(test_query, 
				"delete from bookings where schedule_id='%d", 
				schedule_id);

	else if(user_id != 0 && schedule_id != 0)
		sprintf(test_query, 
				"delete from bookings where user_id='%d' and schedule_id='%d'", 
				user_id, schedule_id);

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "schedule_id", json_object_new_int(schedule_id));
	replyToAllClientsWithIncomingMessage(request);
	json_object_object_add(request, "user_id", json_object_new_int(user_id));	
	
	//replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;

}

//
//Actions with trains
//

json_object* addTrain(void *data, char *name, int quantity_carriage, int quantity_seats, double rate_per_km, double avg_speed){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("add-train"));	
	
	sprintf(test_query, 
			"insert into bookings (name, quantity_carriage, quantity_seats, rate_per_km, avg_speed) values ('%s', '%d', '%d', '%lf', '%lf')", 
			 name, quantity_carriage, quantity_seats, rate_per_km, avg_speed); 

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "train-name", json_object_new_string(name));
	
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}

json_object* updateTrain(void *data, char *name, int quantity_carriage, int quantity_seats, double rate_per_km, double avg_speed){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("update-train"));	
	
	sprintf(test_query, 
			"update bookings set (name, quantity_carriage, quantity_seats, rate_per_km, avg_speed) values ('%s', '%d', '%d', '%lf', '%lf')", 
			 name, quantity_carriage, quantity_seats, rate_per_km, avg_speed); 

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "train-name", json_object_new_string(name));
	
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}

json_object* getAllTrains(void *data){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("get-all-trains"));	
	json_object *list_of_trains = json_object_new_array();
	sprintf(test_query, 
			"select * from trains"); 

	mysql_query(connection, test_query);// Querry execution
	while ((row = mysql_fetch_row(result)) != NULL){ 
		char* ptr;
		json_object *trains = json_object_new_object();
		json_object_object_add(trains, "train_id", json_object_new_int(atoi(row[0])));
		json_object_object_add(trains, "name", json_object_new_int(atoi(row[1])));
		json_object_object_add(trains, "quantity_carriage", json_object_new_int(atoi(row[2])));
		json_object_object_add(trains, "quantity_seats", json_object_new_int(atoi(row[3])));
		json_object_object_add(trains, "rate_per_km", json_object_new_double(strtod(row[5], ptr)));
		json_object_object_add(trains, "avg_speed", json_object_new_double(strtod(row[6], ptr)));
		json_object_array_add(list_of_trains, trains);
	}
	json_object_object_add(request, "result", list_of_trains);
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;

}

json_object* getTrain(void *data, int train_id){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("get-all-trains"));	
	sprintf(test_query, 
			"select * from trains where id = '%d'", train_id); 

	mysql_query(connection, test_query);// Querry execution

	char* ptr;
	json_object *train = json_object_new_object();
	json_object_object_add(train, "train_id", json_object_new_int(atoi(row[0])));
	json_object_object_add(train, "name", json_object_new_int(atoi(row[1])));
	json_object_object_add(train, "quantity_carriage", json_object_new_int(atoi(row[2])));
	json_object_object_add(train, "quantity_seats", json_object_new_int(atoi(row[3])));
	json_object_object_add(train, "rate_per_km", json_object_new_double(strtod(row[5], ptr)));
	json_object_object_add(train, "avg_speed", json_object_new_double(strtod(row[6], ptr)));

	json_object_object_add(request, "result", train);
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;

}

json_object* deleteTrain(void *data, int train_id){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("delete-train"));	
	
		sprintf(test_query, 
				"delete from trains where id='%d", 
				train_id); 
	
	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "train_id", json_object_new_int(train_id));
	
	//replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}

//
//Actions with wallets
//

json_object* addWallet(void *data, int user_id, double balance){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("add-wallet"));	
	
	sprintf(test_query, 
			"insert into wallets (user_id, balance) values ('%d', '%lf')", 
			 user_id, balance); 

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "user_id", json_object_new_int(user_id));
	
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}

json_object* updateWallet(void *data, int user_id, double balance){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("update-wallet"));	
	
	sprintf(test_query, 
			"update wallets set balance = '%lf' where user_id = '%d'", 
			 balance, user_id); 

	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "user_id", json_object_new_int(user_id));
	
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}

json_object* deleteWallet(void *data, int user_id){

	char test_query[256];
	json_object *request = json_object_new_object();
	json_object_object_add(request, "action", json_object_new_string("delete-wallet"));	
	
		sprintf(test_query, 
				"delete from wallets where user_id='%d", 
				user_id); 
	
	mysql_query(connection, test_query);// Querry execution
	json_object_object_add(request, "user_id", json_object_new_int(user_id));
	
	//replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return request;
}



#endif // ACTIONS_H
