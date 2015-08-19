using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class EventQrcv {

	static void Main(String[] args)  {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		LBMReceiver rcv = null; /* Source object: for sending messages. */
        LBMEventQueue evq = null; /* Event Queue object: process receiver events on app thread */
        LBMReceiverCallback cb = new LBMReceiverCallback(onReceive);    /* Wrapping the onReceive functor in a callback */
		
		ctx = new LBMContext();
		evq = new LBMEventQueue();
        {
            LBMTopic topic = null;
            topic = ctx.lookupTopic("test.topic");

            /* The event queue object is passed into the receiver constructor */
            /* This causes events to be queued in an unbounded Q.  In order   */
            /* to process these messages the evq.Run() method must be called  */
            rcv = new LBMReceiver(ctx, topic, onReceive, null, evq);
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
			System.Console.Error.WriteLine("Error closing LBM objects: " + ex.Message);
			System.Environment.Exit(1);
		}
	}
    static int onReceive(Object cbArg, LBMMessage msg)
    {
        switch (msg.type())
        {
            case LBM.MSG_DATA:
                System.Console.Out.WriteLine("Msg Received: " + msg.dataString());
                break;
            case LBM.MSG_BOS:
                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
                break;
            case LBM.MSG_EOS:
                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
                break;
            default:
                System.Console.Out.WriteLine("Unknown lbm_msg_t type " + msg.type() + " [" + msg.topicName() + "][" + msg.source() + "]");
                break;
        }
        System.Console.Out.Flush();
        msg.dispose();
        return 0;
    }
}



