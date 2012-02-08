/* example-start rangewidgets rangewidgets.c */

#include <gtk/gtk.h>

GtkWidget *hscale, *vscale;

void cb_pos_menu_select (GtkWidget *item, GtkPositionType pos)
{
  /* 両方のスケールウィジェットに対して値の表示位置を設定する */
  gtk_scale_set_value_pos (GTK_SCALE (hscale), pos);
  gtk_scale_set_value_pos (GTK_SCALE (vscale), pos);
}

void cb_update_menu_select (GtkWidget *item, GtkUpdateType policy)
{
  /* 両方のスケールウィジェットに対してアップデートポリシーを設定する */
  gtk_range_set_update_policy (GTK_RANGE (hscale), policy);
  gtk_range_set_update_policy (GTK_RANGE (vscale), policy);
}

void cb_digits_scale (GtkAdjustment *adj)
{
  /* adj->vaule で丸められる小数位の桁数を設定 */
  gtk_scale_set_digits (GTK_SCALE (hscale), (gint) adj->value);
  gtk_scale_set_digits (GTK_SCALE (vscale), (gint) adj->value);
}

void cb_page_size (GtkAdjustment *get, GtkAdjustment *set)
{
  /* "Page Size" スケールによって指定された値で、サンプルの
     アジャストメントの page_size と page_increment を設定 */
  /* set the page size and page increment size of the sample
     adjustment to the value specified by the "Page Size" scale */
  set->page_size = get->value;
  set->page_increment = get->value;
  /* ここで "changed" シグナルを発行し、このアジャストメントに
     接続している全てのウィジェットを再構成 */
  /* now emit the "changed" signal to reconfigure all the widgets that
     are attached to this adjustment */
  gtk_signal_emit_by_name (GTK_OBJECT (set), "changed");
}

void cb_draw_value (GtkToggleButton *button)
{
  /* チェックボタンの状態によって、スケールウィジェットに
     値を表示するかどうかを切替 */
  /* turn the value display on the scale widgets off or on depending
     on the state of the checkbutton */
  gtk_scale_set_draw_value (GTK_SCALE (hscale), button->active);
  gtk_scale_set_draw_value (GTK_SCALE (vscale), button->active);  
}

/* 便宜的な関数 */

GtkWidget *make_menu_item (gchar *name, GtkSignalFunc callback,
                           gpointer data)
{
  GtkWidget *item;
  
  item = gtk_menu_item_new_with_label (name);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      callback, data);
  gtk_widget_show (item);

  return item;
}

void scale_set_default_values (GtkScale *scale)
{
  gtk_range_set_update_policy (GTK_RANGE (scale),
                               GTK_UPDATE_CONTINUOUS);
  gtk_scale_set_digits (scale, 1);
  gtk_scale_set_value_pos (scale, GTK_POS_TOP);
  gtk_scale_set_draw_value (scale, TRUE);
}

/* サンプルのウィンドウを作成 */

void create_range_controls (void)
{
  GtkWidget *window;
  GtkWidget *box1, *box2, *box3;
  GtkWidget *button;
  GtkWidget *scrollbar;
  GtkWidget *separator;
  GtkWidget *opt, *menu, *item;
  GtkWidget *label;
  GtkWidget *scale;
  GtkObject *adj1, *adj2;

  /* 標準的なウィンドウを作成する主要部分 */
  /* standard window-creating stuff */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC(gtk_main_quit),
                      NULL);
  gtk_window_set_title (GTK_WINDOW (window), "range controls");

  box1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), box1);
  gtk_widget_show (box1);

  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  /* value, lower, upper, step_increment, page_increment, page_size */
  /*  page_size 値は、スクロールバーウィジェットとは異なることに注意!!
      取り得る最大値は、(upper - page_size) になる */
  adj1 = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
  
  vscale = gtk_vscale_new (GTK_ADJUSTMENT (adj1));
  scale_set_default_values (GTK_SCALE (vscale));
  gtk_box_pack_start (GTK_BOX (box2), vscale, TRUE, TRUE, 0);
  gtk_widget_show (vscale);

  box3 = gtk_vbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX (box2), box3, TRUE, TRUE, 0);
  gtk_widget_show (box3);

  /* 同じアジャストメントを再利用 */
  hscale = gtk_hscale_new (GTK_ADJUSTMENT (adj1));
  gtk_widget_set_usize (GTK_WIDGET (hscale), 200, 30);
  scale_set_default_values (GTK_SCALE (hscale));
  gtk_box_pack_start (GTK_BOX (box3), hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  /* 同じアジャストメントを再び再利用 */
  scrollbar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adj1));
  /* この指定で、スクロールバーが移動すると
     スケールは連続的に更新される */
  gtk_range_set_update_policy (GTK_RANGE (scrollbar), 
                               GTK_UPDATE_CONTINUOUS);
  gtk_box_pack_start (GTK_BOX (box3), scrollbar, TRUE, TRUE, 0);
  gtk_widget_show (scrollbar);

  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  /* 値を表示するかどうかを設定するためのチェックボタン */
  button = gtk_check_button_new_with_label
    ("Display value on scale widgets");
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (button), TRUE);
  gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC
                      (cb_draw_value), NULL);
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  
  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);

  /* 値を表示する位置を変更するためのオプションメニュー */
  label = gtk_label_new ("Scale Value Position:");
  gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
  gtk_widget_show (label);
  
  opt = gtk_option_menu_new();
  menu = gtk_menu_new();

  item = make_menu_item ("Top", GTK_SIGNAL_FUNC (cb_pos_menu_select),
                         GINT_TO_POINTER (GTK_POS_TOP));
  gtk_menu_append (GTK_MENU (menu), item);
  
  item = make_menu_item ("Bottom", GTK_SIGNAL_FUNC (cb_pos_menu_select), 
                         GINT_TO_POINTER (GTK_POS_BOTTOM));
  gtk_menu_append (GTK_MENU (menu), item);
  
  item = make_menu_item ("Left", GTK_SIGNAL_FUNC (cb_pos_menu_select),
                         GINT_TO_POINTER (GTK_POS_LEFT));
  gtk_menu_append (GTK_MENU (menu), item);
  
  item = make_menu_item ("Right", GTK_SIGNAL_FUNC (cb_pos_menu_select),
                         GINT_TO_POINTER (GTK_POS_RIGHT));
  gtk_menu_append (GTK_MENU (menu), item);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);
  gtk_box_pack_start (GTK_BOX (box2), opt, TRUE, TRUE, 0);
  gtk_widget_show (opt);

  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);

  /* さらに別のオプションメニュー
     ここはスケールウィジェットのアップデートポリシー設定用 */
  label = gtk_label_new ("Scale Update Policy:");
  gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
  gtk_widget_show (label);
  
  opt = gtk_option_menu_new();
  menu = gtk_menu_new();
  
  item = make_menu_item ("Continuous",
                         GTK_SIGNAL_FUNC (cb_update_menu_select),
                         GINT_TO_POINTER (GTK_UPDATE_CONTINUOUS));
  gtk_menu_append (GTK_MENU (menu), item);
  
  item = make_menu_item ("Discontinuous",
                         GTK_SIGNAL_FUNC (cb_update_menu_select),
                         GINT_TO_POINTER (GTK_UPDATE_DISCONTINUOUS));
  gtk_menu_append (GTK_MENU (menu), item);
  
  item = make_menu_item ("Delayed",
                         GTK_SIGNAL_FUNC (cb_update_menu_select),
                         GINT_TO_POINTER (GTK_UPDATE_DELAYED));
  gtk_menu_append (GTK_MENU (menu), item);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);
  gtk_box_pack_start (GTK_BOX (box2), opt, TRUE, TRUE, 0);
  gtk_widget_show (opt);
  
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  
  /* サンプルのスケールにおける小数点桁数の値を調節する
     GtkHScale ウィジェット */
  label = gtk_label_new ("Scale Digits:");
  gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  adj2 = gtk_adjustment_new (1.0, 0.0, 5.0, 1.0, 1.0, 0.0);
  gtk_signal_connect (GTK_OBJECT (adj2), "value_changed",
                      GTK_SIGNAL_FUNC (cb_digits_scale), NULL);
  scale = gtk_hscale_new (GTK_ADJUSTMENT (adj2));
  gtk_scale_set_digits (GTK_SCALE (scale), 0);
  gtk_box_pack_start (GTK_BOX (box2), scale, TRUE, TRUE, 0);
  gtk_widget_show (scale);

  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);
  
  box2 = gtk_hbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  
  /* スクロールバーの page size を調節する、
     最後の GtkHScale ウィジェット */
  label = gtk_label_new ("Scrollbar Page Size:");
  gtk_box_pack_start (GTK_BOX (box2), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  adj2 = gtk_adjustment_new (1.0, 1.0, 101.0, 1.0, 1.0, 0.0);
  gtk_signal_connect (GTK_OBJECT (adj2), "value_changed",
                      GTK_SIGNAL_FUNC (cb_page_size), adj1);
  scale = gtk_hscale_new (GTK_ADJUSTMENT (adj2));
  gtk_scale_set_digits (GTK_SCALE (scale), 0);
  gtk_box_pack_start (GTK_BOX (box2), scale, TRUE, TRUE, 0);
  gtk_widget_show (scale);

  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);
  gtk_widget_show (separator);

  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);
  gtk_widget_show (box2);

  button = gtk_button_new_with_label ("Quit");
  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                             GTK_SIGNAL_FUNC(gtk_main_quit),
                             NULL);
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);

  gtk_widget_show (window);
}

int main (int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  create_range_controls();

  gtk_main();

  return 0;
}

/* example-end */
