package cast.core.logging;

import Ice.Communicator;
import Ice.Identity;
import Ice.ObjectAdapter;
import cast.cdl.LOGGINGPORT;
import cast.core.LoggingApplication;

public class LoggingServer extends LoggingApplication {

	static final int DEFAULT_LOGGER_PORT = LOGGINGPORT.value;
	final static String DEFAULT_LOGGER_NAME = "CASTLogServer";
	final static String DEFAULT_LOGGER_CATEGORY = "LoggingServer";
	/*
	 * (non-Javadoc)
	 * 
	 * @see Ice.Application#run(java.lang.String[])
	 */
	@Override
	public int run(String[] args) {

		
		Communicator ic = communicator();
		ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints(
				"LoggingServer", "default -p " + LoggingServer.DEFAULT_LOGGER_PORT);

		Identity id = new Identity(LoggingServer.DEFAULT_LOGGER_NAME, LoggingServer.DEFAULT_LOGGER_CATEGORY);
		adapter.add(new CASTLogServer(), id);

		adapter.activate();
		adapter.waitForDeactivate();
		return 0;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		LoggingServer app = new LoggingServer();
		int status = app.main("cast.core.logging.LoggingServer", args);
		System.exit(status);
	}

	
	
}
