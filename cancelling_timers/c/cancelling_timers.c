/* cancelling_timers.c - see http://ultramessaging.github.io/UMExamples/cancelling_timers/c/index.html
 *
 * Copyright (c) 2005-2017 Informatica Corporation. All Rights Reserved.
 * Permission is granted to licensees to use
 * or alter this software for any purpose, including commercial applications,
 * according to the terms laid out in the Software License Agreement.
 *
 * This source code example is provided by Informatica for educational
 * and evaluation purposes only.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INFORMATICA DISCLAIMS ALL WARRANTIES
 * EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
 * PURPOSE.  INFORMATICA DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE.  INFORMATICA SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
 * LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
 * INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
 * TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF INFORMATICA HAS BEEN APPRISED OF
 * THE LIKELIHOOD OF SUCH DAMAGES.
 */

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

typedef enum {PENDING, FIRED, CANCELED} state_type_e;
typedef struct app_timer_s {
	int id;
	state_type_e state;
	int sync;
} app_timer_s;


/* Timer callback function */
int sample_timer_handler(lbm_context_t *ctx, const void *clientd)
{
	struct app_timer_s *my_timer = (struct app_timer_s *)clientd;
	if (my_timer->state != PENDING) {  
		fprintf(stderr, "ERROR: Timer executed and should have been cancelled\n");
		exit(1);
	}

	fprintf(stderr, "ERROR: This timer should have been cancelled by now\n");

	return 0;
}  /* sample_timer_handler */


int cancel_timer_cb(lbm_context_t *ctx, const void *clientd)
{
	struct app_timer_s *my_timer = (struct app_timer_s *)clientd;
	if (my_timer->state == PENDING) {
		lbm_cancel_timer(ctx, my_timer->id, NULL);
		my_timer->state = CANCELED;
	}
	my_timer->sync = 0;
	
	return 0;
}  /* cancel_timer_cb */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;                     /* Context object */
	app_timer_s my_timer;
	int err;

	my_timer.sync = 0;

	/* Initialize context atrributes and create context */
	err = lbm_context_create(&ctx, NULL, NULL, NULL);
	EX_LBM_CHK(err);
	
	my_timer.state = PENDING;
	if ((my_timer.id = lbm_schedule_timer(ctx, sample_timer_handler, &my_timer, NULL, 10000)) == -1) {
		fprintf(stderr, "%s:%d, lbm error: '%s'\n", __FILE__, __LINE__, lbm_errmsg());
		exit(1);
	}

	/* Now wait for callback to end the wait and close the application */
	SLEEP(1);

	if (my_timer.state == PENDING) {
		lbm_schedule_timer(ctx, cancel_timer_cb, &my_timer, NULL, 0);
		/* timer might still fire at this point. */
		my_timer.sync = 1;
	}
	
	while (my_timer.sync)
		SLEEP(1);

	/* Clean up */
	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);

#if defined(_MSC_VER)
	/* Windows-specific cleanup overhead */
	WSACleanup();
#endif

	return 0;
}  /* main */
