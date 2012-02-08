/* example-start buttons buttons.c */

#include <gtk/gtk.h>

/* ��˥��᡼���ȥ�٥���դ��������� hbox ��������ơ����Υܥå�����
 * �֤�... */

GtkWidget *xpm_label_box (GtkWidget *parent, gchar *xpm_filename, gchar *label_text)
{
    GtkWidget *box1;
    GtkWidget *label;
    GtkWidget *pixmapwid;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    GtkStyle *style;

    /* xpm ����ӥ�٥�Τ���˥ܥå�������� */
    box1 = gtk_hbox_new (FALSE, 0);
    gtk_container_border_width (GTK_CONTAINER (box1), 2);

    /* �ܥ���Υ�����������..���Хå����饦��ɥ��顼���������դ�
     * �򤹤롣�⤷ï����������ͳ���ΤäƤ���ʤ顢�ɤ����������ߤ�����*/
    style = gtk_widget_get_style(parent);

    /* ���� xpm �����åդ�.. xpm ���ɤ߹��� */
    pixmap = gdk_pixmap_create_from_xpm (parent->window, &mask,
                                         &style->bg[GTK_STATE_NORMAL],
                                         xpm_filename);
    pixmapwid = gtk_pixmap_new (pixmap, mask);

    /* �ܥ���Υ�٥����� */
    label = gtk_label_new (label_text);

    /* �ԥå����ޥåפȥ�٥��ܥ���˥ѥå� */
    gtk_box_pack_start (GTK_BOX (box1),
                        pixmapwid, FALSE, FALSE, 3);

    gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 3);

    gtk_widget_show(pixmapwid);
    gtk_widget_show(label);

    return (box1);
}

/* ��Υ�����Хå��ؿ� */
void callback (GtkWidget *widget, gpointer data)
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
}


int main (int argc, char *argv[])
{
    /* GtkWidget �ϥ��������åȤ���¢���Ǥ��� */
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;

    gtk_init (&argc, &argv);

    /* ������������ɥ������ */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "Pixmap'd Buttons!");

    /* ���ƤΥ�����ɥ��ˤ����Ԥ��Τ��ɤ��ͤ�����*/
    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                        GTK_SIGNAL_FUNC (gtk_exit), NULL);

    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
            GTK_SIGNAL_FUNC (gtk_exit), NULL);


    /* ������ɥ��Υܡ����������ꤹ�롣*/
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_realize(window);

    /* �������ܥ����������롣*/
    button = gtk_button_new ();

    /* ���ǤϤ�����¿���δؿ��򸫴���Ƥ���٤��Ǥ��롣*/
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (callback), (gpointer) "cool button");

    /* �������Υܥå��������ؿ���Ƥ֡�*/
    box1 = xpm_label_box(window, "info.xpm", "cool button");

    /* ���ƤΥ��������åȤ�ѥå�����ɽ�����롣*/
    gtk_widget_show(box1);

    gtk_container_add (GTK_CONTAINER (button), box1);

    gtk_widget_show(button);

    gtk_container_add (GTK_CONTAINER (window), button);

    gtk_widget_show (window);

    /* gtk_main �ǵ٤ߡ��Ϥޤ�Τ�ڤ��ߤ��Ԥġ�*/
    gtk_main ();

    return 0;
}
/* example-end */
