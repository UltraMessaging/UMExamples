/* RequestResponse.cs, see http://ultramessaging.github.io/UMExamples/RequestResponse/dotnet/ */

using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;
using System.Runtime.InteropServices;

public class RequestResponse
{
    public static int run = 1;

    public static void main(String[] args)
    {
        /* Initialization: create necessary UM objects. */
        LBMContext ctx = new LBMContext();

        /* Create LBMReceiverCallback using the method onReceive defined in this class */
        LBMReceiverCallback rcvCallback = new LBMReceiverCallback(onReceive);

        /* Create receiver objects */
        LBMReceiverAttributes rcv_attr = new LBMReceiverAttributes();
        LBMTopic rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
        LBMReceiver rcv = new LBMReceiver(ctx, rtopic, rcvCallback, null, null);

        /* Initialize and create source */
        LBMTopic stopic = new LBMTopic(ctx, "test.topic", new LBMSourceAttributes());
        LBMSource src = new LBMSource(ctx, stopic);

        String msgData = "request";
        byte[] bytes = new byte[msgData.Length * sizeof(char)];
        System.Buffer.BlockCopy(msgData.ToCharArray(), 0, bytes, 0, bytes.Length);

        LBMRequest req = new LBMRequest(bytes, bytes.Length);
        LBMResponseCallback myResponseCallback = new LBMResponseCallback(onResponse);
        req.addResponseCallback(myResponseCallback);

        /* Sleep for 1 second to allow TR to complete */
        try
        {
            System.Threading.Thread.Sleep(1000);
        }
        catch (Exception ex)
        {
            System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + ex.Message);
            System.Environment.Exit(1);
        }

        /* Send a message. */
        src.send(req, LBM.MSG_FLUSH | LBM.SRC_BLOCK);

        while (run == 1)
        {
            try
            {
                System.Threading.Thread.Sleep(1000);
            }
            catch (Exception ex)
            {
                System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + ex.Message);
                System.Environment.Exit(1);
            }
        }

        /* Cleanup: delete UM objects. */
        req.close();
        src.close();
        rcv.close();
        ctx.close();
    }  /* main */

    /* Class for receiving messages */
    static protected int onReceive(object cbArg, LBMMessage msg)
    {
        switch (msg.type())
        {
            case LBM.MSG_REQUEST:
                {
                    System.Console.Out.Write("Request Received");
                    try
                    {
                        String msgData = "response";
                        byte[] bytes = new byte[msgData.Length * sizeof(char)];
                        System.Buffer.BlockCopy(msgData.ToCharArray(), 0, bytes, 0, bytes.Length);
                        msg.respond(bytes, msgData.Length, LBM.SRC_NONBLOCK);
                    }
                    catch (LBMException ex)
                    {
                        System.Console.Out.Write("Error responding to request: " + ex.Message);
                    }
                }
                break;
        }
        return 0;
    }/* ReceiverCallback */
    
    /* Class for receiving responses */
    public static int onResponse(object cbArg, LBMRequest req, LBMMessage msg)
    {
        switch (msg.type())
        {
            case LBM.MSG_RESPONSE:
                System.Console.Out.WriteLine("Response Received");
                run = 0;
                break;
        }
        return 0;
    } /* ResponseCallback */
} /* class RequestResponse */
