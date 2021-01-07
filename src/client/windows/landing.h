#if !defined(LANDING_H)
#define LANDING_H

#include <gtk/gtk.h>

extern GtkBuilder *builder;

typedef struct{
	GtkEntry *email;
	GtkEntry *password;
	GtkButton *login;
    GtkLabel *message;
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

sign_in_widgets *signInWidgets;
sign_up_widgets *signUpWidgets;

void intialize_landing_window_widgets()
{
    signInWidgets->email = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_email"));
    signInWidgets->password  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_input_password"));
    signInWidgets->login  = (GtkButton *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button_login"));
    signInWidgets->message = (GtkLabel *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_label_message"));
    
    signUpWidgets->email  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_email"));
    signUpWidgets->password  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_password"));
    signUpWidgets->firstname  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_firstname"));
    signUpWidgets->lastname  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_lastname"));
    signUpWidgets->phone  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_phone"));
    signUpWidgets->gender  = "male";
    signUpWidgets->birth_date  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_birth_date"));
    signUpWidgets->passport_serial  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_passport_serial"));
    signUpWidgets->passport_number  = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_input_passport_number"));
}

#endif // LANDING_H
