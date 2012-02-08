/* example-start radiobuttons radiobuttons.c */

#include <gtk/gtk.h>
#include <glib.h>

void close_application( GtkWidget *widget, GdkEvent *event, gpointer data ) {
  gtk_main_quit();
}

main(int argc,char *argv[])
{
  static GtkWidget *window = NULL;
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *button;
  GtkWidget *separator;
  GSList *group;
  
  gtk_init(&argc,&argv);          
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
              GTK_SIGNAL_FUNC(close_application),
              NULL);

  gtk_window_set_title (GTK_WINDOW (window), "radio buttons");
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  box1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), box1);
  gtk_widget_show (box1);

  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  button = gtk_radio_button_new_with_label (NULL, "button1");
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
  button = gtk_radio_button_new_with_label(group, "button2");
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (button), TRUE);
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
  button = gtk_radio_button_new_with_label(group, "button3");
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);
  gtk_widget_show (separator);

  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);
  gtk_widget_show (box2);

  button = gtk_button_new_with_label ("close");
  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                 GTK_SIGNAL_FUNC(close_application),
                 GTK_OBJECT (window));
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (window);
     
  gtk_main();
  return(0);
}
/* example-end */
