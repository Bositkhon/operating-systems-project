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
json_object *getSchedule(int pk);
json_object *getAllSchedule();
int addSchedule(json_object *request);
int deleteSchedule(int pk);
int modifySchedule(json_object *request, int pk);

// feedback.h
json_object *getFeedback(int pk);
json_object *getAllFeedbacks();
int addFeedback(json_object *request);
int deleteFeedback(int pk);
int modifyFeedback(json_object *request, int pk);

// user.h
json_object *getUser(int pk);
json_object *getAllUsers();
int addUser(json_object *request);
int deleteUser(int pk);
int modifyUser(json_object *request, int pk);

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

json_object *getSchedule(int pk)
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *schedules = json_object_new_object();
	json_object *list_of_schedules = json_object_new_object();
	json_object *object = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	json_object_object_add(schedules, "action", json_object_new_string("get-all-schedule"));
	sprintf(query, "select * from schedules join trains on trains_id=trains.id where schedule.id = %d", pk);
	
	mysql_query(conn, query);

	result = mysql_use_result(conn);
	while((row = mysql_fetch_row(result)) != NULL)
	{

		distance = strtod(row[6], &ptr);
		ratePerKm = strtod(row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi(row[1]));
		departureCityString = json_object_new_string(row[2]);
		arrivalCityString = json_object_new_string(row[3]);
		departureDatetimeString = json_object_new_string(row[4]);
		arrivalDatetimeString = json_object_new_string(row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(object, "train-id", trainIdNum);
		json_object_object_add(object, "departure-city", departureCityString);
		json_object_object_add(object, "arrival-city", arrivalCityString);
		json_object_object_add(object, "departure-datetime", departureDatetimeString);
		json_object_object_add(object, "arrival-datetime", arrivalDatetimeString);
		json_object_object_add(object, "distance", distanceDouble);
		json_object_object_add(object, "price", priceDouble);

		json_object_array_add(list_of_schedules, object);

	}
    json_object_object_add(schedules, "result", list_of_schedules);

    return schedules;
}

json_object *getAllSchedules()
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *schedules = json_object_new_object();
	json_object *list_of_schedules = json_object_new_object();
	json_object *object = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	json_object_object_add(schedules, "action", json_object_new_string("get-all-schedule"));
	sprintf(query, "select * from schedules join trains on trains_id=trains.id");

	mysql_query(conn, query);

	result = mysql_use_result(conn);
	while((row = mysql_fetch_row(result)) != NULL)
	{

		distance = strtod(row[6], &ptr);
		ratePerKm = strtod(row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi(row[1]));
		departureCityString = json_object_new_string(row[2]);
		arrivalCityString = json_object_new_string(row[3]);
		departureDatetimeString = json_object_new_string(row[4]);
		arrivalDatetimeString = json_object_new_string(row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(object, "train-id", trainIdNum);
		json_object_object_add(object, "departure-city", departureCityString);
		json_object_object_add(object, "arrival-city", arrivalCityString);
		json_object_object_add(object, "departure-datetime", departureDatetimeString);
		json_object_object_add(object, "arrival-datetime", arrivalDatetimeString);
		json_object_object_add(object, "distance", distanceDouble);
		json_object_object_add(object, "price", priceDouble);

		json_object_array_add(list_of_schedules, object);

	}
    json_object_object_add(schedules, "result", list_of_schedules);

    return schedules;
}

int addSchedule(json_object *request)
{
	const int train_id = json_object_get_int(json_object_object_get(request, "train-id"));
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city"));
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city"));
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime"));
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime"));
	const double distance = json_object_get_double(json_object_object_get(request, "distance"));

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
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city"));
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city"));
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime"));
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime"));
	const double distance = json_object_get_double(json_object_object_get(request, "distance"));
	char query[256];

	sprintf(query, "update schedules set train_id=%d, departure_city='%s', arrival_city='%s', departure_datetime='%s', arrival_datetime='%s', distance=%lf where id=%d",
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance, pk);
	
	return mysql_query(conn, query);
}

json_object * getFeedback(int pk)
{
    char query[256];

    json_object *feedback = json_object_new_object();
    json_object *list_of_feedbacks = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;

    json_object_object_add(feedback, "action", json_object_new_string("get-feedback"));

    sprintf(query, "select * from feedbacks where id=%d", pk);
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        userIdNum = json_object_new_int(atoi(row[1]));
        bodyString = json_object_new_string(row[2]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "user-id", userIdNum);
        json_object_object_add(object, "body", bodyString);

        json_object_array_add(list_of_feedbacks, object);
    }

    json_object_object_add(feedback, "result", list_of_feedbacks);

    return feedback;

}

json_object * getAllFeedbacks()
{
    char query[256];

    json_object *feedback = json_object_new_object();
    json_object *list_of_feedbacks = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;

    json_object_object_add(feedback, "action", json_object_new_string("get-all-feedbacks"));

    sprintf(query, "select * from feedbacks");
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        userIdNum = json_object_new_int(atoi(row[1]));
        bodyString = json_object_new_string(row[2]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "user-id", userIdNum);
        json_object_object_add(object, "body", bodyString);

        json_object_array_add(list_of_feedbacks, object);
    }

    json_object_object_add(feedback, "result", list_of_feedbacks);

    return feedback;

}

int addFeedback(json_object *request)
{
	const int user_id = json_object_get_int(json_object_object_get(request, "user-id"));
	const char *body = json_object_get_string(json_object_object_get(request, "body"));
	
    char query[256];

    sprintf(query, "insert into feedbacks (user_id, body) values (%d, '%s')", user_id, body);

    return mysql_query(conn, query);

}

int deleteFeedback(int pk)
{
	char query[256];

	sprintf(query, "delete from feedbacks where id=%d", pk);
	return mysql_query(conn, query);
}

int modifyFeedback(json_object *request, int pk)
{
    const int id = json_object_get_int(json_object_object_get(request, "id"));
	const int user_id = json_object_get_int(json_object_object_get(request, "user-id"));
	const char *body = json_object_get_string(json_object_object_get(request, "body"));
	
    char query[256];

    sprintf(query, "update feedbacks set id=%d, user_id=%d, body='%s'", id, user_id, body);

    return mysql_query(conn, query);
}

json_object * getUser(int pk)
{
    char query[256];

    json_object *user = json_object_new_object();
    json_object *list_of_users = json_object_new_object();
    json_object *object = json_object_new_object();

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

    json_object_object_add(user, "action", json_object_new_string("get-user"));

    sprintf(query, "select * from user where id=%d", pk);
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        emailString = json_object_new_string(row[1]);
        passwordString = json_object_new_string(row[2]);
        firstNameString = json_object_new_string(row[3]);
        lastNameString = json_object_new_string(row[4]);
        genderString = json_object_new_string(row[5]);
        phoneString = json_object_new_string(row[6]);
        birthDateString = json_object_new_string(row[7]);
        passportSerieString = json_object_new_string(row[8]);
        passportNumberString = json_object_new_string(row[9]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "email", emailString);
        json_object_object_add(object, "password", passwordString);
        json_object_object_add(object, "first-name", firstNameString);
        json_object_object_add(object, "last-name", lastNameString);
        json_object_object_add(object, "gender", genderString);
        json_object_object_add(object, "phone", phoneString);
        json_object_object_add(object, "birth-date", birthDateString);
        json_object_object_add(object, "passport-serie", passportSerieString);
        json_object_object_add(object, "passport-number", passportNumberString);

        json_object_array_add(list_of_users, object);
    }

    json_object_object_add(user, "result", list_of_users);

    return user;

}

json_object * getAllUsers()
{
    char query[256];

    json_object *user = json_object_new_object();
    json_object *list_of_users = json_object_new_object();
    json_object *object = json_object_new_object();

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

    json_object_object_add(user, "action", json_object_new_string("get-user"));

    sprintf(query, "select * from users");
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        emailString = json_object_new_string(row[1]);
        passwordString = json_object_new_string(row[2]);
        firstNameString = json_object_new_string(row[3]);
        lastNameString = json_object_new_string(row[4]);
        genderString = json_object_new_string(row[5]);
        phoneString = json_object_new_string(row[6]);
        birthDateString = json_object_new_string(row[7]);
        passportSerieString = json_object_new_string(row[8]);
        passportNumberString = json_object_new_string(row[9]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "email", emailString);
        json_object_object_add(object, "password", passwordString);
        json_object_object_add(object, "first-name", firstNameString);
        json_object_object_add(object, "last-name", lastNameString);
        json_object_object_add(object, "gender", genderString);
        json_object_object_add(object, "phone", phoneString);
        json_object_object_add(object, "birth-date", birthDateString);
        json_object_object_add(object, "passport-serie", passportSerieString);
        json_object_object_add(object, "passport-number", passportNumberString);

        json_object_array_add(list_of_users, object);
    }

    json_object_object_add(user, "result", list_of_users);

    return user;

}

int addUser(json_object *request)
{
    const char *emailString = json_object_get_string(json_object_object_get(request, "email"));
    const char *passwordString = json_object_get_string(json_object_object_get(request, "password"));
    const char *firstNameString = json_object_get_string(json_object_object_get(request, "first-name "));
    const char *lastNameString = json_object_get_string(json_object_object_get(request, "last-name"));
    const char *genderString = json_object_get_string(json_object_object_get(request, "gender"));
    const char *phoneString = json_object_get_string(json_object_object_get(request, "phone"));
    const char *birthDateString = json_object_get_string(json_object_object_get(request, "birth-date"));
    const char *passportSerieString = json_object_get_string(json_object_object_get(request, "passport-seria"));
    const char *passportNumberString = json_object_get_string(json_object_object_get(request, "passport-number"));
	
    char query[256];

    sprintf(query, "insert into users (email, password, first_name, last_name, gender, phone, birth_date, passport_seria, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        emailString, passwordString, firstNameString, lastNameString, genderString, phoneString, birthDateString, passportSerieString, passportNumberString);

    return mysql_query(conn, query);

}

int removeUser(int pk)
{
    char query[256];

    sprintf(query, "delete from users where id=%d", pk);
    return mysql_query(conn, query);
}

int modifyUser(json_object *request, int pk)
{
    const char *emailString = json_object_get_string(json_object_object_get(request, "email"));
    const char *passwordString = json_object_get_string(json_object_object_get(request, "password"));
    const char *firstNameString = json_object_get_string(json_object_object_get(request, "first-name "));
    const char *lastNameString = json_object_get_string(json_object_object_get(request, "last-name"));
    const char *genderString = json_object_get_string(json_object_object_get(request, "gender"));
    const char *phoneString = json_object_get_string(json_object_object_get(request, "phone"));
    const char *birthDateString = json_object_get_string(json_object_object_get(request, "birth-date"));
    const char *passportSerieString = json_object_get_string(json_object_object_get(request, "passport-seria"));
    const char *passportNumberString = json_object_get_string(json_object_object_get(request, "passport-number"));
	
    char query[256];

    sprintf(query, "update users set email='%s', password='%s', first_name='%s', last_name='%s', gender='%s', phone='%s', birth_date='%s', passport_seria='%s', passport_number='%s')",
        emailString, passwordString, firstNameString, lastNameString, genderString, phoneString, birthDateString, passportSerieString, passportNumberString);

    return mysql_query(conn, query);
}