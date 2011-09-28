#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "libfutures.h"


/*----------------------------------------------------------
 * Internal callbacks
 *----------------------------------------------------------*/

void print_future(Future* future)
{
  g_assert(future != NULL);
  printf( "[future%s: %p\n" \
          "\tfunc:     %p\n" \
          "\tdata:     %p\n" \
          "\tresult:   %p\n" \
          "\tstarted:  %s\n" \
          "\tfinished: %s\n" \
          "\tthread:   %p\n" \
          "\terror:    %p\n" \
          "\tmutex:    %p\n" \
          "]\n",
          (future->lazy)?"(lazy)":"", future, future->func, future->data, future->result, 
          (future->started)?"T":"F", (future->finished)?"T":"F", 
          future->thread, future->error, future->mutex);
}

static gpointer future_wrapper(gpointer udata)
{
  Future * future = udata;
  future->result = future->func(future->data);
  future->finished = TRUE;
  return (gpointer)future->result;
}

static void start_future(Future * future)
{
  
  g_mutex_lock(future->mutex);
  if(!future->started){
    GThread * thread = NULL;
    // GError * err = malloc(sizeof(GError)); 
    thread = g_thread_create((GThreadFunc)future_wrapper, (gpointer)future, TRUE, NULL);
    future->started = TRUE;
    future->thread = thread;
    //  future->error = err;
  }
  g_mutex_unlock(future->mutex);
}


void free_future(Future * future)
{
  if(future->error != NULL){
    g_error_free(future->error);
  }
  if(future->mutex != NULL){
    g_mutex_free(future->mutex);
  }
  free(future);
}

/*----------------------------------------------------------
 * Library Functions 
 *----------------------------------------------------------*/

/**
 * Creates a Future. 
 * The result of the future is accessible via the lf_getfuture() call.
 * This Future is not lazy.
 */
Future * lf_future(lf_func func, gpointer data)
{
  Future * future = lf_lazy_future(func, data);
  future->lazy = FALSE;
  start_future(future);
  return future;
}

/**
 * Lazy Future
 * The Future returned will only be evaluated when lf_getfuture is called.
 */
Future * lf_lazy_future(lf_func func, gpointer data)
{
  if(func == NULL){
    return NULL;
  }
  Future * future = malloc(sizeof(Future));
  future->func = func;
  future->data = data;
  future->result = NULL;
  future->started = FALSE;
  future->finished = FALSE;
  future->lazy = TRUE;
  future->mutex = g_mutex_new();
  return future;
}


/**
 * Will block if the future has not terminated yet. (Could conceivably 
 * block indefinitely in case of a bad Future.)
 * 
 * Essentially, `lf_value(lf_future(func, data));` is a synchronous call to func(data).
 *
 * The future is freed and cannot be accessed once its value has been acquired.
 * 
 */
gpointer lf_value(Future * future)
{
  return lf_value_err(future, NULL);
}

/**
 * Will block if the future has not terminated yet.
 * Handles errors.
 */
gpointer lf_value_err(Future * future, GError * error)
{
  gpointer results = NULL;
  if(future == NULL){
    return NULL;
  }
  
  if(!future->started){
    start_future(future);
  }
  
  if(future->finished){
    results = future->result;
  } else {
//    printf("[future]: not finishsed yet, joining future's thread\n");
    g_thread_join(future->thread);
//    printf("[future]: results are %p\n", future->result);
    results = future->result;
  }
  
  if((error != NULL) && (future->error != NULL)){
    g_error_free(error);
    error = g_error_copy(future->error);
  }
  
  return results;
}


/**
 * Returns when one or more of the Futures in `futures` have finished.
 * Times out after timeout.
 */
int lf_select_futures(GList* futures, const struct timespec *timeout)
{
  /* TODO: Not implemented */
  return -1;
}
