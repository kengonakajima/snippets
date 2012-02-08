/* example-start rulers rulers.c */

#include <gtk/gtk.h>

#define EVENT_METHOD(i, x) GTK_WIDGET_CLASS(GTK_OBJECT(i)->klass)->x

#define XSIZE  600
#define YSIZE  400

/* このルーチンは close ボタンがクリックされた時に制御を得る
 */
void close_application( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    gtk_main_quit();
}


/* main ルーチン
 */
int main( int argc, char *argv[] ) {
    GtkWidget *window, *table, *area, *hrule, *vrule;

    /* gtk を初期化、メインウィンドウを作成する */
    gtk_init( &argc, &argv );

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
            GTK_SIGNAL_FUNC( close_application ), NULL);
    gtk_container_border_width (GTK_CONTAINER (window), 10);

    /* ルーラとドローイングエリアを配置するテーブルを作成 */
    table = gtk_table_new( 3, 2, FALSE );
    gtk_container_add( GTK_CONTAINER(window), table );

    area = gtk_drawing_area_new();
    gtk_drawing_area_size( (GtkDrawingArea *)area, XSIZE, YSIZE );
    gtk_table_attach( GTK_TABLE(table), area, 1, 2, 1, 2,
                      GTK_EXPAND|GTK_FILL, GTK_FILL, 0, 0 );
    gtk_widget_set_events( area, GDK_POINTER_MOTION_MASK |
                                  GDK_POINTER_MOTION_HINT_MASK );

    /* 水平ルーラは上に置かれる。マウスがドローイングエリアを横切って
     * 移動すると、motion_notify_event がこのルーラの対応するイベント
     * ハンドラに渡される。*/
    hrule = gtk_hruler_new();
    gtk_ruler_set_metric( GTK_RULER(hrule), GTK_PIXELS );
    gtk_ruler_set_range( GTK_RULER(hrule), 7, 13, 0, 20 );
    gtk_signal_connect_object( GTK_OBJECT(area), "motion_notify_event",
                               (GtkSignalFunc)EVENT_METHOD(hrule, motion_notify_event),
                               GTK_OBJECT(hrule) );
    /*  GTK_WIDGET_CLASS(GTK_OBJECT(hrule)->klass)->motion_notify_event, */
    gtk_table_attach( GTK_TABLE(table), hrule, 1, 2, 0, 1,
                      GTK_EXPAND|GTK_SHRINK|GTK_FILL, GTK_FILL, 0, 0 );
    
    /* 垂直ルーラは左に置かれる。マウスがドローイングエリアを横切って
     * 移動すると、motion_notify_event がこのルーラの対応するイベント
     * ハンドラに渡される。*/
    vrule = gtk_vruler_new();
    gtk_ruler_set_metric( GTK_RULER(vrule), GTK_PIXELS );
    gtk_ruler_set_range( GTK_RULER(vrule), 0, YSIZE, 10, YSIZE );
    gtk_signal_connect_object( GTK_OBJECT(area), "motion_notify_event",
                               (GtkSignalFunc)
                                  GTK_WIDGET_CLASS(GTK_OBJECT(vrule)->klass)->motion_notify_event,
                               GTK_OBJECT(vrule) );
    gtk_table_attach( GTK_TABLE(table), vrule, 0, 1, 1, 2,
                      GTK_FILL, GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0 );

    /* では全てを表示しよう */
    gtk_widget_show( area );
    gtk_widget_show( hrule );
    gtk_widget_show( vrule );
    gtk_widget_show( table );
    gtk_widget_show( window );
    gtk_main();

    return 0;
}
/* example-end */
