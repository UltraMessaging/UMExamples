/* QuorumState.cs, see http://ultramessaging.github.io/UMExamples/quorum_state/dotnet/ */
using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;

public class QuorumState
{
    public static int srcReady = 0;

    static void Main(String[] args)
    {
        LBMContext ctx = null; /* Context object: container for UM "instance". */
        LBMSource src = null; /* Source object: for sending messages. */

        SrcCB srccb = null;

        /*** Initialization: create necessary UM objects. ***/
        try
        {
            LBMTopic topic = null;
            LBMSourceAttributes srcAttr = null;

            srccb = new SrcCB();
            ctx = new LBMContext();
            srcAttr = new LBMSourceAttributes();
            srcAttr.setValue("ume_store", "127.0.0.1:29999");
            srcAttr.setValue("ume_store_behavior", "qc");
            topic = ctx.allocTopic("test.topic", srcAttr);
            src = ctx.createSource(topic, new LBMSourceEventCallback(srccb.onSourceEvent), null, null);
        }
        catch (LBMException ex)
        {
            System.Console.Error.WriteLine("Error initializing LBM objects: " + ex.Message);
            System.Environment.Exit(1);
        }

        while (true)
        {
            if (srcReady == 1)
            {
                /*** Send a message. ***/
                try
                {
                    src.send(Encoding.ASCII.GetBytes("test"), 4, LBM.MSG_FLUSH | LBM.SRC_NONBLOCK);
                }
                catch (LBMException ex)
                {
                    /* Error trying to send, wait 1 second and try again */
                    try
                    {
                        System.Threading.Thread.Sleep(1000);
                    }
                    catch (Exception eex)
                    {
                        System.Console.Error.WriteLine("Error Thread.sleep interrupted: " + eex.Message);
                        System.Environment.Exit(1);
                    }
                }
            }
            else
            {
                /* No quorum, wait 1 second and check again */
                System.Console.Out.Write("Source is not ready to send (no quorum)");
                try
                {
                    System.Threading.Thread.Sleep(1000);
                }
                catch (Exception eex)
                {
                    System.Console.Error.WriteLine("Error Thread.sleep interrupted: " + eex.Message);
                    System.Environment.Exit(1);
                }
            }
        }
    } /* main */

    class SrcCB
    {
        public void onSourceEvent(Object arg, LBMSourceEvent sourceEvent)
        {
            switch (sourceEvent.type())
            {
                case LBM.SRC_EVENT_UME_REGISTRATION_COMPLETE_EX:
                    srcReady = 1;
                    break;
                case LBM.SRC_EVENT_UME_STORE_UNRESPONSIVE:
                    String errorStr = sourceEvent.dataString();
                    if (errorStr.Contains("quorum lost"))
                        srcReady = 0;
                    break;
            }
            sourceEvent.dispose();
        }
    }
}/* namespace */

