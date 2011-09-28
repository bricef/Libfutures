#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libfutures.h"

gpointer my_function(gpointer data){
  int * my_int = malloc(sizeof(int));
  printf("[ENTER]: my_function\n");
  sleep(4);
  g_assert(data != NULL);
  *my_int = *((int*)data) * 6;
  return my_int;
}

void main(){
  int a = 4;
  int b = 89;
  char* A = "hello";
  char* B = "world";
  g_thread_init(NULL);
  Future * fu1 = NULL;
  Future * fu2 = NULL;
  Future * fu3 = NULL;
  Future * fu4 = NULL;
  gpointer r1 = NULL;
  gpointer r2 = NULL;
  gpointer r3 = NULL;
  gpointer r4 = NULL;

  printf("=== BEGIN FUTURES TEST ===\n");
  
  fu1 = lf_future(my_function, &a);
  fu2 = lf_future(my_function, &b);

  fu3 = lf_lazy_future(my_function, &a);
  fu4 = lf_lazy_future(my_function, &b);

  r1 = lf_value(fu1);
  printf("fu1 : %d\n", *((int*)r1) );
  
  r2 = lf_value(fu2);
  printf("fu2 : %d\n", *((int*)lf_value(fu2)) );
 
  r3 = lf_value(fu3);
  printf("fu3 : %d\n", *((int*)r3) );
 
  r4 = lf_value(fu4);
  printf("fu4 : %d\n", *((int*)lf_value(fu4)) );
  
  free(r1);
  free(r2);
  free(r3);
  free(r4);

  free_future(fu1);
  free_future(fu2);
  free_future(fu3);
  free_future(fu4);
}


