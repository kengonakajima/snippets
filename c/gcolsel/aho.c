/* example-start colorsel colorsel.c */

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

GtkWidget *colorseldlg = NULL;
GtkWidget *drawingarea = NULL;

/* ���ѹ��ϥ�ɥ� */

void color_changed_cb (GtkWidget *widget, GtkColorSelection *colorsel)
{
  gdouble color[3];
  GdkColor gdk_color;
  GdkColormap *colormap;

  /* �ɥ����󥰥��ꥢ�Υ��顼�ޥåפ���� */

  colormap = gdk_window_get_colormap (drawingarea->window);

  /* �����ȥ��顼����� */

  gtk_color_selection_get_color (colorsel,color);

  /* ���̵�� 16 �ӥå����� (0..65535) ��Ŭ�礵���ơ�GdkColor ��¤�Τ�
   * ���� */

  gdk_color.red = (guint16)(color[0]*65535.0);
  gdk_color.green = (guint16)(color[1]*65535.0);
  gdk_color.blue = (guint16)(color[2]*65535.0);

  /* ���γ��� */

  gdk_color_alloc (colormap, &gdk_color);

  /* window �ΥХå����饦��ɥ��顼������ */

  gdk_window_set_background (drawingarea->window, &gdk_color);

  /* window�򥯥ꥢ */

  gdk_window_clear (drawingarea->window);
}

/* �ɥ����󥰥��ꥢ���٥�ȥϥ�ɥ� */

gint area_event (GtkWidget *widget, GdkEvent *event, gpointer client_data)
{
  gint handled = FALSE;
  GtkWidget *colorsel;

  /* button pressed ���٥�Ȥ����������Ĵ�٤� */

  if (event->type == GDK_BUTTON_PRESS && colorseldlg == NULL)
    {
      /* �����������٥�ȤϤ��뤬 colorseldlg �Ϥޤ�̵��! */

      handled = TRUE;

      /* ���顼���쥯����������������� */

      colorseldlg = gtk_color_selection_dialog_new("Select background color");

      /* GtkColorSelection ���������åȤ���� */

      colorsel = GTK_COLOR_SELECTION_DIALOG(colorseldlg)->colorsel;

      /* "color_changed" �����ʥ����³��colorsel ���������åȤ˥��饤
       * ����ȥǡ��������� */

      gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
        (GtkSignalFunc)color_changed_cb, (gpointer)colorsel);

      /* ����������ɽ�� */

      gtk_widget_show(colorseldlg);
    }

  return handled;
}

/* �ĺ����ƽ�λ����ϥ�ɥ� */

void destroy_window (GtkWidget *widget, gpointer client_data)
{
  gtk_main_quit ();
}

/* �ᥤ�� */

gint main (gint argc, gchar *argv[])
{
  GtkWidget *window;

  /* �ġ��륭�åȤ�������gtk��Ϣ�Υ��ޥ�ɥ饤�󥹥��åդ��� */

  gtk_init (&argc,&argv);

  /* �ȥåץ�٥륦����ɥ�������������ȥ�ȥݥꥷ�������� */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(window), "Color selection test");
  gtk_window_set_policy (GTK_WINDOW(window), TRUE, TRUE, TRUE);

  /* "delete" ����� "destroy" ���٥�Ȥ˼���դ�����λ��ǽ�ˤ��� */

  gtk_signal_connect (GTK_OBJECT(window), "delete_event",
    (GtkSignalFunc)destroy_window, (gpointer)window);

  gtk_signal_connect (GTK_OBJECT(window), "destroy",
    (GtkSignalFunc)destroy_window, (gpointer)window);
  
  /* drawingarea ������������������ꤷ�ܥ��󥤥٥�Ȥ򽦤� */

  drawingarea = gtk_drawing_area_new ();

  gtk_drawing_area_size (GTK_DRAWING_AREA(drawingarea), 200, 200);

  gtk_widget_set_events (drawingarea, GDK_BUTTON_PRESS_MASK);

  gtk_signal_connect (GTK_OBJECT(drawingarea), "event", 
    (GtkSignalFunc)area_event, (gpointer)drawingarea);
  
  /* window �� drawingarea ��ä���ξ���Ȥ�ɽ�� */

  gtk_container_add (GTK_CONTAINER(window), drawingarea);

  gtk_widget_show (drawingarea);
  gtk_widget_show (window);
  
  /* gtk �ᥤ��롼�פ�����(����ϵ��äƤ��ʤ�) */

  gtk_main ();

  /* �Ե����ʥ���ѥ������­������ */

  return 0;
}
/* exampahle-end */
