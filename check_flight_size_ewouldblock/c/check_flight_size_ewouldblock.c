/* check_flight_size_ewouldblock.c - see http://ultramessaging.github.io/UMExamples/check_flight_size_ewouldblock/c/index.html
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

#define DEFAULT_APP_FLIGHT_SIZE 500		/* Used for checking flight size */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;    /* Context object */
	lbm_topic_t *topic;    /* Topic object */
	lbm_src_t *src;        /* Source object */
	lbm_src_topic_attr_t * tattr;  /* Source topic attribute object */
	int inflight;          /* Object used for checking inflight counters */
	char flight[4];
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

	sprintf(flight, "%d", DEFAULT_APP_FLIGHT_SIZE);
	err = lbm_src_topic_attr_str_setopt(tattr, "ume_flight_size", flight);
	EX_LBM_CHK(err);

	/* Assuming there is a store running on the localhost at port 29999 */
	err = lbm_src_topic_attr_str_setopt(tattr, "ume_store", "127.0.0.1:29999");
	EX_LBM_CHK(err);

	err = lbm_src_topic_alloc(&topic, ctx, "test.topic", tattr);
	EX_LBM_CHK(err);

	err = lbm_src_create(&src, ctx, topic, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	while(1)
	{
		err = lbm_src_send(src, "test", 4, LBM_SRC_NONBLOCK);
		if (err == LBM_FAILURE)
		{
			if (lbm_errnum() == LBM_EWOULDBLOCK)
			{
				err = lbm_src_get_inflight(src, LBM_FLIGHT_SIZE_TYPE_UME, &inflight, NULL, NULL);
				EX_LBM_CHK(err);

				/* Is inflight counter is equal to the configured flight size? */
				if (inflight == DEFAULT_APP_FLIGHT_SIZE)
				{
					printf("ALERT: Source is blocked on flight size: %d\n", inflight);
				}
			}
		}
	}

	return 0;
}  /* main */
