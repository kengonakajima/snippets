/* example-start table table.c */
#include <gtk/gtk.h>

/* いつものコールバック。
 * この関数に渡されたデータは標準出力に表示される */
void callback (GtkWidget *widget, gpointer data)
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
}

/* このコールバックはプログラムを終了させる */
void delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *table;

    gtk_init (&argc, &argv);

    /* 新しいウィンドウを作成 */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* そのウィンドウのタイトルを設定 */
    gtk_window_set_title (GTK_WINDOW (window), "Table");

    /* ただちに GTK を終了させるように delete_event ハンドラを設定 */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (delete_event), NULL);

    /* このウィンドウのボーダ幅を設定 */
    gtk_container_border_width (GTK_CONTAINER (window), 20);

    /* 2x2 テーブルを作成 */
    table = gtk_table_new (2, 2, TRUE);

    /* メインウィンドウにテーブルを配置 */
    gtk_container_add (GTK_CONTAINER (window), table);

    /* 最初のボタンを作成 */
    button = gtk_button_new_with_label ("button 1");

    /* ボタンが押された時に、"callback" 関数を呼び、その引数
     * として "button 1" へのポインタを渡す */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
              GTK_SIGNAL_FUNC (callback), (gpointer) "button 1");


    /* テーブルの左上の象限 [quadrant] に button 1 を挿入 */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 0, 1, 0, 1);

    gtk_widget_show (button);

    /* 二番目のボタンを作成 */

    button = gtk_button_new_with_label ("button 2");

    /* ボタンが押された時に、"callback" 関数を呼び、その引数
     * として "button 2" へのポインタを渡す */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
              GTK_SIGNAL_FUNC (callback), (gpointer) "button 2");
    /* insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 1, 2, 0, 1);

    gtk_widget_show (button);

    /* "Quit" ボタンを作成 */
    button = gtk_button_new_with_label ("Quit");

    /* ボタンが押された時に、"delete_event" 関数を呼び、プログラムを終
     * 了させる */
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (delete_event), NULL);

    /* テーブルの下側の象限両方に quit ボタンを挿入 */
    gtk_table_attach_defaults (GTK_TABLE(table), button, 0, 2, 1, 2);

    gtk_widget_show (button);

    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
/* example-end */
