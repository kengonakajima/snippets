/* example-start colorsel colorsel.c */

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

GtkWidget *colorseldlg = NULL;
GtkWidget *drawingarea = NULL;

/* 色変更ハンドラ */

void color_changed_cb (GtkWidget *widget, GtkColorSelection *colorsel)
{
  gdouble color[3];
  GdkColor gdk_color;
  GdkColormap *colormap;

  /* ドローイングエリアのカラーマップを取得 */

  colormap = gdk_window_get_colormap (drawingarea->window);

  /* カレントカラーを取得 */

  gtk_color_selection_get_color (colorsel,color);

  /* 符号無し 16 ビット整数 (0..65535) に適合させて、GdkColor 構造体に
   * 挿入 */

  gdk_color.red = (guint16)(color[0]*65535.0);
  gdk_color.green = (guint16)(color[1]*65535.0);
  gdk_color.blue = (guint16)(color[2]*65535.0);

  /* 色の確保 */

  gdk_color_alloc (colormap, &gdk_color);

  /* window のバックグラウンドカラーを設定 */

  gdk_window_set_background (drawingarea->window, &gdk_color);

  /* windowをクリア */

  gdk_window_clear (drawingarea->window);
}

/* ドローイングエリアイベントハンドラ */

gint area_event (GtkWidget *widget, GdkEvent *event, gpointer client_data)
{
  gint handled = FALSE;
  GtkWidget *colorsel;

  /* button pressed イベントを受けたかを調べる */

  if (event->type == GDK_BUTTON_PRESS && colorseldlg == NULL)
    {
      /* イエス、イベントはあるが colorseldlg はまだ無い! */

      handled = TRUE;

      /* カラーセレクションダイアログを作成 */

      colorseldlg = gtk_color_selection_dialog_new("Select background color");

      /* GtkColorSelection ウィジェットを取得 */

      colorsel = GTK_COLOR_SELECTION_DIALOG(colorseldlg)->colorsel;

      /* "color_changed" シグナルを接続、colorsel ウィジェットにクライ
       * アントデータを設定 */

      gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
        (GtkSignalFunc)color_changed_cb, (gpointer)colorsel);

      /* ダイアログを表示 */

      gtk_widget_show(colorseldlg);
    }

  return handled;
}

/* 閉鎖して終了するハンドラ */

void destroy_window (GtkWidget *widget, gpointer client_data)
{
  gtk_main_quit ();
}

/* メイン */

gint main (gint argc, gchar *argv[])
{
  GtkWidget *window;

  /* ツールキットを初期化、gtk関連のコマンドラインスタッフを削除 */

  gtk_init (&argc,&argv);

  /* トップレベルウィンドウを作成、タイトルとポリシーを設定 */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(window), "Color selection test");
  gtk_window_set_policy (GTK_WINDOW(window), TRUE, TRUE, TRUE);

  /* "delete" および "destroy" イベントに取り付け、終了可能にする */

  gtk_signal_connect (GTK_OBJECT(window), "delete_event",
    (GtkSignalFunc)destroy_window, (gpointer)window);

  gtk_signal_connect (GTK_OBJECT(window), "destroy",
    (GtkSignalFunc)destroy_window, (gpointer)window);
  
  /* drawingarea を作成、サイズを設定しボタンイベントを拾う */

  drawingarea = gtk_drawing_area_new ();

  gtk_drawing_area_size (GTK_DRAWING_AREA(drawingarea), 200, 200);

  gtk_widget_set_events (drawingarea, GDK_BUTTON_PRESS_MASK);

  gtk_signal_connect (GTK_OBJECT(drawingarea), "event", 
    (GtkSignalFunc)area_event, (gpointer)drawingarea);
  
  /* window に drawingarea を加え、両方とも表示 */

  gtk_container_add (GTK_CONTAINER(window), drawingarea);

  gtk_widget_show (drawingarea);
  gtk_widget_show (window);
  
  /* gtk メインループに入る(これは帰ってこない) */

  gtk_main ();

  /* 不機嫌なコンパイラを満足させる */

  return 0;
}
/* exampahle-end */
