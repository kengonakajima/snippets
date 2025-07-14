#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <duktape.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#include <SystemConfiguration/SystemConfiguration.h>
#endif

typedef struct {
    char *data;
    size_t size;
} pac_response_t;

static size_t write_callback(void *contents, size_t size, size_t nmemb, pac_response_t *response) {
    size_t realsize = size * nmemb;
    char *ptr = realloc(response->data, response->size + realsize + 1);
    if (!ptr) return 0;
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, realsize);
    response->size += realsize;
    response->data[response->size] = 0;
    return realsize;
}

char* download_pac_script(const char* pac_url) {
    CURL *curl;
    CURLcode res;
    pac_response_t response = {0};
    
    curl = curl_easy_init();
    if (!curl) return NULL;
    
    curl_easy_setopt(curl, CURLOPT_URL, pac_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        free(response.data);
        return NULL;
    }
    
    return response.data;
}

char* execute_pac_script(const char* pac_script, const char* url, const char* host) {
    printf("PAC Script downloaded (%zu bytes)\n", strlen(pac_script));
    printf("PAC Script content:\n%s\n", pac_script);
    
    duk_context *ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create JavaScript context\n");
        return NULL;
    }
    
    duk_push_string(ctx, "function shExpMatch(str, pattern) {\n"
                         "  var re = pattern.replace(/\\./g, '\\\\.')\n"
                         "                  .replace(/\\*/g, '.*')\n"
                         "                  .replace(/\\?/g, '.');\n"
                         "  return new RegExp('^' + re + '$').test(str);\n"
                         "}\n");
    
    if (duk_peval(ctx) != 0) {
        printf("Error evaluating shExpMatch: %s\n", duk_safe_to_string(ctx, -1));
        duk_destroy_heap(ctx);
        return NULL;
    }
    duk_pop(ctx);
    
    if (duk_peval_string(ctx, pac_script) != 0) {
        printf("Error evaluating PAC script: %s\n", duk_safe_to_string(ctx, -1));
        duk_destroy_heap(ctx);
        return NULL;
    }
    duk_pop(ctx);
    
    duk_get_global_string(ctx, "FindProxyForURL");
    duk_push_string(ctx, url);
    duk_push_string(ctx, host);
    
    if (duk_pcall(ctx, 2) != 0) {
        printf("Error calling FindProxyForURL: %s\n", duk_safe_to_string(ctx, -1));
        duk_destroy_heap(ctx);
        return NULL;
    }
    
    const char* result = duk_safe_to_string(ctx, -1);
    char* proxy_result = malloc(strlen(result) + 1);
    strcpy(proxy_result, result);
    
    printf("FindProxyForURL('%s', '%s') = %s\n", url, host, proxy_result);
    
    duk_destroy_heap(ctx);
    return proxy_result;
}

void print_proxy_settings(const char* url) {
#ifdef __APPLE__
    CFStringRef url_string = CFStringCreateWithCString(NULL, url, kCFStringEncodingUTF8);
    CFURLRef cf_url = CFURLCreateWithString(NULL, url_string, NULL);
    
    CFDictionaryRef proxy_settings = CFNetworkCopySystemProxySettings();
    CFArrayRef proxies = CFNetworkCopyProxiesForURL(cf_url, proxy_settings);
    
    CFIndex count = CFArrayGetCount(proxies);
    printf("Proxy settings for %s:\n", url);
    
    char* pac_url = NULL;
    
    CFStringRef global_pac_url = CFDictionaryGetValue(proxy_settings, CFSTR("ProxyAutoConfigURLString"));
    if (global_pac_url && CFGetTypeID(global_pac_url) == CFStringGetTypeID()) {
        pac_url = malloc(512);
        if (CFStringGetCString(global_pac_url, pac_url, 512, kCFStringEncodingUTF8)) {
            printf("Global PAC URL: %s\n", pac_url);
        } else {
            free(pac_url);
            pac_url = NULL;
        }
    }
    
    for (CFIndex i = 0; i < count; i++) {
        CFDictionaryRef proxy = CFArrayGetValueAtIndex(proxies, i);
        
        CFStringRef proxy_type = CFDictionaryGetValue(proxy, kCFProxyTypeKey);
        CFStringRef proxy_host = CFDictionaryGetValue(proxy, kCFProxyHostNameKey);
        CFNumberRef proxy_port = CFDictionaryGetValue(proxy, kCFProxyPortNumberKey);
        
        char type_buffer[256] = {0};
        char host_buffer[256] = {0};
        int port = 0;
        
        if (proxy_type) {
            CFStringGetCString(proxy_type, type_buffer, sizeof(type_buffer), kCFStringEncodingUTF8);
        }
        if (proxy_host) {
            CFStringGetCString(proxy_host, host_buffer, sizeof(host_buffer), kCFStringEncodingUTF8);
        }
        if (proxy_port) {
            CFNumberGetValue(proxy_port, kCFNumberIntType, &port);
        }
        
        printf("Type: %s", type_buffer);
        if (strlen(host_buffer) > 0) {
            printf(", Host: %s", host_buffer);
            if (port > 0) {
                printf(":%d", port);
            }
        }
        
        CFStringRef auto_config_url = CFDictionaryGetValue(proxy, kCFProxyAutoConfigurationURLKey);
        if (auto_config_url && CFGetTypeID(auto_config_url) == CFStringGetTypeID()) {
            char local_pac_url[512] = {0};
            if (CFStringGetCString(auto_config_url, local_pac_url, sizeof(local_pac_url), kCFStringEncodingUTF8)) {
                printf(", PAC URL: %s", local_pac_url);
                if (!pac_url) {
                    pac_url = malloc(512);
                    strcpy(pac_url, local_pac_url);
                }
            }
        }
        printf("\n");
    }
    
    if (pac_url) {
        printf("\nDownloading PAC script...\n");
        char* pac_script = download_pac_script(pac_url);
        if (pac_script) {
            CFStringRef host_string = CFURLCopyHostName(cf_url);
            char host_buffer[256] = {0};
            if (host_string) {
                CFStringGetCString(host_string, host_buffer, sizeof(host_buffer), kCFStringEncodingUTF8);
                CFRelease(host_string);
            }
            char* proxy_result = execute_pac_script(pac_script, url, host_buffer);
            if (proxy_result) {
                printf("Final proxy setting: %s\n", proxy_result);
                free(proxy_result);
            }
            free(pac_script);
        } else {
            printf("Failed to download PAC script\n");
        }
        free(pac_url);
    }
    
    CFRelease(proxies);
    CFRelease(proxy_settings);
    CFRelease(cf_url);
    CFRelease(url_string);
#else
    printf("macOS proxy detection not available on this platform\n");
#endif
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <URL>\n", argv[0]);
        return 1;
    }
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    print_proxy_settings(argv[1]);
    curl_global_cleanup();
    return 0;
}