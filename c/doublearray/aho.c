/* version-0.1 2005/07/10 
 *
 * related works
 *
 * Darts
 *  http://www2.chasen.org/~taku/software/darts/
 * datrie
 *  http://linux.thai.net/~thep/datrie/datrie.html
 */
/*
  Copyright (c) 2005 yusuke@cherubim.icw.co.jp

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  3. Neither the name of authors nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>

/* decl begin */
struct da;
void da_dump(struct da *);
/* decl end */

struct delm {
  int base;
  int check;
};

struct da {
  int size;
  struct delm *arr;
};

struct resolve_elm {
  int c;
  int base;
};

struct resolve_stat {
  int nr;
  struct resolve_elm elm[256];
};

static void
ensure_array(struct da *da, int size)
{
  if (size > da->size) {
    int i;
	int s = size * sizeof(struct delm);
//	printf( "realloc.size:%d\n", s );
    da->arr = realloc(da->arr, s );
    for (i = da->size; i < size; i++) {
      da->arr[i].base = 0;
      da->arr[i].check = 0;
    }
    da->size = size;
  }
}

static void
set_base(struct da *da, int idx, int base)
{
	printf( "set_base: idx:%d base: %d\n", idx, base );
  ensure_array(da, idx + 1);
  da->arr[idx].base = base;
}

static int
get_base(struct da *da, int idx)
{
  ensure_array(da, idx + 1);
  return da->arr[idx].base;
}

static void
set_check(struct da *da, int idx, int check)
{
  ensure_array(da, idx + 1);
  da->arr[idx].check = check;
}

static int
get_check(struct da *da, int idx)
{
  ensure_array(da, idx + 1);
  return da->arr[idx].check;
}

int
da_find(struct da *da, const unsigned char *str, int len)
{
  int i, cur = 1;
  for (i = 0; i < len; i++) {
    int base = get_base(da, cur);
    unsigned char c = str[i];
    if (get_check(da, cur + base + c) != cur) {
      /* not match */
      return 0;
    }
    cur = cur + base + c;
  }
  return cur;
}

static void
push_child(struct resolve_stat *rs, int c, int orig_base)
{
  int n = rs->nr;
  rs->nr++;
  /*printf("c=%c(%d), orig_base=%d\n", c, c, orig_base);*/
  rs->elm[n].c = c;
  rs->elm[n].base = orig_base;
}

static void
collect_child(struct da *da, struct resolve_stat *rs,
	      int parent, unsigned char c)
{
  int i;
  int base = get_base(da, parent);
  rs->nr = 0;
  ensure_array(da, base + 256);
  for (i = 0; i < 256; i++) {
    int idx = parent + base + i;
    if (i == c) {
      push_child(rs, c, 1);
    } else if (get_check(da, idx) == parent) {
      /* escape */
      /*printf("idx=%d,base=%d, parent=%d,i=%d\n",
	idx, base, parent, i);*/
      push_child(rs, i, get_base(da, idx));
      /* clear */
      set_base(da, idx, 0);
      set_check(da, idx, 0);
    }
  }
}

static int
find_place(struct da *da, struct resolve_stat *rs)
{
  int left, i;
  for (left = 2; ; left ++) {
    int conflict = 0;
    ensure_array(da, left + 256);
    for (i = 0; i < rs->nr; i++) {
      if (get_check(da, rs->elm[i].c + left) > 0) {
	conflict = 1;
      }
    }
    if (!conflict) {
      return left;
    }
  }
  return 0;
}

static void
move_children(struct da *da, struct resolve_stat *rs,
	      int parent, int left)
{
  int i;
  int orig_base = get_base(da, parent);
  int move;
  move = parent + orig_base - left;
  /*printf("parent=%d, nr=%d, left=%d, orig_base=%d\n",
    parent, rs->nr, left, orig_base);*/
  set_base(da, parent, left - parent);
  /* put moved children */
  for (i = 0; i < rs->nr; i++) {
    int idx = left + rs->elm[i].c;
    set_check(da, idx, parent);
    set_base(da, idx, rs->elm[i].base + move);
  }
  /* trim grand children */
  for (i = 0; i < rs->nr; i++) {
    int orig_left, old_child;
    int j;
    /**/
    orig_left = parent + orig_base + rs->elm[i].base + rs->elm[i].c;
    old_child = parent + orig_base + rs->elm[i].c;
    /*printf("parent=%d, old_child=%d, %d\n", parent, old_child, move);*/
    for (j = 0; j < 256; j++) {
      int orig_check = get_check(da, orig_left + j);
      if (get_check(da, orig_left + j) == old_child &&
	  get_check(da, orig_check) == 0) {
	int new_check;
	new_check = get_check(da, orig_left +j) - move;
	/*printf("rewrite check[%d] %d to %d (%d)\n",
	       orig_left + j,
	       get_check(da, orig_left + j), new_check,
	       get_check(da, orig_check));*/
	set_check(da, orig_left + j, new_check);
      }
    }
  }
}

static int
resolve_conflict(struct da *da, int parent, unsigned char c)
{
  int left;
  struct resolve_stat rs;
  /* collect */
  collect_child(da, &rs, parent, c);
  /* find */
  left = find_place(da, &rs);
  /*printf("left=%d\n", left);*/
  /* move */
  move_children(da, &rs, parent, left);
  return left + c;
}

static int
do_add(struct da *da, int idx, unsigned char c)
{
  int base = get_base(da, idx);
  int next = idx + base + c;
  int cur_check = get_check(da, next);
  int new_base;
  if (cur_check > 0) {
    printf("conflict(%d,%c)\n", idx, c);
    return resolve_conflict(da, idx, c);
  }
  set_check(da, next, idx);
  /**/
  new_base = 1;
  if (idx == 1) {
    new_base = 256 - next;
  }
  set_base(da, next, new_base);
  return next;
}

void
da_add(struct da *da, const char *str, int len)
{
  int i;
  int prev_idx = 1;
  for (i = 0; i < len; i++) {
    int idx = da_find(da, (const unsigned char *)str, i + 1);
    if (idx == 0) {
      idx = do_add(da, prev_idx, str[i]);
    }
    prev_idx = idx;
  }
}

void
da_dump(struct da *da)
{
  int i;
  printf("da dump size=%d\n", da->size);
  for (i = 0; i < da->size; i++) {
    if (get_check(da, i)) {
      printf("%d: base=%d,check=%d\n", i,
	     get_base(da, i),
	     get_check(da, i));
    }
  }
  printf("da dump done\n");
}

struct da *
da_create(void)
{
  struct da *da;
  da = malloc(sizeof(struct da));
  da->size = 0;
  da->arr = NULL;
  set_base(da, 1, 1);
  set_check(da, 1, 1);
  return da;
}

static void
da_traverse(struct da *da, int idx, char *buf, int len)
{
  int i;
  int base = get_base(da, idx);
  for (i = 0; i < 256; i++) {
    if (get_check(da, idx + base + i) == idx) {
      buf[len] = i;
      if (i == 0) {
	printf("find %s\n", buf);
      } else {
	da_traverse(da, idx + base + i, buf, len + 1);
      }
    }
  }
}

int
main(int argc, char **argv)
{
  struct da *da = da_create();
  char buf[100];
  da_add(da, "bd", 3);
  da_add(da, "aa", 3);
  da_add(da, "ba", 3);
  da_dump(da);
  da_traverse(da, 1, buf, 0);
  return 0;
}
