/* quorum_state.c - see http://ultramessaging.github.io/UMExamples/quorum_state/c/index.html
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
#include <string.h>

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

#if defined(_WIN32)
#   define SLEEP_SEC(x) Sleep((x)*1000)
#else
#   define SLEEP_SEC(x) sleep(x)
#endif

/* Example error checking macro.  Include after each UM call. */
#define EX_LBM_CHK(err) do { \
	if ((err) < 0) { \
		fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
		__FILE__, __LINE__, lbm_errmsg()); \
		exit(1); \
	}  \
} while (0)

int src_ready = 0;


/* Source event handler callback (passed into lbm_src_create()) */
int handle_src_event(lbm_src_t *src, int event, void *ed, void *cd)
{
	switch (event) {
		case LBM_SRC_EVENT_UME_REGISTRATION_COMPLETE_EX:
		{
			src_ready = 1;
			break;
		}
		case LBM_SRC_EVENT_UME_STORE_UNRESPONSIVE:
		{
			const char *infostr = (const char *)ed;
			if (strstr(infostr, "quorum lost") != NULL)
				src_ready = 0;
			break;
		}
	}
	return 0;
}  /* handle_src_event */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;                     /* Context object */
	lbm_topic_t *topic;                     /* Topic object */
	lbm_src_t *src;                         /* Source object */
	lbm_src_topic_attr_t * tattr;           /* Source topic attribute object */
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

	err = lbm_context_create(&ctx, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	err = lbm_src_topic_attr_create(&tattr);
	EX_LBM_CHK(err);

	/* Assuming there is a store running on the localhost at port 29999 */
	err = lbm_src_topic_attr_str_setopt(tattr, "ume_store", "127.0.0.1:29999");
	EX_LBM_CHK(err);

	/* Need to set store behavior to Quorum-Consensus  */
	err = lbm_src_topic_attr_str_setopt(tattr, "ume_store_behavior", "qc");
	EX_LBM_CHK(err);

	err = lbm_src_topic_alloc(&topic, ctx, "test.topic", tattr);
	EX_LBM_CHK(err);

	err = lbm_src_create(&src, ctx, topic, handle_src_event, NULL, NULL);
	EX_LBM_CHK(err);

	while(1)
	{
		if (src_ready)
		{
			err = lbm_src_send(src, "test", 4, LBM_SRC_NONBLOCK);
			if (err  == LBM_FAILURE)
			{
				/* Assume EWOULDBLOCK, wait, and retry. */
				SLEEP_SEC(1);
			}
		}
		else
		{
			/* Print a warning that the source is not ready and sleep */
			printf("Source is not ready to send (no quorum)\n");
			SLEEP_SEC(1);
		}
	}

	return 0;
}  /* main */
