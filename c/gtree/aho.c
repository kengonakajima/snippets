/* example-start tree tree.c */

#include <gtk/gtk.h>

/* ���Ƥ� GtkItem:: ����� GtkTreeItem:: �����ʥ��� */
static void cb_itemsignal (GtkWidget *item, gchar *signame)
{
  gchar *name;
  GtkLabel *label;

  /* ����� GtkBin �ʤΤǻҶ����Ļ��äƤ��ơ����줬��٥�Ǥ��뤳��
     ��桹���ΤäƤ���ΤǼ��Ф� */
  label = GTK_LABEL (GTK_BIN (item)->child);
  /* ��٥�Υƥ����Ȥ���Ф� */
  gtk_label_get (label, &name);
  /* ���Υ����ƥब���äƤ���ĥ꡼�Υ�٥��������� */
  g_print ("%s called for item %s->%p, level %d\n", signame, name,
           item, GTK_TREE (item->parent)->level);
}

/* ��ա�����ϸƤФ�뤳�Ȥ�̵�� */
static void cb_unselect_child (GtkWidget *root_tree, GtkWidget *child,
                               GtkWidget *subtree)
{
  g_print ("unselect_child called for root tree %p, subtree %p, child %p\n",
           root_tree, subtree, child);
}

/* ��ա�����ϡ��������򤵤�Ƥ��褦�Ȥ��ʤ����ȡ��桼������������
   ��򥯥�å����뤿�Ӥ˸ƤФ�롣*/
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

    /* �ꥹ�ȥΡ��ɤ��� GtkWidget �ݥ��󥿤���� */
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

  /* ���ѥȥåץ�٥륦����ɥ� */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT(window), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
  gtk_container_border_width (GTK_CONTAINER(window), 5);

  /* ���ѥ�������ɥ�����ɥ� */
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize (scrolled_win, 150, 200);
  gtk_container_add (GTK_CONTAINER(window), scrolled_win);
  gtk_widget_show (scrolled_win);
  
  /* �롼�ȥĥ꡼����� */
  tree = gtk_tree_new();
  g_print ("root tree is %p\n", tree);
  /* �� GtkTree:: �����ʥ����³ */
  gtk_signal_connect (GTK_OBJECT(tree), "select_child",
                      GTK_SIGNAL_FUNC(cb_select_child), tree);
  gtk_signal_connect (GTK_OBJECT(tree), "unselect_child",
                      GTK_SIGNAL_FUNC(cb_unselect_child), tree);
  gtk_signal_connect (GTK_OBJECT(tree), "selection_changed",
                      GTK_SIGNAL_FUNC(cb_selection_changed), tree);
  /* ����򥹥�����ɥ�����ɥ��˲ä��� */
  gtk_container_add (GTK_CONTAINER(scrolled_win), tree);
  /* ���쥯�����⡼�ɤ����� */
  gtk_tree_set_selection_mode (GTK_TREE(tree),
                               GTK_SELECTION_MULTIPLE);
  /* �����ɽ������ */
  gtk_widget_show (tree);

  for (i = 0; i < 5; i++){
    GtkWidget *subtree, *item;
    gint j;

    /* �ĥ꡼�����ƥ����� */
    item = gtk_tree_item_new_with_label (itemnames[i]);
    /* ���Ƥ� GtkItem:: ����� GtkTreeItem:: �����ʥ����³ */
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
    /* �ƤΥĥ꡼�ˤ����ä��� */
    gtk_tree_append (GTK_TREE(tree), item);
    /* �����ɽ�� - ����Ϥ��ĤǤ�Ԥ���ΤǤ��� */
    gtk_widget_show (item);
    /* ���Υ����ƥ�Υ��֥ĥ꡼����� */
    subtree = gtk_tree_new();
    g_print ("-> item %s->%p, subtree %p\n", itemnames[i], item,
             subtree);

    /* ���֥ĥ꡼�λҶ�ã�Τ���˸ƤФ�뤳���Υ����ʥ���ߤ����ʤ顢
       ����Ϥޤ�ɬ�פǤ��롣��ա�selection_change �ϤȤˤ����롼�ȥ�
       �꡼�Τ���˥����ʥ뤵��롣*/
    gtk_signal_connect (GTK_OBJECT(subtree), "select_child",
                        GTK_SIGNAL_FUNC(cb_select_child), subtree);
    gtk_signal_connect (GTK_OBJECT(subtree), "unselect_child",
                        GTK_SIGNAL_FUNC(cb_unselect_child), subtree);
    /* ����������ƶ���̵�����ʤ��ʤ餳��ϥ��֥ĥ꡼�Ǵ�����̵�뤵��
       �뤫��Ǥ��롣*/
    gtk_tree_set_selection_mode (GTK_TREE(subtree),
                                 GTK_SELECTION_SINGLE);
    /* ����ⲿ��Ԥ�ʤ������ɤ��餫�Ȥ����а㤦��ͳ - �ĥ꡼��
       view_mode ����� view_line ���ͤϥޥåפ���Ƥ�����˥��֥ĥ꡼
       �ޤ����Ť���뤳�Ȥˤ�롣���Τ��ᡢ��Ǥ�������ꤹ��ȼ¤Τ�
       ���� (����ͽ�ۤǤ��ʤ�) �ƶ������롣*/
    gtk_tree_set_view_mode (GTK_TREE(subtree), GTK_TREE_VIEW_ITEM);
    /* ���Υ����ƥ�Υ��֥ĥ꡼�����ꤹ�� - ��ա�����ϥ����ƥब����
       �ƤΥĥ꡼�˲ä�����`��ޤ�'�ϹԤ����Ȥ��Ǥ��ʤ�! */
    gtk_tree_item_set_subtree (GTK_TREE_ITEM(item), subtree);

    for (j = 0; j < 5; j++){
      GtkWidget *subitem;

      /* �ۤȤ��Ʊ����ˡ�ǥ��֥ĥ꡼�����ƥ����� */
      subitem = gtk_tree_item_new_with_label (itemnames[j]);
      /* ���Ƥ� GtkItem:: ����� GtkTreeItem:: �����ʥ����³ */
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
      /* �����ƤΥĥ꡼�˲ä��� */
      gtk_tree_append (GTK_TREE(subtree), subitem);
      /* �����ɽ�� */
      gtk_widget_show (subitem);
    }
  }

  /* ������ɥ���ɽ�����ơ��̤Ƥ��ʤ��롼�פ� */
  gtk_widget_show (window);
  gtk_main();
  return 0;
}
/* example-end */
