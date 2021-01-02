#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <json-c/json.h>

GtkBuilder *builder; 
GtkWidget *window;

GtkEntry *emailEntry;
GtkEntry *passwordEntry;
GtkLabel *messagesLabel;

int sock;
struct sockaddr_in server;
struct hostent *hp;
unsigned short port;
gchar buffer[1024];

json_object *request;

void request_init()
{
    request = json_object_new_object();
    json_object_object_add(request, "action", NULL);
    json_object_object_add(request, "payload", NULL);
}

void error(char *msg)
{
  perror(msg);
  exit(0);
}

void *receiveFromServer (void *data)
{
    int n;
    while (1) {
        n = recv(sock, buffer, 1023, 0);

        if (n < 1) {
            error("Reading from socket");
        }

        buffer[n] = '\0';

        // TODO: action on messaged received
        printf("(Client) The message received: %s\n", buffer);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    request_init();
    pthread_t tid_read;

    gtk_init(&argc, &argv);

    if (argc != 3) {
        printf("Usage: %s server port\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        error("Opening socket");
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);

    if (hp == NULL) {
        error("Unknown host");
    }

    memcpy((char *) &server.sin_addr, *hp->h_addr_list, hp->h_length);

    port = (unsigned short) atoi(argv[2]);

    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        error("Connecting");
    }

    pthread_create(&tid_read, NULL, receiveFromServer, NULL);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "login-page.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowMain"));
    gtk_builder_connect_signals(builder, NULL);

    emailEntry = GTK_WIDGET(gtk_builder_get_object(builder, "emailEntry"));
    passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordEntry"));

    g_object_unref(builder);

    gtk_widget_show(window);   

    gtk_main();

    return 0;
}

void on_windowMain_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}

void signIn()
{

    const gchar *email = gtk_entry_get_text(emailEntry);
    const gchar *password = gtk_entry_get_text(passwordEntry);

    json_object *signInString = json_object_new_string("sign-in");
    json_object *emailString = json_object_new_string(email);
    json_object *passwordString = json_object_new_string(password);

    json_object_object_add(request, "action", signInString);

    json_object *payload = json_object_new_object();
    json_object_object_add(payload, "email", emailString);
    json_object_object_add(payload, "password", passwordString);

    json_object_object_add(request, "payload", payload);

    char temp_buff[100000];

    if (strcpy(temp_buff, json_object_to_json_string(request)) == NULL)
    {
        perror("strcpy");
    }

    if (write(sock, temp_buff, strlen(temp_buff)) == -1)
    {
        perror("write");
    }
}

void signUp()
{
    json_object *signInString = json_object_new_string("sign-up");
    json_object_object_add(request, "action", signInString);

    char temp_buff[100000];

    if (strcpy(temp_buff, json_object_to_json_string(request)) == NULL)
    {
        perror("strcpy");
    }

    if (write(sock, temp_buff, strlen(temp_buff)) == -1)
    {
        perror("write");
    }
}

