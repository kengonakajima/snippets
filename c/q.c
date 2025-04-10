#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rel_err {
    char var_a[50];
    char var_b[50];
    double rel_err;
};

// 比較関数
int compare(const void *a, const void *b) {
    struct rel_err *ra = (struct rel_err *)a;
    struct rel_err *rb = (struct rel_err *)b;
    if (ra->rel_err < rb->rel_err) return -1;
    if (ra->rel_err > rb->rel_err) return 1;
    return 0;
}

int main() {
    struct rel_err data[] = {
        {"var1", "var2", 0.5},
        {"var3", "var4", 0.1},
        {"var5", "var6", 0.3}
    };
    int n = sizeof(data) / sizeof(data[0]);

    // qsortを使用してソート
    qsort(data, n, sizeof(struct rel_err), compare);

    // ソート結果の表示
    for (int i = 0; i < n; i++) {
        printf("%s, %s: %f\n", data[i].var_a, data[i].var_b, data[i].rel_err);
    }

    return 0;
}
