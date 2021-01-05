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

typedef struct {
	GtkEntry *email;
	GtkEntry *password;
	GtkEntry *login;
} sign_in_widgets;

//sign_in_widgets *s_in_widgets = g_slice_new(sign_in_widgets);
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
    sign_in_widgets *s_in_widgets = g_slice_new(sign_in_widgets);

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
    gtk_builder_add_from_file (builder, "glade/user-page.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    s_in_widgets->email  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_email"));
    s_in_widgets->password  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_password"));
    s_in_widgets->login  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button_login"));
    
    gtk_builder_connect_signals(builder, s_in_widgets);


    g_object_unref(builder);

    gtk_widget_show(window);   

    gtk_main();
    g_slice_free(sign_in_widgets, s_in_widgets);

    return 0;
}

void on_window_main_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}

void on_sign_in_button_login_clicked(GtkButton *button, sign_in_widgets *widgets)
{

    const gchar *email = gtk_entry_get_text(widgets->email);
    const gchar *password = gtk_entry_get_text(widgets->password);

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

void on_sign_up_button_register_clicked()
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

