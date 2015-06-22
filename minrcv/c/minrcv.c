/* minrcv.c, see http://ultramessaging.github.io/UMExamples/minrcv/c/ */

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


int app_rcv_callback(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
  switch (msg->type) {
  case LBM_MSG_DATA:  /* an actual message is received */
    printf("Received %d bytes on topic %s: '%.*s'\n",
           msg->len, msg->topic_name, msg->len, msg->data);
    break;

  default:
    printf("Received lbm_msg_t type %x on topic %s\n",
           msg->type, msg->topic_name);
    break;
  }  /* switch */

  return 0;  /* UM callbacks must return 0 */
}  /* app_rcv_callback */


int main(int argc, char **argv)
{
  lbm_context_t *ctx;  /* Context object: container for UM "instance". */
  lbm_rcv_t *rcv;      /* Source object: for subscribing to messages. */
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

    err = lbm_rcv_topic_lookup(&topic, ctx, "Greeting", NULL);
    EX_LBM_CHK(err);

    err = lbm_rcv_create(&rcv, ctx, topic, app_rcv_callback, NULL, NULL);
    EX_LBM_CHK(err);
  }

  for (;;) {  /* loop forever */
    SLEEP(1);
  }


  /* Since app is written as infinite loop, will never get here. */
  err = lbm_src_delete(src);
  EX_LBM_CHK(err);

  err = lbm_context_delete(ctx);
  EX_LBM_CHK(err);


#if defined(_MSC_VER)
  /* Windows-specific cleanup overhead */
  WSACleanup();
#endif
}  /* main */
