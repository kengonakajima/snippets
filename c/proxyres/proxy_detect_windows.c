#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <duktape.h>

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#include <wininet.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wininet.lib")
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
    printf("Debug: Attempting to download PAC script from: %s\n", pac_url);
    
    __try {
        CURL *curl;
        CURLcode res;
        pac_response_t response = {0};
        
        printf("Debug: Initializing curl session...\n");
        curl = curl_easy_init();
        if (!curl) {
            printf("Debug: curl_easy_init failed\n");
            return NULL;
        }
        
        printf("Debug: Setting curl options...\n");
        curl_easy_setopt(curl, CURLOPT_URL, pac_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        
        printf("Debug: Performing curl request...\n");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            printf("Debug: curl_easy_perform failed with code: %d\n", res);
            free(response.data);
            return NULL;
        }
        
        printf("Debug: PAC script downloaded successfully\n");
        return response.data;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("Exception during PAC script download - curl may not be available\n");
        return NULL;
    }
}

char* execute_pac_script(const char* pac_script, const char* url, const char* host) {
    printf("PAC Script downloaded (%zu bytes)\n", strlen(pac_script));
    printf("PAC Script content:\n%s\n", pac_script);
    
    __try {
        printf("Debug: Creating JavaScript context...\n");
        duk_context *ctx = duk_create_heap_default();
        if (!ctx) {
            printf("Failed to create JavaScript context\n");
            return NULL;
        }
        printf("Debug: JavaScript context created successfully\n");
    
    // Add shExpMatch function
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
    
    // Add other PAC helper functions
    duk_push_string(ctx, 
        "function isPlainHostName(host) {\n"
        "  return host.indexOf('.') < 0;\n"
        "}\n"
        "function dnsDomainIs(host, domain) {\n"
        "  return host.length >= domain.length && host.substring(host.length - domain.length) === domain;\n"
        "}\n"
        "function localHostOrDomainIs(host, hostdom) {\n"
        "  return host === hostdom || hostdom.lastIndexOf(host + '.', 0) === 0;\n"
        "}\n"
        "function isResolvable(host) {\n"
        "  return true;\n"
        "}\n"
        "function isInNet(host, pattern, mask) {\n"
        "  return false;\n"
        "}\n"
        "function dnsResolve(host) {\n"
        "  return '127.0.0.1';\n"
        "}\n"
        "function myIpAddress() {\n"
        "  return '127.0.0.1';\n"
        "}\n"
        "function dnsDomainLevels(host) {\n"
        "  return host.split('.').length - 1;\n"
        "}\n"
        "function weekdayRange(wd1, wd2, gmt) {\n"
        "  return true;\n"
        "}\n"
        "function dateRange() {\n"
        "  return true;\n"
        "}\n"
        "function timeRange() {\n"
        "  return true;\n"
        "}\n");
    
    if (duk_peval(ctx) != 0) {
        printf("Error evaluating PAC helper functions: %s\n", duk_safe_to_string(ctx, -1));
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
    __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("Exception during PAC script execution - duktape may not be available\n");
        return NULL;
    }
}

#ifdef _WIN32
char* wchar_to_char(const WCHAR* wstr) {
    if (!wstr) return NULL;
    
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return NULL;
    
    char* str = malloc(len);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    return str;
}

WCHAR* char_to_wchar(const char* str) {
    if (!str) return NULL;
    
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (len <= 0) return NULL;
    
    WCHAR* wstr = malloc(len * sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);
    return wstr;
}

void print_proxy_settings(const char* url) {
    printf("Proxy settings for %s:\n", url);
    printf("Debug: Starting proxy detection...\n");
    
    HINTERNET hSession = NULL;
    WINHTTP_PROXY_INFO proxyInfo = {0};
    WINHTTP_AUTOPROXY_OPTIONS autoProxyOptions = {0};
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = {0};
    
    printf("Debug: Initializing WinHTTP...\n");
    // Initialize WinHTTP
    hSession = WinHttpOpen(L"ProxyDetect/1.0", 
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS,
                          0);
    
    if (!hSession) {
        DWORD error = GetLastError();
        printf("Failed to initialize WinHTTP (Error: %lu)\n", error);
        return;
    }
    printf("Debug: WinHTTP initialized successfully\n");
    
    // Get IE proxy settings
    printf("Debug: Getting IE proxy configuration...\n");
    if (WinHttpGetIEProxyConfigForCurrentUser(&ieProxyConfig)) {
        printf("Debug: IE proxy configuration retrieved successfully\n");
        printf("IE Proxy Configuration:\n");
        
        if (ieProxyConfig.fAutoDetect) {
            printf("  Auto-detect: Enabled\n");
        }
        
        if (ieProxyConfig.lpszAutoConfigUrl) {
            char* pac_url = wchar_to_char(ieProxyConfig.lpszAutoConfigUrl);
            printf("  PAC URL: %s\n", pac_url);
            
            // Try to download and execute PAC script
            if (pac_url) {
                printf("\nDownloading PAC script...\n");
                char* pac_script = download_pac_script(pac_url);
                if (pac_script) {
                    // Extract host from URL
                    char* host = strdup(url);
                    char* scheme_end = strstr(host, "://");
                    if (scheme_end) {
                        char* host_start = scheme_end + 3;
                        char* path_start = strchr(host_start, '/');
                        if (path_start) *path_start = '\0';
                        char* port_start = strchr(host_start, ':');
                        if (port_start) *port_start = '\0';
                        
                        char* proxy_result = execute_pac_script(pac_script, url, host_start);
                        if (proxy_result) {
                            printf("Final proxy setting: %s\n", proxy_result);
                            free(proxy_result);
                        }
                    }
                    free(pac_script);
                    free(host);
                } else {
                    printf("Failed to download PAC script\n");
                }
                free(pac_url);
            }
        }
        
        if (ieProxyConfig.lpszProxy) {
            char* proxy = wchar_to_char(ieProxyConfig.lpszProxy);
            printf("  Manual proxy: %s\n", proxy);
            free(proxy);
        }
        
        if (ieProxyConfig.lpszProxyBypass) {
            char* bypass = wchar_to_char(ieProxyConfig.lpszProxyBypass);
            printf("  Proxy bypass: %s\n", bypass);
            free(bypass);
        }
    } else {
        DWORD error = GetLastError();
        printf("Failed to get IE proxy configuration (Error: %lu)\n", error);
    }
    
    // Try WinHTTP auto-proxy detection
    printf("\nDebug: Trying WinHTTP auto-proxy detection...\n");
    WCHAR* wurl = char_to_wchar(url);
    if (wurl) {
        printf("Debug: URL converted to wide char successfully\n");
        autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
        autoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
        autoProxyOptions.fAutoLogonIfChallenged = TRUE;
        
        if (WinHttpGetProxyForUrl(hSession, wurl, &autoProxyOptions, &proxyInfo)) {
            printf("\nWinHTTP Auto-detected proxy:\n");
            
            switch (proxyInfo.dwAccessType) {
                case WINHTTP_ACCESS_TYPE_NO_PROXY:
                    printf("  No proxy\n");
                    break;
                case WINHTTP_ACCESS_TYPE_DEFAULT_PROXY:
                    printf("  Default proxy\n");
                    break;
                case WINHTTP_ACCESS_TYPE_NAMED_PROXY:
                    if (proxyInfo.lpszProxy) {
                        char* proxy = wchar_to_char(proxyInfo.lpszProxy);
                        printf("  Proxy: %s\n", proxy);
                        free(proxy);
                    }
                    if (proxyInfo.lpszProxyBypass) {
                        char* bypass = wchar_to_char(proxyInfo.lpszProxyBypass);
                        printf("  Bypass: %s\n", bypass);
                        free(bypass);
                    }
                    break;
            }
        } else {
            DWORD error = GetLastError();
            printf("WinHTTP auto-proxy detection failed (Error: %lu)\n", error);
        }
        
        free(wurl);
    }
    
    // Cleanup
    if (proxyInfo.lpszProxy) GlobalFree(proxyInfo.lpszProxy);
    if (proxyInfo.lpszProxyBypass) GlobalFree(proxyInfo.lpszProxyBypass);
    if (ieProxyConfig.lpszAutoConfigUrl) GlobalFree(ieProxyConfig.lpszAutoConfigUrl);
    if (ieProxyConfig.lpszProxy) GlobalFree(ieProxyConfig.lpszProxy);
    if (ieProxyConfig.lpszProxyBypass) GlobalFree(ieProxyConfig.lpszProxyBypass);
    if (hSession) WinHttpCloseHandle(hSession);
}
#else
void print_proxy_settings(const char* url) {
    printf("Windows proxy detection not available on this platform\n");
}
#endif

int main(int argc, char* argv[]) {
    printf("Proxy Detection Tool for Windows\n");
    printf("================================\n\n");
    
    if (argc != 2) {
        printf("Usage: %s <URL>\n", argv[0]);
        printf("Example: %s https://www.google.com\n", argv[0]);
        return 1;
    }
    
    printf("Debug: Program started successfully\n");
    printf("Debug: Checking curl availability...\n");
    
    // Try to initialize curl safely
    __try {
        printf("Debug: Attempting curl initialization...\n");
        CURLcode curl_init_result = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (curl_init_result != CURLE_OK) {
            printf("Failed to initialize curl: %d\n", curl_init_result);
            printf("Continuing without curl support...\n");
        } else {
            printf("Debug: curl initialized successfully\n");
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("Exception during curl initialization - continuing without curl\n");
    }
    
    printf("Detecting proxy settings for: %s\n\n", argv[1]);
    print_proxy_settings(argv[1]);
    
    __try {
        curl_global_cleanup();
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        // Ignore cleanup exceptions
    }
    
    printf("\nDone.\n");
    return 0;
}
