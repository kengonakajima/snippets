/* example-start menu menu.c */

#include <gtk/gtk.h>

static gint button_press (GtkWidget *, GdkEvent *);
static void menuitem_response (gchar *);

int main (int argc, char *argv[])
{

    GtkWidget *window;
    GtkWidget *menu;
    GtkWidget *menu_bar;
    GtkWidget *root_menu;
    GtkWidget *menu_items;
    GtkWidget *vbox;
    GtkWidget *button;
    char buf[128];
    int i;

    gtk_init (&argc, &argv);

    /* �����˥�����ɥ������ */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize( GTK_WIDGET (window), 200, 100);
    gtk_window_set_title(GTK_WINDOW (window), "GTK Menu Test");
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) gtk_main_quit, NULL);

    /* ��˥塼���������åȤ����������줫�� -- ��˥塼���������å�
     * �� gtk_show_widget() ���ƤϤʤ�ʤ�����פ��Ф�!!
     * ����ϥ�˥塼�����ƥ���ݻ������˥塼�ǡ����ץꥱ��������
     * "Root Menu" �򥯥�å�����ȥݥåץ��åפ��롣*/
    menu = gtk_menu_new();

    /* ���� "test-menu" �Τ���� 3 �ĤΥ�˥塼����ȥ��������뾮��
     * �ʥ롼�פ��Ѱդ��롣
     * gtk_menu_append ��Ƥ֤Τ���դ��뤳�ȡ������Ǥϥ�˥塼�˥�˥塼
     * �����ƥ�Υꥹ�Ȥ�ä��Ƥ��롣���̤ϡ��ƥ�˥塼�����ƥ��
     * "clicked" �����ʥ�򽦤������Υ�����Хå��ؿ����Ѱդ��뤬��
     * �����ǤϾ�������Τ���˾�ά���Ƥ��롣*/

    for(i = 0; i < 3; i++)
        {
            /* buf ��̾���򥳥ԡ��� */
            sprintf(buf, "Test-undermenu - %d", i);

            /* ������̾���դ���˥塼�����ƥ�����... */
            menu_items = gtk_menu_item_new_with_label(buf);

            /* ...�����Ƥ�����˥塼�˲ä��롣 */
            gtk_menu_append(GTK_MENU (menu), menu_items);

            /* ��˥塼�����ƥब���򤵤�����դ�Ҥ�����Ԥ��� */
            gtk_signal_connect_object(GTK_OBJECT(menu_items), "activate",
                GTK_SIGNAL_FUNC(menuitem_response), (gpointer) g_strdup(buf));

            /* ���������åȤ�ɽ�� */
            gtk_widget_show(menu_items);
        }

    /* ����ϥ롼�ȥ�˥塼�ȥ�˥塼�С���ɽ��������٥�ˤʤ롣
     * ����ϲ����줿����̲�äƤ����˥塼��ݥåץ��åפ�������ʤ�
     * �ǡ������ʥ�ϥ�ɥ��̵����*/
    root_menu = gtk_menu_item_new_with_label("Root Menu");

    gtk_widget_show(root_menu);

    /* ���ơ��������������줿 "menu" �� "root menu" �Υ�˥塼�ˤʤ��
     * ���˻��ꤹ�롣*/
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (root_menu), menu);

    /* ��˥塼�ȥܥ�������֤��� vbox */
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    /* ��˥塼���ݻ����뤿��Υ�˥塼�С�������������ᥤ�󥦥���
     * �ɥ��˲ä��롣*/
    menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 2);
    gtk_widget_show(menu_bar);

    /* ��˥塼��ݥåץ��åפ��뤿��˼���դ���ܥ������� */
    button = gtk_button_new_with_label("press me");
    gtk_signal_connect_object(GTK_OBJECT(button), "event",
        GTK_SIGNAL_FUNC (button_press), GTK_OBJECT(menu));
    gtk_box_pack_end(GTK_BOX(vbox), button, TRUE, TRUE, 2);
    gtk_widget_show(button);

    /* �����ƺǸ�ˡ���˥塼�����ƥ���˥塼�С����ɲä��� -- ����
     * �ϻ䤬���ä��褦�˸���³���Ƥ��� "root" ��˥塼�����ƥ�Ǥ��� */
    gtk_menu_bar_append(GTK_MENU_BAR (menu_bar), root_menu);

    /* �����꡼������Ƥ���٤�ȿ�Ǥ������Τǡ����ĤǤ� window �ϺǸ�
     * ���ʳ���ɽ�����롣*/
    gtk_widget_show(window);

    gtk_main ();

    return 0;
}

/* widget �Ȥ����Ϥ��줿��˥塼��Ǽ����ƥܥ���ץ쥹�˱������롣
 *
 * ��ա�"widget" �����ϷǼ�������˥塼�Ǥ��äơ������줿�ܥ���Ǥ�
 * �ʤ���
 */

static gint button_press (GtkWidget *widget, GdkEvent *event)
{

    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton *) event; 
        gtk_menu_popup (GTK_MENU(widget), NULL, NULL, NULL, NULL,
                        bevent->button, bevent->time);
        /* ���Υ��٥�Ȥΰ������Ѥ�����Ȥ�ƤӽФ�¦����𡣤��ʤ����
         * ���٥�Ȥ򤳤��ǿ�����Ȥ���*/
        return TRUE;
    }

    /* ���Υ��٥�Ȥΰ������Ѥ�Ǥ��ʤ����Ȥ�ƤӽФ�¦����𡣤��ʤ�
     * �������٥�Ȥ��̲ᤵ���롣*/
    return FALSE;
}


/* ��˥塼�����ƥब���򤵤줿���� string ��ɽ�� */

static void menuitem_response (gchar *string)
{
    printf("%s\n", string);
}
/* example-end */
