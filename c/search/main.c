#include <stdio.h>
#include <search.h>
#include <string.h>
#include <malloc.h>

#if 0
struct info {        /* this is the info stored in the table */
    int age, room;   /* other than the key. */
};

#define NUM_EMPL    5000    /* # of elements in search table */

int main(void)
{
    char string_space[NUM_EMPL*20];    /* space to store strings */
    struct info info_space[NUM_EMPL];  /* space to store employee info*/
    char *str_ptr = string_space;      /* next space in string_space */
    struct info *info_ptr = info_space;/* next space in info_space */
    ENTRY item;
    ENTRY *found_item;    /* name to look for in table */
    char name_to_find[30];

    int i = 0;

    /* create table; no error checking is performed */
    (void) hcreate(NUM_EMPL);
    while (scanf("%s%d%d", str_ptr, &info_ptr->age,
           &info_ptr->room) != EOF && i++ < NUM_EMPL) {

        /* put information in structure, and structure in item */
        item.key = str_ptr;
        item.data = info_ptr;
        str_ptr += strlen(str_ptr) + 1;
        info_ptr++;

        /* put item into table */
        (void) hsearch(item, ENTER);
    }

    /* access table */
    item.key = name_to_find;
    while (scanf("%s", item.key) != EOF) {
        if ((found_item = hsearch(item, FIND)) != NULL) {

            /* if item is in the table */
            (void)printf("found %s, age = %d, room = %d\n",
                found_item->key,
                ((struct info *)found_item->data)->age,
                ((struct info *)found_item->data)->room);
        } else
            (void)printf("no such employee %s\n", name_to_find);
    }
    return 0;
}
#endif


struct info {        /* this is the info stored in the table */
    int age, room;   /* other than the key. */
};


int main(void)
{
    int i;
#define N 300000
    fprintf(stderr, "create\n");
    char **names = (char**)malloc( N * sizeof(char*));
    fprintf(stderr, "malloc\n");
    for(i=0;i<N;i++){
        names[i] = (char*)malloc(32);
    }
    fprintf(stderr, "create\n");    
    hcreate(N);
    fprintf(stderr, "insert\n");    
    for(i=0;i<N;i++){
        int r = random() % N;
        snprintf( names[i],32, "name_%d", r );
        struct info *infoptr = (struct info*)malloc(sizeof(struct info));
        infoptr->age = i;
        infoptr->room = i*2;
        ENTRY item;
        item.key = names[i];
        item.data = infoptr;
        hsearch( item, ENTER );
    }
    fprintf(stderr, "search\n");
    int totalFound=0;
    for(i=0;i<N;i++){
        int r = random() % N;
        char nm[32];
        snprintf(nm,sizeof(nm), "name_%d", r );
        ENTRY item;
        item.key = nm;
        ENTRY *found = hsearch(item,FIND);
        if(found){
            totalFound ++;
        }
    }
    fprintf(stderr, "totalFound:%d\n",totalFound);
    return 0;
}
