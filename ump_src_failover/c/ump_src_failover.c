/* ump_src_failover.c - see http://ultramessaging.github.io/UMExamples/ump_src_failover/c/index.html
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

/* Store configurations */
#define UMP_PRIMARY_STORE "127.0.0.1:29999"
#define UMP_PRIMARY_SESSION_ID "100"
#define UMP_SECONDARY_STORE "127.0.0.1:30000"
#define UMP_SECONDARY_SESSION_ID "200"
#define UMP_CLEAR_STORE_CONFIG "0.0.0.0:0"

typedef enum {NOT_INIT, INIT, READY, UNRESP} state_type_e;
typedef enum {PRIMARY, BACKUP} mode_type_e;
typedef struct app_src_s {
	lbm_context_t *ctx;
	lbm_src_t *src;
	state_type_e state;
	mode_type_e mode;
} app_src_s;


/* Source event handler callback (passed into lbm_src_create()) */
int handle_src_event(lbm_src_t *src, int event, void *ed, void *cd)
{
	struct app_src_s *app_src = (struct app_src_s *)cd;
	switch (event) {
	/* Success event provided only for printing purposes */
	case LBM_SRC_EVENT_UME_REGISTRATION_SUCCESS_EX:
	{
		lbm_src_event_ume_registration_ex_t *reg = (lbm_src_event_ume_registration_ex_t *)ed;
		printf("UME store %u: %s registration success.\n", reg->store_index, reg->store);
		break;
	}
	case LBM_SRC_EVENT_UME_REGISTRATION_COMPLETE_EX:
	{
		lbm_src_event_ume_registration_complete_ex_t *reg = (lbm_src_event_ume_registration_complete_ex_t *)ed;

		printf("UME registration complete. SQN %x.\n", reg->sequence_number);

		app_src->state = READY;
		break;
	}
	case LBM_SRC_EVENT_UME_STORE_UNRESPONSIVE:
	{
		const char *infostr = (const char *)ed;
		
		printf("UME store: %s\n", infostr);
		if (strstr(infostr, "quorum lost") != NULL)
			app_src->state = UNRESP;
		break;
	}
	}

	return 0;
}  /* handle_src_event */


int init_src(app_src_s *app_src)
{
	lbm_src_topic_attr_t * tattr;  /* Source topic attribute object */
	lbm_topic_t *topic;            /* Topic object */
	int err;

	err = lbm_src_topic_attr_create(&tattr);
	EX_LBM_CHK(err);

	err = lbm_src_topic_attr_str_setopt(tattr, "ume_store", UMP_CLEAR_STORE_CONFIG);
	EX_LBM_CHK(err);

	/* Assuming there are stores running on the localhost */
	if (app_src->mode == PRIMARY) {
		err = lbm_src_topic_attr_str_setopt(tattr, "ume_store", UMP_PRIMARY_STORE);
		EX_LBM_CHK(err);
	}
	else {
		err = lbm_src_topic_attr_str_setopt(tattr, "ume_store", UMP_SECONDARY_STORE);
		EX_LBM_CHK(err);
	}

	if (app_src->mode == PRIMARY) {
		err = lbm_src_topic_attr_str_setopt(tattr, "ume_session_id", UMP_PRIMARY_SESSION_ID);
		EX_LBM_CHK(err);
	}
	else {
		err = lbm_src_topic_attr_str_setopt(tattr, "ume_session_id", UMP_SECONDARY_SESSION_ID);
		EX_LBM_CHK(err);
	}

	/* Need to set store behavior to Quorum-Consensus  */
	err = lbm_src_topic_attr_str_setopt(tattr, "ume_store_behavior", "qc");
	EX_LBM_CHK(err);

	err = lbm_src_topic_alloc(&topic, app_src->ctx, "test.topic", tattr);
	EX_LBM_CHK(err);

	err = lbm_src_create(&(app_src->src), app_src->ctx, topic, handle_src_event, app_src, NULL);
	EX_LBM_CHK(err);

	app_src->state = INIT;

	return 0;
}  /* init_src */


void failover_app_src(app_src_s *app_src)
{
	int err;
	
	/* Delete the source */
	err = lbm_src_delete(app_src->src);
	EX_LBM_CHK(err);

	app_src->src = NULL;
	app_src->state = NOT_INIT;
	app_src->mode = BACKUP;
}  /* failover_app_src */


/* Provided simply for printing the state name */
const char* source_state(app_src_s *app_src)
{
	switch (app_src->state)
	{
		case NOT_INIT: return "NOT_INIT";
		case INIT: return "INIT";
		case READY: return "READY";
		case UNRESP: return "UNRESP";
	}
	return NULL;
}  /* source_state */


int main(int argc, char **argv)
{
	app_src_s app_src;           /* App structure to track source */
	lbm_context_t *ctx;          /* Context object */
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

	app_src.state = NOT_INIT;
	app_src.mode = PRIMARY;
	app_src.ctx = ctx;

	while(1)
	{
		/* Initialize the source if we're not initialized */
		if (app_src.state == NOT_INIT)
			init_src(&app_src);

		if (app_src.state == READY)
		{
			if (lbm_src_send(app_src.src, "test", 4, LBM_SRC_NONBLOCK) == LBM_FAILURE)
			{
				/* Assume EWOULDBLOCK and wait */
				SLEEP_SEC(1);
			}
		}
		else
		{
			/* Print a warning that the source is not ready and sleep */
			printf("Source cannot send: %d. Source State: %s\n", lbm_errnum(), source_state(&app_src));
			if (app_src.state == UNRESP && app_src.mode == PRIMARY)
			{
				printf("Primary store unresponsive, failing over to backup\n");
				failover_app_src(&app_src);
			}
			else if (app_src.state == UNRESP && app_src.mode == BACKUP)
			{
				printf("Backup store unresponsive. Exiting\n");
				exit(1);
			}
			SLEEP_SEC(1);
		}
	}
	return 0;
}  /* main */
