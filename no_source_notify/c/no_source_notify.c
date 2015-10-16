/* no_source_notify.c - http://ultramessaging.github.io/UMExamples */

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

#define NO_SOURCE_QUERIES "25"

int no_source = 0;

/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	int err;

	switch (msg->type) {
		case LBM_MSG_NO_SOURCE_NOTIFICATION:
			printf("[%s], no sources found for topic after %s queries\n", msg->topic_name, NO_SOURCE_QUERIES);
			no_source++;
			break;
		default:
			printf("Other event, type=%x\n", msg->type);
			break;
	}
	return 0;
}

main()
{
	lbm_context_t *ctx;                     /* Context object */
	lbm_context_attr_t * cattr;             /* Context attribute object */
	lbm_rcv_t *rcv;                         /* Receive object: for subscribing to messages. */
	lbm_topic_t *rtopic;                    /* Receiver Topic object */
	lbm_rcv_topic_attr_t * rattr;           /* Receiver attribute object */
	int err;                                /* Used for checking API return codes */

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

	err = lbm_rcv_topic_attr_create(&rattr);
        EX_LBM_CHK(err);

	err = lbm_rcv_topic_attr_str_setopt(rattr, "resolution_no_source_notification_threshold", NO_SOURCE_QUERIES);
	EX_LBM_CHK(err);	

	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", rattr);
        EX_LBM_CHK(err);

	err = lbm_rcv_create(&rcv, ctx, rtopic, rcv_handle_msg, NULL, NULL);
	EX_LBM_CHK(err);

	while (!no_source) { }

	err = lbm_rcv_delete(rcv);
	EX_LBM_CHK(err);

	err = lbm_context_delete(ctx);
	EX_LBM_CHK(err);

#if defined(_MSC_VER)
	/* Windows-specific cleanup overhead */
	WSACleanup();
#endif
} /* main */
