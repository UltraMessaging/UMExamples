/* source_notify_callback.c - http://ultramessaging.github.io/UMExamples */
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

typedef struct source_state_s {
  int msgs_rcvd;  /* We want to track message count from each source sending to the topic. */
} source_state_t;

/* Example error checking macro.  Include after each UM call. */
#define EX_LBM_CHK(err) do { \
        if ((err) < 0) { \
                fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
                __FILE__, __LINE__, lbm_errmsg()); \
                exit(1); \
        }  \
} while (0)

void *new_src_notification_callback(const char *source_name, void *clientd)
{
        source_state_t *source_state = (source_state_t *)malloc(sizeof(source_state_t));
        source_state->msgs_rcvd = 0;

        printf("Delivery Controller Created: %s\n", source_name);
        return source_state;  /* This will be available in the receive callback as msg->source_clientd. */
}

int src_delete_notification_callback(const char *source_name, void *clientd, void* src_clientd )
{
        free(src_clientd);  /* This was created by new_src_notification_callback() */

        printf("Delivery Controller Deleted: %s\n", source_name);
        return 0;
}

/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
        int err;
        source_state_t *source_state = (source_state_t *)msg->source_clientd;

        switch (msg->type) {
        case LBM_MSG_DATA:
                source_state->msgs_rcvd ++;
                printf("[%s][%s], Received message %d\n", msg->topic_name, msg->source, source_state->msgs_rcvd);
                break;
        case LBM_MSG_BOS:
                printf("[%s][%s], Beginning of Transport Session\n", msg->topic_name, msg->source);
                break;
        case LBM_MSG_EOS:
                printf("[%s][%s], End of Transport Session\n", msg->topic_name, msg->source);
                break;
        default:
                printf("Other event, type=%x\n", msg->type);
                break;
        }

        return 0;
}

int main(int argc, char **argv)
{
        lbm_context_t *ctx;                     /* Context object */
        lbm_context_attr_t * cattr;             /* Context attribute object */
        lbm_rcv_t *rcv;                         /* Receive object: for subscribing to messages. */
        lbm_topic_t *rtopic;                    /* Receiver Topic object */
        lbm_rcv_topic_attr_t * rattr;           /* Receiver attribute object */
        lbm_rcv_src_notification_func_t srccb;  /* Source notify callback structure */
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

        /* Create receiver for receiving request and sending response */
        err = lbm_rcv_topic_attr_create(&rattr);
        EX_LBM_CHK(err);

        srccb.create_func = new_src_notification_callback;
        srccb.delete_func = src_delete_notification_callback;

        err = lbm_rcv_topic_attr_setopt(rattr, "source_notification_function", &srccb, sizeof(srccb));
        EX_LBM_CHK(err);

        err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", rattr);
	EX_LBM_CHK(err);

        err = lbm_rcv_create(&rcv, ctx, rtopic, rcv_handle_msg, NULL, NULL);
        EX_LBM_CHK(err);

        while (1) { }

        err = lbm_rcv_delete(rcv);
        EX_LBM_CHK(err);

        err = lbm_context_delete(ctx);
        EX_LBM_CHK(err);

#if defined(_MSC_VER)
        /* Windows-specific cleanup overhead */
        WSACleanup();
#endif
        return 0;
} /* main */
