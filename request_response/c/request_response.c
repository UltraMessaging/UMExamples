/* request_response.c - see http://ultramessaging.github.io/UMExamples/request_response/c/index.html
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

/* Global var used to indicate that the application can exit after response received */
int run = 1;


/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	int err;

	switch (msg->type) {
	case LBM_MSG_REQUEST:
		printf("LBM_MSG_REQUEST received\n");
		err = lbm_send_response(msg->response, "response", 8, LBM_SRC_NONBLOCK);
		EX_LBM_CHK(err);
		break;
	}
	return 0;
}  /* rcv_handle_msg */


/* Callback used to handle response message */
int handle_response(lbm_request_t *req, lbm_msg_t *msg, void *clientd)
{
	switch (msg->type) {
	case LBM_MSG_RESPONSE:
		printf("LBM_MSG_RESPONSE received\n");
		run = 0;
		break;
	}
	return 0;
}  /* handle_response */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;                     /* Context object */
	lbm_topic_t *stopic;                    /* Source Topic object */
	lbm_src_t *src;                         /* Source object */
	lbm_src_topic_attr_t * tattr;           /* Source topic attribute object */
	lbm_rcv_t *rcv;                         /* Receive object: for subscribing to messages. */
	lbm_topic_t *rtopic;                    /* Receiver Topic object */
	lbm_request_t *req;                     /* Request object */
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

	/* Create source for sending request */
	err = lbm_src_topic_attr_create(&tattr);
	EX_LBM_CHK(err);

	err = lbm_src_topic_alloc(&stopic, ctx, "test.topic", tattr);
	EX_LBM_CHK(err);

	err = lbm_src_topic_attr_delete(tattr);
	EX_LBM_CHK(err);

	err = lbm_src_create(&src, ctx, stopic, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	err = lbm_send_request(&req, src, "request", 7, handle_response, NULL, NULL, 0);
	EX_LBM_CHK(err);

	while (run) {  /* loop until response received */
		SLEEP(1);
	}

	/* Clean up */
	err = lbm_request_delete(req);
	EX_LBM_CHK(err);

	err = lbm_src_delete(src);
	EX_LBM_CHK(err);
	
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
