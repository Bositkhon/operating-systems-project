#if !defined(FEEDBACK_H)
#define FEEDBACK_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "../include/socket.h"

extern MYSQL *connection;

struct Feedback
{
    unsigned int id;
    unsigned int user_id;
    char *body;
};

json_object * get_feedback(int pk, void *data)
{
    char query[256];

    json_object *feedback = json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;

    sprintf(query, "select * from feedbacks where id=%d", pk);
    mysql_query(connection, query);

    result = mysql_use_result(connection);
    while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
    {
        idNum = json_object_new_int(atoi((const char *)row[0]));
        userIdNum = json_object_new_int(atoi((const char *)row[1]));
        bodyString = json_object_new_string((const char *)row[2]);

        json_object_object_add(feedback, "id", idNum);
        json_object_object_add(feedback, "user_id", userIdNum);
        json_object_object_add(feedback, "body", bodyString);
    }

    mysql_free_result(result);
    return feedback;
}

json_object *get_all_feedbacks(void *data)
{
    char query[256];

    json_object *list_of_feedbacks = json_object_new_array();
    json_object *feedback= json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;


    sprintf(query, "select * from feedbacks");
    mysql_query(connection, query);

    result = mysql_use_result(connection);
    while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
    {
        idNum = json_object_new_int(atoi((const char *)row[0]));
        userIdNum = json_object_new_int(atoi((const char *)row[1]));
        bodyString = json_object_new_string((const char *)row[2]);

        json_object_object_add(feedback, "id", idNum);
        json_object_object_add(feedback, "user_id", userIdNum);
        json_object_object_add(feedback, "body", bodyString);

        json_object_array_add(list_of_feedbacks, feedback);
    }

    mysql_free_result(result);
    return list_of_feedbacks;

}

int create_feedback(unsigned int user_id, char *body, void *data)
{	
    char query[256];
    sprintf(query, "insert into feedbacks (user_id, body) values (%d, '%s')", user_id, body);
    return mysql_query(connection, query);
}

int delete_feedback(int pk, void *data)
{
	char query[256];
	sprintf(query, "delete from feedbacks where id=%d", pk);
	return mysql_query(connection, query);
}

int update_feedback(int pk, unsigned int user_id, char *body)
{
    char query[256];
    sprintf(query, "update feedbacks set user_id=%d, body='%s'", user_id, body);
    return mysql_query(connection, query);
}

#endif // FEEDBACK_H
