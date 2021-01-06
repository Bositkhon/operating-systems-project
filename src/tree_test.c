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

GtkWidget *window;
GtkWidget *fixed;
GtkWidget *view;
GtkWidget *tree;
GtkTreeStore *tree_store;
GtkTreeView *tree_view;
GtkTreeViewColumn *first_name_column;
GtkTreeViewColumn *last_name_column;
GtkTreeViewColumn *email_column;
GtkTreeViewColumn *gender_column;
GtkTreeViewColumn *phone_column;
GtkTreeViewColumn *birth_date_column;
GtkTreeViewColumn *pasport_serial_column;
GtkTreeViewColumn *pasport_number_column;

GtkTreeViewColumn *button_column;

GtkTreeSelection *selection;
GtkCellRenderer *first_name_renderer;
GtkCellRenderer *email_renderer;
GtkCellRenderer *gender_renderer;
GtkCellRenderer *phone_renderer;
GtkCellRenderer *birth_date_renderer;
GtkCellRenderer *pasport_serial_renderer;
GtkCellRenderer *pasport_number_renderer;
GtkCellRenderer *last_name_renderer;
GtkBuilder *builder;
GtkEntry *button;
int i = 0;
void on_destroy();

int main (int argc, char *argv[]){
    
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file ("glade/user_table.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_builder_connect_signals(builder, NULL);

    fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
    view = GTK_WIDGET(gtk_builder_get_object(builder, "scroll_view"));
    tree_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "user_tree_store"));
    tree_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view"));

    first_name_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "first_name_column"));
    last_name_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "last_name_column"));
    email_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "email_column"));
    gender_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "gender_column"));
    phone_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "phone_column"));
    birth_date_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "birth_date_column"));
    pasport_serial_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "passport_serie_column"));
    pasport_number_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "passport_number_column"));
    button_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "button_column"));


    first_name_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "first_name_text"));
    last_name_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "last_name_text"));
    email_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "email_text"));
    gender_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "gender_text"));
    phone_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "phone_text"));
    birth_date_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "birth_date_text"));
    pasport_serial_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "passport_serie_text"));
    pasport_number_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "passport_number_text"));
    button = (GtkEntry *)GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button_login"));

    gtk_tree_view_column_add_attribute(first_name_column, first_name_renderer, "text", 0);
    gtk_tree_view_column_add_attribute(last_name_column, last_name_renderer, "text", 1);
    gtk_tree_view_column_add_attribute(email_column, email_renderer, "text", 2);
    gtk_tree_view_column_add_attribute(gender_column, gender_renderer, "text", 3);
    gtk_tree_view_column_add_attribute(phone_column, phone_renderer, "text", 4);
    gtk_tree_view_column_add_attribute(birth_date_column, birth_date_renderer, "text", 5);
    gtk_tree_view_column_add_attribute(pasport_serial_column, pasport_serial_renderer, "text", 6);
    gtk_tree_view_column_add_attribute(pasport_number_column, pasport_number_renderer, "text", 7);

    gtk_tree_view_column_add_attribute(button_column, button, TRUE, 8);

    GtkTreeIter iter;
    int i =0;
    while(i<=2){
    gtk_tree_store_append(tree_store, &iter, NULL);
    gtk_tree_store_set(tree_store, &iter, 0, "jonibek", -1);
    gtk_tree_store_set(tree_store, &iter, 1, "mansurov", -1);
    gtk_tree_store_set(tree_store, &iter, 2, "j@mail", -1);
    gtk_tree_store_set(tree_store, &iter, 3, "mail", -1);
    gtk_tree_store_set(tree_store, &iter, 4, "99891", -1);
    gtk_tree_store_set(tree_store, &iter, 5, "24/09", -1);
    gtk_tree_store_set(tree_store, &iter, 6, "AB", -1);
    gtk_tree_store_set(tree_store, &iter, 7, "99999", -1);
    
    gtk_tree_store_set(tree_store, &iter, 8, TRUE, -1);
    i++;
    }
    printf("\nHERE\n");

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void on_select_changed(GtkWidget *c){
    printf("\n\n\n\nIn function\n\n\n");
    if(i == 0){
        
        i++;
        return;
    }
    gchar *value;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(c), &model, &iter) == FALSE)
        return;

    gtk_tree_model_get(model, &iter, 0, &value, -1);
    
    printf("\nName: %s\n", value);

}


void on_destroy(){
    gtk_main_quit();
}