/* example-start clist clist.c */

#include        <gtk/gtk.h>
#include        <glib.h>

/* �͡��ʥ�����Хå��Υץ�ȥ����פ��������� */
void button_add_clicked( GtkWidget *button, gpointer data);
void button_clear_clicked( GtkWidget *button, gpointer data);
void button_hide_show_clicked( GtkWidget *button, gpointer data);
void selection_made( GtkWidget *clist, gint row, gint column,
                     GdkEventButton *event, gpointer data);

gint main (int argc, gchar *argv[])
{                                  
    GtkWidget       *window;
    GtkWidget       *vbox, *hbox;
    GtkWidget       *clist;
    GtkWidget       *button_add, *button_clear, *button_hide_show;    
    gchar           *titles[2] = {"Ingredients","Amount"};

    gtk_init(&argc, &argv);
    
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(GTK_WIDGET(window), 300, 150);

    gtk_window_set_title(GTK_WINDOW(window), "GtkCList Example");
    gtk_signal_connect(GTK_OBJECT(window),
                       "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit),
                       NULL);
    
    vbox=gtk_vbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    
    /* GtkCList �������������Ǥ� 2 ���Ȥ� */
    clist = gtk_clist_new_with_titles( 2, titles);

    /* ���쥯����󤬺������줿���ˤ�����Τꤿ�������Ѥ���륳����Х�
     * ���� selection_made �ǡ����Υ����ɤϤ�äȲ��Ǹ��Ĥ��� */
    gtk_signal_connect(GTK_OBJECT(clist), "select_row",
                       GTK_SIGNAL_FUNC(selection_made),
                       NULL);

    /* �ܡ������˱Ƥ��դ���ɬ�פʤ�̵���Τ���������Ϥ��������� :) */
    gtk_clist_set_border(GTK_CLIST(clist), GTK_SHADOW_OUT);

    /* �ʤˤ��ɤ�ʤ˽��פʤΤ��Ȥ����ȡ���������ꤷ�Ƥ��ʤ��������
     * �������ʤ�ʤ��Τ�����ա���� 0 �����������(���ξ���1�ޤ�)��
     */
    gtk_clist_set_column_width (GTK_CLIST(clist), 0, 150);

    /* ��������С���ɬ�פʤȤ����� */
    gtk_clist_set_policy(GTK_CLIST(clist), GTK_POLICY_AUTOMATIC,
                                           GTK_POLICY_AUTOMATIC);

    /* ��ľ�ܥå����� GtkCList ���������åȤ��ɲá������ɽ����*/
    gtk_box_pack_start(GTK_BOX(vbox), clist, TRUE, TRUE, 0);
    gtk_widget_show(clist);

    /* �ܥ��������������򥦥���ɥ��˲ä��롣����ˤĤ��ƤΥ����
     * �Ȥ䤵��ʤ���ϥܥ���Υ��塼�ȥꥢ��򻲾ȡ�
     */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
    gtk_widget_show(hbox);

    button_add = gtk_button_new_with_label("Add List");
    button_clear = gtk_button_new_with_label("Clear List");
    button_hide_show = gtk_button_new_with_label("Hide/Show titles");

    gtk_box_pack_start(GTK_BOX(hbox), button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_clear, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_hide_show, TRUE, TRUE, 0);

    /* ���ĤΥܥ���˥�����Хå�����³ */
    gtk_signal_connect_object(GTK_OBJECT(button_add), "clicked",
                              GTK_SIGNAL_FUNC(button_add_clicked),
                              (gpointer) clist);
    gtk_signal_connect_object(GTK_OBJECT(button_clear), "clicked",
                              GTK_SIGNAL_FUNC(button_clear_clicked),
                              (gpointer) clist);
    gtk_signal_connect_object(GTK_OBJECT(button_hide_show), "clicked",
                              GTK_SIGNAL_FUNC(button_hide_show_clicked),
                              (gpointer) clist);

    gtk_widget_show(button_add);
    gtk_widget_show(button_clear);
    gtk_widget_show(button_hide_show);

    /* ���󥿥ե�����������������Ǥ����Τǥ�����ɥ���ɽ������
     * gtk_main �롼�פ�����
     */
    gtk_widget_show(window);
    gtk_main();
    
    return 0;
}

/* �桼������ "Add List" �ܥ���򲡤�����*/
void button_add_clicked( GtkWidget *button, gpointer data)
{
    int         indx;

    /* �ꥹ�Ȥˤ�����ʤ���Τ�ä��롣4��2����б� */
    gchar      *drink[4][2] = {{"Milk", "3 Oz"},
                               {"Water", "6 l"},
                               {"Carrots", "2"},
                               {"Snakes", "55"}};

    /* �����Ǽºݤ˥ƥ����Ȥ�ä��롣����ϳƹԤ�1�٤��ĹԤ��롣
     */
    for( indx=0; indx < 4; indx++)
        gtk_clist_append( (GtkCList*) data, drink[indx]);

    return;
}

/* �桼������ "Clear List" �ܥ���򲡤�����*/
void button_clear_clicked( GtkWidget *button, gpointer data)
{
    /* gtk_clist_clear ��Ȥäƥꥹ�Ȥ򥯥ꥢ������� gtk_clist_remove
     * ��ƹԤ�1�٤��ĸƤ֤�������®����
     */
    gtk_clist_clear((GtkCList*) data);

    return;
}

/* �桼������ "Hide/Show titles" �ܥ���򲡤�����*/
void button_hide_show_clicked( GtkWidget *button, gpointer data)
{
    /* ñ�ʤ���֤�Ф��Ƥ����ե饰��0 = ���߲Ļ� */
    static short int flag = 0;

    if (flag == 0)
    {
        /* �����ȥ�򱣤��ƥե饰�� 1 ������ */
        gtk_clist_column_titles_hide((GtkCList*) data);
        flag++;
    }
    else
    {
        /* �����ȥ��ɽ�����ƥե饰�� 0 �˥ꥻ�å� */
        gtk_clist_column_titles_show((GtkCList*) data);
        flag--;
    }

    return;
}

/* �⤷�����ˤ����顢�桼�������ꥹ����ιԤ����򤷤Ƥ��롣*/
void selection_made( GtkWidget *clist, gint row, gint column,
                     GdkEventButton *event, gpointer data)
{
    gchar       *text;

    /* ����å��������򤵤줿 row ����� column ����¸����Ƥ���ƥ���
     * �Ȥ���������� text �˥ݥ��󥿤Ȥ��Ƽ�����롣
     */
    gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);

    /* ���򤵤줿 row �˴ؤ������򤿤�ɽ��������� */
    g_print("You selected row %d. More specifically you clicked in column %d, and the text in this cell is %s\n\n", row, column, text);

    return;
}
/* example-end */
