/**
 * 
 */
package cast.server;

import java.io.File;
import java.net.MalformedURLException;
import java.util.Properties;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import Ice.Application;
import Ice.Communicator;
import Ice.Identity;
import Ice.ObjectAdapter;
import Ice.SignalPolicy;
import cast.cdl.CASTRELEASESTRING;
import cast.cdl.JAVASERVERPORT;
import cast.cdl.LOGGINGENVVAR;
import cast.cdl.LOGGINGPROPERTIESFILE;
import cast.core.logging.ComponentLayout;

/**
 * @author nah
 * 
 */
public class ComponentServer extends Application {

	static {
		initLogging();
	}

	/**
	 * Rules for finding the file to use for configuring logging properties.
	 * 
	 * 1. Use the file indicated by the environment CAST_LOGGING_PROPERTIES
	 * 
	 * 2. If the env var or the file it specifies does not exist, use
	 * log4j.properties in the cwd.
	 * 
	 * 3. If that file does not exist, use ~/.log4j.properties instead.
	 * 
	 * 4. And if that fails, fall back to default (stdout, ComponentLayout)
	 * config.
	 * 
	 */
	private static void initLogging() {

		// get the file specified by the env var
		if (configureFromEnvironment()) {
			return;
		} else if (configureFromCWD()) {
			return;

		} else if (configureFromHome()) {
			return;
		} else {
			configureDefault();
		}
	}

	private static void configureDefault() {
		Properties prop = new Properties();
		prop.put("log4j.rootLogger", "INFO, stdout");
		prop.put("log4j.appender.stdout", "org.apache.log4j.ConsoleAppender");
		prop.put("log4j.appender.stdout.layout", ComponentLayout.class
				.getName());
		PropertyConfigurator.configure(prop);
	}

	/**
	 * @param logger
	 * @return
	 */
	private static boolean configureFromEnvironment() {
		String propertiesFileString = System.getenv(LOGGINGENVVAR.value);
		if (propertiesFileString != null) {
			return configureFromFile(propertiesFileString);
		}
		return false;
	}

	private static boolean configureFromCWD() {
		String propertiesFileString = "./" + LOGGINGPROPERTIESFILE.value;
		return configureFromFile(propertiesFileString);
	}

	private static boolean configureFromHome() {
		String propertiesFileString = System.getProperty("user.home") + "/."
				+ LOGGINGPROPERTIESFILE.value;
		return configureFromFile(propertiesFileString);
	}

	/**
	 * @param logger
	 * @param _filename
	 * @return
	 */
	private static boolean configureFromFile(String _filename) {
		try {

			File propertiesFile = new File(_filename);
			if (propertiesFile.exists()) {
				// logger.debug("Using cwd log4j properties: " + _filename);
				PropertyConfigurator.configure(propertiesFile.toURI().toURL());
				return true;
			} else {
				// System.out.println("failed to use log4j properties: " +
				// propertiesFile);
				// logger.warn("Cannot find log4j propoerties file specified by $"
				// + LOGGINGENVVAR.value + ": " + _filename);
			}
		} catch (MalformedURLException e) {
			// logger.warn("Configuration failed with log4j propoerties file: "
			// + _filename, e);
		}
		return false;
	}

	private Logger logger = Logger
			.getLogger("cast.server.java.ComponentServer");

	/**
	 * 
	 */
	public ComponentServer() {
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

		logger.info("Java server: \"" + CASTRELEASESTRING.value + "\"");

		Communicator ic = communicator();
		ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints(
				"ComponentServer1", "default -p " + JAVASERVERPORT.value);

		Identity id = new Identity("ComponentFactory", "ComponentFactory");
		adapter.add(new CASTComponentFactory(), id);

		Identity manid = new Identity("comp.man", CASTComponentManager.class
				.getCanonicalName());
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
		ComponentServer app = new ComponentServer();
		int status = app.main("cast.server.ComponentServer", args);
		System.exit(status);
	}
}
