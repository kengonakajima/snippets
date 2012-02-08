/* example-start clist clist.c */

#include        <gtk/gtk.h>
#include        <glib.h>

/* 様々なコールバックのプロトタイプだけがある */
void button_add_clicked( GtkWidget *button, gpointer data);
void button_clear_clicked( GtkWidget *button, gpointer data);
void button_hide_show_clicked( GtkWidget *button, gpointer data);
void selection_made( GtkWidget *clist, gint row, gint column,
                     GdkEventButton *event, gpointer data);

gint main (int argc, gchar *argv[])
{                                  
    GtkWidget       *window;
    GtkWidget       *vbox, *hbox;
    GtkWidget       *clist;
    GtkWidget       *button_add, *button_clear, *button_hide_show;    
    gchar           *titles[2] = {"Ingredients","Amount"};

    gtk_init(&argc, &argv);
    
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(GTK_WIDGET(window), 300, 150);

    gtk_window_set_title(GTK_WINDOW(window), "GtkCList Example");
    gtk_signal_connect(GTK_OBJECT(window),
                       "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit),
                       NULL);
    
    vbox=gtk_vbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    
    /* GtkCList を作成。この例では 2 列を使う */
    clist = gtk_clist_new_with_titles( 2, titles);

    /* セレクションが作成された時にそれを知りたい。使用されるコールバッ
     * クは selection_made で、そのコードはもっと下で見つかる */
    gtk_signal_connect(GTK_OBJECT(clist), "select_row",
                       GTK_SIGNAL_FUNC(selection_made),
                       NULL);

    /* ボーダーに影を付ける必要など無いのだが、これはいい感じだ :) */
    gtk_clist_set_border(GTK_CLIST(clist), GTK_SHADOW_OUT);

    /* なにがどんなに重要なのかというと、これを設定していなれば列幅が
     * 正しくならないのだ。注意、列は 0 から数えられる(この場合は1まで)。
     */
    gtk_clist_set_column_width (GTK_CLIST(clist), 0, 150);

    /* スクロールバーは必要なときだけ */
    gtk_clist_set_policy(GTK_CLIST(clist), GTK_POLICY_AUTOMATIC,
                                           GTK_POLICY_AUTOMATIC);

    /* 垂直ボックスに GtkCList ウィジェットを追加、これを表示。*/
    gtk_box_pack_start(GTK_BOX(vbox), clist, TRUE, TRUE, 0);
    gtk_widget_show(clist);

    /* ボタンを作成しこれらをウィンドウに加える。これについてのコメン
     * トやさらなる例はボタンのチュートリアルを参照。
     */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
    gtk_widget_show(hbox);

    button_add = gtk_button_new_with_label("Add List");
    button_clear = gtk_button_new_with_label("Clear List");
    button_hide_show = gtk_button_new_with_label("Hide/Show titles");

    gtk_box_pack_start(GTK_BOX(hbox), button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_clear, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_hide_show, TRUE, TRUE, 0);

    /* 三つのボタンにコールバックを接続 */
    gtk_signal_connect_object(GTK_OBJECT(button_add), "clicked",
                              GTK_SIGNAL_FUNC(button_add_clicked),
                              (gpointer) clist);
    gtk_signal_connect_object(GTK_OBJECT(button_clear), "clicked",
                              GTK_SIGNAL_FUNC(button_clear_clicked),
                              (gpointer) clist);
    gtk_signal_connect_object(GTK_OBJECT(button_hide_show), "clicked",
                              GTK_SIGNAL_FUNC(button_hide_show_clicked),
                              (gpointer) clist);

    gtk_widget_show(button_add);
    gtk_widget_show(button_clear);
    gtk_widget_show(button_hide_show);

    /* インタフェースが完全に設定できたのでウィンドウを表示して
     * gtk_main ループに突入
     */
    gtk_widget_show(window);
    gtk_main();
    
    return 0;
}

/* ユーザーが "Add List" ボタンを押した。*/
void button_add_clicked( GtkWidget *button, gpointer data)
{
    int         indx;

    /* リストにくだらないものを加える。4行2列に対応 */
    gchar      *drink[4][2] = {{"Milk", "3 Oz"},
                               {"Water", "6 l"},
                               {"Carrots", "2"},
                               {"Snakes", "55"}};

    /* ここで実際にテキストを加える。これは各行に1度ずつ行われる。
     */
    for( indx=0; indx < 4; indx++)
        gtk_clist_append( (GtkCList*) data, drink[indx]);

    return;
}

/* ユーザーが "Clear List" ボタンを押した。*/
void button_clear_clicked( GtkWidget *button, gpointer data)
{
    /* gtk_clist_clear を使ってリストをクリア。これは gtk_clist_remove
     * を各行に1度ずつ呼ぶよりだいぶ速い。
     */
    gtk_clist_clear((GtkCList*) data);

    return;
}

/* ユーザーが "Hide/Show titles" ボタンを押した。*/
void button_hide_show_clicked( GtkWidget *button, gpointer data)
{
    /* 単なる状態を覚えておくフラグ。0 = 現在可視 */
    static short int flag = 0;

    if (flag == 0)
    {
        /* タイトルを隠してフラグを 1 に設定 */
        gtk_clist_column_titles_hide((GtkCList*) data);
        flag++;
    }
    else
    {
        /* タイトルを表示してフラグを 0 にリセット */
        gtk_clist_column_titles_show((GtkCList*) data);
        flag--;
    }

    return;
}

/* もしここにきたら、ユーザーがリスト中の行を選択している。*/
void selection_made( GtkWidget *clist, gint row, gint column,
                     GdkEventButton *event, gpointer data)
{
    gchar       *text;

    /* クリックされ選択された row および column に保存されているテキス
     * トを獲得。引数 text にポインタとして受け取る。
     */
    gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);

    /* 選択された row に関する情報をただ表示するだけ */
    g_print("You selected row %d. More specifically you clicked in column %d, and the text in this cell is %s\n\n", row, column, text);

    return;
}
/* example-end */
