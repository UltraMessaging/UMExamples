/* Code Disclaimer? */

#include <stdio.h>

#if defined(_MSC_VER)
/* Windows-only includes */
#include <winsock2.h>
#define SLEEP(s) Sleep((s)*1000)
#else
/* Unix-only includes */
#include <stdlib.h>
#include <unistd.h>
#define SLEEP(s) sleep(s)
#endif

#include <lbm/lbm.h>

/* Example error checking macro.  Include after each UM call. */
#define EX_LBM_CHK(err) do { \
	if ((err) < 0) { \
		fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
		__FILE__, __LINE__, lbm_errmsg()); \
		exit(1); \
	}  \
} while (0)

int wait = 1;

/* Timer callback function */
int sample_timer_handler(lbm_context_t *ctx, const void *clientd)
{
	printf("Timer executed. Set wait to 0 so application can cleanly exit\n");
	wait = 0;
}

int main(int argc, char **argv)
{
	lbm_context_t *ctx;                     /* Context object */
	lbm_context_attr_t * cattr;             /* Context attribute object */
	int err;                                /* Used for checking API return codes */
	int timer_id;				/* Used to get timer id from schedule API */

	/* Initialize context atrributes and create context */
	err = lbm_context_attr_create(&cattr);
	EX_LBM_CHK(err);

	err = lbm_context_create(&ctx, cattr, NULL, NULL);
	EX_LBM_CHK(err);
	
	if ((timer_id = lbm_schedule_timer(ctx, sample_timer_handler, NULL, NULL, 1000)) == -1) {
		fprintf(stderr, "%s:%d, lbm error: '%s'\n", __FILE__, __LINE__, lbm_errmsg());
		exit(1);
	}

	/* Now wait for callback to end the wait and close the application */
	while (wait)
		SLEEP(1);

	/* Clean up */
	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);
  
#if defined(_MSC_VER)
	/* Windows-specific cleanup overhead */
	WSACleanup();
#endif
        return 0;
}

