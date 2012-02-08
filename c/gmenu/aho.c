/* example-start menu menu.c */

#include <gtk/gtk.h>

static gint button_press (GtkWidget *, GdkEvent *);
static void menuitem_response (gchar *);

int main (int argc, char *argv[])
{

    GtkWidget *window;
    GtkWidget *menu;
    GtkWidget *menu_bar;
    GtkWidget *root_menu;
    GtkWidget *menu_items;
    GtkWidget *vbox;
    GtkWidget *button;
    char buf[128];
    int i;

    gtk_init (&argc, &argv);

    /* 新規にウィンドウを作成 */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize( GTK_WIDGET (window), 200, 100);
    gtk_window_set_title(GTK_WINDOW (window), "GTK Menu Test");
    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) gtk_main_quit, NULL);

    /* メニューウィジェットを初期化、それから -- メニューウィジェット
     * を gtk_show_widget() してはならない事を思い出せ!!
     * これはメニューアイテムを保持するメニューで、アプリケーションの
     * "Root Menu" をクリックするとポップアップする。*/
    menu = gtk_menu_new();

    /* 次に "test-menu" のために 3 つのメニューエントリを作成する小さ
     * なループを用意する。
     * gtk_menu_append を呼ぶのに注意すること。ここではメニューにメニュー
     * アイテムのリストを加えている。普通は、各メニューアイテムの
     * "clicked" シグナルを拾い、そのコールバック関数を用意するが、
     * ここでは場所の節約のために省略している。*/

    for(i = 0; i < 3; i++)
        {
            /* buf に名前をコピー。 */
            sprintf(buf, "Test-undermenu - %d", i);

            /* 新規に名前付きメニューアイテムを作成... */
            menu_items = gtk_menu_item_new_with_label(buf);

            /* ...そしてこれをメニューに加える。 */
            gtk_menu_append(GTK_MENU (menu), menu_items);

            /* メニューアイテムが選択されると注意をひく事を行う。 */
            gtk_signal_connect_object(GTK_OBJECT(menu_items), "activate",
                GTK_SIGNAL_FUNC(menuitem_response), (gpointer) g_strdup(buf));

            /* ウィジェットを表示 */
            gtk_widget_show(menu_items);
        }

    /* これはルートメニューとメニューバーに表示されるラベルになる。
     * これは押された時に眠っているメニューをポップアップするだけなの
     * で、シグナルハンドラは無い。*/
    root_menu = gtk_menu_item_new_with_label("Root Menu");

    gtk_widget_show(root_menu);

    /* さて、新しく作成された "menu" が "root menu" のメニューになるよ
     * うに指定する。*/
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (root_menu), menu);

    /* メニューとボタンを配置する vbox */
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    /* メニューを保持するためのメニューバーを作成、これをメインウィン
     * ドウに加える。*/
    menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 2);
    gtk_widget_show(menu_bar);

    /* メニューをポップアップするために取り付けるボタンを作成 */
    button = gtk_button_new_with_label("press me");
    gtk_signal_connect_object(GTK_OBJECT(button), "event",
        GTK_SIGNAL_FUNC (button_press), GTK_OBJECT(menu));
    gtk_box_pack_end(GTK_BOX(vbox), button, TRUE, TRUE, 2);
    gtk_widget_show(button);

    /* そして最後に、メニューアイテムをメニューバーに追加する -- これ
     * は私が狂ったように言い続けてきた "root" メニューアイテムである */
    gtk_menu_bar_append(GTK_MENU_BAR (menu_bar), root_menu);

    /* スクリーンに全てを一度に反映したいので、いつでも window は最後
     * の段階で表示する。*/
    gtk_widget_show(window);

    gtk_main ();

    return 0;
}

/* widget として渡されたメニューを掲示してボタンプレスに応答する。
 *
 * 注意、"widget" 引数は掲示されるメニューであって、押されたボタンでは
 * ない。
 */

static gint button_press (GtkWidget *widget, GdkEvent *event)
{

    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton *) event; 
        gtk_menu_popup (GTK_MENU(widget), NULL, NULL, NULL, NULL,
                        bevent->button, bevent->time);
        /* このイベントの扱いが済んだことを呼び出し側に報告。すなわち、
         * イベントをここで振り落とす。*/
        return TRUE;
    }

    /* このイベントの扱いが済んでいないことを呼び出し側に報告。すなわ
     * ち、イベントを通過させる。*/
    return FALSE;
}


/* メニューアイテムが選択された時に string を表示 */

static void menuitem_response (gchar *string)
{
    printf("%s\n", string);
}
/* example-end */
