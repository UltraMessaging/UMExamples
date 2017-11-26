/* server.c
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


/* Header for request messages. */
typedef struct req_hdr_s {
	char data[8];
} req_hdr_t;


#define MAX_RESP_SIZE 7000
typedef struct client_s {
	int state;  /* 0=waiting for registration, 1=registered. */
	char *topic_name;
	char *source_name;
	lbm_context_t *ctx;
	lbm_src_t *resp_src;
	char send_buf[MAX_RESP_SIZE + sizeof(req_hdr_t)];
} client_t;


/* Very simplistic LBM error handling. */
#define E(E_err) do { \
	if ((E_err) < 0) { \
		fprintf(stderr, "Error, %s:%d: '%s': %s\n", \
			__FILE__, __LINE__, #E_err, lbm_errmsg()); \
		fflush(stderr); \
		abort(); /* core dump */ \
	} \
} while (0)


/* Null check. */
#define N(N_ptr) do { \
	if ((N_ptr) == NULL) { \
		fprintf(stderr, "Error, %s:%d: '%s' is NULL\n", \
			__FILE__, __LINE__, #N_ptr); \
		fflush(stderr); \
		abort(); /* core dump */ \
	} \
} while (0)


void handle_register(client_t *client, const char *client_resp_name)
{
	int err;
	/* This work should probably be passed to a separate worker thread, but
	 * I'll do it here to simplify the code. */

	if (client->state == 1) {
		printf("Source '%s' re-confirmed\n", client->topic_name);
		/* Reply to client. */
		E(lbm_src_send(client->resp_src, "rOK", 4, LBM_MSG_FLUSH | LBM_SRC_NONBLOCK));
	}
	else if (client->state == 0) {
		lbm_src_topic_attr_t * src_attr;
		lbm_topic_t *lbm_topic;

		/* Create source to send responses to client. */
		E(lbm_src_topic_attr_create(&src_attr));
		E(lbm_src_topic_attr_str_setopt(src_attr, "transport", "lbt-ru"));
		E(lbm_src_topic_attr_str_setopt(src_attr, "transport_lbtru_port", "12070"));
		E(lbm_src_topic_attr_str_setopt(src_attr,
			"transport_source_side_filtering_behavior", "inclusion"));
		E(lbm_src_topic_alloc(&lbm_topic, client->ctx, client_resp_name, src_attr));
		/* The following create can fail if a new client joins with the same
		 * response topic name as an existing client.  Should handle this cleanly. */
		err = lbm_src_create(&client->resp_src, client->ctx, lbm_topic,
			NULL, NULL, NULL);
		if (err) {
			printf("Source '%s' failed; %s\n", client_resp_name, lbm_errmsg());
		}
		else {
			client->state = 1;
			client->topic_name = strdup(client_resp_name);  N(client->topic_name);
			printf("Source '%s' created\n", client->topic_name);
		}

		E(lbm_src_topic_attr_delete(src_attr));
	}
}  /* handle_register */


void handle_req(client_t *client, req_hdr_t *req_hdr, const char *data, size_t len)
{
	/* Response message is put after the header. */
	char *response_msg = &client->send_buf[sizeof(req_hdr_t)];

	/* This work should probably be passed to a separate worker thread, but
	 * I'll do it here to simplify the code. */

	/* Responses copy the header from the request. */
	memcpy(&client->send_buf[0], (char *)req_hdr, sizeof(req_hdr_t));

	/* Do the work of the request and put the response in response_msg.
	 * (For this sample, just echo back the request.) */
	strcpy(response_msg, data);
	
	/* Reply to client. */
	E(lbm_src_send(client->resp_src, client->send_buf,
		strlen(client->send_buf) + 1, LBM_MSG_FLUSH | LBM_SRC_NONBLOCK));
	printf("sent response to '%s'.\n", client->topic_name);
}  /* handle_req */


int request_rcv_cb(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd) 
{
	client_t *client = (client_t *)msg->source_clientd;

	switch (msg->type)
	{
	case LBM_MSG_DATA:  /* Received a message from the client. */
		printf("Received %ld bytes on topic %s: '%.*s'\n",
			 (long)msg->len, msg->topic_name, (int)msg->len, msg->data);

		/* Register message. */
		if (msg->len > 1 && msg->data[0] == 'r') {
			handle_register(client, &msg->data[1]);
		}

		/* Request message. */
		else if (msg->len >= sizeof(req_hdr_t) && msg->data[0] == 'R') {
			req_hdr_t req_hdr;
			memcpy((char *)&req_hdr, msg->data, sizeof(req_hdr_t));

			handle_req(client, &req_hdr, &msg->data[sizeof(req_hdr_t)],
				msg->len - sizeof(req_hdr_t));
		}
		else {
			printf("Ignored unrecognized command '%c'\n", msg->data[0]);
			break;
		}
		break;

	case LBM_MSG_BOS:
		printf("[%s][%s], BOS\n", msg->topic_name, msg->source);
		break;

	case LBM_MSG_EOS:
		printf("[%s][%s], EOS\n", msg->topic_name, msg->source);
		break;

	default:    /* unexpected receiver event */
		printf("Receive event type %x topic='%s', source='%s'\n", msg->type, msg->topic_name, msg->source);
		break;
	}  /* switch msg->type */

	return 0;
}  /* request_rcv_cb */


void *start_client_source(const char *source_name, void *clientd)
{
	lbm_context_t *ctx = (lbm_context_t *)clientd;;
	client_t *new_client = (client_t *)malloc(sizeof(client_t));  N(new_client);

	new_client->state = 0;  /* Waiting for register. */
	new_client->topic_name = NULL;
	new_client->source_name = strdup(source_name);  N(new_client->source_name);
	new_client->ctx = ctx;
	new_client->resp_src = NULL;

	return new_client;
}  /* start_client_source */


int end_client_source(const char *source_name, void *clientd, void *source_clientd)
{
	if (source_clientd) {
		client_t *client = (client_t *)source_clientd;

		if (client->topic_name) {
			free(client->topic_name);
		}
		if (client->source_name) {
			free(client->source_name);
		}

		E(lbm_src_delete(client->resp_src));

		free(source_clientd);
	}

	return 0;
}  /* end_client_source */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;
	lbm_rcv_t *rcv;  /* Receiver for requests from clients. */

#if defined(_MSC_VER)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (wsStat != 0) {
		printf("line %d: wsStat=%d\n",__LINE__,wsStat); exit(1);
	}
#endif

	E(lbm_config("serv.cfg"));

	E(lbm_context_create(&ctx, NULL, NULL, NULL));

	/* Create receiver for requests from clients. */
	{
		lbm_rcv_src_notification_func_t source_notification_function;
		lbm_rcv_topic_attr_t *rcv_attr;
		lbm_topic_t *topic;

		E(lbm_rcv_topic_attr_create(&rcv_attr));

		/* Set up per-source state management. */
		source_notification_function.create_func = start_client_source;
		source_notification_function.delete_func = end_client_source;
		source_notification_function.clientd = ctx;  /* Pass this to start_client_source(). */
		E(lbm_rcv_topic_attr_setopt(rcv_attr, "source_notification_function",
			&source_notification_function, sizeof(source_notification_function)));

		E(lbm_rcv_topic_lookup(&topic, ctx, "Server1.Request", rcv_attr));
		/* Pass context object as clientd. */
		E(lbm_rcv_create(&rcv, ctx, topic, request_rcv_cb, ctx, NULL));

		E(lbm_rcv_topic_attr_delete(rcv_attr));
	}

	while (1) {
		SLEEP(1);
	}

	/* Finished all receiving from this topic, delete the receiver object. */
	E(lbm_rcv_delete(rcv));

	/* Do not need to delete the topic object - LBM keeps track of topic
	 * objects and deletes them as-needed.  */

	/* Finished with all LBM functions, delete the context object. */
	E(lbm_context_delete(ctx));

#if defined(_MSC_VER)
	WSACleanup();
#endif

	return 0;
}  /* main */
