/* UMPReceiverCallbacks.java, see http://ultramessaging.github.io/UMExamples/ump_receiver_callbacks/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;


public class UMPReceiverCallbacks {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null;           /* Context object: container for UM "instance". */
		LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
		LBMTopic rtopic = null;          /* Topic object */

		/* Initialize and create receiver and receiver callback */
		ReceiverCallback myReceiverCallback = new ReceiverCallback();

		/*** Initialization: create necessary UM objects. ***/
		ctx = new LBMContext();

		rtopic = new LBMTopic(ctx, "test.topic");
		rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

		while(true)
		{

		}
	} /* main */

	/* Class for receiving messages */
	private static class ReceiverCallback implements LBMReceiverCallback {
		public int onReceive(Object cbArg, LBMMessage msg)
		{
			switch (msg.type())
			{
				case LBM.MSG_DATA:
					System.out.print("[" + msg.topicName() + "][" + msg.source() + "][" + msg.sequenceNumber() + "]");
					if ((msg.flags() & LBM.MSG_FLAG_UME_RETRANSMIT) != 0) {
						System.out.print("-RX-");
					}
					if ((msg.flags() & LBM.MSG_FLAG_OTR) != 0) {
						System.out.print("-OTR-");
					}
					System.out.print(", ");
					System.out.println(msg.data().length + " bytes");
					break;
				case LBM.MSG_REQUEST:
					if ((msg.flags() & LBM.MSG_FLAG_UME_RETRANSMIT) != 0) {
                                                System.out.print("-RX-");
                                        }
                                        if ((msg.flags() & LBM.MSG_FLAG_OTR) != 0) {
                                                System.out.print("-OTR-");
                                        }
                                        System.out.print(", ");
                                        System.out.println(msg.data().length + " bytes");
                                        break;
				case LBM.MSG_BOS:
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
					break;
				case LBM.MSG_EOS:
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
					break;
				case LBM.MSG_UNRECOVERABLE_LOSS:
					System.err.println("[" + msg.topicName() + "][" + msg.source() + "][" + Long.toHexString(msg.sequenceNumber()) + "], LOST");
					break;
				case LBM.MSG_UNRECOVERABLE_LOSS_BURST:
					System.err.println("[" + msg.topicName() + "][" + msg.source() + "][" + Long.toHexString(msg.sequenceNumber()) + "], LOST BURST");
					break;

				case LBM.MSG_UME_REGISTRATION_ERROR:
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "] UME registration error: " + msg.dataString());
					break;
				case LBM.MSG_UME_REGISTRATION_SUCCESS_EX:
					UMERegistrationSuccessInfo reg = msg.registrationSuccessInfo();
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "] store " + reg.storeIndex() + ": "
						+ reg.store() + " UME registration successful. Sequence number is " + reg.sequenceNumber());
					break;
				case LBM.MSG_UME_REGISTRATION_COMPLETE_EX:
					UMERegistrationCompleteInfo regcomplete = msg.registrationCompleteInfo();
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "] UME registration complete. SQN " 
						+ regcomplete.sequenceNumber() + ". Flags " + regcomplete.flags());
					break;
				case LBM.MSG_UME_DEREGISTRATION_SUCCESS_EX:
					System.out.print("DEREGISTRATION SUCCESSFUL ");
					UMEDeregistrationSuccessInfo dereg = msg.deregistrationSuccessInfo();
					System.out.print("[" + msg.topicName() + "][" + msg.source() + "] store " + dereg.storeIndex() + ": "
						+ dereg.store() + " UME deregistration successful. SrcRegID " + dereg.sourceRegistrationId() + " RcvRegID " + dereg.receiverRegistrationId()
						+ ". Flags " + dereg.flags() + " ");
					System.out.println();
					break;
				case LBM.MSG_UME_DEREGISTRATION_COMPLETE_EX:
					System.out.println("DEREGISTRATION COMPLETE");
					break;
				case LBM.MSG_UME_REGISTRATION_CHANGE:
					System.out.println("[" + msg.topicName() + "][" + msg.source() + "] UME registration change: " + msg.dataString());
					break;
				default:
					System.out.println("Other event, type=" + msg.type());
					break;
			}
			return 0;
		}
	}  /* ReceiverCallback */
} /* class MinSrc */
