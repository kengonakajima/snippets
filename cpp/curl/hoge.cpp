#include <stdio.h>
#include <curl/curl.h>

int main() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "curl init fail\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://hajimarium-server.test-tube.net/cl.js");
    //        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

    res = curl_easy_perform(curl);
    if(res!=CURLE_OK) {
        fprintf(stderr,"perform fail:%s\n", curl_easy_strerror(res));
        return 1;
    }

    curl_easy_cleanup(curl);

    curl_global_cleanup();
    return 0;
}
