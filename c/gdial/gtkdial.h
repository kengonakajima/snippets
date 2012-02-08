/* example-start gtkdial gtkdial.h */

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
#ifndef __GTK_DIAL_H__
#define __GTK_DIAL_H__


#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_DIAL(obj)          GTK_CHECK_CAST (obj, gtk_dial_get_type (), GtkDial)
#define GTK_DIAL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_dial_get_type (), GtkDialClass)
#define GTK_IS_DIAL(obj)       GTK_CHECK_TYPE (obj, gtk_dial_get_type ())


typedef struct _GtkDial        GtkDial;
typedef struct _GtkDialClass   GtkDialClass;

struct _GtkDial
{
  GtkWidget widget;

  /* ���åץǡ��ȥݥꥷ�� (GTK_UPDATE_[CONTINUOUS/DELAYED/DISCONTINUOUS]) */
  guint policy : 2;

  /* ���߲����줿���줿�ܥ��󡢲�����Ƥ��ʤ���� 0 */
  guint8 button;

  /* ������륳��ݡ��ͥ�Ȥ����� */
  gint radius;
  gint pointer_width;

  /* ���åץǡ��ȥ����ޡ��� ID������ʤ���� 0 */
  guint32 timer;

  /* ���ߤγ��� */
  gfloat angle;

  /* �����Ѳ����������ΤäƤ�������ˡ������㥹�ȥ��Ȥ���θŤ��ͤ�
     ��Ǽ */
  gfloat old_value;
  gfloat old_lower;
  gfloat old_upper;

  /* ���Υ������Τ���˥ǡ������Ǽ���륢���㥹�ȥ��� ���֥������� */
  GtkAdjustment *adjustment;
};

struct _GtkDialClass
{
  GtkWidgetClass parent_class;
};


GtkWidget*     gtk_dial_new                    (GtkAdjustment *adjustment);
guint          gtk_dial_get_type               (void);
GtkAdjustment* gtk_dial_get_adjustment         (GtkDial      *dial);
void           gtk_dial_set_update_policy      (GtkDial      *dial,
                                                GtkUpdateType  policy);

void           gtk_dial_set_adjustment         (GtkDial      *dial,
                                                GtkAdjustment *adjustment);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_DIAL_H__ */
/* example-end */
