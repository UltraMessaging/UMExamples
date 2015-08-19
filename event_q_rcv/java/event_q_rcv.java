import com.latencybusters.lbm.*;

public class EventQrcv {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; 		/* Context object: container for UM "instance". */
		LBMReceiver rcv = null; 	/* Source object: for sending messages. */
		LBMEventQueue evq = null; 	/* Event Queue object: process receiver events on app thread */
		
		LBMRcvReceiver rcvCB = null;
		
		ctx = new LBMContext();
		evq = new LBMEventQueue();
		
		{
			LBMTopic topic = null;	
			topic = ctx.allocTopic("test.topic");
			

			/* The event queue object is passed into the receiver constructor */
            /* This causes events to be queued in an unbounded Q.  In order   */
            /* to process these messages the evq.Run() method must be called  */
			rcv = new LBMReceiver(ctx, topic, rcvCB, null, evq);
		}
		
		
		/* run the event queue for 60 seconds         	*/
		/* all the receiver events will be processed  	*/
		/* in this thread. This includes message 		*/
		/* processing.							  		*/
		evq.run(60000);
		
		try {
			/* Shutdown order is important.  Event Queues should */
            /* be .close()d after the context and receiver.      */
			rcv.close();
			ctx.close();
			evq.close();
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
	/* other events.  Normally this callback would be executed on   */
	/* the context thread.  In this example, we are using an eventQ */
	/* This means this callback will be executed during the .run()  */
	/* call on the eventQ object									*/
	public int onReceive(Object cbArg, LBMMessage msg)
	{
		switch (msg.type())
		{
			case LBM.MSG_DATA:
				System.out.println("Msg Received: " + msg.dataString());
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


