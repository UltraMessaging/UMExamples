using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using com.latencybusters.lbm;

public class late_join_receiver {

	static void Main(String[] args)  {
		LBMContext ctx = null;      /* Context object: container for UM "instance". */
		LBMReceiver lateRcv = null; /* Receiver object: for sending messages. */
        LBMReceiverCallback cb = new LBMReceiverCallback(onReceive);    /* Wrapping the onReceive functor in a callback */
        LBMSource early_src = null;   /* Source object: for sending messages. */

		ctx = new LBMContext();


        try
        {
            LBMTopic srcTopic = null;
            LBMSourceAttributes srcAttr = null;

            srcAttr = new LBMSourceAttributes();

            srcAttr.setValue("late_join", "1");
			srcAttr.setValue("retransmit_retention_size_threshold", "1");

            srcTopic = ctx.allocTopic("test.topic", srcAttr);
            early_src = new LBMSource(ctx, srcTopic);

            early_src.send(Encoding.ASCII.GetBytes("test"), 4, LBM.MSG_FLUSH | LBM.SRC_NONBLOCK);
        }
        catch (LBMException ex)
        {
            System.Console.Error.WriteLine("Error initializing LBM objects:  " + ex.Message);
            System.Environment.Exit(1);
        }

        {
            LBMTopic topic = null;
            topic = ctx.lookupTopic("test.topic");

            lateRcv = new LBMReceiver(ctx, topic, onReceive, null, null);
        }

        Thread.Sleep(100);

		try {
            early_src.close();
			lateRcv.close();
			ctx.close();
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
                if((msg.flags() & LBM.MSG_FLAG_OTR)==LBM.MSG_FLAG_OTR)
                    System.Console.Out.WriteLine("Processing OTR Message. SQN: " + msg.sequenceNumber());
				else if((msg.flags() & LBM.MSG_FLAG_RETRANSMIT)==LBM.MSG_FLAG_RETRANSMIT)
                    System.Console.Out.WriteLine("Processing Late Join Message. SQN: " + msg.sequenceNumber());
				else 
					System.Console.Out.WriteLine("Processing Normal Message. SQN: " + msg.sequenceNumber());

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



