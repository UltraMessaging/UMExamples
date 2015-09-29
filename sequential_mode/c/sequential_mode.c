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


#if defined(_WIN32)
DWORD WINAPI seq_thread(void *arg)
#else
void *seq_thread(void *arg)
#endif /* _WIN32 */
{
	lbm_context_t *ctx = (lbm_context_t *) arg;

	while (1)
	{
		if (lbm_context_process_events(ctx, 500) == -1)
		{
			printf("ERROR processing context events: %s\n", lbm_errmsg());
			exit(1);
		}
	}
}

main()
{
	lbm_context_t *ctx;             /* pointer to context object */
	lbm_context_attr_t * cattr;     /* pointer to context attribute object */
	int err;                        /* return status of lbm functions (true=error) */
#if defined(_WIN32)
        HANDLE wthrdh;
#else
        pthread_t pthids;
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

	if (lbm_context_attr_create(&cattr) != 0)
	{
		fprintf(stderr, "lbm_context_attr_create: %s\n", lbm_errmsg());
		exit(1);
	}

	/* Setting the resolver address using the string method */
	if (lbm_context_attr_str_setopt(cattr, "operational_mode", "sequential") != 0) 
	{
		fprintf(stderr, "lbm_context_attr_str_setopt:operational_mode: %s\n", lbm_errmsg());
		exit(1);
	}

        err = lbm_context_create(&ctx, cattr, NULL, NULL);
        if (err)
        {
                printf("line %d: %s\n", __LINE__, lbm_errmsg());
                exit(1);
        }

#if defined(_WIN32)
	if ((wthrdh = CreateThread(NULL, 0, seq_thread, ctx, 0, NULL)) == NULL) {
		fprintf(stderr, "could not create thread\n");
		exit(1);
	}
#else
	if (pthread_create(&pthids, NULL, seq_thread, ctx) != 0) {
		fprintf(stderr, "could not spawn thread\n");
		exit(1);
	}
#endif /* _WIN32 */

	/* Wait forever */
	while (1) { }
}

