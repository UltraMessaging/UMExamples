/* SeqNumberCallback.java, see http://ultramessaging.github.io/UMExamples/seq_number_callback/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;


public class SeqNumberCallback {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null;           /* Context object: container for UM "instance". */
		LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
		LBMTopic rtopic = null;          /* Topic object */
		LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

		/* Initialize and create receiver and receiver callback */
		ReceiverCallback myReceiverCallback = new ReceiverCallback();

		/*** Initialization: create necessary UM objects. ***/
		ctx = new LBMContext();
		rcv_attr = new LBMReceiverAttributes();

		UMERcvRecInfo umerecinfocb = new UMERcvRecInfo();
                rcv_attr.setRecoverySequenceNumberCallback(umerecinfocb, null);

		rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
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
                                default:
                                        System.out.println("Other event, type=" + msg.type());
                                        break;
                        }
			return 0;
		}
	}  /* ReceiverCallback */

	private static class UMERcvRecInfo implements UMERecoverySequenceNumberCallback {
		public int setRecoverySequenceNumberInfo(Object cbArg, UMERecoverySequenceNumberCallbackInfo cbInfo) {

			long new_low = 100;
			
			System.out.println("lowSequenceNumber["+cbInfo.lowSequenceNumber()+"] is the starting sequence number as determined by registration consensus.");
			System.out.println("highSequenceNumber["+cbInfo.highSequenceNumber()+"] is the highest sequence number available in the persisted stores.");
			System.out.println("lowRxReqMaxSequenceNumber["+cbInfo.lowRxReqMaxSequenceNumber()+"] is the lowest sequence number to be requested.");
			try {
				cbInfo.setLowSequenceNumber(new_low);
			}
			catch (LBMEInvalException e) {
				System.err.println(e.getMessage());
			}
			System.out.flush();
			return 0;
		}
	}


} /* class MinSrc */
