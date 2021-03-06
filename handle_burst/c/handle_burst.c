/* handle_burst.c - see http://ultramessaging.github.io/UMExamples/handle_burst/c/index.html
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


/* Used to track the last sequence number received */
int last_sqn = 0;


/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	switch (msg->type) {
		case LBM_MSG_DATA:
			last_sqn = msg->sequence_number;
			break;
		case LBM_MSG_UNRECOVERABLE_LOSS:
			last_sqn = msg->sequence_number;
			break;
		case LBM_MSG_UNRECOVERABLE_LOSS_BURST:
		{
			int burst = msg->sequence_number - last_sqn;
			printf("WARNING: Burst loss of %d messages!\n", burst);
			last_sqn = msg->sequence_number;
			break;
		}
		default:
			printf("Other event, type=%x\n", msg->type);
			break;
	}

	return 0;
}  /* rcv_handle_msg */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;     /* Context object */
	lbm_rcv_t *rcv;         /* Receive object: for subscribing to messages. */
	lbm_topic_t *rtopic;    /* Receiver Topic object */
	int err;

#if defined(_WIN32)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (wsStat != 0)
	{
		printf("line %d: wsStat=%d\n",__LINE__,wsStat);
		exit(1);
	}
#endif

	/* Initialize context atrributes and create context */
	err = lbm_context_create(&ctx, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	/* Create receiver for receiving request and sending response */
	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", NULL);
	EX_LBM_CHK(err);

	err = lbm_rcv_create(&rcv, ctx, rtopic, rcv_handle_msg, NULL, NULL);
	EX_LBM_CHK(err);

	while (1) { }

	err = lbm_rcv_delete(rcv);
	EX_LBM_CHK(err);

	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);

#if defined(_MSC_VER)
	/* Windows-specific cleanup overhead */
	WSACleanup();
#endif
	return 0;
} /* main */
