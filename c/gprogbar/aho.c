/* example-start progressbar progressbar.c */

#include <gtk/gtk.h>

static int ptimer = 0;
int pstat = TRUE;

/* ���δؿ����ץ��쥹�С������ù�������pstat �� FALSE �Ǥ���Хꥻ��
�Ȥ��� */
gint progress (gpointer data)
{
    gfloat pvalue;
    
    /* �ץ��쥹�С��θ��ߤ��ͤ����� */
    pvalue = GTK_PROGRESS_BAR (data)->percentage;
    
    if ((pvalue >= 1.0) || (pstat == FALSE)) {
        pvalue = 0.0;
        pstat = TRUE;
    }
    pvalue += 0.01;
    
    gtk_progress_bar_update (GTK_PROGRESS_BAR (data), pvalue);
    
    return TRUE;
}

/* ���δؿ��ϥץ��쥹�С��Υꥻ�åȥ����ʥ��ȯ�Ԥ��� */
void progress_r (void)
{
    pstat = FALSE;
}

void destroy (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *table;
    GtkWidget *pbar;
    
    gtk_init (&argc, &argv);
    
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (destroy), NULL);
    
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    
    table = gtk_table_new(3,2,TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    
    label = gtk_label_new ("Progress Bar Example");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,2,0,1);
    gtk_widget_show(label);
    
    /* �������ץ��쥹�С�����������ơ��֥����˥ѥå�����ɽ������ */
    pbar = gtk_progress_bar_new ();
    gtk_table_attach_defaults(GTK_TABLE(table), pbar, 0,2,1,2);
    gtk_widget_show (pbar);
    
    /* �ץ��쥹�С��μ�ư������򰷤�����˥����ॢ���Ȥ����ꤹ�� */
    ptimer = gtk_timeout_add (100, progress, pbar);
    
    /* ���Υܥ���ϥץ��쥹�С���ꥻ�åȤ��륷���ʥ��ȯ�Ԥ��� */
    button = gtk_button_new_with_label ("Reset");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (progress_r), NULL);
    gtk_table_attach_defaults(GTK_TABLE(table), button, 0,1,2,3);
    gtk_widget_show(button);
    
    button = gtk_button_new_with_label ("Cancel");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (destroy), NULL);
    
    gtk_table_attach_defaults(GTK_TABLE(table), button, 1,2,2,3);
    gtk_widget_show (button);
    
    gtk_widget_show(table);
    gtk_widget_show(window);
    
    gtk_main ();
    
    return 0;
}
/* example-end */
