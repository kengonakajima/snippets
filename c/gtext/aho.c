/* example-start text text.c */

/* text.c */

#include <stdio.h>
#include <gtk/gtk.h>

void text_toggle_editable (GtkWidget *checkbutton,
                           GtkWidget *text)
{
  gtk_text_set_editable(GTK_TEXT(text),
                        GTK_TOGGLE_BUTTON(checkbutton)->active);
}

void text_toggle_word_wrap (GtkWidget *checkbutton,
                            GtkWidget *text)
{
  gtk_text_set_word_wrap(GTK_TEXT(text),
                         GTK_TOGGLE_BUTTON(checkbutton)->active);
}

void close_application( GtkWidget *widget, gpointer data )
{
       gtk_main_quit();
}

int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *hbox;
  GtkWidget *button;
  GtkWidget *check;
  GtkWidget *separator;
  GtkWidget *table;
  GtkWidget *vscrollbar;
  GtkWidget *text;
  GdkColormap *cmap;
  GdkColor colour;
  GdkFont *fixed_font;

  FILE *infile;

  gtk_init (&argc, &argv);
 
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (window, 600, 500);
  gtk_window_set_policy (GTK_WINDOW(window), TRUE, TRUE, FALSE);  
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC(close_application),
                      NULL);
  gtk_window_set_title (GTK_WINDOW (window), "Text Widget Example");
  gtk_container_border_width (GTK_CONTAINER (window), 0);
  
  
  box1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), box1);
  gtk_widget_show (box1);
  
  
  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  gtk_widget_show (box2);
  
  
  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
  gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);
  gtk_widget_show (table);
  
  /* GtkText ���������åȤ���� */
  text = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT (text), TRUE);
  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (text);

  /* GtkText ���������åȤ˿�ľ��������С����ɲ� */
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
                    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);

  /* �����ƥ५�顼�ޥåפ���������֤������Ƥ� */
  cmap = gdk_colormap_get_system();
  colour.red = 0xffff;
  colour.green = 0;
  colour.blue = 0;
  if (!gdk_color_alloc(cmap, &colour)) {
    g_error("couldn't allocate colour");
  }

  /* fixed �ե���Ȥ��ɤ߹��� */
  fixed_font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");

  
      

  /* ���������åȤμ��β��ǥ�����ɥ���������뤳�Ȥǡ��ƥ����������������� */
  gtk_widget_realize (text);

  /* ʣ���ι������б�����٤����ƥ����ȥ��������åȤ���� */ 
  gtk_text_freeze (GTK_TEXT (text));
  
  /* ���դ��Υƥ����Ȥ����� */
  gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL,
                   "Supports ", -1);
  gtk_text_insert (GTK_TEXT (text), NULL, &colour, NULL,
                   "colored ", -1);
  gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL,
                   "text and different ", -1);
  gtk_text_insert (GTK_TEXT (text), fixed_font, &text->style->black, NULL,
                   "fonts\n\n", -1);
  
  /* �ƥ����ȥ�����ɥ��˥ե����� text.c ���ɤ߹��� */

  infile = fopen("text.c", "r");
  
  if (infile) {
    char buffer[1024];
    int nchars;
    
    while (1)
      {
        nchars = fread(buffer, 1, 1024, infile);
        gtk_text_insert (GTK_TEXT (text), fixed_font, NULL,
                         NULL, buffer, nchars);
        
        if (nchars < 1024)
          break;
      }
    
    fclose (infile);
  }

  /* �ƥ����ȥ��������åȤ���ह�뤳�Ȥǡ���������Ĥ���ɽ���Ǥ����
   * ���ˤ��� */
  gtk_text_thaw (GTK_TEXT (text));
  
  hbox = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (box2), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  check = gtk_check_button_new_with_label("Editable");
  gtk_box_pack_start (GTK_BOX (hbox), check, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT(check), "toggled",
                      GTK_SIGNAL_FUNC(text_toggle_editable), text);
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(check), TRUE);
  gtk_widget_show (check);
  check = gtk_check_button_new_with_label("Wrap Words");
  gtk_box_pack_start (GTK_BOX (hbox), check, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT(check), "toggled",
                      GTK_SIGNAL_FUNC(text_toggle_word_wrap), text);
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(check), FALSE);
  gtk_widget_show (check);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);
  gtk_widget_show (separator);

  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);
  gtk_widget_show (box2);
  
  button = gtk_button_new_with_label ("close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC(close_application),
                      NULL);
  gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);

  gtk_widget_show (window);

  gtk_main ();
  
  return 0;       
}


