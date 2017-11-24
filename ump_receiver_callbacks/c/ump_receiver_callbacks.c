/* ump_receiver_callbacks.c - http://ultramessaging.github.io/UMExamples */

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

/* Callback used to handle request message for receiver */
int rcv_handle_msg(lbm_rcv_t *rcv, lbm_msg_t *msg, void *clientd)
{
	int err;

	switch (msg->type) {
		case LBM_MSG_DATA:
			printf("DATA [%s][%s][%d]%s%s, %u bytes\n", msg->topic_name, msg->source, msg->sequence_number,
				((msg->flags & LBM_MSG_FLAG_UME_RETRANSMIT) ? "-RX-" : ""),
				((msg->flags & LBM_MSG_FLAG_OTR) ? "-OTR-" : ""), (unsigned int)msg->len);
			break;
		case LBM_MSG_REQUEST:
			printf("REQUEST [%s][%s][%d]%s%s, %u bytes\n", msg->topic_name, msg->source, msg->sequence_number,
                                ((msg->flags & LBM_MSG_FLAG_UME_RETRANSMIT) ? "-RX-" : ""),
                                ((msg->flags & LBM_MSG_FLAG_OTR) ? "-OTR-" : ""), (unsigned int)msg->len);
			break;
		case LBM_MSG_UNRECOVERABLE_LOSS:
			printf("[%s][%s][%x], LOST\n", msg->topic_name, msg->source, msg->sequence_number);
			break;
		case LBM_MSG_UNRECOVERABLE_LOSS_BURST:
			printf("[%s][%s][%x], LOST BURST\n", msg->topic_name, msg->source, msg->sequence_number);
			break;
		case LBM_MSG_BOS:
			printf("[%s][%s], Beginning of Transport Session\n", msg->topic_name, msg->source);
			break;
		case LBM_MSG_EOS:
			printf("[%s][%s], End of Transport Session\n", msg->topic_name, msg->source);
			break;
		case LBM_MSG_NO_SOURCE_NOTIFICATION:
			printf("[%s], no sources found for topic\n", msg->topic_name);
			break;
		case LBM_MSG_UME_REGISTRATION_ERROR:
			printf("[%s][%s] UME registration error: %s\n", msg->topic_name, msg->source, msg->data);
			break;
		case LBM_MSG_UME_REGISTRATION_SUCCESS_EX:
		{
			lbm_msg_ume_registration_ex_t *reg = (lbm_msg_ume_registration_ex_t *)(msg->data);
			printf("[%s][%s] store %u: %s UME registration successful. SrcRegID %u RcvRegID %u. Flags %x ",
				msg->topic_name, msg->source, reg->store_index, reg->store,
				reg->src_registration_id, reg->rcv_registration_id, reg->flags);
			if (reg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_OLD)
				printf("OLD[SQN %x] ", reg->sequence_number);
			if (reg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_NOCACHE)
				printf("NOCACHE ");
			if (reg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_RPP)
				printf("RPP ");
			if (reg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_SRC_SID)
				printf("Src Session ID 0x%" PRIx64 " ", reg->src_session_id);
			printf("\n");
			break;
		}
		case LBM_MSG_UME_REGISTRATION_COMPLETE_EX:
                {
			lbm_msg_ume_registration_complete_ex_t *reg = (lbm_msg_ume_registration_complete_ex_t *)(msg->data);
			printf("[%s][%s] UME registration complete. SQN %x. Flags %x ",
				msg->topic_name, msg->source, reg->sequence_number, reg->flags);
			if (reg->flags & LBM_MSG_UME_REGISTRATION_COMPLETE_EX_FLAG_QUORUM)
				printf("QUORUM ");
			if (reg->flags & LBM_MSG_UME_REGISTRATION_COMPLETE_EX_FLAG_RXREQMAX)
				printf("RXREQMAX ");
			if (reg->flags & LBM_MSG_UME_REGISTRATION_COMPLETE_EX_FLAG_SRC_SID)
				printf("Src Session ID 0x%" PRIx64 " ", reg->src_session_id);
			printf("\n");
			break;
		}
		case LBM_MSG_UME_DEREGISTRATION_SUCCESS_EX:
		{
			lbm_msg_ume_deregistration_ex_t *dereg = (lbm_msg_ume_deregistration_ex_t *)(msg->data);
			printf("[%s][%s] store %u: %s UME deregistration successful. SrcRegID %u RcvRegID %u. Flags %x ",
				msg->topic_name, msg->source, dereg->store_index, dereg->store,
				dereg->src_registration_id, dereg->rcv_registration_id, dereg->flags);
			if (dereg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_OLD)
				printf("OLD[SQN %x] ", dereg->sequence_number);
			if (dereg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_NOCACHE)
				printf("NOCACHE ");
			if (dereg->flags & LBM_MSG_UME_REGISTRATION_SUCCESS_EX_FLAG_RPP)
				printf("RPP ");
			printf("\n");
			break;
		}
		case LBM_MSG_UME_DEREGISTRATION_COMPLETE_EX:
			printf("[%s][%s] UME deregistration complete.\n", msg->topic_name, msg->source);
			break;
		case LBM_MSG_UME_REGISTRATION_CHANGE:
			printf("[%s][%s] UME registration change: %s\n", msg->topic_name, msg->source, msg->data);
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
	err = lbm_rcv_topic_lookup(&rtopic, ctx, "test.topic", NULL);
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
