#if !defined(BOOKING_H)
#define BOOKING_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "../include/socket.h"

extern MYSQL *connection;

struct Booking
{
    unsigned int id;
    unsigned int user_id;
    unsigned int schedule_id;
};

int create_booking(int user_id, int schedule_id, void *data){
	char test_query[256];
	sprintf(test_query, "insert into bookings (user_id, schedule_id) values (%d, %d)", user_id, schedule_id); 

	//replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return mysql_query(connection, test_query);
}

json_object* get_all_bookings(void *data) {
	char test_query[256];
	json_object *list_of_bookings = json_object_new_array();
	sprintf(test_query, "select * from bookings natural join users join schedules where bookings.schedule_id = schedules.id");

	mysql_query(connection, test_query);// Querry execution
	while ((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL){ 
		json_object *bookings = json_object_new_object();
		json_object_object_add(bookings, "booking_id", json_object_new_int(atoi((const char*)row[0])));
		json_object_object_add(bookings, "user_id", json_object_new_int(atoi((const char*)row[1])));
		json_object_object_add(bookings, "schedule_id", json_object_new_int(atoi((const char*)row[2])));
		json_object_object_add(bookings, "email", json_object_new_string((const char*)row[3]));
		json_object_object_add(bookings, "first_name", json_object_new_string((const char*)row[5]));
		json_object_object_add(bookings, "last_name", json_object_new_string((const char*)row[6]));
		json_object_object_add(bookings, "passport_serie", json_object_new_string((const char*)row[11]));
		json_object_object_add(bookings, "passport_number", json_object_new_string((const char*)row[12]));
		json_object_array_add(list_of_bookings, bookings);
	}
//	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result);
	return list_of_bookings;
}

int delete_booking(int pk, void *data){
	char test_query[256];
    sprintf(test_query, "delete from bookings where id=%d", pk);
    
    mysql_free_result(result);
	return mysql_query(connection, test_query);	
	//replyToClientWithMessage((void *)(long)data, request);
}

#endif // BOOKING_H
