#import <CoreFoundation/CoreFoundation.h>


int main() {
    CFLocaleRef cflocale = CFLocaleCopyCurrent();
    CFStringRef value = (CFStringRef)CFLocaleGetValue(cflocale, kCFLocaleLanguageCode);
    printf("lang:%s\n",CFStringGetCStringPtr(value, kCFStringEncodingUTF8) );
    CFRelease(cflocale);
    return 0;    
}
