#if !defined(USER_H)
#define USER_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <socket.h>

extern MYSQL *connection;

struct User
{
    unsigned int id;
    char *email;
    char *password;
	char *first_name;
	char *last_name;
	char *gender;
	char *phone;
	char *birth_day;
	char *passport_serial;
	char *passport_number;
};

json_object * get_user(int pk)
{
    char query[256];

    json_object *user = json_object_new_object();

    json_object *idNum;
    json_object *emailString;
    json_object *passwordString;
    json_object *firstNameString;
    json_object *lastNameString;
    json_object *genderString;
    json_object *phoneString;
    json_object *birthDateString;
    json_object *passportSerieString;
    json_object *passportNumberString;

    sprintf(query, "select * from users where id=%d", pk);
    mysql_query(connection, query);

    result = mysql_use_result(connection);
    while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
    {
        idNum = json_object_new_int(atoi((const char *)row[0]));
        emailString = json_object_new_string((const char *)row[1]);
        passwordString = json_object_new_string((const char *)row[2]);
        firstNameString = json_object_new_string((const char *)row[3]);
        lastNameString = json_object_new_string((const char *)row[4]);
        genderString = json_object_new_string((const char *)row[5]);
        phoneString = json_object_new_string((const char *)row[6]);
        birthDateString = json_object_new_string((const char *)row[7]);
        passportSerieString = json_object_new_string((const char *)row[8]);
        passportNumberString = json_object_new_string((const char *)row[9]);

        json_object_object_add(user, "id", idNum);
        json_object_object_add(user, "email", emailString);
        json_object_object_add(user, "password", passwordString);
        json_object_object_add(user, "first-name", firstNameString);
        json_object_object_add(user, "last-name", lastNameString);
        json_object_object_add(user, "gender", genderString);
        json_object_object_add(user, "phone", phoneString);
        json_object_object_add(user, "birth-date", birthDateString);
        json_object_object_add(user, "passport-serie", passportSerieString);
        json_object_object_add(user, "passport-number", passportNumberString);
    }

    mysql_free_result(result);
    return user;

}

json_object * get_all_users()
{
    char query[256];

    json_object *list_of_users = json_object_new_array();
    json_object *user = json_object_new_object();

    json_object *idNum;
    json_object *emailString;
    json_object *passwordString;
    json_object *firstNameString;
    json_object *lastNameString;
    json_object *genderString;
    json_object *phoneString;
    json_object *birthDateString;
    json_object *passportSerieString;
    json_object *passportNumberString;


    sprintf(query, "select * from users");
    mysql_query(connection, query);

    result = mysql_use_result(connection);
    while((row = (MYSQL_ROW *)mysql_fetch_row(result)) != NULL)
    {
        idNum = json_object_new_int(atoi((const char*)row[0]));
        emailString = json_object_new_string((const char*)row[1]);
        passwordString = json_object_new_string((const char*)row[2]);
        firstNameString = json_object_new_string((const char*)row[3]);
        lastNameString = json_object_new_string((const char*)row[4]);
        genderString = json_object_new_string((const char*)row[5]);
        phoneString = json_object_new_string((const char*)row[6]);
        birthDateString = json_object_new_string((const char*)row[7]);
        passportSerieString = json_object_new_string((const char*)row[8]);
        passportNumberString = json_object_new_string((const char*)row[9]);

        json_object_object_add(user, "id", idNum);
        json_object_object_add(user, "email", emailString);
        json_object_object_add(user, "password", passwordString);
        json_object_object_add(user, "first-name", firstNameString);
        json_object_object_add(user, "last-name", lastNameString);
        json_object_object_add(user, "gender", genderString);
        json_object_object_add(user, "phone", phoneString);
        json_object_object_add(user, "birth-date", birthDateString);
        json_object_object_add(user, "passport-serie", passportSerieString);
        json_object_object_add(user, "passport-number", passportNumberString);

        json_object_array_add(list_of_users, user);
    }

    mysql_free_result(result);
    return list_of_users;

}

int create_user(char *email,
				char *password,
				char *first_name,
				char *last_name,
				char *gender,
				char *phone,
				char *birth_date,
				char *passport_series,
				char *passport_number,
				void *data) {
	
    char query[256];
    sprintf(query, "insert into users (email, password, first_name, last_name, gender, phone, birth_date, passport_seria, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        email, password, first_name, last_name, gender, phone, birth_date, passport_series, passport_number);

    return mysql_query(connection, query);

}

int delete_user(int pk, void *data)
{
    char query[256];

    sprintf(query, "delete from users where id=%d", pk);
    return mysql_query(connection, query);
}

int update_user(int pk,
				char *email,
				char *first_name,
				char *last_name,
				char *gender,
				char *phone,
				char *birth_date,
				char *passport_series,
				char *passport_number,
				void *data) {
    char query[256];

    sprintf(query, "update users set email='%s', first_name='%s', last_name='%s', gender='%s', phone='%s', birth_date='%s', passport_serial='%s', passport_number='%s' where id=%d)",
		email, first_name, last_name, gender, phone, birth_date, passport_series, passport_number, pk);

    return mysql_query(connection, query);
}

int update_password(unsigned int pk, char *password) {
	char query[256];
	sprintf(query, "update users set password=%s where id=%d", password);
	return mysql_query(connection, query);
}

#endif // USER_H
