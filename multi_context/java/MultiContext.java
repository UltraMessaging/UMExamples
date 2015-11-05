/* MultiContext.java, see http://ultramessaging.github.io/UMExamples/multi_context/java/ */

import com.latencybusters.lbm.*;

public class MultiContext {
	public static String xml_conf = "./multi_context_config.xml";
	public static String app_name = "main";

	public static void main(String[] args) throws LBMException
	{
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
