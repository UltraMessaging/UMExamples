/* Code Disclaimer? */

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

int src_ready = 0;

/* Source event handler callback (passed into lbm_src_create()) */
int handle_src_event(lbm_src_t *src, int event, void *ed, void *cd)
{
	switch (event) {
		case LBM_SRC_EVENT_UME_REGISTRATION_COMPLETE_EX:
		{
			src_ready = 1;
			break;
		}
		case LBM_SRC_EVENT_UME_STORE_UNRESPONSIVE:
		{
			const char *infostr = (const char *)ed;
			if (strstr(infostr, "quorum lost") != NULL)
				src_ready = 0;
			break;
		}
	}
	return 0;
}

main()
{
        lbm_context_t *ctx;                     /* Context object */
        lbm_topic_t *topic;                     /* Topic object */
        lbm_src_t *src;                         /* Source object */
        lbm_src_topic_attr_t * tattr;           /* Source topic attribute object */
        lbm_context_attr_t * cattr;             /* Context attribute object */
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

        if (lbm_context_attr_create(&cattr) != 0)
        {
                fprintf(stderr, "lbm_context_attr_create: %s\n", lbm_errmsg());
                exit(1);
        }

        err = lbm_context_create(&ctx, cattr, NULL, NULL);
        if (err)
        {
                printf("line %d: %s\n", __LINE__, lbm_errmsg());
                exit(1);
        }

        if (lbm_src_topic_attr_create(&tattr) != 0)
        {
                fprintf(stderr, "lbm_src_topic_attr_create: %s\n", lbm_errmsg());
                exit(1);
        }

        /* Assuming there is a store running on the localhost at port 29999 */
        if (lbm_src_topic_attr_str_setopt(tattr, "ume_store", "127.0.0.1:29999") != 0 )
        {
                fprintf(stderr, "lbm_src_topic_attr_str_setopt:ume_store: %s\n", lbm_errmsg());
                exit(1);
        }

	/* Need to set store behavior to Quorum-Consensus  */
	if (lbm_src_topic_attr_str_setopt(tattr, "ume_store_behavior", "qc") != 0 )
	{
		fprintf(stderr, "lbm_src_topic_attr_str_setopt:ume_store_behavior: %s\n", lbm_errmsg());
		exit(1);
	}

        err = lbm_src_topic_alloc(&topic, ctx, "test.topic", tattr);
        if (err)
        {
                printf("lbm_src_topic_alloc - line %d: %s\n", __LINE__, lbm_errmsg());
                exit(1);
        }

        err = lbm_src_create(&src, ctx, topic, handle_src_event, NULL, NULL);
        if (err)
        {
                printf("lbm_src_create - line %d: %s\n", __LINE__, lbm_errmsg());
                exit(1);
        }

	while(1)
	{
		if (src_ready)
		{
			if (lbm_src_send(src, "test", 4, LBM_SRC_NONBLOCK) == LBM_FAILURE)
			{
				/* Assume EWOULDBLOCK and wait */
				SLEEP_SEC(1);
			}
		}
		else
		{
			/* Print a warning that the source is not ready and sleep */
			printf("Source is not ready to send (no quorum)\n");
			SLEEP_SEC(1);
		}
	}
}
