/* example-start list list.c */

/* gtk+ �إå��ե�����򥤥󥯥롼��
 * printf() �ؿ���ɬ�פʤΤ� stdio.h �򥤥󥯥롼��
 */
#include        <gtk/gtk.h>
#include        <stdio.h>

/* ����ϥꥹ�ȥ����ƥ�˥ǡ�������¸���뤿��Υǡ�������ʸ��
 * ��Ǥ���
 */
const   gchar   *list_item_data_key="list_item_data";


/* GtkList ���������åȤ���³���뤳�Ȥˤʤ륷���ʥ�ϥ�ɥ��
 * �ץ�ȥ�����
 */
static  void    sigh_print_selection    (GtkWidget      *gtklist,
                                         gpointer       func_data);
static  void    sigh_button_event       (GtkWidget      *gtklist,
                                         GdkEventButton *event,
                                         GtkWidget      *frame);


/* �桼�����󥿥ե������򥻥åȥ��åפ���ᥤ��ؿ� */

gint main (int argc, gchar *argv[])
{                                  
    GtkWidget       *separator;
    GtkWidget       *window;
    GtkWidget       *vbox;
    GtkWidget       *scrolled_window;
    GtkWidget       *frame;
    GtkWidget       *gtklist;
    GtkWidget       *button;
    GtkWidget       *list_item;
    GList           *dlist;
    guint           i;
    gchar           buffer[64];
    
    
    /* gtk+ ������ (���θ� gdk ��) */

    gtk_init(&argc, &argv);
    
    
    /* ���ƤΥ��������åȤ�����뤿��Υ�����ɥ��������
     * gtk_main_quit() �򤽤Υ�����ɥ��� "destory" ���٥�Ȥ���³����
     * ������ɥ��ޥ͡������ close-window-events �����
     */
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GtkList ��");
    gtk_signal_connect(GTK_OBJECT(window),
                       "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit),
                       NULL);
    
    
    /* ������ɥ�����¦�ǡ���ľ�˥��������åȤ����֤��뤿���
     * ɬ�פʥܥå���
     */
    vbox=gtk_vbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    
    /* ����������� GtkList ���������åȤ����֤��뤿���
     * ��������ɥ�����ɥ�
     */
    scrolled_window=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_usize(scrolled_window, 250, 150);
    gtk_container_add(GTK_CONTAINER(vbox), scrolled_window);
    gtk_widget_show(scrolled_window);
    
    /* GtkList ���������åȤ����
     * sigh_print_selection() �����ʥ�ϥ�ɥ�ؿ���
     * GtkList �� "selection_changed" �����ʥ����³����
     * �����ѹ����줿���λ������򤵤줿�����ƥ��ɽ������
     */
    gtklist=gtk_list_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), gtklist);
    gtk_widget_show(gtklist);
    gtk_signal_connect(GTK_OBJECT(gtklist),
                       "selection_changed",
                       GTK_SIGNAL_FUNC(sigh_print_selection),
                       NULL);
    
    /* �ꥹ�ȥ����ƥ���֤������ "Prison" [α�ֽ�] ����� ;)
     */
    frame=gtk_frame_new("Prison");
    gtk_widget_set_usize(frame, 200, 50);
    gtk_container_border_width(GTK_CONTAINER(frame), 5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
    gtk_container_add(GTK_CONTAINER(vbox), frame);
    gtk_widget_show(frame);
    
    /* GtkList �� sigh_button_event() �����ʥ�ϥ�ɥ����³��
     * ����ϥꥹ�ȥ����ƥ�� "arresting" [����] �򰷤�
     */
    gtk_signal_connect(GTK_OBJECT(gtklist),
                       "button_release_event",
                       GTK_SIGNAL_FUNC(sigh_button_event),
                       frame);
    
    /* ���ѥ졼�������
     */
    separator=gtk_hseparator_new();
    gtk_container_add(GTK_CONTAINER(vbox), separator);
    gtk_widget_show(separator);
    
    /* �Ǹ�ˡ��ܥ��������������� "clicked" �����ʥ�򥦥���ɥ�����
     * �Ǥ˷���դ���
     */
    button=gtk_button_new_with_label("Close");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_widget_show(button);
    gtk_signal_connect_object(GTK_OBJECT(button),
                              "clicked",
                              GTK_SIGNAL_FUNC(gtk_widget_destroy),
                              GTK_OBJECT(window));
    
    
    /* ����ǳơ������줾��Υ�٥����äƤ���ꥹ�ȥ����ƥ�� 5 �ĺ�
     * ������������� gtk_container_add() ��Ȥä� GtkList �˲ä���
     * �ޤ����Υ�٥뤫��ƥ�����ʸ�����ʹ���Ф��ơ��ƥꥹ�ȥ����ƥ�
     * �� list_item_data_key ���б��դ���
     */
    for (i=0; i<5; i++) {
        GtkWidget       *label;
        gchar           *string;
        
        sprintf(buffer, "�ꤹ�Ȥ����ƥࡩ �� Label #%d", i);
        label=gtk_label_new(buffer);
        list_item=gtk_list_item_new();
        gtk_container_add(GTK_CONTAINER(list_item), label);
        gtk_widget_show(label);
        gtk_container_add(GTK_CONTAINER(gtklist), list_item);
        gtk_widget_show(list_item);
        gtk_label_get(GTK_LABEL(label), &string);
        gtk_object_set_data(GTK_OBJECT(list_item),
                            list_item_data_key,
                            string);
    }
    /* �����ǡ����٤Ϻ����� gtk_list_item_new_with_label() ��ȤäƤ�
     * �� 5 ��٥�������롣
     * ��٥�ݥ��󥿤���äƤʤ��Τǥ�٥뤫��ƥ�����ʸ�����ʹ����
     * �����Ȥ��Ǥ��ʤ�����������Ϣ��ꥹ��(GList)����˥ꥹ�ȥ�����
     * �����Ƥ�ä���Τˡ��ꥹ�ȥ����ƥ�γ� list_item_data_key ��
     * ��Ʊ���ƥ�����ʸ������б����롣���줫��
     * gtk_list_append_items() ����Ƥ�Ǥ�����ä��롣
     * ���Ϣ��ꥹ�Ȥ���˥����ƥ���֤��Τˤ� g_list_prepend() ���
     * ���Τǡ��������¤ӤϹ߽�ˤʤ�(����� g_list_append() ���
     * ���о���ˤʤ�)��
     */
    dlist=NULL;
    for (; i<10; i++) {
        sprintf(buffer, "List Item with Label %d", i);
        list_item=gtk_list_item_new_with_label(buffer);
        dlist=g_list_prepend(dlist, list_item);
        gtk_widget_show(list_item);
        gtk_object_set_data(GTK_OBJECT(list_item),
                            list_item_data_key,
                            "ListItem with integrated Label");
    }
    gtk_list_append_items(GTK_LIST(gtklist), dlist);
    
    /* �Ǹ�ˤ��� window �򸫤Ƥߤ������ߤ����ʤ��ä�? ;)
     */
    gtk_widget_show(window);
    
    /* gtk �Υᥤ�󥤥٥�ȥ롼�פ����С�
     */
    gtk_main();
    
    /* �ᥤ�󥦥���ɥ����˲����줿��ȡ�
     * gtk_main_quit() ���ƤФ�Ƥ��θ�ˤ����ˤ��롣
     */
    return 0;
}

/* ����� GtkList �� button press/release ���٥�Ȥ���³���줿
 * �����ʥ�ϥ�ɥ�Ǥ��롣
 */
void
sigh_button_event       (GtkWidget      *gtklist,
                         GdkEventButton *event,
                         GtkWidget      *frame)
{
    /* 3����(���ֱ��Υޥ����ܥ���)��Υ���줿�Ф����Τ߹Ԥ�
     */
    if (event->type==GDK_BUTTON_RELEASE &&
        event->button==3) {
        GList           *dlist, *free_list;
        GtkWidget       *new_prisoner;
        
        /* ���μ��ͤȤʤ롢�������򤵤�Ƥ���ꥹ�ȥ����ƥ����� ;)
         */
        dlist=GTK_LIST(gtklist)->selection;
        if (dlist)
                new_prisoner=GTK_WIDGET(dlist->data);
        else
                new_prisoner=NULL;
        
        /* ���˴ƶؤ���Ƥ���ꥹ�ȥ����ƥ��õ�����ꥹ�Ȥ�����᤹��
         * gtk_container_children() ���֤������Ϣ��ꥹ�Ȥ���������
         * ��˺��ʤ���
         */
        dlist=gtk_container_children(GTK_CONTAINER(frame));
        free_list=dlist;
        while (dlist) {
            GtkWidget       *list_item;
            
            list_item=dlist->data;
            
            gtk_widget_reparent(list_item, gtklist);
            
            dlist=dlist->next;
        }
        g_list_free(free_list);
        
        /* �⤷����������[prisoner]���ᤨ�Ƥ���ʤ顢
         * ��� GtkList ���������ե졼�� "Prison" ��������ࡣ
         * �������˥����ƥ�� unselect ����ɬ�פ����롣
         */
        if (new_prisoner) {
            GList   static_dlist;
            
            static_dlist.data=new_prisoner;
            static_dlist.next=NULL;
            static_dlist.prev=NULL;
            
            gtk_list_unselect_child(GTK_LIST(gtklist),
                                    new_prisoner);
            gtk_widget_reparent(new_prisoner, frame);
        }
    }
}

/* ����� GtkList �� "selection_changed" �����ʥ��ȯ�Ԥ������˸Ƥ�
 * ��륷���ʥ�ϥ�ɥ�Ǥ��롣
 */
void
sigh_print_selection    (GtkWidget      *gtklist,
                         gpointer       func_data)
{
    GList   *dlist;
    
    /* GtkList �����򤵤줿�����ƥ�����Ϣ��ꥹ�Ȥ�������롣
     * ����ϥ꡼�ɥ���꡼�Ȥ��ư������Ȥ�˺��ʤ�!
     */
    dlist=GTK_LIST(gtklist)->selection;
    
    /* �⤷���򤵤줿�����ƥब̵����Ϥ䤹�뤳�Ȥ�̵���Τʤ�С�
     * �����桼�����˶���������Ǥ��롣
     */
    if (!dlist) {
        g_print("Selection cleared\n");
        return;
    }
    /* ok�����򤵤�Ƥ���ΤǤ����ץ��Ȥ��롣
     */
    g_print("The selection is a ");
    
    /* ���Ϣ��ꥹ�Ȥ���ꥹ�ȥ����ƥ�����ơ�list_item_data_key ����
     * ������ǡ�����ʹ���Ф��������Ƥ��������ץ��Ȥ��롣
     */
    while (dlist) {
        GtkObject       *list_item;
        gchar           *item_data_string;
        
        list_item=GTK_OBJECT(dlist->data);
        item_data_string=gtk_object_get_data(list_item,
                                             list_item_data_key);
        g_print("%s ", item_data_string);
        
        dlist=dlist->next;
    }
    g_print("\n");
}
/* example-end */
