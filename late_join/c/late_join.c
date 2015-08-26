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
		if (msg->flags & LBM_MSG_FLAG_RETRANSMIT)
			printf("Processing Late Join Message. SQN: %d\n", msg->sequence_number);
		else if (msg->flags & LBM_MSG_FLAG_OTR)
			printf("Processing OTR Message. SQN: %d\n", msg->sequence_number);
		else
			printf("Normal Message. SQN: %d\n", msg->sequence_number);

		//some processing
		break;
	case LBM_MSG_BOS:
		printf("Beggining of transport.\n");
		break;
	case LBM_MSG_EOS:
		printf("End of transport.\n");
		break;
	default:
		printf("Unknown lbm_msg_t type %x [%s][%s]\n", msg->type, msg->topic_name, msg->source);
		break;
	}
	return 0;
}

main()
{
	lbm_context_t *ctx;							/* pointer to context object */
	lbm_topic_t *topic_1;						/* pointer to topic object */
	lbm_src_t *early_src;						/* pointer to source object */
	lbm_rcv_t *late_rcv;						/* pointer to receiver object */
	lbm_topic_t *rtopic;						/* pointer to receiver topic object */
	lbm_context_attr_t * cattr;					/* pointer to context attribute object */
	lbm_src_topic_attr_t *tattr;				/* pointer to source attribute object */
	int err;									/* return status of lbm functions (true=error) */

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
	
	/* Initializing the source attribute object */
	if (lbm_src_topic_attr_create(&tattr) != 0)
	{
		fprintf(stderr, "lbm_src_topic_attr_create: %s\n", lbm_errmsg());
		exit(1);
	}
	
	/* Setting the transport via the source topic string method */
	if (lbm_src_topic_attr_str_setopt(tattr, "late_join", "1") != 0 )
	{
		fprintf(stderr, "lbm_src_topic_attr_str_setopt:transport: %s\n", lbm_errmsg());
		exit(1);
	}
	if (lbm_src_topic_attr_str_setopt(tattr, "retransmit_retention_size_threshold", "1")
	{
		fprintf(stderr, "lbm_src_topic_attr_str_setopt:transport: %s\n", lbm_errmsg());
		exit(1);
	}
	
	/* Allocating the source topic */
	err = lbm_src_topic_alloc(&topic_1, ctx, "test.topic", tattr);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	 /* Creating the source */
	err = lbm_src_create(&early_src, ctx, topic_1, NULL, NULL, NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	/* Wait for topic resolution to happen */
	Sleep(1);

	/* Send a message... we should expect no one to get it because we haven't created the receiver
	   yet. */
	err = lbm_src_send(early_src, "Hello!", 6, LBM_MSG_FLUSH | LBM_SRC_BLOCK);
	/* Delete the context attribute object */
	lbm_context_attr_delete(cattr);

	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}
	err = lbm_rcv_create(&late_rcv, ctx, rtopic, rcv_handle_msg, NULL, NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	SLEEP(100000);

	/* Finished with all LBM functions, delete the receiver and context object. */
	/* Delete the source topic attribute objects */
	lbm_src_topic_attr_delete(tattr);

	/* Finished with all LBM functions, delete the source and context object. */
	lbm_src_delete(early_src);
	lbm_rcv_delete(late_rcv);
	lbm_context_delete(ctx);

#if defined(_MSC_VER)
	WSACleanup();
#endif
}

