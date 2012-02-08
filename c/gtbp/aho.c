#include <gtk/gtk.h>
#include <stdio.h>

gchar *list_item_data_key = "list_item_data";


static  void    sigh_print_selection    (GtkWidget      *gtklist,
                                         gpointer       func_data);
static  void    sigh_button_event       (GtkWidget      *gtklist,
                                         GdkEventButton *event,
                                         GtkWidget      *frame);


gint main( int argc , gchar *argv[] )
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *scrolled_window;
    GtkWidget *gtklist;
    GtkWidget *separator;
    GtkWidget *button;
    GtkWidget *list_item;
    GList *dlist;
    gchar buffer[64];
    int i;

    gtk_init( &argc , &argv );

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW(window) , "GTKLISTの例" );

    gtk_signal_connect( GTK_OBJECT(window) , "destroy",
                        GTK_SIGNAL_FUNC(gtk_main_quit ),NULL );

    /* これを追加しても何もおこらない */
    vbox = gtk_vbox_new( FALSE, 5 );
    gtk_container_border_width( GTK_CONTAINER(vbox) , 5 );
    gtk_container_add( GTK_CONTAINER(window) , vbox );
    gtk_widget_show( vbox );

    /* ここまで追加するとすくろーるばーつきの エリアがでた。 */
    scrolled_window = gtk_scrolled_window_new( NULL , NULL );
    gtk_widget_set_usize( scrolled_window , 250 , 150 );
    gtk_container_add( GTK_CONTAINER( vbox ) , scrolled_window );
    gtk_widget_show( scrolled_window );

    /* ここでlistつくるとエリアが白くなった。何か生まれたかんじ。 */

    gtklist = gtk_list_new();
    gtk_container_add( GTK_CONTAINER(scrolled_window) , gtklist );
    gtk_widget_show( gtklist );

    /* で、このイベントを connectすると？
     でも何もおこらない。エントリがないからかな  */
    gtk_signal_connect( GTK_OBJECT(gtklist),"selection_changed",
                        GTK_SIGNAL_FUNC(sigh_print_selection),NULL);
    
    /* セパレータを作成すると、ウインドウのいちばん下に線ができた。
     この線はウインドウのおおきさをかえるとのびちぢみしてくれる。
     しかもいい位置に。 これはなくてもいいのだ 。*/
#if 1
    separator = gtk_hseparator_new();
    gtk_container_add( GTK_CONTAINER( vbox ) , separator );
    gtk_widget_show( separator );
#endif
    
    /* ボタンをつくるのだ */
    button = gtk_button_new_with_label( "しめる" );
    gtk_container_add( GTK_CONTAINER(vbox) , button );
    gtk_widget_show( button );
    gtk_signal_connect_object( GTK_OBJECT(button),"clicked",
                               GTK_SIGNAL_FUNC(gtk_widget_destroy),
                               GTK_OBJECT(window));
    
    

    /* らべるをぺたぺた追加 */
    for( i = 0 ;i < 5 ; i++ ){
        GtkWidget *label;
        gchar *string;

        sprintf( buffer , "fuck with label #%d" , i );
        label = gtk_label_new( buffer );
        list_item = gtk_list_item_new();
        gtk_container_add( GTK_CONTAINER(list_item),label );
        gtk_widget_show( label );
        gtk_container_add(GTK_CONTAINER(gtklist), list_item );
        gtk_widget_show( list_item );
        gtk_label_get( GTK_LABEL(label),&string);
        gtk_object_set_data( GTK_OBJECT( list_item),list_item_data_key,
                             string);
    }

    dlist = NULL;
    for(;i<10;i++){
        sprintf(buffer, "List Item with Label %d", i);
        list_item=gtk_list_item_new_with_label(buffer);
        dlist=g_list_prepend(dlist, list_item);
        gtk_widget_show(list_item);
        gtk_object_set_data(GTK_OBJECT(list_item),
                            list_item_data_key,
                            "ListItem with integrated Label");
    }
    gtk_list_append_items(GTK_LIST(gtklist),dlist);
    
    
    gtk_widget_show( window );

    gtk_main();

    return 0;
}

void sigh_print_selection( GtkWidget *gtklist , gpointer func_data )
{
    GList *dlist;

    dlist = GTK_LIST( gtklist ) -> selection;

    if( !dlist ){
        g_print( "くりあされてもた\n" );
        return;
    }

    g_print( "なにかせんたくされてる\n" );

    while(dlist ){

        GtkObject *list_item;
        gchar *item_data_string;

        list_item = GTK_OBJECT( dlist->data );
        item_data_string = gtk_object_get_data( list_item ,
                                                list_item_data_key );
        g_print( "[%s] " , item_data_string );
        dlist = dlist->next;
    }
    g_print( "\n" );
    
}
