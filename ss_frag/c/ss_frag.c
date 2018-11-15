/* ss_frag.c - see http://ultramessaging.github.io/UMExamples/ss_frag/c/index.html
 *
 * Copyright (c) 2005-2018 Informatica Corporation. All Rights Reserved.
 * Permission is granted to licensees to use
 * or alter this software for any purpose, including commercial applications,
 * according to the terms laid out in the Software License Agreement.
 *
 * This source code example is provided by Informatica for educational
 * and evaluation purposes only.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INFORMATICA DISCLAIMS ALL WARRANTIES
 * EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
 * PURPOSE.  INFORMATICA DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE.  INFORMATICA SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
 * LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
 * INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
 * TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF INFORMATICA HAS BEEN APPRISED OF
 * THE LIKELIHOOD OF SUCH DAMAGES.
 */

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


/* Application structure associated with a smart source.
 */
typedef struct {
  lbm_context_t *ctx;
  lbm_ssrc_t *ssrc;
  char *msgbuf_plain;
  char *msgbuf_props;
  int max_msg_len;
  lbm_ssrc_send_ex_info_t info_prop;
  lbm_uint32_t int_value_array[2];
  char *key_ptr_array[2];
  char remain_key[8];
} smart_source_t;


/* Receiver state used to reassemble fragmented messages.  Used as per-source
 * clientd.
 */
typedef struct {
  int collecting;  /* 1=true, 0=false. */
  char *reassem_buf;
  size_t reassem_buf_size;
  size_t message_len;  /* Size of message being reassembled. */
  size_t offset;  /* Current offset within reassem_buf[]. */
  long num_bad_frags;  /* Statistic: bad fragemnts. */
} rcv_state_t;


/*
 * Simple lbm error handling.
 */

/* Macro to approximate the basename() function. */
#define BASENAME(_p) ((strrchr(_p, '/') == NULL) ? (_p) : (strrchr(_p, '/')+1))

/* Pass in an LBM return status.  If not success, print error and abort. */
#define LBM_ERR(_e) do {\
  if ((_e) != LBM_OK) {\
    printf("LBM_ERR: at %s:%d (%s)\n",\
      BASENAME(__FILE__), __LINE__, lbm_errmsg());\
    fflush(stdout);\
    abort();\
  }\
} while (0)

/* Print error and abort. */
#define ERR(_s) do {\
  printf("ERR: at %s:%d (%s)\n",\
    BASENAME(__FILE__), __LINE__, _s);\
  fflush(stdout);\
  abort();\
} while (0)


/* Called by receiver callback when a received messager has the "Remain"
 * message property, indicating a fragmented message.
 */
void handle_msg_frag(lbm_msg_t *msg, rcv_state_t *rcv_state,
  lbm_int32_t remaining)
{
  size_t tot_len;

  if (rcv_state->collecting) {
    /* Make sure the fragment is OK. */
    if (remaining >= 0 &&
      (rcv_state->message_len == (rcv_state->offset + msg->len + remaining)))
    {
      /* Collect the fragment. */
      memcpy(&rcv_state->reassem_buf[rcv_state->offset], msg->data,
        msg->len);
      rcv_state->offset += msg->len;

      if (remaining == 0) {  /* No more, deliver the data. */
        printf("PROCESS message, buf[0]=%d, buf[%ld]=%d\n",
          rcv_state->reassem_buf[0],
          rcv_state->message_len - 1,
          rcv_state->reassem_buf[rcv_state->message_len - 1]);

        /* No longer collecting. */
        rcv_state->collecting = 0;
      }
    }  /* if fragment OK */
    else {  /* fragment is not ok. */
      /* Bad fragment (should never happen). */
      printf("Collect error: bad fragment, remaining=%d, offset=%ld, len=%lu, message_len=%ld\n", remaining, rcv_state->offset, msg->len, rcv_state->message_len);
      rcv_state->num_bad_frags ++;

      /* Error, stop collecting. */
      rcv_state->collecting = 0;
    }
  }

  else {  /* not collecting */
    /* Not collecting a fragmented message, is this message first frag? */
    if (remaining < 0) {
      /* First fragment. */
      rcv_state->collecting = 1;
      rcv_state->offset = 0;
      rcv_state->message_len = msg->len + (- remaining);
      /* Expand buffer if necessary. */
      if (rcv_state->message_len > rcv_state->reassem_buf_size) {
        rcv_state->reassem_buf_size = rcv_state->message_len;
        rcv_state->reassem_buf = (char *)realloc(rcv_state->reassem_buf,
          rcv_state->reassem_buf_size);
      }

      /* Collect data. */
      memcpy(&rcv_state->reassem_buf[rcv_state->offset], msg->data, msg->len);
      rcv_state->offset += msg->len;
    }  /* remaining < 0 */
    else {  /* Found middle frag without a first frag; discard. */
      /* Probably joined stream in middle of message. */
      printf("info: found middle frag but not collecting, remaining=%d, offset=%ld, len=%lu, message_len=%ld\n", remaining, rcv_state->offset, msg->len, rcv_state->message_len);
      rcv_state->num_bad_frags ++;
    }
  }  /* not collecting */
}  /* handle_msg_frag */


/* UM receiver callback.
 */
int msg_rcv_cb(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
  rcv_state_t *rcv_state = (rcv_state_t *)msg->source_clientd;
  int err;

  switch (msg->type) {
    case LBM_MSG_DATA:
      if (msg->properties == NULL) {
        /* Fast path (not part of a fragmented messqge). */
        if (rcv_state->collecting) {
          /* Error, non-frag, but state is collecting (should never happen). */
          printf("Collect error non-frag, offset=%ld, len=%lu, message_len=%ld\n", rcv_state->offset, msg->len, rcv_state->message_len);
          rcv_state->collecting = 0;
          rcv_state->num_bad_frags ++;
        }

        /* Deliver data message. */
        printf("PROCESS message, buf[0]=%d, buf[%ld]=%d\n",
          msg->data[0],
          msg->len - 1,
          msg->data[msg->len - 1]);
      }
      else {  /* msg properties */
        lbm_int32_t remaining;
        size_t prop_size = sizeof(remaining);
        int prop_type = LBM_MSG_PROPERTY_INT;

        /* Found property, is it for fragmentation? */
        err = lbm_msg_properties_get(msg->properties, "Remain",
          &remaining, &prop_type, &prop_size);
        if (err == LBM_OK) {
          handle_msg_frag(msg, rcv_state, remaining);
        }
        else {
          printf("handle messages with other properties.\n");
        }
      }  /* msg properties not null */
      break;

    case LBM_MSG_UNRECOVERABLE_LOSS:
    case LBM_MSG_UNRECOVERABLE_LOSS_BURST:
        if (rcv_state->collecting) {
          /* Stop collecting after error. */
          rcv_state->collecting = 0;
          rcv_state->num_bad_frags ++;
        }

    default:
      printf("Non data receiver event, type=%d\n", msg->type);
  }  /* switch msg type */

  return 0;
}  /* msg_rcv_cb */


/* Callback when a receiver delivery controller is created for a new source. */
void *per_src_clientd_create(const char *source_name, void *clientd)
{
  rcv_state_t *rcv_state;

  rcv_state = (rcv_state_t *)malloc(sizeof(rcv_state_t));

  rcv_state->reassem_buf_size = 8192;
  rcv_state->reassem_buf = (char *)malloc(rcv_state->reassem_buf_size);
  rcv_state->collecting = 0;
  rcv_state->num_bad_frags = 0;

  return rcv_state;
}  /* per_src_clientd_create */


/* Callback when a receiver delivery controller for an ex-source is deleted. */
int per_src_clientd_delete(const char *source_name, void *clientd,
  void *src_clientd)
{
  rcv_state_t *rcv_state = (rcv_state_t *)src_clientd;

  if (rcv_state->collecting) {
    /* Error, state is collecting. */
    printf("Collect error, incomplete fragmented message, offset=%ld, message_len=%ld\n", rcv_state->offset, rcv_state->message_len);
  }
  if (rcv_state->num_bad_frags > 0) {
    printf("Number of bad fragments=%ld\n", rcv_state->num_bad_frags);
  }

  free(rcv_state->reassem_buf);
  free(rcv_state);

  return 0;
}  /* per_src_clientd_delete */


/* Create a SmartSource, and init associated state.
 */
void smart_source_init(smart_source_t *smart_source, lbm_context_t *ctx,
  char *topic)
{
  lbm_topic_t *lbm_topic = NULL;
  lbm_src_topic_attr_t *src_tattr = NULL;
  int prop_count;
  size_t opt_len;
  int err;

  smart_source->ctx = ctx;

  err = lbm_src_topic_attr_create(&src_tattr);
  LBM_ERR(err);

  /* Make sure user's config allows at least 1 msg property. */
  opt_len = sizeof(prop_count);
  err = lbm_src_topic_attr_getopt(src_tattr,
    "smart_src_message_property_int_count", &prop_count, &opt_len);
  LBM_ERR(err);
  if (prop_count == 0) {
    /* No props configured, add one. */
    prop_count = 1;
    opt_len = sizeof(prop_count);
    err = lbm_src_topic_attr_setopt(src_tattr,
      "smart_src_message_property_int_count", &prop_count, opt_len);
    LBM_ERR(err);
  }

  /* Find out the user's config for max message length. */
  opt_len = sizeof(smart_source->max_msg_len);
  err = lbm_src_topic_attr_getopt(src_tattr,
    "smart_src_max_message_length", &(smart_source->max_msg_len), &opt_len);
  LBM_ERR(err);

  err = lbm_src_topic_alloc(&lbm_topic, ctx, topic, src_tattr);
  LBM_ERR(err);

  err = lbm_ssrc_create(&(smart_source->ssrc), ctx, lbm_topic, NULL, NULL,
    NULL);
  LBM_ERR(err);

  err = lbm_src_topic_attr_delete(src_tattr);
  LBM_ERR(err);

  err = lbm_ssrc_buff_get(smart_source->ssrc, &smart_source->msgbuf_plain, 0);
  LBM_ERR(err);
  err = lbm_ssrc_buff_get(smart_source->ssrc, &smart_source->msgbuf_props, 0);
  LBM_ERR(err);

  /* Set up message property. */
  smart_source->info_prop.flags = LBM_SSRC_SEND_EX_FLAG_PROPERTIES;
  smart_source->info_prop.mprop_int_cnt = 1;
  smart_source->info_prop.mprop_int_vals = smart_source->int_value_array;
  smart_source->info_prop.mprop_int_keys = &smart_source->key_ptr_array[0];
  smart_source->key_ptr_array[0] = smart_source->remain_key;
  strncpy(smart_source->remain_key, "Remain", sizeof(smart_source->remain_key));
}  /* smart_source_init */


/* Send a message, fragmenting it if necessary.
 */
void smart_source_send(smart_source_t *smart_source, char *buf, size_t len,
  int flags)
{
  int err;

  if (len <= smart_source->max_msg_len) {
    /* Message fits in one buffer, send it. */
    memcpy(smart_source->msgbuf_plain, buf, len);
    err = lbm_ssrc_send_ex(smart_source->ssrc, smart_source->msgbuf_plain,
      len, 0, NULL);
    LBM_ERR(err);
  }

  else {  /* Message does not fit in one buffer */
    int offset;
    int remaining;
    int this_len;

    /* Fragment message. */
    remaining = len;
    offset = 0;

    this_len = smart_source->max_msg_len;
    remaining -= this_len;
    /* Indicate first fragment with negative remainer. */
    smart_source->int_value_array[0] = - remaining;
    memcpy(smart_source->msgbuf_props, &buf[offset], this_len);
    err = lbm_ssrc_send_ex(smart_source->ssrc, smart_source->msgbuf_props,
      this_len, 0, &(smart_source->info_prop));
    LBM_ERR(err);
    offset += this_len;

    /* From now on, need to update the properties. */
    smart_source->info_prop.flags |= LBM_SSRC_SEND_EX_FLAG_UPDATE_PROPERTY_VALUES;

    /* Send rest of fragments. */
    while (remaining > 0) {
      if (remaining > smart_source->max_msg_len) {
        this_len = smart_source->max_msg_len;
      }
      else {
        this_len = remaining;
      }
      remaining -= this_len;
      smart_source->int_value_array[0] = remaining;
      memcpy(smart_source->msgbuf_props, &buf[offset], this_len);
      err = lbm_ssrc_send_ex(smart_source->ssrc, smart_source->msgbuf_props,
        this_len, 0, &(smart_source->info_prop));

      offset += this_len;
    }  /* while */
  }
}  /* smart_source_send */


int main(int argc, char **argv) {
  lbm_context_t *rctx = NULL;
  lbm_context_t *sctx = NULL;
  lbm_rcv_topic_attr_t *rcv_attr = NULL;
  lbm_rcv_src_notification_func_t src_notif_func;  /* config option */
  lbm_topic_t *lbm_topic = NULL;
  lbm_rcv_t *rcv;
  smart_source_t smart_source;
  char *send_buff;
  int err;

  err = lbm_config("test.cfg");
  LBM_ERR(err);

  /* Create a pair of contexts, one for publisher, the other for subscriber.
   */

  err = lbm_context_create(&rctx, NULL, NULL, NULL);
  LBM_ERR(err);

  err = lbm_context_create(&sctx, NULL, NULL, NULL);
  LBM_ERR(err);


  /* Create receiver.
   */

  err = lbm_rcv_topic_attr_create(&rcv_attr);
  LBM_ERR(err);

  src_notif_func.create_func = per_src_clientd_create;
  src_notif_func.delete_func = per_src_clientd_delete;
  src_notif_func.clientd = NULL;  /* Or your own state structure. */
  err = lbm_rcv_topic_attr_setopt(rcv_attr, "source_notification_function",
    &src_notif_func, sizeof(src_notif_func));
  LBM_ERR(err);

  err = lbm_rcv_topic_lookup(&lbm_topic, rctx, "topic_1", rcv_attr);
  LBM_ERR(err);

  err = lbm_rcv_create(&rcv, rctx, lbm_topic, msg_rcv_cb, NULL, NULL);
  LBM_ERR(err);


  /* Create source.
   */
  smart_source_init(&smart_source, sctx, "topic_1");

  send_buff = (char *)malloc(70000); /* Bigger than 64k, for fun. */

  SLEEP(1);  /* For topic res. */


  /* Send messages.
   */

  send_buff[0] = 1;  send_buff[999] = 1;
  smart_source_send(&smart_source, send_buff, 1000, LBM_MSG_FLUSH);
  SLEEP(1);

  send_buff[0] = 2;  send_buff[4999] = 2;
  smart_source_send(&smart_source, send_buff, 5000, LBM_MSG_FLUSH);
  SLEEP(1);

  send_buff[0] = 3;  send_buff[19999] = 3;
  smart_source_send(&smart_source, send_buff, 20000, LBM_MSG_FLUSH);
  SLEEP(1);

  send_buff[0] = 4;  send_buff[99] = 4;
  smart_source_send(&smart_source, send_buff, 100, LBM_MSG_FLUSH);
  SLEEP(1);

  send_buff[0] = 5;  send_buff[69999] = 5;
  smart_source_send(&smart_source, send_buff, 70000, LBM_MSG_FLUSH);
  SLEEP(1);


  /* Shut down.
   */

  free(send_buff);

  (void)lbm_ssrc_delete(smart_source.ssrc);
  (void)lbm_rcv_delete(rcv);

  (void)lbm_context_delete(rctx);
  (void)lbm_context_delete(sctx);

  return 0;
}  /* main */
