/* example-start pixmap pixmap.c */

#include <gtk/gtk.h>


/* Open-File ��������� XPM �ǡ��� */
static const char * xpm_data[] = {
"16 16 3 1",
"       c None",
".      c #000000000000",
"X      c #FFFFFFFFFFFF",
"                ",
"   ......       ",
"   .XXX.X.      ",
"   .XXX.XX.     ",
"   .XXX.XXX.    ",
"   .XXX.....    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .........    ",
"                ",
"                "
};


/* �ƤӽФ��줿��(delete_event �����ʥ��ͳ)�����ץꥱ��������λ��
 */
void close_application( GtkWidget *widget, GdkEvent *event, gpointer data )
{
    gtk_main_quit();
}


/* �ܥ���򥯥�å����줿���˸ƤӽФ���롣����Ϥ�����å�������ɽ��
 * ���롣*/

void button_clicked( GtkWidget *widget, gpointer data ) {
    printf( "button clicked\n" );
}

int main( int argc, char *argv[] )
{
    /* GtkWidget �ϥ��������åȤ��ݻ����뷿�Ǥ��� */
    GtkWidget *window, *pixmapwid, *button;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    GtkStyle *style;
    
    /* �ᥤ�󥦥���ɥ�����������ץꥱ�������λ�Τ����
     * delete_event �����ʥ�����դ��� */
    gtk_init( &argc, &argv );
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_signal_connect( GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (close_application), NULL );
    gtk_container_border_width( GTK_CONTAINER (window), 10 );
    gtk_widget_show( window );

    /* ���� gdk ���� pixmap �� */
    style = gtk_widget_get_style( window );
    pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
                                           &style->bg[GTK_STATE_NORMAL],
                                           (gchar **)xpm_data );

    /* pixmap ���ä��ԥå����ޥåץ��������å� */
    pixmapwid = gtk_pixmap_new( pixmap, mask );
    gtk_widget_show( pixmapwid );

    /* �ԥå����ޥåץ��������åȤ��ä��ܥ��� */
    button = gtk_button_new();
    gtk_container_add( GTK_CONTAINER(button), pixmapwid );
    gtk_container_add( GTK_CONTAINER(window), button );
    gtk_widget_show( button );

    gtk_signal_connect( GTK_OBJECT(button), "clicked",
                        GTK_SIGNAL_FUNC(button_clicked), NULL );

    /* ������ɥ���ɽ�� */
    gtk_main ();
          
    return 0;
}
/* example-end */
