/* example-start tictactoe tictactoe.h */

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
