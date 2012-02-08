/* example-start tree tree.c */

#include <gtk/gtk.h>

/* 全ての GtkItem:: および GtkTreeItem:: シグナル用 */
static void cb_itemsignal (GtkWidget *item, gchar *signame)
{
  gchar *name;
  GtkLabel *label;

  /* これは GtkBin なので子供を一つ持っていて、これがラベルであること
     を我々は知っているので取り出す */
  label = GTK_LABEL (GTK_BIN (item)->child);
  /* ラベルのテキストを取り出す */
  gtk_label_get (label, &name);
  /* このアイテムが入っているツリーのレベルを取得する */
  g_print ("%s called for item %s->%p, level %d\n", signame, name,
           item, GTK_TREE (item->parent)->level);
}

/* 注意、これは呼ばれることは無い */
static void cb_unselect_child (GtkWidget *root_tree, GtkWidget *child,
                               GtkWidget *subtree)
{
  g_print ("unselect_child called for root tree %p, subtree %p, child %p\n",
           root_tree, subtree, child);
}

/* 注意、これは、既に選択されていようといなかろうと、ユーザーがアイテ
   ムをクリックするたびに呼ばれる。*/
static void cb_select_child (GtkWidget *root_tree, GtkWidget *child,
                             GtkWidget *subtree)
{
  g_print ("select_child called for root tree %p, subtree %p, child %p\n",
           root_tree, subtree, child);
}

static void cb_selection_changed (GtkWidget *tree)
{
  GList *i;
  
  g_print ("selection_change called for tree %p\n", tree);
  g_print ("selected objects are:\n");

  i = GTK_TREE_SELECTION(tree);
  while (i){
    gchar *name;
    GtkLabel *label;
    GtkWidget *item;

    /* リストノードから GtkWidget ポインタを取得 */
    item = GTK_WIDGET (i->data);
    label = GTK_LABEL (GTK_BIN (item)->child);
    gtk_label_get (label, &name);
    g_print ("\t%s on level %d\n", name, GTK_TREE
             (item->parent)->level);
    i = i->next;
  }
}

int main (int argc, char *argv[])
{
  GtkWidget *window, *scrolled_win, *tree;
  static gchar *itemnames[] = {"Foo", "Bar", "Baz", "Quux",
                               "Maurice"};
  gint i;

  gtk_init (&argc, &argv);

  /* 汎用トップレベルウィンドウ */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT(window), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
  gtk_container_border_width (GTK_CONTAINER(window), 5);

  /* 汎用スクロールドウィンドウ */
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize (scrolled_win, 150, 200);
  gtk_container_add (GTK_CONTAINER(window), scrolled_win);
  gtk_widget_show (scrolled_win);
  
  /* ルートツリーを作成 */
  tree = gtk_tree_new();
  g_print ("root tree is %p\n", tree);
  /* 全 GtkTree:: シグナルを接続 */
  gtk_signal_connect (GTK_OBJECT(tree), "select_child",
                      GTK_SIGNAL_FUNC(cb_select_child), tree);
  gtk_signal_connect (GTK_OBJECT(tree), "unselect_child",
                      GTK_SIGNAL_FUNC(cb_unselect_child), tree);
  gtk_signal_connect (GTK_OBJECT(tree), "selection_changed",
                      GTK_SIGNAL_FUNC(cb_selection_changed), tree);
  /* これをスクロールドウィンドウに加える */
  gtk_container_add (GTK_CONTAINER(scrolled_win), tree);
  /* セレクションモードを設定 */
  gtk_tree_set_selection_mode (GTK_TREE(tree),
                               GTK_SELECTION_MULTIPLE);
  /* これを表示する */
  gtk_widget_show (tree);

  for (i = 0; i < 5; i++){
    GtkWidget *subtree, *item;
    gint j;

    /* ツリーアイテムを作成 */
    item = gtk_tree_item_new_with_label (itemnames[i]);
    /* 全ての GtkItem:: および GtkTreeItem:: シグナルを接続 */
    gtk_signal_connect (GTK_OBJECT(item), "select",
                        GTK_SIGNAL_FUNC(cb_itemsignal), "select");
    gtk_signal_connect (GTK_OBJECT(item), "deselect",
                        GTK_SIGNAL_FUNC(cb_itemsignal), "deselect");
    gtk_signal_connect (GTK_OBJECT(item), "toggle",
                        GTK_SIGNAL_FUNC(cb_itemsignal), "toggle");
    gtk_signal_connect (GTK_OBJECT(item), "expand",
                        GTK_SIGNAL_FUNC(cb_itemsignal), "expand");
    gtk_signal_connect (GTK_OBJECT(item), "collapse",
                        GTK_SIGNAL_FUNC(cb_itemsignal), "collapse");
    /* 親のツリーにこれを加える */
    gtk_tree_append (GTK_TREE(tree), item);
    /* これを表示 - これはいつでも行われるのである */
    gtk_widget_show (item);
    /* このアイテムのサブツリーを作成 */
    subtree = gtk_tree_new();
    g_print ("-> item %s->%p, subtree %p\n", itemnames[i], item,
             subtree);

    /* サブツリーの子供達のために呼ばれるこれらのシグナルを欲しいなら、
       これはまだ必要である。注意、selection_change はとにかくルートツ
       リーのためにシグナルされる。*/
    gtk_signal_connect (GTK_OBJECT(subtree), "select_child",
                        GTK_SIGNAL_FUNC(cb_select_child), subtree);
    gtk_signal_connect (GTK_OBJECT(subtree), "unselect_child",
                        GTK_SIGNAL_FUNC(cb_unselect_child), subtree);
    /* これは全く影響が無い、なぜならこれはサブツリーで完全に無視され
       るからである。*/
    gtk_tree_set_selection_mode (GTK_TREE(subtree),
                                 GTK_SELECTION_SINGLE);
    /* これも何も行わないが、どちらかといえば違う理由 - ツリーの
       view_mode および view_line の値はマップされている場合にサブツリー
       まで伝播されることによる。そのため、後でこれを設定すると実のと
       ころ (何か予想できない) 影響がある。*/
    gtk_tree_set_view_mode (GTK_TREE(subtree), GTK_TREE_VIEW_ITEM);
    /* このアイテムのサブツリーを設定する - 注意、これはアイテムがその
       親のツリーに加えられる`後まで'は行うことができない! */
    gtk_tree_item_set_subtree (GTK_TREE_ITEM(item), subtree);

    for (j = 0; j < 5; j++){
      GtkWidget *subitem;

      /* ほとんど同じ方法でサブツリーアイテムを作成 */
      subitem = gtk_tree_item_new_with_label (itemnames[j]);
      /* 全ての GtkItem:: および GtkTreeItem:: シグナルを接続 */
      gtk_signal_connect (GTK_OBJECT(subitem), "select",
                          GTK_SIGNAL_FUNC(cb_itemsignal), "select");
      gtk_signal_connect (GTK_OBJECT(subitem), "deselect",
                          GTK_SIGNAL_FUNC(cb_itemsignal), "deselect");
      gtk_signal_connect (GTK_OBJECT(subitem), "toggle",
                          GTK_SIGNAL_FUNC(cb_itemsignal), "toggle");
      gtk_signal_connect (GTK_OBJECT(subitem), "expand",
                          GTK_SIGNAL_FUNC(cb_itemsignal), "expand");
      gtk_signal_connect (GTK_OBJECT(subitem), "collapse",
                          GTK_SIGNAL_FUNC(cb_itemsignal), "collapse");
      g_print ("-> -> item %s->%p\n", itemnames[j], subitem);
      /* これを親のツリーに加える */
      gtk_tree_append (GTK_TREE(subtree), subitem);
      /* これを表示 */
      gtk_widget_show (subitem);
    }
  }

  /* ウィンドウを表示して、果てしなきループへ */
  gtk_widget_show (window);
  gtk_main();
  return 0;
}
/* example-end */
