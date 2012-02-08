/* example-start gtkdial gtkdial.c */

/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * �ܥ饤�֥��ϥե꡼�����եȥ������Ǥ������ʤ��ϡ�Free Software 
 * Foundation ����ɽ���� GNU �饤�֥����̸�ͭ���ѵ����Ρ֥С������
 *  2�װ����Ϥ���ʹߤγƥС��������椫�餤���줫�����򤷡����ΥС�
 * �����������˽��ä��ܥ饤�֥�������դޤ����ѹ����뤳�Ȥ�
 * �Ǥ��ޤ���
 *
 * �ܥ饤�֥���ͭ�ѤȤϻפ��ޤ��������դˤ����äƤϡ��Ծ����ڤ�����
 * ��ŪŬ�����ˤĤ��Ƥΰ��ۤ��ݾڤ�ޤ�ơ������ʤ��ݾڤ�Ԥ��ޤ���
 * �ܺ٤ˤĤ��Ƥ� GNU �饤�֥����̸�ͭ���ѵ������ɤߤ���������
 *
 * ���ʤ��ϡ��ܥ饤�֥��Ȱ��� GNU �饤�֥����̸�ͭ���ѵ����μ�
 * ���������äƤ���Ϥ��Ǥ��������Ǥʤ����ϡ�Free Software 
 * Foundation , Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA �ؼ���񤤤Ƥ���������
 * [����:�����Ϥ��Ƥ��ޤ�����������ʸ��ϱ�ʸ�Ǥ�]
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
#include <math.h>
#include <stdio.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>

#include "gtkdial.h"

#define SCROLL_DELAY_LENGTH  300
#define DIAL_DEFAULT_SIZE 100

/* ���θ��³���ؿ������ */

static void gtk_dial_class_init               (GtkDialClass    *klass);
static void gtk_dial_init                     (GtkDial         *dial);
static void gtk_dial_destroy                  (GtkObject        *object);
static void gtk_dial_realize                  (GtkWidget        *widget);
static void gtk_dial_size_request             (GtkWidget      *widget,
                                               GtkRequisition *requisition);
static void gtk_dial_size_allocate            (GtkWidget     *widget,
                                               GtkAllocation *allocation);
static gint gtk_dial_expose                   (GtkWidget        *widget,
                                                GdkEventExpose   *event);
static gint gtk_dial_button_press             (GtkWidget        *widget,
                                                GdkEventButton   *event);
static gint gtk_dial_button_release           (GtkWidget        *widget,
                                                GdkEventButton   *event);
static gint gtk_dial_motion_notify            (GtkWidget        *widget,
                                                GdkEventMotion   *event);
static gint gtk_dial_timer                    (GtkDial         *dial);

static void gtk_dial_update_mouse             (GtkDial *dial, gint x, gint y);
static void gtk_dial_update                   (GtkDial *dial);
static void gtk_dial_adjustment_changed       (GtkAdjustment    *adjustment,
                                                gpointer          data);
static void gtk_dial_adjustment_value_changed (GtkAdjustment    *adjustment,
                                                gpointer          data);

/* ������ǡ���*/

static GtkWidgetClass *parent_class = NULL;

guint
gtk_dial_get_type ()
{
  static guint dial_type = 0;

  if (!dial_type)
    {
      GtkTypeInfo dial_info =
      {
        "GtkDial",
        sizeof (GtkDial),
        sizeof (GtkDialClass),
        (GtkClassInitFunc) gtk_dial_class_init,
        (GtkObjectInitFunc) gtk_dial_init,
        (GtkArgSetFunc) NULL,
        (GtkArgGetFunc) NULL,
      };

      dial_type = gtk_type_unique (gtk_widget_get_type (), &dial_info);
    }

  return dial_type;
}

static void
gtk_dial_class_init (GtkDialClass *class)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;

  parent_class = gtk_type_class (gtk_widget_get_type ());

  object_class->destroy = gtk_dial_destroy;

  widget_class->realize = gtk_dial_realize;
  widget_class->expose_event = gtk_dial_expose;
  widget_class->size_request = gtk_dial_size_request;
  widget_class->size_allocate = gtk_dial_size_allocate;
  widget_class->button_press_event = gtk_dial_button_press;
  widget_class->button_release_event = gtk_dial_button_release;
  widget_class->motion_notify_event = gtk_dial_motion_notify;
}

static void
gtk_dial_init (GtkDial *dial)
{
  dial->button = 0;
  dial->policy = GTK_UPDATE_CONTINUOUS;
  dial->timer = 0;
  dial->radius = 0;
  dial->pointer_width = 0;
  dial->angle = 0.0;
  dial->old_value = 0.0;
  dial->old_lower = 0.0;
  dial->old_upper = 0.0;
  dial->adjustment = NULL;
}

GtkWidget*
gtk_dial_new (GtkAdjustment *adjustment)
{
  GtkDial *dial;

  dial = gtk_type_new (gtk_dial_get_type ());

  if (!adjustment)
    adjustment = (GtkAdjustment*) gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  gtk_dial_set_adjustment (dial, adjustment);

  return GTK_WIDGET (dial);
}

static void
gtk_dial_destroy (GtkObject *object)
{
  GtkDial *dial;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_DIAL (object));

  dial = GTK_DIAL (object);

  if (dial->adjustment)
    gtk_object_unref (GTK_OBJECT (dial->adjustment));

  if (GTK_OBJECT_CLASS (parent_class)->destroy)
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

GtkAdjustment*
gtk_dial_get_adjustment (GtkDial *dial)
{
  g_return_val_if_fail (dial != NULL, NULL);
  g_return_val_if_fail (GTK_IS_DIAL (dial), NULL);

  return dial->adjustment;
}

void
gtk_dial_set_update_policy (GtkDial      *dial,
                             GtkUpdateType  policy)
{
  g_return_if_fail (dial != NULL);
  g_return_if_fail (GTK_IS_DIAL (dial));

  dial->policy = policy;
}

void
gtk_dial_set_adjustment (GtkDial      *dial,
                          GtkAdjustment *adjustment)
{
  g_return_if_fail (dial != NULL);
  g_return_if_fail (GTK_IS_DIAL (dial));

  if (dial->adjustment)
    {
      gtk_signal_disconnect_by_data (GTK_OBJECT (dial->adjustment), (gpointer) dial);
      gtk_object_unref (GTK_OBJECT (dial->adjustment));
    }

  dial->adjustment = adjustment;
  gtk_object_ref (GTK_OBJECT (dial->adjustment));

  gtk_signal_connect (GTK_OBJECT (adjustment), "changed",
                      (GtkSignalFunc) gtk_dial_adjustment_changed,
                      (gpointer) dial);
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) gtk_dial_adjustment_value_changed,
                      (gpointer) dial);

  dial->old_value = adjustment->value;
  dial->old_lower = adjustment->lower;
  dial->old_upper = adjustment->upper;

  gtk_dial_update (dial);
}

static void
gtk_dial_realize (GtkWidget *widget)
{
  GtkDial *dial;
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_DIAL (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  dial = GTK_DIAL (widget);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget) | 
    GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
    GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
    GDK_POINTER_MOTION_HINT_MASK;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gdk_window_set_user_data (widget->window, widget);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void 
gtk_dial_size_request (GtkWidget      *widget,
                       GtkRequisition *requisition)
{
  requisition->width = DIAL_DEFAULT_SIZE;
  requisition->height = DIAL_DEFAULT_SIZE;
}

static void
gtk_dial_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  GtkDial *dial;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_DIAL (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;
  dial = GTK_DIAL (widget);

  if (GTK_WIDGET_REALIZED (widget))
    {

      gdk_window_move_resize (widget->window,
                              allocation->x, allocation->y,
                              allocation->width, allocation->height);

    }
  dial->radius = MIN(allocation->width,allocation->height) * 0.45;
  dial->pointer_width = dial->radius / 5;
}

static gint
gtk_dial_expose (GtkWidget      *widget,
                 GdkEventExpose *event)
{
  GtkDial *dial;
  GdkPoint points[3];
  gdouble s,c;
  gdouble theta;
  gint xc, yc;
  gint tick_length;
  gint i;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_DIAL (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->count > 0)
    return FALSE;
  
  dial = GTK_DIAL (widget);

  gdk_window_clear_area (widget->window,
                         0, 0,
                         widget->allocation.width,
                         widget->allocation.height);

  xc = widget->allocation.width/2;
  yc = widget->allocation.height/2;

  /* ���������� */

  for (i=0; i<25; i++)
    {
      theta = (i*M_PI/18. - M_PI/6.);
      s = sin(theta);
      c = cos(theta);

      tick_length = (i%6 == 0) ? dial->pointer_width : dial->pointer_width/2;
      
      gdk_draw_line (widget->window,
                     widget->style->fg_gc[widget->state],
                     xc + c*(dial->radius - tick_length),
                     yc - s*(dial->radius - tick_length),
                     xc + c*dial->radius,
                     yc - s*dial->radius);
    }

  /* �ݥ���(��)������ */

  s = sin(dial->angle);
  c = cos(dial->angle);


  points[0].x = xc + s*dial->pointer_width/2;
  points[0].y = yc + c*dial->pointer_width/2;
  points[1].x = xc + c*dial->radius;
  points[1].y = yc - s*dial->radius;
  points[2].x = xc - s*dial->pointer_width/2;
  points[2].y = yc - c*dial->pointer_width/2;

  gtk_draw_polygon (widget->style,
                    widget->window,
                    GTK_STATE_NORMAL,
                    GTK_SHADOW_OUT,
                    points, 3,
                    TRUE);
  
  return FALSE;
}

static gint
gtk_dial_button_press (GtkWidget      *widget,
                       GdkEventButton *event)
{
  GtkDial *dial;
  gint dx, dy;
  double s, c;
  double d_parallel;
  double d_perpendicular;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_DIAL (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  dial = GTK_DIAL (widget);

  /* �ޥ����Υܥ���򲡤������֤����ݥ��󥿤��ΰ����˴ޤޤ�Ƥ��뤫
     �ɤ�������ꤹ�롣����ϡ��ݥ��󥿤��̤�饤�󤫤�ޥ����β�����
     �����ޤǤο�ʿ�ȿ�ľ�ε�Υ��׻����뤳�Ȥˤ�äƵ��롣*/
  /* Determine if button press was within pointer region - we 
     do this by computing the parallel and perpendicular distance of
     the point where the mouse was pressed from the line passing through
     the pointer */
  
  dx = event->x - widget->allocation.width / 2;
  dy = widget->allocation.height / 2 - event->y;
  
  s = sin(dial->angle);
  c = cos(dial->angle);
  
  d_parallel = s*dy + c*dx;
  d_perpendicular = fabs(s*dx - c*dy);
  
  if (!dial->button &&
      (d_perpendicular < dial->pointer_width/2) &&
      (d_parallel > - dial->pointer_width))
    {
      gtk_grab_add (widget);

      dial->button = event->button;

      gtk_dial_update_mouse (dial, event->x, event->y);
    }

  return FALSE;
}

static gint
gtk_dial_button_release (GtkWidget      *widget,
                          GdkEventButton *event)
{
  GtkDial *dial;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_DIAL (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  dial = GTK_DIAL (widget);

  if (dial->button == event->button)
    {
      gtk_grab_remove (widget);

      dial->button = 0;

      if (dial->policy == GTK_UPDATE_DELAYED)
        gtk_timeout_remove (dial->timer);
      
      if ((dial->policy != GTK_UPDATE_CONTINUOUS) &&
          (dial->old_value != dial->adjustment->value))
        gtk_signal_emit_by_name (GTK_OBJECT (dial->adjustment), "value_changed");
    }

  return FALSE;
}

static gint
gtk_dial_motion_notify (GtkWidget      *widget,
                         GdkEventMotion *event)
{
  GtkDial *dial;
  GdkModifierType mods;
  gint x, y, mask;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_DIAL (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  dial = GTK_DIAL (widget);

  if (dial->button != 0)
    {
      x = event->x;
      y = event->y;

      if (event->is_hint || (event->window != widget->window))
        gdk_window_get_pointer (widget->window, &x, &y, &mods);

      switch (dial->button)
        {
        case 1:
          mask = GDK_BUTTON1_MASK;
          break;
        case 2:
          mask = GDK_BUTTON2_MASK;
          break;
        case 3:
          mask = GDK_BUTTON3_MASK;
          break;
        default:
          mask = 0;
          break;
        }

      if (mods & mask)
        gtk_dial_update_mouse (dial, x,y);
    }

  return FALSE;
}

static gint
gtk_dial_timer (GtkDial *dial)
{
  g_return_val_if_fail (dial != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_DIAL (dial), FALSE);

  if (dial->policy == GTK_UPDATE_DELAYED)
    gtk_signal_emit_by_name (GTK_OBJECT (dial->adjustment), "value_changed");

  return FALSE;
}

static void
gtk_dial_update_mouse (GtkDial *dial, gint x, gint y)
{
  gint xc, yc;
  gfloat old_value;

  g_return_if_fail (dial != NULL);
  g_return_if_fail (GTK_IS_DIAL (dial));

  xc = GTK_WIDGET(dial)->allocation.width / 2;
  yc = GTK_WIDGET(dial)->allocation.height / 2;

  old_value = dial->adjustment->value;
  dial->angle = atan2(yc-y, x-xc);

  if (dial->angle < -M_PI/2.)
    dial->angle += 2*M_PI;

  if (dial->angle < -M_PI/6)
    dial->angle = -M_PI/6;

  if (dial->angle > 7.*M_PI/6.)
    dial->angle = 7.*M_PI/6.;

  dial->adjustment->value = dial->adjustment->lower + (7.*M_PI/6 - dial->angle) *
    (dial->adjustment->upper - dial->adjustment->lower) / (4.*M_PI/3.);

  if (dial->adjustment->value != old_value)
    {
      if (dial->policy == GTK_UPDATE_CONTINUOUS)
        {
          gtk_signal_emit_by_name (GTK_OBJECT (dial->adjustment), "value_changed");
        }
      else
        {
          gtk_widget_draw (GTK_WIDGET(dial), NULL);

          if (dial->policy == GTK_UPDATE_DELAYED)
            {
              if (dial->timer)
                gtk_timeout_remove (dial->timer);

              dial->timer = gtk_timeout_add (SCROLL_DELAY_LENGTH,
                                             (GtkFunction) gtk_dial_timer,
                                             (gpointer) dial);
            }
        }
    }
}

static void
gtk_dial_update (GtkDial *dial)
{
  gfloat new_value;
  
  g_return_if_fail (dial != NULL);
  g_return_if_fail (GTK_IS_DIAL (dial));

  new_value = dial->adjustment->value;
  
  if (new_value < dial->adjustment->lower)
    new_value = dial->adjustment->lower;

  if (new_value > dial->adjustment->upper)
    new_value = dial->adjustment->upper;

  if (new_value != dial->adjustment->value)
    {
      dial->adjustment->value = new_value;
      gtk_signal_emit_by_name (GTK_OBJECT (dial->adjustment), "value_changed");
    }

  dial->angle = 7.*M_PI/6. - (new_value - dial->adjustment->lower) * 4.*M_PI/3. /
    (dial->adjustment->upper - dial->adjustment->lower);

  gtk_widget_draw (GTK_WIDGET(dial), NULL);
}

static void
gtk_dial_adjustment_changed (GtkAdjustment *adjustment,
                              gpointer       data)
{
  GtkDial *dial;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);

  dial = GTK_DIAL (data);

  if ((dial->old_value != adjustment->value) ||
      (dial->old_lower != adjustment->lower) ||
      (dial->old_upper != adjustment->upper))
    {
      gtk_dial_update (dial);

      dial->old_value = adjustment->value;
      dial->old_lower = adjustment->lower;
      dial->old_upper = adjustment->upper;
    }
}

static void
gtk_dial_adjustment_value_changed (GtkAdjustment *adjustment,
                                    gpointer       data)
{
  GtkDial *dial;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);

  dial = GTK_DIAL (data);

  if (dial->old_value != adjustment->value)
    {
      gtk_dial_update (dial);

      dial->old_value = adjustment->value;
    }
}
/* example-end */
