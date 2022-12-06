/*
 */


#include <stdio.h>
#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    printf("write_data: size:%zu nmemb:%zu\n",size,nmemb);
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

size_t read_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    /*
  FILE *readhere = (FILE *)userdata;
  curl_off_t nread;
 
  // copy as much data as possible into the 'ptr' buffer, but no more than     'size' * 'nmemb' bytes! 
  size_t retcode = fread(ptr, size, nmemb, readhere);
 
  nread = (curl_off_t)retcode;


  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);
  */
      printf("read_callback: %zu %zu\n",size,nmemb);
      size_t tot=size*nmemb;
      for(int i=0;i<10;i++) ptr[i]='A';
      return 10;
}

static int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr)
{
  // print the debug information to stdout
  fwrite(data, size, 1, stdout);
  return 0;
}

int main(int argc, char *argv[])
{
  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* set URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://127.0.0.1:22222/");

  /* enable server-sent events */
  //  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, "Accept: text/event-stream");

  /* no progress meter please */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  /* send all data to this function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

  /* we want the headers be written to this file handle */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, stdout);

  /* set the event receiver function */
  curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, read_callback);

  curl_easy_setopt(curl_handle, CURLOPT_DEBUGFUNCTION, debug_callback);
  //  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);

  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

  // disable peer verification
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
  //  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 0L);

  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
  
  

  /* get it! */
  curl_easy_perform(curl_handle);

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  return 0;
}
