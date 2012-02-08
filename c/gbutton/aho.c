/* example-start buttons buttons.c */

#include <gtk/gtk.h>

/* 中にイメージとラベルを付けた新しい hbox を作成して、そのボックスを
 * 返す... */

GtkWidget *xpm_label_box (GtkWidget *parent, gchar *xpm_filename, gchar *label_text)
{
    GtkWidget *box1;
    GtkWidget *label;
    GtkWidget *pixmapwid;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    GtkStyle *style;

    /* xpm およびラベルのためにボックスを作成 */
    box1 = gtk_hbox_new (FALSE, 0);
    gtk_container_border_width (GTK_CONTAINER (box1), 2);

    /* ボタンのスタイルを取得..。バックグラウンドカラーを取得するふり
     * をする。もし誰か本当の理由を知っているなら、どうか教えて欲しい。*/
    style = gtk_widget_get_style(parent);

    /* さて xpm スタッフに.. xpm を読み込む */
    pixmap = gdk_pixmap_create_from_xpm (parent->window, &mask,
                                         &style->bg[GTK_STATE_NORMAL],
                                         xpm_filename);
    pixmapwid = gtk_pixmap_new (pixmap, mask);

    /* ボタンのラベルを作成 */
    label = gtk_label_new (label_text);

    /* ピックスマップとラベルをボタンにパック */
    gtk_box_pack_start (GTK_BOX (box1),
                        pixmapwid, FALSE, FALSE, 3);

    gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 3);

    gtk_widget_show(pixmapwid);
    gtk_widget_show(label);

    return (box1);
}

/* 例のコールバック関数 */
void callback (GtkWidget *widget, gpointer data)
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
}


int main (int argc, char *argv[])
{
    /* GtkWidget はウィジェットの貯蔵型である */
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;

    gtk_init (&argc, &argv);

    /* 新しいウィンドウを作成 */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "Pixmap'd Buttons!");

    /* 全てのウィンドウにこれを行うのは良い考えだ。*/
    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                        GTK_SIGNAL_FUNC (gtk_exit), NULL);

    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
            GTK_SIGNAL_FUNC (gtk_exit), NULL);


    /* ウィンドウのボーダ幅を設定する。*/
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_realize(window);

    /* 新しいボタンを作成する。*/
    button = gtk_button_new ();

    /* 今ではこれらの多くの関数を見慣れているべきである。*/
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (callback), (gpointer) "cool button");

    /* これは例のボックス作成関数を呼ぶ。*/
    box1 = xpm_label_box(window, "info.xpm", "cool button");

    /* 全てのウィジェットをパックして表示する。*/
    gtk_widget_show(box1);

    gtk_container_add (GTK_CONTAINER (button), box1);

    gtk_widget_show(button);

    gtk_container_add (GTK_CONTAINER (window), button);

    gtk_widget_show (window);

    /* gtk_main で休み、始まるのを楽しみに待つ。*/
    gtk_main ();

    return 0;
}
/* example-end */
