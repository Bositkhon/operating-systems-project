#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <json-c/json.h>
#include <mysql.h>

#define BUFSIZE 2048 // максимальный размер буфера
int clientfd_set[100]; // массив который будет хранить дескрипторы подключений клиентов
char buffer[BUFSIZE]; // массив буфера
pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER; // не знаю зачем она нужна, но не используется
int read_buffer_ok; // не используется

int sockfd, clientfd, sockfd_len, client_len;
unsigned short port;
struct sockaddr_in server_addr, client_addr;

MYSQL *conn;
MYSQL_RES *result;
MYSQL_ROW *row;

void *service_read(void*); // инициализация метода
void *replyToClientWithMessage(void *data, json_object *request);
void *replyToAllClientsWithIncomingMessage(void *data); // инициализация метода
void dispatchAction(const char *action,  void *data);
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
	
json_object *response;


// schedule.h
int addSchedule(json_object *request);
int deleteSchedule(int pk);
int modifySchedule(json_object *request, int pk);
int getSchedule(void *data, int all=0, int pk);

int main(int argc, char *argv[])
{
	
	char *server = "localhost";
  	char *user = "amupd";
  	char *password = "password"; /* set me first */
  	char *database = "testdb";

	pthread_t thread_id[100]; // массив дескрипторов тредов для подключенных клиентов
	conn = mysql_init(NULL); //database initialization

    // валидация аргументов 
	if (argc < 2){
		fprintf(stderr, "usage %s portnumber\n", argv[0]);
		exit(0);
	}
	port = atoi(argv[1]); // порт из аргумента программы
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // создание сокет дескриптора с настройками
    // если не удалось создать сокет сервер
	if (sockfd < 0) {
		perror("Opening socket");
		exit(0);
	}

	server_addr.sin_family = AF_INET; // TCP
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port); // обозначение порта

	sockfd_len = sizeof(server_addr); // размер адреса

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { // создание сокета и присвоение порта
		perror("Binding socket");
		exit(0);
	}
	
	if (listen(sockfd, 5) < 0) { // начало прослушки
		perror("Listening");
		exit(0);
	}

	if(mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)==NULL) // Connects to a MySQL server.
	{
		perror("SQL connection error");
		exit(1);
	}
	while (1) {
		client_len = sizeof(client_addr); // размер адреса клиента
		clientfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len); // ожидание подключения клиента
		if (clientfd < 0) { // если не удалось подключиться
			perror("Accpeting");
		}
		//fcntl(clientfd, F_SETFL, O_NONBLOCK | O_FSYNC);
		for (int i = 0; i < 100; i++) { // проходимся по массиву подключенных клиентов
			if (clientfd_set[i] == 0) { // если есть подключенный
				clientfd_set[i] = clientfd; // создаем "сессию" для него
				pthread_create(&thread_id[i], NULL, service_read, (void *)(long)i); // создает отдельный процесс который запускает метод service_read которому передается переменная i
				break;
			}
		}
	}
	mysql_close(conn);// Closes a server connection.
	return 0;
}

void *service_read(void *data)
{
	int i = (int)(long)data; // что передолось в этот метод переконвертируемся обратно на тип int чтобы последующие разы отдавать ответ нужному клиенту
	int clientfd = clientfd_set[i]; // находим дескриптора клиента
	int n; // для прочитанных битов
	
	printf("Thread created, fd = %d\n", clientfd);
	while (1) {
		//pthread_mutex_lock(&mutex_buffer);
		n = recv(clientfd, buffer, BUFSIZE - 2, 0); // читаем что было отправлено от клиента
		buffer[n] = '\0'; // символ конца строки 

		if (buffer[0] == 'e') { // если сообщение начинается на букву 'e'
			close(clientfd); // закрываем подключение
			clientfd_set[i] = 0; // удаляем дескриптор
			pthread_exit(0); // заканчиваем тред процесс
		}

		response = json_tokener_parse(buffer);

		const char *action = json_object_get_string(json_object_object_get(response, "action"));
		dispatchAction(action, (void *)(long) clientfd);

		printf("(Server) Received message: %s\n", json_object_to_json_string(response)); // выводим в консоли сообщение 
		// fflush(stdout);
		//replyToClientWithMessage((void *)(long) clientfd, "This is my text");
		// replyToAllClientsWithIncomingMessage((void *)(long) clientfd); // этот метод доставляет сообщение полученное от любого клиента всем клиентам
	}
	//pthread_mutex_unlock(&mutex_buffer);
	return NULL;
}

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

void dispatchAction(const char *action, void *data)
{
	json_object *payload = json_object_object_get(response, "payload");

	if (strcmp(action, "sign-in") == 0) {
		const char *email = json_object_get_string(json_object_object_get(payload, "email"));
		const char *password = json_object_get_string(json_object_object_get(payload, "password"));

		signIn((void *)(long)data, email, password);
	} else if (strcmp(action, "sign-up") == 0) {
		const char *email = json_object_get_string(json_object_object_get(payload, "email"));
		const char *password = json_object_get_string(json_object_object_get(payload, "password"));
		const char *first_name = json_object_get_string(json_object_object_get(payload, "first_name"));
		const char *last_name = json_object_get_string(json_object_object_get(payload, "last_name"));
		const char *gender = json_object_get_string(json_object_object_get(payload, "gender"));
		const char *phone = json_object_get_string(json_object_object_get(payload, "phone"));
		const char *birth_day = json_object_get_string(json_object_object_get(payload, "birth_day"));
		const char *passport_serial = json_object_get_string(json_object_object_get(payload, "passport_serial"));
		const char *passport_number = json_object_get_string(json_object_object_get(payload, "passport_number"));

		signUp((void *)(long)data, email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number);
	}
}

void signIn(void *data, const char *email, const char *password)
{
	char test_query[256];
	int result_id;
	json_object *request = json_object_new_object();
		
	sprintf(test_query, "select id from user where email='%s' and password='%s'", email, password); 
	mysql_query(conn, test_query);// Querry execution

	result = mysql_use_result(conn);// Initiates a row-by-row result set retrieval.

	if (result == NULL)
	{
		perror("Initiates a row-by-row result error");
		exit(1);
	}
	while ((row = mysql_fetch_row(result)) != NULL){ // Fetches the next row from the result set.
		result_id = atoi(row[0]);
	}

	int rows_num = mysql_num_rows(result);// Returns the number of rows in a result set.
	
	json_object_object_add(request, "action", json_object_new_string("sign-in"));
	if(rows_num==0)
	{
		json_object_object_add(request, "user-id", json_object_new_int(-1));
	}
	else
	{
		json_object_object_add(request, "user-id", json_object_new_int(result_id));	
	}
	replyToClientWithMessage((void *)(long)data, request);
	mysql_free_result(result); // Frees memory used by a result set.
	

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
		json_object_object_add(request, "action", json_object_new_string("sign-up"));	

		if(validation(email))
		{
			json_object_object_add(request, "user-id", json_object_new_int(-1));
		}	
		else
		{
			sprintf(test_query, "insert into user (email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", 
						email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number); 
			mysql_query(conn, test_query);// Querry execution
			signIn((void *)(long)data, email, password);
		}

		printf("%s, %s, %s, %s, %s, %s, %s, %s, %s",
		email, password, first_name, last_name, gender, phone, birth_day, passport_serial, passport_number);
}

int validation(char *email)
{
	char test_query[256];
	int return_value;
	
	sprintf(test_query, "select id from user where email='%s'", email); 
	mysql_query(conn, test_query);// Querry execution

	result = mysql_use_result(conn); // Initiates a row-by-row result set retrieval.
	if (result == NULL)
	{
		perror("Initiates a row-by-row result error");
		exit(1);
	}
	while ((row = mysql_fetch_row(result)) != NULL){} // Fetches the next row from the result set.

	int rows_num = mysql_num_rows(result); // Returns the number of rows in a result set.

	if(rows_num==0)
	{
		return_value = 1;
	}
	else
	{
		return_value = 0;
	}
	mysql_free_result(result);
	return return_value;
}

json_object getSchedule(void *data, int all=1, int pk)
{
    char query[256], s_id[5];

	int coutner = 0;

	json_object *schedules = json_object_new_object();
	json_object *schedules_counter = json_object_new_object();
	json_object *schedules_data = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;

	json_object_object_add(schedules, "action", json_object_new_string("get-all-schedule"));
	if(all == 0)
	{
		sprintf(query, "select * from schedules where id = %d", pk);
	}
	else 
	{
		sprintf(query, "select * from schedules");
	}
	mysql_query(conn, query);

	result = mysql_use_result(conn);
	while((row = mysql_fetch_row(result)) != NULL)
	{
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(row[1]);
		departureCityString = json_object_new_string(row[2]);
		arrivalCityString = json_object_new_string(row[3]);
		departureDatetimeString = json_object_new_string(row[4]);
		arrivalDatetimeString = json_object_new_string(row[5]);
		distanceDouble = json_object_new_double(row[6]);

		json_object_object_add(schedules_data, "train-id", trainIdNum);
		json_object_object_add(schedules_data, "departure-city", departureCityString));
		json_object_object_add(schedules_data, "arrival-city", arrivalCityString));
		json_object_object_add(schedules_data, "departure-datetime", departureDatetimeString));
		json_object_object_add(schedules_data, "arrival-datetime", arrivalDatetimeString));
		json_object_object_add(schedules_data, "distance", distanceDouble));

		json_object_object_add(schedules_counter, itoa(row[0], s_id, 5), schedules_data);
		//TODO array
	}
    json_object_object_add(schedules, "result", schedules_counter)

    return schedules;
}

int addSchedule(json_object *request)
{
	const int train_id = json_object_get_int(json_object_object_get(request, "train-id"));
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city");
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city");
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime");
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime");
	const double double distance = json_object_get_double(json_object_object_get(request, "distance");

	char query[256];

	sprintf(query, 
		"insert into schedules (train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance) values (%d, '%s', '%s', '%s', '%s', %lf)", 
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance);

	return mysql_query(conn, query);
}

int deleteSchedule(int pk)
{
	char query[256];

	sprintf(query, "delete from schedules where id=%d", pk);//TODO after booking deletion
	return mysql_query(conn, query);
}

int modifySchedule(json_object *request, int pk)
{
	const int train_id = json_object_get_int(json_object_object_get(request, "train-id"));
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city");
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city");
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime");
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime");
	const double distance = json_object_get_double(json_object_object_get(request, "distance");
	char query[256];

	sprintf(query, "update schedules set train_id=%d, departure_city='%s', arrival_city='%s', departure_datetime='%s', arrival_datetime='%s', distance=%lf where id=%d",
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance, pk);
	
	return mysql_query(conn, query);
}