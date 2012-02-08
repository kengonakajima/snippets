/* example-start packbox packbox.c */

#include <stdio.h>
#include "gtk/gtk.h"

void
delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

/* �ܥ����٥���������줿������ hbox ���롣�桹�δؿ��������ѿ���
 * �����Ϥ��δؿ����Ϥ���롣�ܥå�����ɽ�������ʤ�������¦�Τ�Τ���
 * ��ɽ�������Ƥ�����*/
GtkWidget *make_box (gint homogeneous, gint spacing,
                     gint expand, gint fill, gint padding) 
{
    GtkWidget *box;
    GtkWidget *button;
    char padstr[80];
    
    /* �����Ƥ�줿 homogeneous �� spacing �����ꤷ�� hbox �򿷤�����
     * ������ */
    box = gtk_hbox_new (homogeneous, spacing);
    
    /* �����Ƥ�줿����򤷤���Ϣ�Υܥ����������� */
    button = gtk_button_new_with_label ("gtk_box_pack");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    button = gtk_button_new_with_label ("(box,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    button = gtk_button_new_with_label ("button,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    /* expand ���ͤ˰�¸������٥��դ��ܥ����������� */
    if (expand == TRUE)
            button = gtk_button_new_with_label ("TRUE,");
    else
            button = gtk_button_new_with_label ("FALSE,");
    
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    /* ����Ͼ嵭�� "expand" �Υܥ���������Ʊ���Ǥ��뤬��ά��������
     * �ȤäƤ���[ifʸ�Τ����˾�Ｐ��ȤäƤ���] */
    button = gtk_button_new_with_label (fill ? "TRUE," : "FALSE,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    sprintf (padstr, "%d);", padding);
    
    button = gtk_button_new_with_label (padstr);
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    return box;
}

int
main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *separator;
    GtkWidget *label;
    GtkWidget *quitbox;
    int which;
    
    /* ��������롢�����˺����! :) */
    gtk_init (&argc, &argv);
    
    if (argc != 2) {
        fprintf (stderr, "usage: packbox num, where num is 1, 2, or 3.\n");
        /* ���� GTK �򥯥꡼�󥢥åפ���exit ���ơ����� 1 �ǽ�λ */
        gtk_exit (1);
    }
    
    which = atoi (argv[1]);

    /* window ����� */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* ��� main window �� destroy �����ʥ����³����Τ�Ф��Ƥ����
     * �����������Ŭ�ڤ�ľ��Ū���񤤤Τ�������˽��פǤ��� */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    
    /* ��ʿ�ܥå����򤽤���˥ѥå����뤿��˿�ľ�ܥå���(vbox)�����
     * ���롣����ϡ����� vbox �ˡ��ܥ�����������줿��ʿ�ܥå�����¾
     * �ξ������Ѥळ�Ȥ������Τ���*/
    box1 = gtk_vbox_new (FALSE, 0);
    
    /* ɽ�������㡣����Ͼ�οޤ��б����롣*/
    switch (which) {
    case 1:
        /* ��������٥��������롣*/
        label = gtk_label_new ("gtk_hbox_new (FALSE, 0);");
        
        /* ��٥�򺸥����ɤ��¤٤롣������ǤϤ��δؿ������¾�Υ���
         * �����å�°���ˤĤ��Ƶ������롣*/
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);

        /* ��ľ�ܥå��� (vbox box1) �˥�٥��ѥå����롣vbox �˲ä�
         * ��줿���������åȤ�¾�Τ�ΤΥȥåפ����˥ѥå�����뤳��
         * ��Ф��Ƥ�������*/
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        
        /* ��٥��ɽ�� */
        gtk_widget_show (label);
        
        /* �ܥå��������ؿ���Ƥ� - homogeneous = FALSE, spacing = 0,
         * expand = FALSE, fill = FALSE, padding = 0 */
        box2 = make_box (FALSE, 0, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        /* �ܥå��������ؿ���Ƥ� - homogeneous = FALSE, spacing = 0,
         * expand = FALSE, fill = FALSE, padding = 0 */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* ���ѥ졼��������������ˤĤ��Ƥϸ�ۤɾܤ����ؤ֤���
         * ������ñ����*/
        separator = gtk_hseparator_new ();
        
        /* vbox �˥��ѥ졼����ѥå����롣�����Υ��������åȤ� vbox 
         * �˥ѥå�����Ƥ���Τǡ������Ͽ�ľ���Ѥޤ�롣*/
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        /* �⤦��Ĥο�������٥��������������ɽ�����롣*/
        label = gtk_label_new ("gtk_hbox_new (TRUE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* �⤦��Ĥο��������ѥ졼����*/
        separator = gtk_hseparator_new ();
        /* ���� gtk_box_pack_start �ؤκǸ�� 3 �����ϼ����̤�:
         expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        break;

    case 2:

        /* ��������٥�������box1 �� main() �λϤ�᤯�Ǻ������줿
         * vbox �Ǥ��롣*/
        label = gtk_label_new ("gtk_hbox_new (FALSE, 10);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        separator = gtk_hseparator_new ();
        /* ���� gtk_box_pack_start �ؤκǸ�� 3 �����ϼ����̤�:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        label = gtk_label_new ("gtk_hbox_new (FALSE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* �����ϼ����̤�: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        separator = gtk_hseparator_new ();
        /* ���� gtk_box_pack_start �ؤκǸ�� 3 �����ϼ����̤�:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        break;
    
    case 3:

       /* ����� gtk_box_pack_end() �Υ��������åȤα�·����ǽ�Ϥ�������
        * ���ΤǤ��롣��1�ˡ���˿������ܥå�����������롣*/
        box2 = make_box (FALSE, 0, FALSE, FALSE, 0);
        /* �Ǹ�����֤�����٥��������롣*/
        label = gtk_label_new ("end");
        /* gtk_box_pack_end() ��ȤäƤ����ѥå����뤿�ᡢmake_box()
         * �ƤӽФ��Ǻ������줿 hbox �α�ü�����֤���롣*/
        gtk_box_pack_end (GTK_BOX (box2), label, FALSE, FALSE, 0);
        /* ��٥��ɽ�����롣*/
        gtk_widget_show (label);
        
        /* box2 �� box1 �˥ѥå�����(���� vbox ��Ф��Ƥ��뤫��? :) */
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* ��ü�Υ��ѥ졼�� */
        separator = gtk_hseparator_new ();
        /* ���������Ū�˥��ѥ졼���� 400 �ԥ��������� 5 �ԥ�����⤵
         * �����ꤹ�롣����椨���κ������� hbox �� 400 �ԥ��������ˤ�
         * �ꡢ"end" ��٥�Ϥ��� hbox ���¾�Υ�٥��ʬΥ����롣
         * �����ǡ����� hbox �����ƤΥ��������åȤϲ�ǽ�ʤ餪�ߤ�����
         * ������ΤȤ��ƥѥå�����롣*/
        gtk_widget_set_usize (separator, 400, 5);
        /* ���� main() �κǽ��ն�Ǻ������줿 vbox (box1) ����˥��ѥ졼
         * ����ѥå����롣*/
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);    
    }
    
    /* �⤦��Ĥο����� hbox ���������..��ɬ�פʤ������������Ȥ���
       ���Ȥ�Ф��Ƥ�����! */
    quitbox = gtk_hbox_new (FALSE, 0);
    
    /* quit �ܥ���*/
    button = gtk_button_new_with_label ("Quit");
    
    /* ���� window ���˲����륷���ʥ�����ꤹ�롣����� "destroy" ����
     * �ʥ�򤳤� window �����롣���������������줿�桹�Υ����ʥ��
     * ��ɥ�ˤ�äư�����������뤳�Ȥ�Ф��Ƥ�������*/
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                               GTK_SIGNAL_FUNC (gtk_main_quit),
                               GTK_OBJECT (window));
    /* quitbox ��ˤ��� button ��ѥå����롣gtk_box_pack_start �ؤκ�
     * ��� 3 ������ expand, fill, padding ���� */
    gtk_box_pack_start (GTK_BOX (quitbox), button, TRUE, FALSE, 0);
    /* quitbox �� vbox (box1) ����˥ѥå����롣*/
    gtk_box_pack_start (GTK_BOX (box1), quitbox, FALSE, FALSE, 0);
    
    /* �����Ƥβ桹�Υ��������åȤ�ޤ�� vbox (box1) �򡢥ᥤ���
     * window ����˥ѥå����롣*/
    gtk_container_add (GTK_CONTAINER (window), box1);
    
    /* ���������Ƥ�ɽ�����롣*/
    gtk_widget_show (button);
    gtk_widget_show (quitbox);
    
    gtk_widget_show (box1);
    /* ���Ƥ���٤˥ݥåץ��åפ��뤿��˺Ǹ�ˤ��� window ��ɽ����*/
    gtk_widget_show (window);
    
    /* �������������ᥤ��δؿ��� */
    gtk_main ();

    /* gtk_main_quit() ���ƤФ줿�������椬���뤬��gtk_exit ���Ȥ�줿
     * ���Ϥ����ǤϤʤ���*/
    
    return 0;
}
/* example-end */
