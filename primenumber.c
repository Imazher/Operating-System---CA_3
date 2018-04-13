
#include <pthread.h>		/* For various thread functions */
#include <math.h>		/* For sqrt                     */
#include <stdlib.h>		/* For calloc                   */
#include <assert.h>

typedef struct
{

  pthread_cond_t start_working;

  pthread_mutex_t cond_mutex;

  long max_search;
} thread_parameters_t;

typedef struct
{
  long f[2];
} factor_t;


void
print_factorization (long n, factor_t * azher)
{
  int i;
  if (azher[n].f[0])
    {
      for (i = 0; i < 2; ++i)
	if (azher[n].f[i])
	  print_factorization (azher[n].f[i], azher);
    }
  else
    printf (" %ld ", n);
}


void *
primes_computer_runner (void *param)
{
  factor_t *azher;
  long i, prime, limit;
  thread_parameters_t *thread_parameters = (thread_parameters_t *) param;
  pthread_mutex_lock (&thread_parameters->cond_mutex);
  pthread_cond_wait (&thread_parameters->start_working,
		     &thread_parameters->cond_mutex);

  printf ("Thread woken up to find primes less than %ld.\n",
	  thread_parameters->max_search);

  azher =
    (factor_t *) calloc (thread_parameters->max_search, sizeof (factor_t));
  assert (azher != NULL);

  limit = (long) sqrt (thread_parameters->max_search) + 1;
  
  for (prime = 2; prime <= limit; ++prime)
    for (i = 2; i * prime < thread_parameters->max_search; ++i)
      {
	azher[i * prime].f[0] = i;
	azher[i * prime].f[1] = prime;

      }
	 
  for (i = 2; i < thread_parameters->max_search; ++i)
    if (!azher[i].f[0])
      printf ("* %ld is prime.\n", i);
#ifdef SHOW_NONPRIME
    else
      {
	printf ("  %ld is nonprime, factorization: (", i);
	print_factorization (i, azher);
	puts (")");
      }
#endif

  free (azher);

  return NULL;
}

int
main (int argc, char *argv[])
{
  
  thread_parameters_t thread_parameters;
 
  pthread_cond_init (&thread_parameters.start_working, NULL);
  pthread_mutex_init (&thread_parameters.cond_mutex, NULL);

  
  pthread_t computational_thread;

  
  pthread_create (&computational_thread, NULL, primes_computer_runner,
		  (void *) &thread_parameters);

  puts (
	"Enter an integer and the program will print all primes less than\n"
	"the integer you enter. (^D or ^C to quit.)\n"
#ifdef SHOW_NONPRIME
	
#endif
    );

  
  if (!scanf ("%ld", &thread_parameters.max_search))
    return 0;
  
  pthread_cond_broadcast (&thread_parameters.start_working);
  
  pthread_join (computational_thread, NULL);

  return 0;
}
