/* example-start scribble-simple scribble-simple.c */

/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * 本ライブラリはフリー・ソフトウェアです。あなたは、Free Software 
 * Foundation が公表した GNU ライブラリ一般公有使用許諾の「バージョン
 *  2」或いはそれ以降の各バージョンの中からいずれかを選択し、そのバー
 * ジョンが定める条項に従って本ライブラリを再頌付または変更することが
 * できます。
 *
 * 本ライブラリは有用とは思いますが、頌付にあたっては、市場性及び特定
 * 目的適合性についての暗黙の保証を含めて、いかなる保証も行いません。
 * 詳細については GNU ライブラリ一般公有使用許諾をお読みください。
 *
 * あなたは、本ライブラリと一緒に GNU ライブラリ一般公有使用許諾の写
 * しを受け取っているはずです。そうでない場合は、Free Software 
 * Foundation , Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA へ手紙を書いてください。
 * [訳注:翻訳はしていますが、正式な文書は英文です]
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

/* 描画領域のためのバッキングピックスマップ */
static GdkPixmap *pixmap = NULL;

/* 適切なサイズの新しいバッキングピックスマップを作成 */
static gint
configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
  if (pixmap)
    gdk_pixmap_unref(pixmap);

  pixmap = gdk_pixmap_new(widget->window,
                          widget->allocation.width,
                          widget->allocation.height,
                          -1);
  gdk_draw_rectangle (pixmap,
                      widget->style->white_gc,
                      TRUE,
                      0, 0,
                      widget->allocation.width,
                      widget->allocation.height);

  return TRUE;
}

/* バッキングピックスマップからスクリーンを再描写 */
static gint
expose_event (GtkWidget *widget, GdkEventExpose *event)
{
  gdk_draw_pixmap(widget->window,
                  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  pixmap,
                  event->area.x, event->area.y,
                  event->area.x, event->area.y,
                  event->area.width, event->area.height);

  return FALSE;
}

/* スクリーン上に長方形を描画 */
static void
draw_brush (GtkWidget *widget, gdouble x, gdouble y)
{
  GdkRectangle update_rect;

  update_rect.x = x - 5;
  update_rect.y = y - 5;
  update_rect.width = 10;
  update_rect.height = 10;
  gdk_draw_rectangle (pixmap,
                      widget->style->black_gc,
                      TRUE,
                      update_rect.x, update_rect.y,
                      update_rect.width, update_rect.height);
  gtk_widget_draw (widget, &update_rect);
}

static gint
button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  if (event->button == 1 && pixmap != NULL)
    draw_brush (widget, event->x, event->y);

  return TRUE;
}

static gint
motion_notify_event (GtkWidget *widget, GdkEventMotion *event)
{
  int x, y;
  GdkModifierType state;

  if (event->is_hint)
    gdk_window_get_pointer (event->window, &x, &y, &state);
  else
    {
      x = event->x;
      y = event->y;
      state = event->state;
    }
    
  if (state & GDK_BUTTON1_MASK && pixmap != NULL)
    draw_brush (widget, x, y);
  
  return TRUE;
}

void
quit ()
{
  gtk_exit (0);
}

int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *drawing_area;
  GtkWidget *vbox;

  GtkWidget *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "Test Input");

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC (quit), NULL);

  /* 描画領域 を作成 */

  drawing_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 200, 200);
  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_show (drawing_area);

  /* バッキングピックスマップの処理に使用するシグナル */

  gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
                      (GtkSignalFunc) expose_event, NULL);
  gtk_signal_connect (GTK_OBJECT(drawing_area),"configure_event",
                      (GtkSignalFunc) configure_event, NULL);

  /* イベントシグナル */

  gtk_signal_connect (GTK_OBJECT (drawing_area), "motion_notify_event",
                      (GtkSignalFunc) motion_notify_event, NULL);
  gtk_signal_connect (GTK_OBJECT (drawing_area), "button_press_event",
                      (GtkSignalFunc) button_press_event, NULL);

  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_POINTER_MOTION_MASK
                         | GDK_POINTER_MOTION_HINT_MASK);

  /* .. そして quit ボタン */
  button = gtk_button_new_with_label ("Quit");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (window));
  gtk_widget_show (button);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
/* example-end */
