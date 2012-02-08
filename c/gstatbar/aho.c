/* example-start statusbar statusbar.c */

#include <gtk/gtk.h>
#include <glib.h>

GtkWidget *status_bar;

void push_item (GtkWidget *widget, gpointer data)
{
  static int count = 1;
  char buff[20];

  g_snprintf(buff, 20, "Item %d", count++);
  gtk_statusbar_push( GTK_STATUSBAR(status_bar), (guint) &data, buff);

  return;
}

void pop_item (GtkWidget *widget, gpointer data)
{
  gtk_statusbar_pop( GTK_STATUSBAR(status_bar), (guint) &data );
  return;
}

int main (int argc, char *argv[])
{

    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button;

    int context_id;

    gtk_init (&argc, &argv);

    /* ������ɥ��򿷵����� */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize( GTK_WIDGET (window), 200, 100);
    gtk_window_set_title(GTK_WINDOW (window), "GTK Statusbar Example");
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) gtk_exit, NULL);
 
    vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
          
    status_bar = gtk_statusbar_new();      
    gtk_box_pack_start (GTK_BOX (vbox), status_bar, TRUE, TRUE, 0);
    gtk_widget_show (status_bar);

    context_id = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar), "Statusbar example");

    button = gtk_button_new_with_label("push item");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
        GTK_SIGNAL_FUNC (push_item), &context_id);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 2);
    gtk_widget_show(button);              

    button = gtk_button_new_with_label("pop last item");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
        GTK_SIGNAL_FUNC (pop_item), &context_id);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 2);
    gtk_widget_show(button);              

    /* ��ˤ��Υ�����ɥ���Ǹ���ʳ���ɽ��������Τǡ������꡼��ˤ�
     * �������ƤˤȤӤĤ��� */
    gtk_widget_show(window);

    gtk_main ();

    return 0;
}
/* example-end */
