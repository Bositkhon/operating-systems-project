#if !defined(DATABASE_H)
#define DATABASE_H

#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#define DB_SERVER "localhost"
#define DB_USER "bositkhon"
#define DB_PASSWORD "bosit4me"
#define DB_NAME "railway"

MYSQL *connection;
MYSQL_RES *result;
MYSQL_ROW *row;

void establish_connection_to_database()
{
    connection = mysql_init(NULL);

    if (mysql_real_connect(connection, DB_SERVER, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) {
        perror("MySQL Connection can not be established");
        abort();
    }
}

#endif // DATABASE_H
