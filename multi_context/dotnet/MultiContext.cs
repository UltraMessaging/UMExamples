/* MultiContext.cs, see http://ultramessaging.github.io/UMExamples/multi_context/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class MultiContext {
        public static String xml_conf = "./multi_context_config.xml";
        public static String app_name = "main";

        static void Main(String[] args) {
                LBM.setConfigurationXml(xml_conf, app_name);

                LBMContextAttributes ctxAttrOne = new LBMContextAttributes();
                ctxAttrOne.setFromXml("TRD1");
                LBMContext ctxOne = new LBMContext(ctxAttrOne);

                LBMContextAttributes ctxAttrTwo = new LBMContextAttributes();
                ctxAttrTwo.setFromXml("TRD2");
                LBMContext ctxTwo = new LBMContext(ctxAttrTwo);

                ctxOne.close();
                ctxTwo.close();
        }
}
