#if !defined(DATABASE_H)
#define DATABASE_H

#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#define DB_SERVER "localhost"
#define DB_USER "root"
#define DB_PASSWORD "1530606"
#define DB_NAME "test"

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
