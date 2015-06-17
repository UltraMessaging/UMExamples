/* main.c - main driver for example app; mostly boilerplate. */

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

/* Example error checking program.  Include after each UM call. */
#define EX_LBM_CHK(err) do { \
  if ((err) < 0) { \
    fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
      __FILE__, __LINE__, lbm_errmsg()); \
    exit(1); \
  }  \
} while (0)


/*** Include the example code ***/
#include "minsrc.c"

int main(int argc, char **argv)
{
  lbm_context_t *ctx;  /* Context object: container for UM "instance". */
  lbm_src_t *src;      /* Source object: for sending messages. */
  int err;             /* UM functions normally return 0 (success) or -1 (error). */

#if defined(_MSC_VER)
  /* Windows-specific startup overhead */
  WSADATA wsadata;
  int wsa_err = WSAStartup(MAKEWORD(2,2), &wsadata);
  if (wsa_err != 0) {
    printf("line %d: wsStat=%d\n",__LINE__,wsStat);
    fprintf(stderr, "%s:%d, WSAStartup error: %d\n",
      __FILE__, __LINE__, wsa_err);
    exit(1);
  }
#endif


  /*** Initialization: create necessary UM objects. ***/

  err = lbm_context_create(&ctx, NULL, NULL, NULL);
  EX_LBM_CHK(err);

  {
    lbm_topic_t *topic;    /* Topic object: only needed temporarily. */

    err = lbm_src_topic_alloc(&topic, ctx, "Greeting", NULL);
    EX_LBM_CHK(err);

    err = lbm_src_create(&src, ctx, topic, NULL, NULL, NULL);
    EX_LBM_CHK(err);
  }

  SLEEP(1);  /* Let topic resolution execute. */


  /*** Send a message. ***/

  err = lbm_src_send(src, "Hello!", 6, LBM_MSG_FLUSH | LBM_SRC_BLOCK);
  EX_LBM_CHK(err);


  /*** Cleanup: delete UM objects. ***/

  SLEEP(3);  /* Linger a bit to allow retransmissions. */

  err = lbm_src_delete(src);
  EX_LBM_CHK(err);

  err = lbm_context_delete(ctx);
  EX_LBM_CHK(err);


#if defined(_MSC_VER)
  /* Windows-specific cleanup overhead */
  WSACleanup();
#endif
}  /* main */
