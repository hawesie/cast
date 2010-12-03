package xsrad.logging;

import org.apache.log4j.Logger;

public class XsradDebug {

	public static void log(String log)
	{
		Logger logger = Logger.getLogger("xsrad.debug");
		logger.trace("" + log + "");
	}
	
}
