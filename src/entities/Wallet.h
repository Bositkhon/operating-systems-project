#if !defined(WALLET_H)
#define WALLET_H

#include <json-c/json.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <socket.h>

extern MYSQL *connection;

struct Wallet
{
    unsigned int id;
    unsigned int user_id;
    double balance;
};

int create_wallet(int user_id, double balance, void *data){
	char test_query[256];
	sprintf(test_query, "insert into wallets (user_id, balance) values (%d, %lf)", user_id, balance); 

	mysql_free_result(result);
//	replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
}

int increase_wallet_amount(double amount, int pk, void *data) {
    char test_query[256];
	sprintf(test_query, "update wallets set balance = balance + %lf where id=%d", amount, pk); 

	mysql_free_result(result);
//	replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
}

int reduce_wallet_amount(double amount, int pk, void *data) {
    char test_query[256];
	sprintf(test_query, "update wallets set balance = balance - %lf where id=%d", amount, pk); 

	mysql_free_result(result);
//	replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
}

int delete_wallet(int user_id, void *data){
	char test_query[256];
    sprintf(test_query, "delete from wallets where id=%d", user_id);
	
	mysql_free_result(result);
	//replyToClientWithMessage((void *)(long)data, request);
	return mysql_query(connection, test_query);// Querry execution
}

#endif // WALLET_H
