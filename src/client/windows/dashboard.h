#if !defined(DASHBOARD_H)
#define DASHBOARD_H

#include <gtk/gtk.h>

extern GtkBuilder *builder;

typedef struct {
    GtkWidget *view;
    GtkWidget *tree;

    GtkTreeStore *tree_store;

    GtkTreeView *tree_view;

    GtkTreeViewColumn *departure_city_column;
    GtkTreeViewColumn *arrival_city_column;
    GtkTreeViewColumn *departure_datetime_column;
    GtkTreeViewColumn *arrival_datetime_column;
    GtkTreeViewColumn *distance_column;

    GtkTreeSelection *schedule_tree_selection;

    GtkCellRenderer *departure_city_renderer;
    GtkCellRenderer *arrival_city_renderer;
    GtkCellRenderer *departure_datetime_renderer;
    GtkCellRenderer *arrival_datetime_renderer;
    GtkCellRenderer *distance_renderer;
} schedules_tree_view_widgets;

typedef struct {
    GtkButton *button_logout;
} dashboard_widgets;

dashboard_widgets *dashboardWidgets;
schedules_tree_view_widgets *schedulesTreeViewWidgets;

void initialize_dashboard_window_widgets();

void initialize_dashboard_window_widgets()
{
    dashboardWidgets->button_logout = (GtkButton *)GTK_WIDGET(gtk_builder_get_object(builder, "dashboard_button_logout"));

    schedulesTreeViewWidgets->tree_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "schedule_tree_store"));
    schedulesTreeViewWidgets->tree_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "schedule_tree_view"));
    schedulesTreeViewWidgets->departure_city_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "departure_city_column"));
    schedulesTreeViewWidgets->arrival_city_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "arrival_city_column"));
    schedulesTreeViewWidgets->departure_datetime_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "departure_date_column"));
    schedulesTreeViewWidgets->arrival_datetime_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "arrival_date_column"));
    schedulesTreeViewWidgets->distance_column = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "distance_column"));
    schedulesTreeViewWidgets->departure_city_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "departure_city_text"));
    schedulesTreeViewWidgets->arrival_city_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "arrival_city_text"));
    schedulesTreeViewWidgets->departure_datetime_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "departure_date_text"));
    schedulesTreeViewWidgets->arrival_datetime_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "arrival_date_text"));
    schedulesTreeViewWidgets->distance_renderer = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "distance_text"));

    gtk_tree_view_column_add_attribute(
        schedulesTreeViewWidgets->departure_city_column,
        schedulesTreeViewWidgets->departure_city_renderer,
        "text", 
        0
    );
    gtk_tree_view_column_add_attribute(
        schedulesTreeViewWidgets->arrival_city_column,
        schedulesTreeViewWidgets->arrival_city_renderer,
        "text", 1
    );

    gtk_tree_view_column_add_attribute(
        schedulesTreeViewWidgets->departure_datetime_column,
        schedulesTreeViewWidgets->departure_datetime_renderer,
        "text",
        2
    );
    gtk_tree_view_column_add_attribute(
        schedulesTreeViewWidgets->arrival_datetime_column,
        schedulesTreeViewWidgets->arrival_datetime_renderer,
        "text",
        3
    );
    gtk_tree_view_column_add_attribute(
        schedulesTreeViewWidgets->distance_column,
        schedulesTreeViewWidgets->distance_renderer,
        "text",
        4
    );
}

#endif // DASHBOARD_H
