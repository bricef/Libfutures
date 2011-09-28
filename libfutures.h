/**
 * Explicit futures library. 
 *
 * - Care must be take to only send thread-safe functions into this library.
 * - Care must be taken to free the data structures passed into this library.
 * - Caching of futures' values is left to the caller. A Futures is undefined
 * once ls_value* is called on it.
 */

#include <glib.h>
#include <time.h>

typedef gpointer (* lf_func)(gpointer udata);

typedef struct 
{
  lf_func func;
  gpointer data;
  gpointer result; /* will either point to NULL or the results. */
  gboolean started;
  gboolean finished;
  gboolean lazy;
  GThread * thread;
  GError * error;
  GMutex * mutex;
}
Future; 

/**
 * Creates a Future. 
 * The result of the future is accessible via the lf_getfuture() call.
 * This Future is not lazy.
 */
Future * lf_future(lf_func func, gpointer data);

/**
 * Will block if the future has not terminated yet. (Could conceivably 
 * block indefinitely in case of a bad Future.)
 * 
 * Essentially, `lf_value(lf_future(func, data));` is a synchronous call to func(data).
 * 
 */
gpointer lf_value(Future * future);

/**
 * Will block if the future has not terminated yet.
 * Handles errors.
 */
gpointer lf_value_err(Future * future, GError * error);

/**
 * Lazy Future
 * The Future returned will only be evaluated when lf_getfuture is called.
 */
Future * lf_lazy_future(lf_func func, gpointer data);


/**
 * Returns when one or more of the Futures in `futures` have finished.
 * Times out after timeout.
 */
int lf_select_futures(GList* futures, const struct timespec *timeout);

