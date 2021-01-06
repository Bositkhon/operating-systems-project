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

GtkWidget *window_user;
GtkWidget *window_dashboard;


typedef struct{
	GtkEntry *email;
	GtkEntry *password;
	GtkButton *login;
	GtkLabel *message
} sign_in_widgets;

typedef struct{
    GtkEntry *email;
	GtkEntry *password;
	GtkEntry *firstname;
    GtkEntry *lastname;
    GtkEntry *phone;
    gchar *gender;
    GtkEntry *birth_date;
    GtkEntry *passport_serial;
    GtkEntry *passport_number;
} sign_up_widgets;

typedef struct {
	GtkButton *button_logout;
} dashboard_widgets;

sign_in_widgets *s_in_widgets;
sign_up_widgets *s_up_widgets;
dashboard_widgets *d_widgets;

int sock;
struct sockaddr_in server;
struct hostent *hp;
unsigned short port;
gchar buffer[2048];

json_object *request;
json_object *response;

void request_init()
{
    request = json_object_new_object();
    json_object_object_add(request, "action", NULL);
    json_object_object_add(request, "payload", NULL);
}

void response_init()
{
    response = json_object_new_object();
    json_object_object_add(response, "action", NULL);
    json_object_object_add(response, "payload", NULL);
}

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
            // const char *message = json_object_get_string(json_object_object_get(payload, "message"));
         
            if (success) {
                // TODO: on success
                gtk_widget_hide(window_user);
					 gtk_widget_show(window_dashboard);
            } else {
                // TODO: on fail
                json_object *message = json_object_object_get(payload, "message");
                gtk_label_set_text(s_in_widgets->message, json_object_get_string(message));
            }
        } else if (strcmp(action, "sign-up")) {
            // json_object *payload = json_object_object_get(response, "payload");
            // const int success = json_object_get_int(json_object_object_get(payload, "success"));
            // const char *message = json_object_get_string(json_object_object_get(payload, "message"));
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
    s_in_widgets = g_slice_new(sign_in_widgets);
    s_up_widgets = g_slice_new(sign_up_widgets);
    d_widgets = g_slice_new(dashboard_widgets);
    

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
    gtk_builder_add_from_file (builder, "glade/main.glade", NULL);

    window_user = GTK_WIDGET(gtk_builder_get_object(builder, "window_user"));
    window_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "window_dashboard"));
    
    s_in_widgets->email = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_email"));
    s_in_widgets->message = (GtkLabel *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_label_message"));
    s_in_widgets->password  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_password"));
    s_in_widgets->login  = (GtkButton *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button_login"));
    
    s_up_widgets->email  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_email"));
    s_up_widgets->password  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_password"));
    s_up_widgets->firstname  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_firstname"));
    s_up_widgets->lastname  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_lastname"));
    s_up_widgets->phone  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_phone"));
    s_up_widgets->gender  = "male";
    s_up_widgets->birth_date  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_birth_date"));
    s_up_widgets->passport_serial  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_passport_serial"));
    s_up_widgets->passport_number  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_passport_number"));
    
    d_widgets->button_logout = (GtkButton *)GTK_WIDGET(gtk_builder_get_object(builder, "dashboard_button_logout"));
    
    
    gtk_builder_connect_signals(builder, NULL);


    g_object_unref(builder);
    
    
    gtk_widget_show(window_user);
     

    gtk_main();
    g_slice_free(sign_in_widgets, s_in_widgets);
    g_slice_free(sign_up_widgets, s_up_widgets);
    g_slice_free(dashboard_widgets, d_widgets);
     
    return 0;
}

void on_window_user_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}
void on_window_dashboard_destroy()
{
    send(sock, "e", 1, 0);
    gtk_main_quit();
}

void on_sign_in_button_login_clicked(GtkButton *button)
{

    const gchar *email = gtk_entry_get_text(s_in_widgets->email);
    const gchar *password = gtk_entry_get_text(s_in_widgets->password);

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

void on_sign_up_button_register_clicked(GtkButton *button)
{
    json_object *signInString = json_object_new_string("sign-up");
    json_object_object_add(request, "action", signInString);
    
    json_object *payload = json_object_new_object();
    json_object_object_add(payload, "email", json_object_new_string(gtk_entry_get_text(s_up_widgets->email)));
    
    json_object_object_add(payload, "password", json_object_new_string(gtk_entry_get_text(s_up_widgets->password)));
    json_object_object_add(payload, "first_name", json_object_new_string(gtk_entry_get_text(s_up_widgets->firstname)));
    json_object_object_add(payload, "last_name", json_object_new_string(gtk_entry_get_text(s_up_widgets->lastname)));
    json_object_object_add(payload, "phone", json_object_new_string(gtk_entry_get_text(s_up_widgets->phone)));
    json_object_object_add(payload, "gender", json_object_new_string(s_up_widgets->gender));
    json_object_object_add(payload, "birth_date", json_object_new_string(gtk_entry_get_text(s_up_widgets->birth_date)));
    json_object_object_add(payload, "passport_serial", json_object_new_string(gtk_entry_get_text(s_up_widgets->passport_serial)));
    json_object_object_add(payload, "passport_number", json_object_new_string(gtk_entry_get_text(s_up_widgets->passport_number)));
    
    
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

void on_dashboard_button_logout_clicked(){
		gtk_widget_hide(window_dashboard);
		gtk_widget_show(window_user);
}

void on_sign_up_gender_female_toggled(GtkRadioButton *r_btn){
	gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_btn));
	if(T) s_up_widgets->gender = "female";
}
void on_sign_up_gender_male_toggled(GtkRadioButton *r_btn){
	gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_btn));
	if(T) s_up_widgets->gender = "male";
}

