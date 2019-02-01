#include <stdio.h>
#include <curl/curl.h>
#include <pthread.h>
#include <unistd.h>

static void *pull_one(void*arg) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "curl init fail\n");
        return NULL;
    }

    sleep(10);
    curl_easy_setopt(curl, CURLOPT_URL, "https://hajimarium-server.test-tube.net/cl.js");
    //        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

    res = curl_easy_perform(curl);
    if(res!=CURLE_OK) {
        fprintf(stderr,"perform fail:%s\n", curl_easy_strerror(res));
        return NULL;
    }

    curl_easy_cleanup(curl);

    curl_global_cleanup();

    fprintf(stderr,"pull_one done\n");
    return NULL;
}

int main() {
    //    pull_one();

    pthread_t tid;
    int err=pthread_create(&tid,NULL, pull_one, NULL );
    if(err!=0) {
        fprintf(stderr, "pthcr error\n");
        return 1;
    }
    while(1) {
        usleep(100*1000);
        fprintf(stderr,".");

        // no in macos
        //        void *retval;
        //        int r=pthread_tryjoin_np(tid, &retval);
        //        fprintf(stderr, "r:%d\n",r);

    }
    err=pthread_join(tid,NULL);
    fprintf(stderr, "done\n");
    return 0;
}
