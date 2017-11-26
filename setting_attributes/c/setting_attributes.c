/* setting_attributes.c - see http://ultramessaging.github.io/UMExamples/setting_attributes/c/index.html
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


int main(int argc, char **argv)
{
	lbm_context_t *ctx;             /* pointer to context object */
	lbm_topic_t *topic_1;           /* pointer to topic object */
	lbm_topic_t *topic_2;           /* pointer to topic object */
	lbm_src_t *src_1;               /* pointer to source object */
	lbm_src_t *src_2;               /* pointer to source object */
	lbm_src_topic_attr_t *tattr_1;  /* pointer to source attribute object */
	lbm_src_topic_attr_t *tattr_2;  /* pointer to source attribute object */
	lbm_context_attr_t * cattr;     /* pointer to context attribute object */
	lbm_uint16_t res_port;          /* Int to set resolver port */
	lbm_uint16_t des_port;          /* Int to set lbtrm destination port */
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

	/* Initialize the defaults for the context attribute object */
	err = lbm_context_attr_create(&cattr);
	EX_LBM_CHK(err);

	/* Setting the resolver address using the string method */
	err = lbm_context_attr_str_setopt(cattr, "resolver_multicast_address", "224.10.11.12");
	EX_LBM_CHK(err);

	/* Setting the resolver port using the data-type value method */
	res_port = 12345;
	err = lbm_context_attr_setopt(cattr, "resolver_multicast_port", &res_port, sizeof(lbm_uint16_t));
	EX_LBM_CHK(err);

	/* Creating the context */
	err = lbm_context_create(&ctx, cattr, NULL, NULL);
	EX_LBM_CHK(err);

	/* Delete the context attribute object */
	err = lbm_context_attr_delete(cattr);
	EX_LBM_CHK(err);

	/* Initializing the source attribute object */
	err = lbm_src_topic_attr_create(&tattr_1);

	/* Setting the transport via the source topic string method */
	err = lbm_src_topic_attr_str_setopt(tattr_1, "transport", "lbtrm");
	EX_LBM_CHK(err);

	/* Setting the lbtrm destination port via the direct value set method */
	des_port = 14001;
	err = lbm_src_topic_attr_setopt(tattr_1, "transport_lbtrm_destination_port", &des_port, sizeof(lbm_uint16_t));
	EX_LBM_CHK(err);

	/* Allocating the topic */
	err = lbm_src_topic_alloc(&topic_1, ctx, "test.topic.1", tattr_1);
	EX_LBM_CHK(err);

	/* Creating the source */
	err = lbm_src_create(&src_1, ctx, topic_1, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	/* Initialized the second source attributes as a copy of the first */
	err = lbm_src_topic_attr_dup(&tattr_2, tattr_1);
	EX_LBM_CHK(err);

	/* Now modify the destination port for this second source to put the
	 * publisher on a different transport. */
	des_port = 14002;
	err = lbm_src_topic_attr_setopt(tattr_2, "transport_lbtrm_destination_port", &des_port, sizeof(lbm_uint16_t));
	EX_LBM_CHK(err);

	/* Allocating the second topic */
	err = lbm_src_topic_alloc(&topic_2, ctx, "test.topic.2", tattr_2);
	EX_LBM_CHK(err);

	/* Creating the source */
	err = lbm_src_create(&src_2, ctx, topic_2, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	/* Delete the first and second source topic attribute objects */
	err = lbm_src_topic_attr_delete(tattr_1);
	EX_LBM_CHK(err);
	err = lbm_src_topic_attr_delete(tattr_2);
	EX_LBM_CHK(err);

	/* Finished with all LBM functions, delete the source and context object. */
	err = lbm_src_delete(src_1);
	EX_LBM_CHK(err);
	err = lbm_src_delete(src_2);
	EX_LBM_CHK(err);
	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);

#if defined(_MSC_VER)
	WSACleanup();
#endif
	return 0;
}  /* main */
