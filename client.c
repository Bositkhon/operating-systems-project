#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int sock;
struct sockaddr_in server;
struct hostent *hp;
unsigned short port;
gchar buffer[1024];
GtkWidget *entry;
GtkWidget *income;

void error(char *msg)
{
  perror(msg);
  exit(0);
}

// отправка текста хранящегося в GtkEntry в сокет
void socket_send_msg(GtkWidget *widget, gpointer *entry)
{
  int n;
  const gchar *msg = gtk_entry_get_text(GTK_ENTRY(entry)); // вытащить текст от объекта entry
  n = send(sock, msg, strlen(msg),0); // отправить в сокет текст из entry (возвращает количество байтов)

  // если ничего не отправилось
  if (n < strlen(msg)) {
      error("Writing to socket");
  }
}

// получения ответа от сокета и обновление текств в GtkEntry
void *socket_recv_msg(void *data)
{
  // *data is of no use
  int n;
  while (1) {
    n = recv(sock, buffer, 1023, 0); // чтение сообщения из сокета (возвращает количество прочитанных байтов)

    // если не удалось что-либо прочесть из сокета
    if (n < 1) {
        error("reading from socket");
    }

    buffer[n]='\0'; // CLRF
    gtk_entry_set_text(GTK_ENTRY(income), buffer); // изменить текст в объекте income на сообщение которое пришло из сокета
    printf("The message received: %s\n", buffer);
  }

  return NULL;
}

void quit(GtkWidget *widget, gpointer *data)
{
  send(sock, "e", 1, 0); // отправлка сообщения о закрытии соединения
  gtk_main_quit(); // закрытие gui application
}

int main(int argc, char *argv[])
{
  GtkWidget *window; // объект GUI
  GtkWidget *vbox;  // объект GUI
  GtkWidget *button; // объект GUI
  pthread_t tid_read;

  gtk_init(&argc, &argv); // инициализация gtk программы с заданными аргументами консольной программы

  // проверка на валидность аргументов
  if (argc != 3) {
    printf("Usage: %s server port\n", argv[0]);
    exit(1);
  }

  sock = socket(AF_INET, SOCK_STREAM, 0); // создание сокета
  if (sock < 0) error("Opening socket");

  server.sin_family = AF_INET; // тип сокета
  hp = gethostbyname(argv[1]); // хост подключения сокета
  
  // если такого хоста не существует
  if (hp == NULL) {
      error("Unknown host");
  }

  memcpy((char *)&server.sin_addr, *hp->h_addr_list, hp->h_length); // копирует адрес из hp в server.sin_addr
  port = (unsigned short)atoi(argv[2]); // получение порта подключения из аргументов
  server.sin_port = htons(port); // обозначение порта подключения
  
  if (connect(sock, (struct sockaddr *)&server, sizeof server) < 0) { // подключение к сокету
    error("Connecting");
  }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // самое главное окно в gtk application
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(quit), NULL); // присванивание события при закрытии программы чтобы вызывала метод quit с аргументами NULL
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE); // обозначение позиции расположения

  vbox = gtk_vbox_new(TRUE, 0); // создание объекта gtk box
  gtk_container_add(GTK_CONTAINER(window), vbox); // запушить в окно window объект vbox

  income = gtk_entry_new(); // инициализация объект input field
  gtk_box_pack_start(GTK_BOX(vbox), income, TRUE, TRUE, 0); // запушить в окно window объект income

  entry = gtk_entry_new(); // инициализация объекта input field
  gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0); // запушить в окно window объект entry

  button = gtk_button_new_with_label("Send"); // инициализации объекта кнопки с label = send
  gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);  // запушить в окно window кнопку  button
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(socket_send_msg), entry); // присвание события нажатия на кнопку button и запуск метода socket_send_msg с аргументов entry
  
  // если всё успешно прошло то создаем типо threadа который запускает функцию socket_recv_msg с аргументом NULL
  pthread_create(&tid_read, NULL, socket_recv_msg, NULL);

  gtk_widget_show_all(window); // включает видимость окна window
  gtk_main(); // запуск gui application

  //if (close(sock) < 0) error("closing");
  //printf("Client terminating\n");

  return 0;
}
