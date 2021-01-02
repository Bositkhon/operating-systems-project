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

#define BUFSIZE 2048 // максимальный размер буфера
int clientfd_set[100]; // массив который будет хранить дескрипторы подключений клиентов
char buffer[BUFSIZE]; // массив буфера
pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER; // не знаю зачем она нужна, но не используется
int read_buffer_ok; // не используется
void *service_read(void*); // инициализация метода
void *replyToClientWithMessage(void *data, char *message);
void *replyToAllClientsWithIncomingMessage(void *data); // инициализация метода

// тип сообщения
struct msg {
	char *msg;
};

int main(int argc, char *argv[])
{
	int sockfd, clientfd, sockfd_len, client_len;
	unsigned short port;
	struct sockaddr_in server_addr, client_addr;
	pthread_t thread_id[100]; // массив дескрипторов тредов для подключенных клиентов
	
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

		json_object * jobj = json_tokener_parse(buffer);

		char *action = json_object_get_string(json_object_object_get(jobj, "action"));

		// роунтинг
		if (strcmp(action, "sign-in") == 0) {
			printf("SIGN IN\n");
		} else if (strcmp(action, "sign-up") == 0) {
			printf("SIGN-UP\n");
		}

		printf("(Server) Received message: %s\n", json_object_to_json_string(jobj)); // выводим в консоли сообщение 
		// fflush(stdout);
		replyToClientWithMessage((void *)(long) clientfd, "This is my text");
		// replyToAllClientsWithIncomingMessage((void *)(long) clientfd); // этот метод доставляет сообщение полученное от любого клиента всем клиентам
	}
	//pthread_mutex_unlock(&mutex_buffer);
	return NULL;
}

void *replyToClientWithMessage(void *data, char *message)
{
  	int clientfd = (int)(long)data;
  	int length = strlen(message);
	
	if (clientfd > 0) {
		send(clientfd, message, length, 0);
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
