/* example-start tictactoe tictactoe.h */

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
#ifndef __TICTACTOE_H__
#define __TICTACTOE_H__


#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TICTACTOE(obj)          GTK_CHECK_CAST (obj, tictactoe_get_type (), Tictactoe)
#define TICTACTOE_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, tictactoe_get_type (), TictactoeClass)
#define IS_TICTACTOE(obj)       GTK_CHECK_TYPE (obj, tictactoe_get_type ())


typedef struct _Tictactoe       Tictactoe;
typedef struct _TictactoeClass  TictactoeClass;

struct _Tictactoe
{
  GtkVBox vbox;
  
  GtkWidget *buttons[3][3];
};

struct _TictactoeClass
{
  GtkVBoxClass parent_class;

  void (* tictactoe) (Tictactoe *ttt);
};

guint          tictactoe_get_type        (void);
GtkWidget*     tictactoe_new             (void);
void           tictactoe_clear           (Tictactoe *ttt);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TICTACTOE_H__ */

/* example-end */
