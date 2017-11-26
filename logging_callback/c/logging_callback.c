/* logging_callback.c - see http://ultramessaging.github.io/UMExamples/logging_callback/c/index.html
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


/* Logging callback */
int lbm_log_msg(int level, const char *message, void *clientd)
{
	printf("LOG Level %d: %s\n", level, message);
	return 0;
}  /* lbm_log_msg */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;           /* pointer to context object */
	lbm_topic_t *topic;           /* pointer to topic object */
	lbm_src_topic_attr_t *tattr;  /* pointer to source attribute object */
	lbm_src_t *src;               /* pointer to source object */
	int err;

#if defined(_MSC_VER)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (wsStat != 0)
	{
		printf("line %d: wsStat=%d\n",__LINE__,wsStat);
		exit(1);
	}
#endif

	/* Setup logging callback */
	err = lbm_log(lbm_log_msg, NULL);
	EX_LBM_CHK(err);

	err = lbm_context_create(&ctx, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	/* Initializing the source attribute object */
	err = lbm_src_topic_attr_create(&tattr);
	EX_LBM_CHK(err);

	/* Setting late_join */
	err = lbm_src_topic_attr_str_setopt(tattr, "late_join", "1");
	EX_LBM_CHK(err);

	/* Allocating the topic */
	err = lbm_src_topic_alloc(&topic, ctx, "test.topic.1", tattr);
	EX_LBM_CHK(err);

	/* Creating the source */
	err = lbm_src_create(&src, ctx, topic, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	/* At the very least, the following core message should print via the logging callback: */
	/* LOG Level 5: Core-5688-539: NOTICE: Source "test.topic.1" has no retention settings (1 message retained max) */

	/* Cleanup */
	err = lbm_src_delete(src);
	EX_LBM_CHK(err);
	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);

	return 0;
}  /* main */
