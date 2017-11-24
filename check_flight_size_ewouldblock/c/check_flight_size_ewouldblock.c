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

#define DEFAULT_APP_FLIGHT_SIZE 500		/* Used for checking flight size */

int main(int argc, char **argv)
{
	lbm_context_t *ctx;			/* Context object */
	lbm_topic_t *topic;			/* Topic object */
	lbm_src_t *src;				/* Source object */
	lbm_src_topic_attr_t * tattr;		/* Source topic attribute object */
	lbm_context_attr_t * cattr;		/* Context attribute object */
	int inflight;	                        /* Object used for checking inflight counters */
	char flight[4];
	int err;				/* Used for checking API return codes */
	
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

	sprintf(flight, "%d", DEFAULT_APP_FLIGHT_SIZE);
	if (lbm_src_topic_attr_str_setopt(tattr, "ume_flight_size", flight) != 0 )
	{
		fprintf(stderr, "lbm_src_topic_attr_str_setopt:transport: %s\n", lbm_errmsg());
		exit(1);
	}

	/* Assuming there is a store running on the localhost at port 29999 */
	if (lbm_src_topic_attr_str_setopt(tattr, "ume_store", "127.0.0.1:29999") != 0 )
		{
		fprintf(stderr, "lbm_src_topic_attr_str_setopt:transport: %s\n", lbm_errmsg());
		exit(1);
	}

	err = lbm_src_topic_alloc(&topic, ctx, "test.topic", tattr);
	if (err)
	{
		printf("lbm_src_topic_alloc - line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	err = lbm_src_create(&src, ctx, topic, NULL, NULL, NULL);
	if (err)
	{
		printf("lbm_src_create - line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	while(1)
	{
		if (lbm_src_send(src, "test", 4, LBM_SRC_NONBLOCK) == LBM_FAILURE)
		{
			if (lbm_errnum() == LBM_EWOULDBLOCK)
                        {
				if (lbm_src_get_inflight(src, LBM_FLIGHT_SIZE_TYPE_UME, &inflight, NULL, NULL) == 0) 
				{
					/* Check to see if the inflight counter is equal to the configured flight size */
					if (inflight == DEFAULT_APP_FLIGHT_SIZE)
					{
						/* Print alert */
						printf("ALERT: Source is blocked on flight size: %d\n", inflight);
					}
				}
			}
		}
	}
        return 0;
}


