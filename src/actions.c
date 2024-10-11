#include "actions.h"

// Callback function for the start button click
void on_start_button_clicked(GtkButton *button, gpointer data)
{
    Widgets *widgets = (Widgets *)data;

    // Set default values for port_entry and env_entry if they are empty
    const char *port_text = gtk_editable_get_text(GTK_EDITABLE(widgets->port_entry));
    if (strlen(port_text) == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(widgets->port_entry), g_strdup_printf("%d", DEFAULT_PORT));
        port_text = gtk_editable_get_text(GTK_EDITABLE(widgets->port_entry));
    }

    const char *env_text = gtk_editable_get_text(GTK_EDITABLE(widgets->env_entry));
    if (strlen(env_text) == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(widgets->env_entry), DEFAULT_ENV);
        env_text = gtk_editable_get_text(GTK_EDITABLE(widgets->env_entry));
    }

    int port = atoi(port_text);
    if (port == 0)
    {
        port = DEFAULT_PORT;
    }

    const bool hermes_enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(widgets->hermes_checkbox));

    // Get the current working directory
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    // Construct the command to start the Metro Bundler
    char *p_url = g_strdup_printf("%s", cwd);
    char *p_cmd = g_strdup_printf("ENV=%s npx react-native start --port %d", env_text, port);
    // append hermes flag if enabled
    if (hermes_enabled)
    {
        p_cmd = g_strdup_printf("%s --experimental-debugger", p_cmd);
    }

    char *cmd = g_strdup_printf("cd %s && %s", p_url, p_cmd);
    char *osascript_cmd = g_strdup_printf(
        "osascript -e 'tell application \"Terminal\" to do script \"%s\"'", cmd);

    // Execute the osascript command to open a new Terminal window and run the command
    system(osascript_cmd);

    // Free the allocated memory for cmd and osascript_cmd
    g_free(cmd);
    g_free(osascript_cmd);

    // Immediately disable the start button and port entry
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->start_button), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->port_entry), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->env_entry), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->hermes_checkbox), FALSE);

    // Set up a periodic check for the connection and update the label
    g_timeout_add(1000, check_connection, widgets); // Check every 1 second
}

// Callback function for the terminate button click
void on_terminate_button_clicked(GtkButton *button, gpointer data)
{
    Widgets *widgets = (Widgets *)data;

    // Terminate the process running on the specified port
    system("pkill -f 'react-native'");

    // Update the label and button states
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->start_button), TRUE);      // Enable the start button
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->terminate_button), FALSE); // Disable the terminate button
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->restart_button), FALSE);   // Disable the restart button
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->env_entry), TRUE);          // Enable the IP entry
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->port_entry), TRUE);        // Enable the port entry
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->hermes_checkbox), TRUE);
}

// Callback function for the restart button click
void on_restart_button_clicked(GtkButton *button, gpointer data)
{
    Widgets *widgets = (Widgets *)data;

    // Terminate the process running on the specified port
    system("pkill -f 'react-native'");

    // Start the process again
    on_start_button_clicked(button, data);
}

void on_dark_mode_button_clicked(GtkButton *button, gpointer user_data)
{
  GtkSettings *settings = gtk_settings_get_default();
  static gboolean dark_mode;
  g_object_get(settings, "gtk-application-prefer-dark-theme", &dark_mode, NULL);
  g_object_set(settings, "gtk-application-prefer-dark-theme", !dark_mode, NULL);
}