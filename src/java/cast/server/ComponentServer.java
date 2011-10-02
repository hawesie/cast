/**
 * 
 */
package cast.server;



import Ice.Application;
import Ice.Communicator;
import Ice.Identity;
import Ice.ObjectAdapter;
import Ice.SignalPolicy;
import cast.cdl.CASTRELEASESTRING;
import cast.cdl.JAVASERVERPORT;
import cast.core.LoggingApplication;
import cast.core.logging.ComponentLogger;
import cast.core.logging.LogAdditions;

/**
 * @author nah
 * 
 */
public class ComponentServer extends LoggingApplication {

	
	/**
	 * 
	 */
	public ComponentServer() {
		m_logger = ComponentLogger
				.getLogger("cast.server.java.ComponentServer");
		m_logAddy = new LogAdditions("cast.server.java.ComponentServer", "");
	}

	/**
	 * @param _signalPolicy
	 */
	public ComponentServer(SignalPolicy _signalPolicy) {
		super(_signalPolicy);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see Ice.Application#run(java.lang.String[])
	 */
	@Override
	public int run(String[] arg0) {

		info("Java server: \"" + CASTRELEASESTRING.value + "\"");

		Communicator ic = communicator();
		ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints(
				"ComponentServer1", "default -p " + JAVASERVERPORT.value);

		Identity id = new Identity("ComponentFactory", "ComponentFactory");
		adapter.add(new CASTComponentFactory(), id);

		Identity manid = new Identity("comp.man",
				CASTComponentManager.class.getCanonicalName());
		adapter.add(new CASTComponentManager(), manid);

		// adapter.addServantLocator(new cast.server.ComponentLocator(), "");
		adapter.activate();
		adapter.waitForDeactivate();
		return 0;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		LoggingApplication app = new ComponentServer();
		int status = app.main("cast.server.ComponentServer", args);
		System.exit(status);
	}
}
