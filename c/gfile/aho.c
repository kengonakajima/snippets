/* example-start filesel filesel.c */

#include <gtk/gtk.h>

/* ���򤵤줿�ե�����̾����������󥽡���˥ץ��Ȥ��� */
void file_ok_sel (GtkWidget *w, GtkFileSelection *fs)
{
    g_print ("%s\n", gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
}

void destroy (GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

int main (int argc, char *argv[])
{
    GtkWidget *filew;
    
    gtk_init (&argc, &argv);
    
    /* �������ե����륻�쥯����󥦥������åȤ���� */
    filew = gtk_file_selection_new ("File selection");
    
    gtk_signal_connect (GTK_OBJECT (filew), "destroy",
                        (GtkSignalFunc) destroy, &filew);
    /* ok_button �� file_ok_sel �ؿ�����³ */
    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
                        "clicked", (GtkSignalFunc) file_ok_sel, filew );
    
    /* ���������åȤ��˲����뤿��� cancel_button ����³ */
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
                               "clicked", (GtkSignalFunc) gtk_widget_destroy,
                               GTK_OBJECT (filew));
    
    /* ��¸���������ǥǥե���ȥե�����̾��Ϳ���뤫�Τ褦�ˡ��ե���
     * ��̾�����ꤷ�褦��*/
    gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
                                     "penguin.png");
    
    gtk_widget_show(filew);
    gtk_main ();
    return 0;
}
/* example-end */
