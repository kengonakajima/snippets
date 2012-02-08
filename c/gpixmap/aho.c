/* example-start pixmap pixmap.c */

#include <gtk/gtk.h>


/* Open-File アイコンの XPM データ */
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


/* 呼び出された時(delete_event シグナル経由)、アプリケーションを終了。
 */
void close_application( GtkWidget *widget, GdkEvent *event, gpointer data )
{
    gtk_main_quit();
}


/* ボタンをクリックされた時に呼び出される。これはただメッセージを表示
 * する。*/

void button_clicked( GtkWidget *widget, gpointer data ) {
    printf( "button clicked\n" );
}

int main( int argc, char *argv[] )
{
    /* GtkWidget はウィジェットを保持する型である */
    GtkWidget *window, *pixmapwid, *button;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    GtkStyle *style;
    
    /* メインウィンドウを作成、アプリケーション終了のために
     * delete_event シグナルを取り付ける */
    gtk_init( &argc, &argv );
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_signal_connect( GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (close_application), NULL );
    gtk_container_border_width( GTK_CONTAINER (window), 10 );
    gtk_widget_show( window );

    /* さて gdk から pixmap を */
    style = gtk_widget_get_style( window );
    pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
                                           &style->bg[GTK_STATE_NORMAL],
                                           (gchar **)xpm_data );

    /* pixmap をもったピックスマップウィジェット */
    pixmapwid = gtk_pixmap_new( pixmap, mask );
    gtk_widget_show( pixmapwid );

    /* ピックスマップウィジェットをもったボタン */
    button = gtk_button_new();
    gtk_container_add( GTK_CONTAINER(button), pixmapwid );
    gtk_container_add( GTK_CONTAINER(window), button );
    gtk_widget_show( button );

    gtk_signal_connect( GTK_OBJECT(button), "clicked",
                        GTK_SIGNAL_FUNC(button_clicked), NULL );

    /* ウィンドウを表示 */
    gtk_main ();
          
    return 0;
}
/* example-end */
