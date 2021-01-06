#if !defined(TRAIN_H)
#define TRAIN_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "../include/socket.h"

extern MYSQL *connection;

struct Train
{
    unsigned int id;
    char *name;
    unsigned int quantity_seats;
    double rate_per_km;
    double avg_speed;
};

int create_train(char *name, int quantity_carriage, int quantity_seats, double rate_per_km, double avg_speed, void *data){

	char test_query[256];
	
	sprintf(test_query, 
			"insert into trains (name, quantity_carriage, quantity_seats, rate_per_km, avg_speed) values ('%s', %d, %d, %lf, %lf)", 
			name, quantity_carriage, quantity_seats, rate_per_km, avg_speed); 

	mysql_free_result(result);
	
	return mysql_query(connection, test_query);// Querry execution
	
//	replyToClientWithMessage((void *)(long)data, request);
}

int update_train(char *name, int quantity_carriage, int quantity_seats, double rate_per_km, double avg_speed, void *data){

	char test_query[256];
	
	sprintf(test_query, 
			"update trains set (name, quantity_carriage, quantity_seats, rate_per_km, avg_speed) values ('%s', %d, %d, %lf, %lf)", 
			name, quantity_carriage, quantity_seats, rate_per_km, avg_speed); 

	mysql_free_result(result);
//	replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
	
}

json_object* get_all_trains(void *data){

	char test_query[256];
	json_object *list_of_trains = json_object_new_array();
	sprintf(test_query, "select * from trains"); 

	mysql_query(connection, test_query);// Querry execution

	while ((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL){ 
		char* ptr;
		json_object *trains = json_object_new_object();
		json_object_object_add(trains, "train_id", json_object_new_int(atoi((const char *)row[0])));
		json_object_object_add(trains, "name", json_object_new_int(atoi((const char *)row[1])));
		json_object_object_add(trains, "quantity_carriage", json_object_new_int(atoi((const char *)row[2])));
		json_object_object_add(trains, "quantity_seats", json_object_new_int(atoi((const char *)row[3])));
		json_object_object_add(trains, "rate_per_km", json_object_new_double(strtod((const char *)row[5], &ptr)));
		json_object_object_add(trains, "avg_speed", json_object_new_double(strtod((const char *)row[6], &ptr)));
		json_object_array_add(list_of_trains, trains);
	}
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return list_of_trains;

}

json_object* get_train(int train_id, void *data){

	char test_query[256];
	sprintf(test_query, "select * from trains where id = '%d'", train_id); 

	mysql_query(connection, test_query);// Querry execution

	char* ptr;
	json_object *train = json_object_new_object();
	json_object_object_add(train, "train_id", json_object_new_int(atoi((const char *)row[0])));
	json_object_object_add(train, "name", json_object_new_int(atoi((const char *)row[1])));
	json_object_object_add(train, "quantity_carriage", json_object_new_int(atoi((const char *)row[2])));
	json_object_object_add(train, "quantity_seats", json_object_new_int(atoi((const char *)row[3])));
	json_object_object_add(train, "rate_per_km", json_object_new_double(strtod((const char *)row[5], &ptr)));
	json_object_object_add(train, "avg_speed", json_object_new_double(strtod((const char *)row[6], &ptr)));
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return train;

}

int delete_train(int pk, void *data) {

	char test_query[256];
    sprintf(test_query, "delete from trains where id='%d", pk); 
	
	mysql_free_result(result);
	//replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
	
}

#endif // TRAIN_H
