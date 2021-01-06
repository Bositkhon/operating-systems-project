#if !defined(SCHEDULE_H)
#define SCHEDULE_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "../include/socket.h"

extern MYSQL *connection;

struct Schedule
{
    unsigned int id;
    unsigned int train_id;
    char *departure_city;
    char *arrival_city;
    char *departure_datetime;
    char *arrival_datetime;
    double distance;
};

json_object *get_schedule(int pk, void *data)
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *schedule = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	sprintf(query, "select * from schedules join trains on trains_id=trains.id where schedule.id = %d", pk);
	
	mysql_query(connection, query);

	result = mysql_use_result(connection);
	while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
	{

		distance = strtod((const char*)row[6], &ptr);
		ratePerKm = strtod((const char*)row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi((const char*)row[1]));
		departureCityString = json_object_new_string((const char*)row[2]);
		arrivalCityString = json_object_new_string((const char*)row[3]);
		departureDatetimeString = json_object_new_string((const char*)row[4]);
		arrivalDatetimeString = json_object_new_string((const char*)row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(schedule, "train_id", trainIdNum);
		json_object_object_add(schedule, "departure_city", departureCityString);
		json_object_object_add(schedule, "arrival_city", arrivalCityString);
		json_object_object_add(schedule, "departure_datetime", departureDatetimeString);
		json_object_object_add(schedule, "arrival_datetime", arrivalDatetimeString);
		json_object_object_add(schedule, "distance", distanceDouble);
		json_object_object_add(schedule, "price", priceDouble);


	}
    mysql_free_result(result);
    return schedule;
}

json_object *get_all_schedules(void *data)
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *list_of_schedules = json_object_new_array();
	json_object *schedule = json_object_new_object();

	json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	sprintf(query, "select * from schedules join trains on train_id=trains.id");

	mysql_query(connection, query);

	result = mysql_use_result(connection);
	while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
	{
        idNum = json_object_new_int(atoi((const char *)row[0]));
		distance = strtod((const char *)row[6], &ptr);
		ratePerKm = strtod((const char *)row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi((const char *)row[1]));
		departureCityString = json_object_new_string((const char *)row[2]);
		arrivalCityString = json_object_new_string((const char *)row[3]);
		departureDatetimeString = json_object_new_string((const char *)row[4]);
		arrivalDatetimeString = json_object_new_string((const char *)row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(schedule, "id", idNum);
		json_object_object_add(schedule, "train_id", trainIdNum);
		json_object_object_add(schedule, "departure_city", departureCityString);
		json_object_object_add(schedule, "arrival_city", arrivalCityString);
		json_object_object_add(schedule, "departure_datetime", departureDatetimeString);
		json_object_object_add(schedule, "arrival_datetime", arrivalDatetimeString);
		json_object_object_add(schedule, "distance", distanceDouble);
		json_object_object_add(schedule, "price", priceDouble);

		json_object_array_add(list_of_schedules, schedule);

	}
    mysql_free_result(result);
    return list_of_schedules;
}

int create_schedule(unsigned int train_id,
                    char *departure_city,
                    char *arrival_city,
                    char *departure_datetime,
                    char *arrival_datetime,
                    double distance,
                    void *data) {
	char query[256];

	sprintf(query, 
		"insert into schedules (train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance) values (%d, '%s', '%s', '%s', '%s', %lf)", 
        train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance);

    mysql_free_result(result);
	return mysql_query(connection, query);
}

int delete_schedule(int pk, void *data)
{
	char query[256];
	sprintf(query, "delete from schedules where id=%d", pk);//TODO after booking deletion
	return mysql_query(connection, query);
}

int update_schedule(int pk,
                    int train_id,
                    char *departure_city,
                    char *arrival_city,
                    char *departure_datetime,
                    char *arrival_datetime,
                    double distance,
                    void *data) {
    char query[256];

	sprintf(query, "update schedules set train_id=%d, departure_city='%s', arrival_city='%s', departure_datetime='%s', arrival_datetime='%s', distance=%lf where id=%d",
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance, pk);
	
    mysql_free_result(result);
	return mysql_query(connection, query);
}

#endif // SCHEDULE_H
