#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proxy.h>

void print_usage(const char *program_name) {
    printf("使用方法: %s <URL>\n", program_name);
    printf("例: %s http://example.com\n", program_name);
}

int main(int argc, char *argv[]) {
    // コマンドライン引数のチェック
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *url = argv[1];
    
    // libproxyのプロキシファクトリを初期化
    pxProxyFactory *factory = px_proxy_factory_new();
    if (!factory) {
        fprintf(stderr, "プロキシファクトリの初期化に失敗しました\n");
        return 1;
    }
    
    // URLに対するプロキシ情報を取得
    char **proxies = px_proxy_factory_get_proxies(factory, url);
    if (!proxies) {
        fprintf(stderr, "プロキシ情報の取得に失敗しました\n");
        px_proxy_factory_free(factory);
        return 1;
    }
    
    // 取得したプロキシ情報を表示
    printf("URL: %s のプロキシ設定:\n", url);
    
    if (!proxies[0] || strcmp(proxies[0], "direct://") == 0) {
        printf("直接接続（プロキシなし）\n");
    } else {
        for (int i = 0; proxies[i]; i++) {
            printf("プロキシ %d: %s\n", i + 1, proxies[i]);
        }
    }
    
    // メモリを解放
    for (int i = 0; proxies[i]; i++) {
        free(proxies[i]);
    }
    free(proxies);
    px_proxy_factory_free(factory);
    
    return 0;
}