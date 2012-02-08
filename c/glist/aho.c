/* example-start list list.c */

/* gtk+ ヘッダファイルをインクルード
 * printf() 関数が必要なので stdio.h をインクルード
 */
#include        <gtk/gtk.h>
#include        <stdio.h>

/* これはリストアイテムにデータを保存するためのデータ識別文字
 * 列である
 */
const   gchar   *list_item_data_key="list_item_data";


/* GtkList ウィジェットに接続することになるシグナルハンドラの
 * プロトタイプ
 */
static  void    sigh_print_selection    (GtkWidget      *gtklist,
                                         gpointer       func_data);
static  void    sigh_button_event       (GtkWidget      *gtklist,
                                         GdkEventButton *event,
                                         GtkWidget      *frame);


/* ユーザインタフェースをセットアップするメイン関数 */

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
    
    
    /* gtk+ を初期化 (その後 gdk も) */

    gtk_init(&argc, &argv);
    
    
    /* 全てのウィジェットを入れるためのウィンドウを作成、
     * gtk_main_quit() をそのウィンドウの "destory" イベントに接続して
     * ウィンドウマネージャの close-window-events を処理
     */
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GtkList 例");
    gtk_signal_connect(GTK_OBJECT(window),
                       "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit),
                       NULL);
    
    
    /* ウィンドウの内側で、垂直にウィジェットを配置するために
     * 必要なボックス
     */
    vbox=gtk_vbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    
    /* これは内部に GtkList ウィジェットを配置するための
     * スクロールドウィンドウ
     */
    scrolled_window=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_usize(scrolled_window, 250, 150);
    gtk_container_add(GTK_CONTAINER(vbox), scrolled_window);
    gtk_widget_show(scrolled_window);
    
    /* GtkList ウィジェットを作成
     * sigh_print_selection() シグナルハンドラ関数を
     * GtkList の "selection_changed" シグナルに接続し、
     * 選択が変更されたその時に選択されたアイテムを表示する
     */
    gtklist=gtk_list_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), gtklist);
    gtk_widget_show(gtklist);
    gtk_signal_connect(GTK_OBJECT(gtklist),
                       "selection_changed",
                       GTK_SIGNAL_FUNC(sigh_print_selection),
                       NULL);
    
    /* リストアイテムを置くために "Prison" [留置所] を作成 ;)
     */
    frame=gtk_frame_new("Prison");
    gtk_widget_set_usize(frame, 200, 50);
    gtk_container_border_width(GTK_CONTAINER(frame), 5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
    gtk_container_add(GTK_CONTAINER(vbox), frame);
    gtk_widget_show(frame);
    
    /* GtkList に sigh_button_event() シグナルハンドラを接続、
     * これはリストアイテムの "arresting" [逮捕] を扱う
     */
    gtk_signal_connect(GTK_OBJECT(gtklist),
                       "button_release_event",
                       GTK_SIGNAL_FUNC(sigh_button_event),
                       frame);
    
    /* セパレータを作成
     */
    separator=gtk_hseparator_new();
    gtk_container_add(GTK_CONTAINER(vbox), separator);
    gtk_widget_show(separator);
    
    /* 最後に、ボタンを作成し、その "clicked" シグナルをウィンドウの破
     * 滅に結び付ける
     */
    button=gtk_button_new_with_label("Close");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_widget_show(button);
    gtk_signal_connect_object(GTK_OBJECT(button),
                              "clicked",
                              GTK_SIGNAL_FUNC(gtk_widget_destroy),
                              GTK_OBJECT(window));
    
    
    /* これで各々がそれぞれのラベルを持っているリストアイテムを 5 つ作
     * 成した。これを gtk_container_add() を使って GtkList に加え、
     * またそのラベルからテキスト文字列を聞き出して、各リストアイテム
     * の list_item_data_key と対応付ける
     */
    for (i=0; i<5; i++) {
        GtkWidget       *label;
        gchar           *string;
        
        sprintf(buffer, "りすとあいてム？ 死 Label #%d", i);
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
    /* ここで、今度は作成に gtk_list_item_new_with_label() を使っても
     * う 5 ラベル作成する。
     * ラベルポインタを持ってないのでラベルからテキスト文字列を聞き出
     * すことができない。それで二重連結リスト(GList)の中にリストアイテ
     * ム全てを加えるのに、リストアイテムの各 list_item_data_key をた
     * だ同じテキスト文字列と対応さる。それから
     * gtk_list_append_items() を一回呼んでこれらを加える。
     * 二重連結リストの中にアイテムを置くのには g_list_prepend() を使
     * うので、これらの並びは降順になる(代わりに g_list_append() を使
     * えば昇順になる)。
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
    
    /* 最後にこの window を見てみたい、みたくないって? ;)
     */
    gtk_widget_show(window);
    
    /* gtk のメインイベントループに点火。
     */
    gtk_main();
    
    /* メインウィンドウが破壊されたると、
     * gtk_main_quit() が呼ばれてその後にここにくる。
     */
    return 0;
}

/* これは GtkList の button press/release イベントに接続された
 * シグナルハンドラである。
 */
void
sigh_button_event       (GtkWidget      *gtklist,
                         GdkEventButton *event,
                         GtkWidget      *frame)
{
    /* 3番目(一番右のマウスボタン)が離されたばあいのみ行う
     */
    if (event->type==GDK_BUTTON_RELEASE &&
        event->button==3) {
        GList           *dlist, *free_list;
        GtkWidget       *new_prisoner;
        
        /* 次の囚人となる、現在選択されているリストアイテムを取得 ;)
         */
        dlist=GTK_LIST(gtklist)->selection;
        if (dlist)
                new_prisoner=GTK_WIDGET(dlist->data);
        else
                new_prisoner=NULL;
        
        /* 既に監禁されているリストアイテムを探し、リストの中に戻す。
         * gtk_container_children() が返した二重連結リストを解放するの
         * をお忘れなく。
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
        
        /* もし新しい囚人[prisoner]を捕えているなら、
         * 彼を GtkList から削除しフレーム "Prison" に放り込む。
         * その前にアイテムを unselect する必要がある。
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

/* これは GtkList が "selection_changed" シグナルを発行した場合に呼ば
 * れるシグナルハンドラである。
 */
void
sigh_print_selection    (GtkWidget      *gtklist,
                         gpointer       func_data)
{
    GList   *dlist;
    
    /* GtkList で選択されたアイテムの二重連結リストを取得する。
     * これはリードオンリーとして扱うことをお忘れなく!
     */
    dlist=GTK_LIST(gtklist)->selection;
    
    /* もし選択されたアイテムが無くもはやすることが無いのならば、
     * ただユーザーに教えるだけである。
     */
    if (!dlist) {
        g_print("Selection cleared\n");
        return;
    }
    /* ok、選択されているのでそれをプリントする。
     */
    g_print("The selection is a ");
    
    /* 二重連結リストからリストアイテムを得て、list_item_data_key に対
     * 応するデータを聞き出す。そしてただこれをプリントする。
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
