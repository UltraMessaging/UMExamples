/* client.c
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
#define MSLEEP(s) Sleep(s)
#else
/* Unix-only includes */
#include <stdlib.h>
#include <unistd.h>
#define MSLEEP(s) usleep((s) * 1000)
#endif

#include <lbm/lbm.h>

typedef struct server_s {
	lbm_context_t *ctx;
	char *topic_name; /* Topic the server is subscribed to. */
	lbm_src_t *src;   /* Source to send requests to server. */
	lbm_rcv_t *rcv;   /* Receiver for responses from server. */
	int state;        /* 0=Waiting for registration, 1=registered. */
} server_t;

/* Header for request messages. */
typedef struct req_hdr_s {
	char data[8];
} req_hdr_t;


#define E(E_err) do { \
	if ((E_err) < 0) { \
		fprintf(stderr, "Error, %s:%d: %s: %s\n", \
			__FILE__, __LINE__, #E_err, lbm_errmsg()); \
		fflush(stderr); \
		abort(); \
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


int response_rcv_cb(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	server_t *server = (server_t *)clientd;
	req_hdr_t *req_hdr;
	const char *req_data;

	switch (msg->type)
	{
	case LBM_MSG_DATA:    /* Received a message from the client. */
		printf("Received %ld bytes on topic %s: '%.*s'\n",
			(long)msg->len, msg->topic_name, (int)msg->len, msg->data);

		if (msg->len > 1 && msg->data[0] == 'r') {
			/* Should check for success. */
			server->state = 1;  /* Registered. */
		}
		else if (msg->len > sizeof(req_hdr_t) && msg->data[0] == 'R') {
			req_hdr = (req_hdr_t *)msg->data;
			req_data = &msg->data[sizeof(req_hdr_t)];
			/* Process response message. */
		}
		break;

	case LBM_MSG_BOS:
		printf("[%s][%s], BOS\n", msg->topic_name, msg->source);
		break;

	case LBM_MSG_EOS:
		printf("[%s][%s], EOS\n", msg->topic_name, msg->source);
		break;

	default:    /* unexpected receiver event */
		printf("Receive event type %d topic='%s', source='%s'\n",
			msg->type, msg->topic_name, msg->source);
		break;
	}  /* switch msg->type */

	return 0;
}  /* response_rcv_cb */


int main(int argc, char **argv)
{
	lbm_context_t *ctx;
	server_t server;  /* State information for the server. */
	char response_topic_name[256];

	/* Get (pretty much) unique client name. */
	sprintf(response_topic_name, "Client.%lx.Response", (long)getpid());

#if defined(_MSC_VER)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (wsStat != 0) {
		printf("line %d: wsStat=%d\n",__LINE__,wsStat); exit(1);
	}
#endif

	server.ctx = NULL;

	server.topic_name = "Server1.Request";
	server.src = NULL;   /* Source to send requests to server. */
	server.rcv = NULL;   /* Receiver for responses from server. */
	server.state = 0;    /* Waiting for registration. */

	E(lbm_config("client.cfg"));

	E(lbm_context_create(&ctx, NULL, NULL, NULL));
	server.ctx = ctx;

	/* Create source to send requests to server. */
	{
		lbm_topic_t *topic;
		E(lbm_src_topic_alloc(&topic, ctx, server.topic_name, NULL));
		E(lbm_src_create(&server.src, ctx, topic, NULL, NULL, NULL));
	}

	/* Create receiver for responses from server. */
	{
		lbm_topic_t *topic;
		E(lbm_rcv_topic_lookup(&topic, ctx, response_topic_name, NULL));
		E(lbm_rcv_create(&server.rcv, ctx, topic, response_rcv_cb, &server, NULL));
	}

	MSLEEP(1);  /* Try to reduce the number of tries to register. */

	/* Register with the server.  May need multiple tries. */
	{
		int try_cnt = 0;
		int backoff_delay = 2;
		char register_msg[257];
		sprintf(register_msg, "r%s", response_topic_name);

		while (server.state == 0) {
			try_cnt ++;
			E(lbm_src_send(server.src, register_msg,
				strlen(register_msg) + 1, LBM_MSG_FLUSH | LBM_SRC_BLOCK));
			printf("Sent '%s' to %s; sleep for %d ms\n", register_msg,
				server.topic_name, backoff_delay);

			/* Exponential backoff, to max of 1 sec. */
			MSLEEP(backoff_delay);
			backoff_delay *= 2;  /* Exponential backoff to max of 1 sec. */
			if (backoff_delay > 1000) {
				backoff_delay = 1000;
			}
		}
		printf("Took %d tries to register with server.\n", try_cnt);
	}

	/* Main work of the program, which includes sending 5 requests. */
	{
		int i;
		char send_buf[500];
		req_hdr_t *req_hdr = (req_hdr_t *)send_buf;

		memset((char *)req_hdr, '0', sizeof(req_hdr_t));
		req_hdr->data[0] = 'R';

		for (i = 0; i < 5; i++) {
			/* The application builds a request into <tt>request_msg</tt>. */
			char request_msg[257];
			sprintf(request_msg, "%s.%d", response_topic_name, i);

			/* The application message is copied in after the header. */
			strcpy(&send_buf[sizeof(req_hdr_t)], request_msg);
			E(lbm_src_send(server.src, send_buf, strlen(send_buf) + 1,
				LBM_MSG_FLUSH | LBM_SRC_NONBLOCK));
			MSLEEP(1000);
		}
	}

	printf("Client exiting.\n");
	E(lbm_rcv_delete(server.rcv));
	E(lbm_src_delete(server.src));

	E(lbm_context_delete(ctx));

#if defined(_MSC_VER)
	WSACleanup();
#endif

	return 0;
}  /* main */
