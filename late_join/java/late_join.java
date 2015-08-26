import com.latencybusters.lbm.*;

public class late_join {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; 		/* Context object: container for UM "instance". */
		LBMReceiver late_rcv = null; 	/* Source object: for sending messages. */
		LBMSource early_source = null; 		/* Used for sending messages on a single topic */
		LBMRcvReceiver rcvCB = null;
		LBMTopic src_topic = null;
		
		ctx = new LBMContext();
		
		{
			LBMSourceAttributes srcAttr = null;

			srcAttr = new LBMSourceAttributes();
			
			/* Set configuration value using strings */
			srcAttr.setValue("late_join", "1");
			srcAttr.setValue("retransmit_retention_size_threshold", "1");
			
			src_topic = new LBMTopic(ctx, "test.topic", srcAttr);
			early_source = new LBMSource(ctx, src_topic);
			
			early_source.send("test".getBytes(), "test".getBytes().length, LBM.MSG_FLUSH | LBM.SRC_NONBLOCK);
			{
				LBMTopic topic = null;	
				topic = ctx.allocTopic("test.topic");
				late_rcv = new LBMReceiver(ctx, topic, rcvCB, null, null);
			}
		}
		
		try {
			late_rcv.close();
			early_source.close();
			ctx.close();
		}
		catch(LBMException ex)
		{
			System.err.println("Error closing LBM objects: " + ex.toString());
			System.exit(1);
		}
	}
}

class LBMRcvReceiver implements LBMReceiverCallback
{
	/* This on receive callback is required to process messages and */
	/* other events.  This callback is executed on the context		*/
	/*  thread.  */
	
	public int onReceive(Object cbArg, LBMMessage msg)
	{
		switch (msg.type())
		{
			case LBM.MSG_DATA:
				if((msg.flags() & LBM.MSG_FLAG_OTR)==LBM.MSG_FLAG_OTR)
					System.out.println("Processing Late Join Message. SQN: " + msg.sequenceNumber());
				else if((msg.flags() & LBM.MSG_FLAG_RETRANSMIT)==LBM.MSG_FLAG_RETRANSMIT)
					System.out.println("Processing OTR Message. SQN: " + msg.sequenceNumber());
				else 
					System.out.println("Processing Normal Message. SQN: " + msg.sequenceNumber());
				break;
			case LBM.MSG_BOS:
				System.out.println("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
				break;
			case LBM.MSG_EOS:
				System.out.println("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
				break;
			default:
				System.out.println("Unknown lbm_msg_t type " + msg.type() + " [" + msg.topicName() + "][" + msg.source() + "]");
				break;
		}
		System.out.flush();	
		msg.dispose();		
		return 0;
	}
}


