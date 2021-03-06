/* seq_number_callback.c - see http://ultramessaging.github.io/UMExamples/seq_number_callback/c/index.html
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


/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	switch (msg->type) {
		case LBM_MSG_UME_REGISTRATION_SUCCESS_EX:
		{
			lbm_msg_ume_registration_ex_t *reg = (lbm_msg_ume_registration_ex_t *)(msg->data);
			printf("[%s][%s] store %u: %s UME registration successful. Sequence Number is %d.\n",
				msg->topic_name, msg->source, reg->store_index, reg->store,reg->sequence_number);

			break;
		}
		case LBM_MSG_UME_REGISTRATION_COMPLETE_EX:
		{
			lbm_msg_ume_registration_complete_ex_t *reg = (lbm_msg_ume_registration_complete_ex_t *)(msg->data);
			printf("[%s][%s] UME registration complete. SQN %x. Flags %x ",
				msg->topic_name, msg->source, reg->sequence_number, reg->flags);
			break;
		}
		default:
			printf("Other event, type=%x\n", msg->type);
			break;
	}

	return 0;
}  /* rcv_handle_msg */


int ume_rcv_seqnum_ex(lbm_ume_rcv_recovery_info_ex_func_info_t *info, void *clientd)
{
	printf("info->low_sequence_number[%d] is the starting sequence number as determined by registration consensus.\n", info->low_sequence_number);
	printf("info->high_sequence_number[%d] is the highest sequence number available in the persisted stores.\n", info->high_sequence_number);
	printf("info->low_rxreq_max_sequence_number[%d] is the lowest sequence number to be requested.\n", info->low_rxreq_max_sequence_number);

	/* Overwrite this variable to set a new starting sequence number.
	 * If the message is available in the store, then the receiver will 
	 * retransmission starting at this sequence number leading up to the
	 * high sequence number. If the new starting sequence number has
	 * yet to be published, the receiver will discard new messages until
	 * this sequence number is sent */
	info->low_sequence_number = 100;

	return 0;
}  /* ume_rcv_seqnum_ex */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;            /* Context object */
	lbm_rcv_t *rcv;                /* Receive object: for subscribing to messages. */
	lbm_topic_t *rtopic;           /* Receiver Topic object */
	lbm_rcv_topic_attr_t * rattr;  /* Receiver attribute object */
	lbm_ume_rcv_recovery_info_ex_func_t cb; /* Sequence number info callback function */
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
	err = lbm_rcv_topic_attr_create(&rattr);
	EX_LBM_CHK(err);

	cb.func = ume_rcv_seqnum_ex;
	cb.clientd = NULL;
	err = lbm_rcv_topic_attr_setopt(rattr, "ume_recovery_sequence_number_info_function", &cb, sizeof(cb));
	EX_LBM_CHK(err);

	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", rattr);
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
}  /* main */
