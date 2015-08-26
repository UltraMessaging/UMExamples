/* LoggingCallback.cs, see http://ultramessaging.github.io/UMExamples/logging_callback/dotnet/ */
using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;

public class LoggingCallback
{
    static void Main(String[] args)
    {
        LBMContext ctx = null; /* Context object: container for UM "instance". */
        LBMSource src = null; /* Source object: for sending messages. */
        LBM lbm = new LBM();

        /*** Initialization: create necessary UM objects. ***/
        try
        {
            LBMTopic topic = null;
            LBMSourceAttributes srcAttr = null;

            lbm.setLogger(new LBMLogging(logger));

            ctx = new LBMContext();
            srcAttr = new LBMSourceAttributes();
            srcAttr.setValue("late_join", "1");
            topic = ctx.allocTopic("test.topic", srcAttr);
            src = ctx.createSource(topic, null, null, null);
        }
        catch (LBMException ex)
        {
            System.Console.Error.Write("Error initializing LBM objects: " + ex.Message);
            System.Environment.Exit(1);
        }

        /* Cleanup */
        src.close();
        ctx.close();
    } /* main */

    static void logger(int level, string message)
    {
        System.Console.Out.Write("LOG Level " + level + ": " + message);
    }
} /* class LoggingCallback */
