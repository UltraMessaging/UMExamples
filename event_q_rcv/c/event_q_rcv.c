/* Example on setting Ultra Messaging attributes */

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

/* Received message handler (passed into lbm_rcv_create()) */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	switch (msg->type) {
	case LBM_MSG_DATA:
		printf("received message. processing message on eventQ");
		//some processing
		break;
	default:
		printf("Unknown lbm_msg_t type %x [%s][%s]\n", msg->type, msg->topic_name, msg->source);
		break;
	}
	return 0;
}

int main(int argc, char **argv)
{
	lbm_context_t *ctx;							/* pointer to context object */
	lbm_topic_t *topic_1;						/* pointer to topic object */
	lbm_rcv_t *rcv;								/* pointer to source object */
	lbm_topic_t *rtopic;						/* pointer to receiver topic object */
	lbm_context_attr_t * cattr;					/* pointer to context attribute object */
	int err;									/* return status of lbm functions (true=error) */
	lbm_event_queue_t *evq = NULL;				/* pointer to eventQ handle */

#if defined(_MSC_VER)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (wsStat != 0)
	{
		printf("line %d: wsStat=%d\n", __LINE__, wsStat);
		exit(1);
	}
#endif

	/* Initialize the defaults for the context attribute object */
	if (lbm_context_attr_create(&cattr) != 0)
	{
		fprintf(stderr, "lbm_context_attr_create: %s\n", lbm_errmsg());
		exit(1);
	}

	/* Creating the context */
	err = lbm_context_create(&ctx, cattr, NULL, NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	/* Delete the context attribute object */
	lbm_context_attr_delete(cattr);

	/* Create an event queue and associate it with a callback */
	err = lbm_event_queue_create(&evq, NULL, NULL, NULL);
	if (err == LBM_FAILURE) {
		fprintf(stderr, "lbm_event_queue_create: %s\n", lbm_errmsg());
		exit(1);
	}

	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}
	
	err = lbm_rcv_create(&rcv, ctx, rtopic, rcv_handle_msg, NULL, evq);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	/* This runs the eventQ for 10 seconds.  This means for the next 10 seconds */
	/* all of the sources events will be processed on this thread.				*/
	if (lbm_event_dispatch(evq, 10000) == LBM_FAILURE) {
		fprintf(stderr, "lbm_event_dispatch returned error: %s\n", lbm_errmsg());
	}

	/* Finished with all LBM functions, delete the source and context object. */
	lbm_rcv_delete(rcv);
	lbm_context_delete(ctx);
	lbm_event_queue_delete(evq);
#if defined(_MSC_VER)
	WSACleanup();
#endif
        return 0;
}

