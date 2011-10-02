/**
 * 
 */
package cast.core;

import java.io.File;
import java.net.MalformedURLException;
import java.util.Properties;

import org.apache.log4j.PropertyConfigurator;

import Ice.Application;
import Ice.SignalPolicy;
import cast.cdl.LOGGINGENVVAR;
import cast.cdl.LOGGINGPROPERTIESFILE;
import cast.core.logging.ComponentLayout;
import cast.core.logging.ComponentLogger;
import cast.core.logging.LogAdditions;

/**
 * 
 * Ice Application that also uses Log4J and needs to configure it at start-up.
 * 
 * @author nah
 * 
 */
public abstract class LoggingApplication extends Application {

	static {
		initLogging();
	}

	
	public LoggingApplication() {
	}

	public LoggingApplication(SignalPolicy _signalPolicy) {
		super(_signalPolicy);
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
	protected static void initLogging() {

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

	protected static void configureDefault() {
		Properties prop = new Properties();
		prop.put("log4j.rootLogger", "INFO, stdout");
		prop.put("log4j.appender.stdout", "org.apache.log4j.ConsoleAppender");
		prop.put("log4j.appender.stdout.layout",
				ComponentLayout.class.getName());
		PropertyConfigurator.configure(prop);
	}

	/**
	 * @param logger
	 * @return
	 */
	protected static boolean configureFromEnvironment() {
		String propertiesFileString = System.getenv(LOGGINGENVVAR.value);
		if (propertiesFileString != null) {
			return configureFromFile(propertiesFileString);
		}
		return false;
	}

	protected static boolean configureFromCWD() {
		String propertiesFileString = "./" + LOGGINGPROPERTIESFILE.value;
		return configureFromFile(propertiesFileString);
	}

	protected static boolean configureFromHome() {
		String propertiesFileString = System.getProperty("user.home") + "/."
				+ LOGGINGPROPERTIESFILE.value;
		return configureFromFile(propertiesFileString);
	}

	/**
	 * @param logger
	 * @param _filename
	 * @return
	 */
	protected static boolean configureFromFile(String _filename) {
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

	protected ComponentLogger m_logger;
	protected LogAdditions m_logAddy;

	protected void info(Object _o) {
		m_logger.info(_o, m_logAddy);
	}

}
