#include  <gtk/gtk.h>
#include  <stdio.h>


/*

  




 */
static void destroy( GtkWidget *widget , gpointer data );
static gint delete_event( GtkWidget *widget , GdkEvent *event,gpointer data );
static gint clicked( GtkWidget *widget , GdkEvent *event,gpointer data );

int main( int argc , char **argv )
{
    GtkWidget *window;
    GtkWidget *note;
    
/*    GtkWidget *button;*/

    gtk_init( &argc , &argv );

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_signal_connect( GTK_OBJECT(window) , "delete_event" ,
                        GTK_SIGNAL_FUNC( delete_event ), NULL );



    note = gtk_notebook_new();
    gtk_notebook_set_tab_pos( GTK_NOTEBOOK(note), GTK_POS_TOP );
    gtk_container_add( GTK_CONTAINER(window) , note );
    
    gtk_widget_show( note );
    gtk_widget_show( window );
   
    gtk_main();

    return 0;
}

static void destroy( GtkWidget *widget , gpointer data )
{
    fprintf( stderr, "destroy\n" );
    gtk_main_quit(); /* ���줬�ʤ��ȡ��ץ����Ͻ�λ���ʤ� */
    
}
static gint delete_event( GtkWidget *widget , GdkEvent *event , gpointer data )
{
    g_print( "delete_event\n" );
    return FALSE;       /* ������ TRUE�ˤ�����ȡ�fvwm ��delete �Ǥ�
                           �����ʤ��ʤ�Τ���FALSE���ȡ�delete �Υ��ޥ��
                           ���̤��Ƥ��ޤä� destroy�ˤ錄�äƤ��ޤ���*/
}

static gint clicked( GtkWidget *widget , GdkEvent *event,gpointer data )
{
    g_print("clicked\n" );
    return TRUE;
}
#if 0
    /* ���ä��ϡ�fvwm �� delete ���ä� */
    gtk_signal_connect( GTK_OBJECT(window), "destroy" ,
                        GTK_SIGNAL_FUNC( destroy) , NULL );
#endif
