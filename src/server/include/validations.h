#if !defined(VALIDATIONS_H)
#define VALIDATIONS_H

#include <stdio.h>
#include "database.h"

extern MYSQL *connection;

int user_exists(const char *email)
{
	char test_query[256];
	int return_value;

	sprintf(test_query, "select id from users where email='%s'", email);

	mysql_query(connection, test_query);

	result = mysql_use_result(connection);

	if (result == NULL) {
		printf("error");
	}

	while ((row = (MYSQL_ROW *) mysql_fetch_row(result)) != NULL) {
		// Some dummy shit
	}

	int rows_num = mysql_num_rows(result);

	mysql_free_result(result);

	return rows_num;
}

#endif // VALIDATIONS_H
