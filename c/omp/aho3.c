int main (int argc, char *argv[]) {
        int id, nthreads;
          #pragma omp parallel private(id)
          {
                  id = omp_get_thread_num();
                  printf("Hello World from thread %d\n", id);
                      #pragma omp barrier
                  if ( id == 0 ) {
                          nthreads = omp_get_num_threads();
                          printf("There are %d threads\n",nthreads);
                  }
          }
          return 0;
}
