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
#include "include/request_response.h"
#include "windows/landing.h"
#include "windows/dashboard.h"

void dashboard_show();
void *receiveFromServer(void *data);
void switch_windows(GtkWidget *window_current, GtkWidget *window_to);

GtkBuilder *builder; 

GtkWidget *landing_window;
GtkWidget *dashboard_window;

extern sign_in_widgets *signInWidgets;
extern sign_up_widgets *signUpWidgets;

extern dashboard_widgets *dashboardWidgets;
extern schedules_tree_view_widgets *schedulesTreeViewWidgets;

extern json_object *request;
extern json_object *response;

int sock;
struct sockaddr_in server;
struct hostent *hp;
unsigned short port;
gchar buffer[2048];

void error(char *msg)
{
  perror(msg);
  exit(0);
}

void *receiveFromServer(void *data)
{
    int n;
    while (1) {
        n = recv(sock, buffer, 2048, 0);

        if (n < 1) {
            error("Reading from socket");
        }
        buffer[n] = '\0';
        response = json_tokener_parse(buffer);

        const char *action = json_object_get_string(json_object_object_get(response, "action"));

        if (strcmp(action, "sign-in")) {
            json_object *payload = json_object_object_get(response, "payload");

            const int success = json_object_get_int(json_object_object_get(payload, "success"));
            if (success) {
                // TODO: on success
                dashboard_show();
            } else {
                // TODO: on fail
                json_object *message = json_object_object_get(payload, "message");
                gtk_label_set_text(signInWidgets->message, json_object_get_string(message));
            }
        } else if (strcmp(action, "sign-up")) {
            // TODO: on sign-up
        }

        printf("(Client) The message received: %s\n", json_object_to_json_string(response));
    }

    return 0;
}

int main(int argc, char *argv[])
{
    request_init();

    pthread_t tid_read;

    gtk_init(&argc, &argv);

    signInWidgets = g_slice_new(sign_in_widgets);
    signUpWidgets = g_slice_new(sign_up_widgets);
    dashboardWidgets = g_slice_new(dashboard_widgets);
    schedulesTreeViewWidgets = g_slice_new(schedules_tree_view_widgets);

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

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/main.glade", NULL);

    landing_window = GTK_WIDGET(gtk_builder_get_object(builder, "landing_window"));
    dashboard_window = GTK_WIDGET(gtk_builder_get_object(builder, "dashboard_window"));
    
    intialize_landing_window_widgets();

    initialize_dashboard_window_widgets();
    
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);
    
    gtk_widget_show(landing_window);
    gtk_widget_show(dashboard_window);
    gtk_widget_hide(dashboard_window);

    pthread_create(&tid_read, NULL, receiveFromServer, NULL);

    gtk_main();

    g_slice_free(sign_in_widgets, signInWidgets);
    g_slice_free(sign_up_widgets, signUpWidgets);
    g_slice_free(dashboard_widgets, dashboardWidgets);
    g_slice_free(schedules_tree_view_widgets, schedulesTreeViewWidgets);

    return 0;
}

void on_landing_window_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}
void on_dashboard_window_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}

void switch_windows(GtkWidget *window_current, GtkWidget *window_to)
{
	gtk_widget_hide(window_current);
	gtk_widget_show(window_to);
}

void dashboard_show()
{
    GtkTreeIter iter;
    int i = 0;
    while (i <= 2) {
        gtk_tree_store_append(schedulesTreeViewWidgets->tree_store, &iter, NULL);
        gtk_tree_store_set(schedulesTreeViewWidgets->tree_store, &iter, 0, "Tashkent", -1);
        gtk_tree_store_set(schedulesTreeViewWidgets->tree_store, &iter, 1, "Samarkand", -1);
        gtk_tree_store_set(schedulesTreeViewWidgets->tree_store, &iter, 2, "2020-11", -1);
        gtk_tree_store_set(schedulesTreeViewWidgets->tree_store, &iter, 3, "2020-11", -1);
        gtk_tree_store_set(schedulesTreeViewWidgets->tree_store, &iter, 4, (double) 330, -1);

        i++;
    }

    schedulesTreeViewWidgets->schedule_tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(schedulesTreeViewWidgets->tree_view));

    switch_windows(landing_window, dashboard_window);
}

void on_sign_in_button_login_clicked(GtkButton *button)
{
    json_object *email = json_object_new_string(gtk_entry_get_text(signInWidgets->email));
    json_object *password = json_object_new_string(gtk_entry_get_text(signInWidgets->password));

    json_object_object_add(request, "action", json_object_new_string("sign-in"));

    json_object *payload = json_object_new_object();
    json_object_object_add(payload, "email", email);
    json_object_object_add(payload, "password", password);

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

    gtk_entry_set_text(signInWidgets->email, "");
    gtk_entry_set_text(signInWidgets->password, "");
}

void on_sign_up_button_register_clicked(GtkButton *button)
{
    json_object_object_add(request, "action", json_object_new_string("sign-up"));
    
    json_object *payload = json_object_new_object();
    json_object_object_add(payload, "email", json_object_new_string(gtk_entry_get_text(signUpWidgets->email)));
    json_object_object_add(payload, "password", json_object_new_string(gtk_entry_get_text(signUpWidgets->password)));
    json_object_object_add(payload, "first_name", json_object_new_string(gtk_entry_get_text(signUpWidgets->firstname)));
    json_object_object_add(payload, "last_name", json_object_new_string(gtk_entry_get_text(signUpWidgets->lastname)));
    json_object_object_add(payload, "phone", json_object_new_string(gtk_entry_get_text(signUpWidgets->phone)));
    json_object_object_add(payload, "gender", json_object_new_string(signUpWidgets->gender));
    json_object_object_add(payload, "birth_date", json_object_new_string(gtk_entry_get_text(signUpWidgets->birth_date)));

    json_object_object_add(
        payload,
        "passport_serial",
        json_object_new_string(gtk_entry_get_text(signUpWidgets->passport_serial))
    );

    json_object_object_add(
        payload,
        "passport_number", 
        json_object_new_string(gtk_entry_get_text(signUpWidgets->passport_number))
    );
    
    
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

void on_dashboard_button_logout_clicked()
{
    gtk_widget_hide(dashboard_window);
    gtk_widget_show(landing_window);
}

void on_sign_up_gender_female_toggled(GtkRadioButton *r_btn){
	gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_btn));
	if (T) signUpWidgets->gender = "female";
}
void on_sign_up_gender_male_toggled(GtkRadioButton *r_btn){
	gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_btn));
	if (T) signUpWidgets->gender = "male";
}

