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

void socket_send_msg(GtkWidget *widget, gpointer *entry)
{
    int n;
    const gchar *msg = gtk_entry_get_text(GTK_ENTRY(entry));
    n = send(sock, msg, strlen(msg),0);
    if (n < strlen(msg))
        error("Writing to socket");
}


void *socket_recv_msg(void *data)
{
    // *data is of no use
    int n;
    while (1) {
        n = recv(sock, buffer, 1023, 0);
        if (n < 1) error("reading from socket");
        buffer[n]='\0';
        gtk_entry_set_text(GTK_ENTRY(income), buffer);
    }
        return NULL;
}

void quit(GtkWidget *widget, gpointer *data)
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}
int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button;
    pthread_t tid_read;

    gtk_init(&argc, &argv);

    if (argc != 3) {
        printf("Usage: %s server port\n", argv[0]);
        exit(1);
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) error("Opening socket");

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == NULL)
        error("Unknown host");

    memcpy((char *)&server.sin_addr, *hp->h_addr_list, hp->h_length);
    port = (unsigned short)atoi(argv[2]);
    server.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *)&server, sizeof server) < 0) {
        error("Connecting");
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

    vbox = gtk_vbox_new(TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    income = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), income, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

    button = gtk_button_new_with_label("Send");
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(socket_send_msg), entry);
    pthread_create(&tid_read, NULL, socket_recv_msg, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    //if (close(sock) < 0) error("closing");
    //printf("Client terminating\n");

    return 0;
}
