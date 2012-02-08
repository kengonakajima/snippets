/*
 */

typedef struct _s_node
{
    
#define MAX_CHILD 16
    int child_num;
    struct _s_node *child[MAX_CHILD];
#define MAX_MEMBER 10
    int member_num;
    int member[MAX_MEMBER];
} s_node;
