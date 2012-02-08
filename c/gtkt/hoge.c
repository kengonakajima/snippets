/* example-start packbox packbox.c */

#include <stdio.h>
#include "gtk/gtk.h"

void
delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
}

/* ボタンラベルで満たされた新しい hbox を作る。我々の関心がある変数の
 * 引数はこの関数に渡される。ボックスは表示させないが、内側のものは全
 * て表示させておく。*/
GtkWidget *make_box (gint homogeneous, gint spacing,
                     gint expand, gint fill, gint padding) 
{
    GtkWidget *box;
    GtkWidget *button;
    char padstr[80];
    
    /* 割当てられた homogeneous と spacing を設定した hbox を新しく作
     * 成する */
    box = gtk_hbox_new (homogeneous, spacing);
    
    /* 割当てられた設定をした一連のボタンを作成する */
    button = gtk_button_new_with_label ("gtk_box_pack");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    button = gtk_button_new_with_label ("(box,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    button = gtk_button_new_with_label ("button,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    /* expand の値に依存したラベル付きボタンを作成する */
    if (expand == TRUE)
            button = gtk_button_new_with_label ("TRUE,");
    else
            button = gtk_button_new_with_label ("FALSE,");
    
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    /* これは上記の "expand" のボタン生成と同じであるが、略記形式を
     * 使っている[if文のかわりに条件式を使っている] */
    button = gtk_button_new_with_label (fill ? "TRUE," : "FALSE,");
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    sprintf (padstr, "%d);", padding);
    
    button = gtk_button_new_with_label (padstr);
    gtk_box_pack_start (GTK_BOX (box), button, expand, fill, padding);
    gtk_widget_show (button);
    
    return box;
}

int
main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *separator;
    GtkWidget *label;
    GtkWidget *quitbox;
    int which;
    
    /* 初期化する、これを忘れるな! :) */
    gtk_init (&argc, &argv);
    
    if (argc != 2) {
        fprintf (stderr, "usage: packbox num, where num is 1, 2, or 3.\n");
        /* ただ GTK をクリーンアップし、exit ステータス 1 で終了 */
        gtk_exit (1);
    }
    
    which = atoi (argv[1]);

    /* window を作成 */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* 常に main window へ destroy シグナルを接続するのを覚えているべ
     * きだ。これは適切な直感的振舞いのために非常に重要である */
    gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                        GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_container_border_width (GTK_CONTAINER (window), 10);
    
    /* 水平ボックスをその中にパックするために垂直ボックス(vbox)を作成
     * する。これは、この vbox に、ボタンで満たされた水平ボックスを他
     * の上部に積むことを許すものだ。*/
    box1 = gtk_vbox_new (FALSE, 0);
    
    /* 表示する例。これは上の図に対応する。*/
    switch (which) {
    case 1:
        /* 新しいラベルを作成する。*/
        label = gtk_label_new ("gtk_hbox_new (FALSE, 0);");
        
        /* ラベルを左サイドに並べる。この節ではこの関数および他のウィ
         * ジェット属性について議論する。*/
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);

        /* 垂直ボックス (vbox box1) にラベルをパックする。vbox に加え
         * られたウィジェットは他のもののトップから順にパックされること
         * を覚えておこう。*/
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        
        /* ラベルを表示 */
        gtk_widget_show (label);
        
        /* ボックス作成関数を呼ぶ - homogeneous = FALSE, spacing = 0,
         * expand = FALSE, fill = FALSE, padding = 0 */
        box2 = make_box (FALSE, 0, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        /* ボックス作成関数を呼ぶ - homogeneous = FALSE, spacing = 0,
         * expand = FALSE, fill = FALSE, padding = 0 */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* セパレータを作成、これらについては後ほど詳しく学ぶが、
         * 全く簡単だ。*/
        separator = gtk_hseparator_new ();
        
        /* vbox にセパレータをパックする。これらのウィジェットは vbox 
         * にパックされているので、これらは垂直に積まれる。*/
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        /* もう一つの新しいラベルを作成し、これを表示する。*/
        label = gtk_label_new ("gtk_hbox_new (TRUE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* もう一つの新しいセパレータ。*/
        separator = gtk_hseparator_new ();
        /* この gtk_box_pack_start への最後の 3 引数は次の通り:
         expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        break;

    case 2:

        /* 新しいラベルを作成、box1 は main() の始め近くで作成された
         * vbox である。*/
        label = gtk_label_new ("gtk_hbox_new (FALSE, 10);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        separator = gtk_hseparator_new ();
        /* この gtk_box_pack_start への最後の 3 引数は次の通り:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        label = gtk_label_new ("gtk_hbox_new (FALSE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* 引数は次の通り: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        separator = gtk_hseparator_new ();
        /* この gtk_box_pack_start への最後の 3 引数は次の通り:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        break;
    
    case 3:

       /* これは gtk_box_pack_end() のウィジェットの右揃えの能力を説明す
        * るものである。第1に、先に新しいボックスを作成する。*/
        box2 = make_box (FALSE, 0, FALSE, FALSE, 0);
        /* 最後に配置されるラベルを作成する。*/
        label = gtk_label_new ("end");
        /* gtk_box_pack_end() を使ってこれをパックするため、make_box()
         * 呼び出しで作成された hbox の右端に配置される。*/
        gtk_box_pack_end (GTK_BOX (box2), label, FALSE, FALSE, 0);
        /* ラベルを表示する。*/
        gtk_widget_show (label);
        
        /* box2 を box1 にパックする(この vbox を覚えているかい? :) */
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* 下端のセパレータ */
        separator = gtk_hseparator_new ();
        /* これは明示的にセパレータを 400 ピクセル幅で 5 ピクセル高さ
         * に設定する。それゆえこの作成した hbox も 400 ピクセル幅にな
         * り、"end" ラベルはこの hbox 中の他のラベルと分離される。
         * 一方で、この hbox の全てのウィジェットは可能ならお互いに閉
         * じたものとしてパックされる。*/
        gtk_widget_set_usize (separator, 400, 5);
        /* この main() の最初付近で作成された vbox (box1) の中にセパレー
         * タをパックする。*/
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);    
    }
    
    /* もう一つの新しい hbox を作成する..。必要なだけたくさんを使える
       ことを覚えておこう! */
    quitbox = gtk_hbox_new (FALSE, 0);
    
    /* quit ボタン。*/
    button = gtk_button_new_with_label ("Quit");
    
    /* この window を破壊するシグナルを設定する。これは "destroy" シグ
     * ナルをこの window に送る。これは前で定義された我々のシグナルハ
     * ンドラによって引き起こされることを覚えておこう。*/
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                               GTK_SIGNAL_FUNC (gtk_main_quit),
                               GTK_OBJECT (window));
    /* quitbox 中にこの button をパックする。gtk_box_pack_start への最
     * 後の 3 引数は expand, fill, padding だ。 */
    gtk_box_pack_start (GTK_BOX (quitbox), button, TRUE, FALSE, 0);
    /* quitbox を vbox (box1) の中にパックする。*/
    gtk_box_pack_start (GTK_BOX (box1), quitbox, FALSE, FALSE, 0);
    
    /* 今全ての我々のウィジェットを含んだ vbox (box1) を、メインの
     * window の中にパックする。*/
    gtk_container_add (GTK_CONTAINER (window), box1);
    
    /* そして全てを表示する。*/
    gtk_widget_show (button);
    gtk_widget_show (quitbox);
    
    gtk_widget_show (box1);
    /* 全てを一度にポップアップするために最後にこの window を表示。*/
    gtk_widget_show (window);
    
    /* そして勿論、メインの関数。 */
    gtk_main ();

    /* gtk_main_quit() が呼ばれた時に制御が帰るが、gtk_exit が使われた
     * 場合はそうではない。*/
    
    return 0;
}
/* example-end */
