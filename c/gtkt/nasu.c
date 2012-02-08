/* example-start table table.c */
#include <gtk/gtk.h>

/* ���Ĥ�Υ�����Хå���
 * ���δؿ����Ϥ��줿�ǡ�����ɸ����Ϥ�ɽ������� */
void callback (GtkWidget *widget, gpointer data)
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
}

/* ���Υ�����Хå��ϥץ�����λ������ */
void delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *table;

    gtk_init (&argc, &argv);

    /* ������������ɥ������ */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* ���Υ�����ɥ��Υ����ȥ������ */
    gtk_window_set_title (GTK_WINDOW (window), "Table");

    /* �������� GTK ��λ������褦�� delete_event �ϥ�ɥ������ */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (delete_event), NULL);

    /* ���Υ�����ɥ��Υܡ����������� */
    gtk_container_border_width (GTK_CONTAINER (window), 20);

    /* 2x2 �ơ��֥����� */
    table = gtk_table_new (2, 2, TRUE);

    /* �ᥤ�󥦥���ɥ��˥ơ��֥������ */
    gtk_container_add (GTK_CONTAINER (window), table);

    /* �ǽ�Υܥ������� */
    button = gtk_button_new_with_label ("button 1");

    /* �ܥ��󤬲����줿���ˡ�"callback" �ؿ���Ƥӡ����ΰ���
     * �Ȥ��� "button 1" �ؤΥݥ��󥿤��Ϥ� */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
              GTK_SIGNAL_FUNC (callback), (gpointer) "button 1");


    /* �ơ��֥�κ���ξݸ� [quadrant] �� button 1 ������ */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 0, 1, 0, 1);

    gtk_widget_show (button);

    /* �����ܤΥܥ������� */

    button = gtk_button_new_with_label ("button 2");

    /* �ܥ��󤬲����줿���ˡ�"callback" �ؿ���Ƥӡ����ΰ���
     * �Ȥ��� "button 2" �ؤΥݥ��󥿤��Ϥ� */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
              GTK_SIGNAL_FUNC (callback), (gpointer) "button 2");
    /* insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 1, 2, 0, 1);

    gtk_widget_show (button);

    /* "Quit" �ܥ������� */
    button = gtk_button_new_with_label ("Quit");

    /* �ܥ��󤬲����줿���ˡ�"delete_event" �ؿ���Ƥӡ��ץ�����
     * λ������ */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (delete_event), NULL);

    /* �ơ��֥�β�¦�ξݸ�ξ���� quit �ܥ�������� */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 0, 2, 1, 2);

    gtk_widget_show (button);

    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
/* example-end */
