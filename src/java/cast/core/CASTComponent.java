/*
 * CAST - The CoSy Architecture Schema Toolkit
 *
 * Copyright (C) 2006-2007 Nick Hawes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

/**
 * Basic components that are used to make up a CAST architecture.
 */
package cast.core;

import java.util.Map;
import java.util.Vector;
import java.util.concurrent.Semaphore;

import org.apache.log4j.Level;

import Ice.Communicator;
import Ice.Current;
import Ice.Identity;
import Ice.ObjectAdapter;
import cast.CASTException;
import cast.cdl.CASTTime;
import cast.cdl.COMPONENTNUMBERKEY;
import cast.cdl.ComponentDescription;
import cast.cdl.DEBUGKEY;
import cast.cdl.LOGKEY;
import cast.core.logging.ComponentLogger;
import cast.core.logging.LogAdditions;
import cast.interfaces.ComponentManagerPrx;
import cast.interfaces.TimeServerPrx;
import cast.interfaces._CASTComponentOperations;

/**
 * A basic component in the CAST framework. This class provides some basic
 * functionality useful for debugging
 * 
 * @author nah
 */
public abstract class CASTComponent implements _CASTComponentOperations,
		Runnable {

	/**
	 * The CAST time server.
	 */
	private TimeServerPrx m_timeServer;

	// access controller used to prevent asynchronous access
	protected Semaphore m_semaphore;

	protected boolean m_asleep;

	protected Object m_unlockNotification;

	private String m_startColourEscape;

	private boolean m_configureCalled;

	private boolean m_startCalled;

	private String m_componentID;

	private ComponentManagerPrx m_manager;

	private Thread m_runThread;

	private Vector<Identity> m_serverIdentities;;

	// ice thingies
	private ObjectAdapter m_adapter;
	private Identity m_iceIdentity;

	protected ComponentLogger m_logger;

	private LogAdditions m_additions;

	private Level m_logLevel = null;

	@Deprecated
	protected boolean m_bDebugOutput;
	@Deprecated
	protected boolean m_bLogOutput;

	/**
	 * Construct a new component with the given unique ID.
	 * 
	 * @param _id
	 *            The id used to identify this component.
	 */
	public CASTComponent() {
		// new fair (FIFO) semaphore with a single permit
		m_semaphore = new Semaphore(1, true);
		m_unlockNotification = new Object();
		m_startCalled = false;
		m_configureCalled = false;
		m_startColourEscape = END_COLOUR_ESCAPE;
		m_componentID = null;
		m_manager = null;
		// start with a default logger
		m_logger = ComponentLogger.getLogger("cast.init");
		m_additions = new LogAdditions("", END_COLOUR_ESCAPE);
		m_bDebugOutput = false;
		m_bLogOutput = false;
	}

	protected CASTTime getCASTTime() {
		assert (m_timeServer != null);
		CASTTime ct = m_timeServer.getCASTTime();
		assert (ct != null);
		return ct;
	}

	public TimeServerPrx getTimeServer() {
		assert (m_timeServer != null);
		return m_timeServer;
	}

	public ComponentManagerPrx getComponentManager() {
		assert (m_manager != null);
		return m_manager;
	}

	/**
	 * Returns the Ice.Communicator used by this component.
	 * 
	 * @return
	 */
	protected Communicator getCommunicator() {
		assert (m_adapter != null);
		return m_adapter.getCommunicator();
	}

	/**
	 * Print some debugging output.
	 * 
	 * @param _o
	 */
	public void debug(Object _o) {
		m_logger.trace(_o, m_additions);
	}

	/**
	 * Print some debugging output.
	 * 
	 * @param _o
	 */
	public void debug(Object _o, Throwable _t) {
		m_logger.trace(_o, _t, m_additions);
	}

	/**
	 * Use getComponentID instead.
	 * 
	 * @return
	 */
	@Deprecated
	public String getProcessIdentifier() {
		return getComponentID();
	}

	/**
	 * Gets the unique ID of this component.
	 * 
	 * @return
	 */
	public String getComponentID() {
		return m_componentID;
	}

	public String getID(Current __current) {
		return getComponentID();
	}

	/**
	 * Wait until the unlockComponent is successfully called.
	 */
	protected void waitForUnlock() {

		synchronized (m_unlockNotification) {
			try {
				m_unlockNotification.wait();
			} catch (InterruptedException e) {
				logException(e);
				System.exit(1);
			}
		}
	}

	/**
	 * Log an exception to the ERROR logger, using the message from the
	 * throwable.
	 * 
	 * @param _t
	 */

	public void logException(Throwable _t) {
		m_logger.error(_t.getLocalizedMessage(), _t, getLogAdditions());
	}

	/**
	 * Log an exception to the ERROR logger.
	 * 
	 * @param _message
	 * @param _t
	 */
	public void logException(Object _message, Throwable _t) {
		m_logger.error(_message, _t, getLogAdditions());
	}

	/**
	 * Determines whether the component is currently locked.
	 * 
	 * @return
	 */
	public boolean isLocked() {
		return m_semaphore.availablePermits() == 0;
	}

	/**
	 * Acquire the semaphore for access to this component.
	 */
	public void lockComponent() {
		try {
			m_semaphore.acquire();
			// debug code to find deadlocks
			// if (!m_semaphore.tryAcquire(1, TimeUnit.SECONDS)) {
			// throw new RuntimeException("Unable to access queue!");
			// }
		} catch (InterruptedException e) {
			logException(e);
		}
	}

	/**
	 * Print some logging output.
	 * 
	 * @param _o
	 */
	public void log(Object _o) {
		m_logger.debug(_o, m_additions);
	}

	/**
	 * Print some logging output.
	 * 
	 * @param _o
	 */
	public void log(Object _o, Throwable _t) {
		m_logger.debug(_o, _t, m_additions);
	}

	protected final String getStartColourEscape() {
		return m_startColourEscape;
	}

	// not yet
	// public void println(String _format, Object... _strings) {
	// println(CASTUtils.concatenate(_strings));
	// System.out.printf("", 2);
	// }
	//
	// public void log(String _format, Object... _strings) {
	// log(CASTUtils.concatenate(_strings));
	// }
	//
	// public void debug(String _format, Object... _strings) {
	// debug(CASTUtils.concatenate(_strings));
	// }

	/**
	 * Print out the input in a formatted way.
	 * 
	 * @param _s
	 *            String to print.
	 */
	public void println(Object _o) {
		m_logger.info(_o, m_additions);
	}

	/**
	 * Print out the input in a formatted way.
	 * 
	 * @param _s
	 *            String to print.
	 */
	public void println(Object _o, Throwable _t) {
		m_logger.info(_o, _t, m_additions);
	}

	// private void startColourise(StringBuffer _buff) {
	// _buff.append(m_startColourEscape);
	// }
	//
	// private void endColourise(StringBuffer _buff) {
	// _buff.append(END_COLOUR_ESCAPE);
	// }

	/**
	 * Method called in separate thread to run processing component. This method
	 * is called for each component when it is started by the framework process
	 * server.
	 */
	protected void runComponent() {
	}

	/**
	 * Put the processes thread to sleep for a number of milliseconds.
	 * 
	 * @param _millis
	 *            Number of milliseconds to sleep for.
	 */
	protected void sleepComponent(long _millis) {
		try {
			m_asleep = true;
			Thread.sleep(_millis);
			m_asleep = false;
		} catch (InterruptedException e) {
			logException(e);
		}
	}

	/**
	 * Release the semaphore for access to this component.
	 */
	public void unlockComponent() {
		m_semaphore.release();
		synchronized (m_unlockNotification) {
			m_unlockNotification.notifyAll();
		}
	}

	private final static String END_COLOUR_ESCAPE = "\033[0m";

	protected void startInternal() {
		debug("CASTComponent.startInternal()");
		m_startCalled = true;
	}

	public final boolean isRunning() {
		return m_startCalled;
	}

	protected void start() {

	}

	protected void configure(Map<String, String> _config) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * balt.core.processes.FrameworkComponent#configure(java.util.Properties)
	 */
	protected void configureInternal(Map<String, String> __config) {

		m_configureCalled = true;

		String logString = __config.get(LOGKEY.value);
		if (logString != null) {
			if (Boolean.parseBoolean(logString)) {
				m_logLevel = Level.DEBUG;
				m_bLogOutput = true;
			}
		}

		String debugString = __config.get(DEBUGKEY.value);
		if (debugString != null) {
			if (Boolean.parseBoolean(debugString)) {
				m_logLevel = Level.TRACE;
				m_bLogOutput = true;
				m_bDebugOutput = true;
			}
		}

		String numberString = __config.get(COMPONENTNUMBERKEY.value);
		assert (numberString != null);
		int myNumber = Integer.parseInt(numberString);
		int printNumber = (myNumber % 7);
		int bold = (myNumber / 7) % 2;
		if (printNumber == 0) {
			// 0 = do nothing
			m_startColourEscape = "\033[0m";
		} else {
			// otherwise use the connected colour
			m_startColourEscape = "\033[3" + printNumber;
			if (bold != 0) {
				m_startColourEscape += ";1m";
			} else {
				m_startColourEscape += "m";
			}
		}
		// println("MEMEMEMEMEMEMEME");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see balt.core.processes.FrameworkComponent#stop()
	 */
	protected void stopInternal() {
		try {
			m_runThread.join();
		} catch (InterruptedException e) {
			logException(e);
		}

		synchronized (m_unlockNotification) {
			m_unlockNotification.notifyAll();
		}
	}

	protected void stop() {
	}

	public void beat(Current __current) {
	}

	public void configure(Map<String, String> _config, Current __current) {
		configureInternal(_config);
		configureLogging();
		configure(_config);
	}

	/**
	 * 
	 */
	private void configureLogging() {
		// silly null to make the getLogger method reset the logger after config
		m_logger = null;

		// create internal logging stuff
		m_logger = getLogger();
		m_additions = getLogAdditions();
	}

	/**
	 * Get extra information to use for logging.
	 * 
	 * @return
	 */
	public LogAdditions getLogAdditions() {
		if (m_additions == null) {
			m_additions = new LogAdditions(getComponentID(),
					m_startColourEscape);
		}
		return m_additions;
	}

	public void run(Current __current) {
		m_runThread = new Thread(this, getComponentID() + "-runComponent");
		m_runThread.start();
	}

	public void setComponentManager(ComponentManagerPrx _man, Current __current) {
		CASTUtils.setComponentManager(_man);
		m_manager = _man;
	}

	public void setID(String _id, Current __current) {
		assert (m_componentID == null);
		m_componentID = _id;
	}

	public void start(Current __current) {
		startInternal();
		start();
	}

	public void stop(Current __current) {
		lockComponent();
		m_startCalled = false;
		stop();
		stopInternal();
		// remove self from adapter
		// getObjectAdapter().remove(getIceIdentity());
		unlockComponent();

	}

	/**
	 * Called when the thread starts at runtime
	 */
	public void run() {
		assert (m_startCalled);
		assert (m_configureCalled);
		try {
			runComponent();
		} catch (RuntimeException e) {
			m_logger.error("Caught exception from runComponent(): "
					+ e.getMessage());
			logException(e);
		}
	}

	/**
	 * @param adapter
	 *            the adapter to set
	 */
	public void setObjectAdapter(ObjectAdapter adapter) {
		m_adapter = adapter;
	}

	/**
	 * @return the adapter
	 */
	public ObjectAdapter getObjectAdapter() {
		assert (m_adapter != null);
		return m_adapter;
	}

	/**
	 * @param iceIdentity
	 *            the iceIdentity to set
	 */
	public void setIceIdentity(Identity iceIdentity) {
		m_iceIdentity = iceIdentity;
	}

	/**
	 * @return the iceIdentity
	 */
	public Identity getIceIdentity() {
		assert (m_iceIdentity != null);
		return m_iceIdentity;
	}

	public void destroy(Current __current) {
		destroy();
		destroyInternal(__current);
	}

	public void destroyInternal(Current __current) {
		if (m_serverIdentities != null) {
			for (Identity id : m_serverIdentities) {
				getObjectAdapter().remove(id);
			}
		}
		__current.adapter.remove(__current.id);
	}

	/**
	 * Called after stop, just before component destruction.
	 */
	protected void destroy() {

	}

	public void setTimeServer(TimeServerPrx _ts, Current __current) {
		m_timeServer = _ts;
	}

	/**
	 * Registers the provided servant as a Ice server that can offer the
	 * provided interface. Servants are automatically removed at shutdown.
	 * 
	 * @param <InterfaceType>
	 * @param _interface
	 * @param _servant
	 * @throws CASTException
	 */
	public <InterfaceType extends Ice.Object> void registerIceServer(
			Class<InterfaceType> _interface, InterfaceType _servant) {
		registerIceInterface(getComponentID(), CASTUtils
				.toServantCategory(_interface), _servant);
	}

	/**
	 * Registers the provided servant as a Ice server that can offer the
	 * provided interface. Servants are automatically removed at shutdown.
	 * 
	 * @param <InterfaceType>
	 * @param name
	 * @param category
	 * @param _servant
	 */
	public <InterfaceType extends Ice.Object> void registerIceInterface(
			String name, String category, InterfaceType _servant) {
		Identity id = new Identity(name, category);
		getObjectAdapter().add(_servant, id);

		if (m_serverIdentities == null) {
			m_serverIdentities = new Vector<Identity>(1);
		}
		m_serverIdentities.add(id);
	}

	/**
	 * Resolve an ice server provided by the given component providing the
	 * supplied interface.
	 * 
	 * @param <Type>
	 * @param _componentID
	 * @param _cls
	 * @throws CASTException
	 */
	public <Type extends Ice.Object, TypePrx extends Ice.ObjectPrx> TypePrx getIceServer(
			String _componentID, Class<Type> _cls, Class<TypePrx> _prxCls)
			throws CASTException {

		ComponentDescription desc = getComponentManager()
				.getComponentDescription(_componentID);
		assert (desc != null);

		String host = desc.hostName;
		return CASTUtils.getCASTIceServer(_componentID, host, desc.language,
				_cls, _prxCls, getCommunicator());
	}

	/**
	 * Gets the logger currently used by the component. Used during configure()
	 * to retrieve logger for the component.
	 * 
	 * @return
	 */
	public ComponentLogger getLogger() {
		if (m_logger == null) {
			m_logger = ComponentLogger.getLogger(getLoggerName());
			if (m_logLevel != null) {
				m_logger.setLevel(m_logLevel);
				// println("log level set to" + m_logLevel);
			}
		}
		return m_logger;
	}

	/**
	 * Get a logger with a name that extends the component logger name.
	 * 
	 * @param _postfix
	 *            String to be directly appended to base logger name.
	 * @return
	 */
	public ComponentLogger getLogger(String _postfix) {
		return ComponentLogger.getLogger(getLoggerName() + _postfix);
	}

	/**
	 * Override to alter logger name.
	 * 
	 * @param _name
	 * @return
	 */
	public String getLoggerName() {
		return getComponentID();
	}

}
