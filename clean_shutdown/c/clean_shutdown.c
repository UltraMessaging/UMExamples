/* Code Disclaimer? */

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

int cleanup = 0;

/* Example error checking macro.  Include after each UM call. */
  #define EX_LBM_CHK(err) do { \
      if ((err) < 0) { \
              fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
              __FILE__, __LINE__, lbm_errmsg()); \
              exit(1); \
      }  \
  } while (0)

#if defined(_WIN32)
DWORD WINAPI eq_thread(void *arg)
#else
void *eq_thread(void *arg)
#endif /* _WIN32 */
{
	lbm_event_queue_t *evq = (lbm_event_queue_t *) arg;

	while (1)
	{
		if (lbm_event_dispatch(evq, 1000) == -1)
		{
			printf("ERROR processing context events: %s\n", lbm_errmsg());
			exit(1);
		}

		if (cleanup)
			break;
	}
}

/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	int err;

	switch (msg->type) {
		case LBM_MSG_DATA:
			printf("LBM_MSG_DATA received\n");
			break;
	}

	cleanup = 1;
}

main()
{
	lbm_context_t *ctx;             /* pointer to context object */
	lbm_context_attr_t * cattr;     /* pointer to context attribute object */
	lbm_topic_t *stopic;            /* Source Topic object */
	lbm_src_t *src;                 /* Source object */
	lbm_src_topic_attr_t * tattr;   /* Source topic attribute object */
	lbm_rcv_t *rcv;                 /* Receive object: for subscribing to messages. */
	lbm_topic_t *rtopic;            /* Receiver Topic object */
	lbm_event_queue_t *evq = NULL;  /* pointer to eventQ handle */
	int err;                        /* return status of lbm functions (true=error) */
#if defined(_WIN32)
        HANDLE wthrdh;
#else
        pthread_t pthid;
#endif /* _WIN32 */


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

	err = lbm_event_queue_create(&evq, NULL, NULL, NULL);
	EX_LBM_CHK(err);

	err = lbm_context_attr_create(&cattr);
	EX_LBM_CHK(err);

	err = lbm_context_create(&ctx, cattr, NULL, NULL);
	EX_LBM_CHK(err);

	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", NULL);
	EX_LBM_CHK(err);

	err = lbm_rcv_create(&rcv, ctx, rtopic, rcv_handle_msg, NULL, evq);
	EX_LBM_CHK(err);

	err = lbm_src_topic_attr_create(&tattr);
	EX_LBM_CHK(err);

	err = lbm_src_topic_alloc(&stopic, ctx, "test.topic", tattr);
	EX_LBM_CHK(err);

	err = lbm_src_topic_attr_delete(tattr);
	EX_LBM_CHK(err);

	err = lbm_src_create(&src, ctx, stopic, NULL, NULL, NULL);
	EX_LBM_CHK(err);

#if defined(_WIN32)
        if ((wthrdh = CreateThread(NULL, 0, eq_thread, evq, 0, NULL)) == NULL) {
                fprintf(stderr, "could not create thread\n");
                exit(1);
        }
#else
        if (pthread_create(&pthid, NULL, eq_thread, evq) != 0) {
                fprintf(stderr, "could not spawn thread\n");
                exit(1);
        }
#endif /* _WIN32 */

	SLEEP(1);

	err = lbm_src_send(src, "hello", 5, LBM_MSG_FLUSH);
	EX_LBM_CHK(err);	

	/* Wait */
	while (!cleanup) {  
		SLEEP(1);
	}

	lbm_event_dispatch_unblock(evq);

#if defined(_WIN32)
                WaitForSingleObject(wthrdh, INFINITE);
#else
                pthread_join(pthid, NULL);
#endif /* _WIN32 */

	lbm_rcv_delete(rcv);
	lbm_src_delete(src);
	lbm_context_delete(ctx);
	lbm_event_queue_delete(evq);
}

