/**
 * 
 */
package cast.core.logging;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.helpers.LogLog;
import org.apache.log4j.spi.LoggingEvent;

import Ice.Communicator;
import Ice.Identity;
import Ice.ObjectPrx;
import cast.interfaces.LoggingServerPrx;
import cast.interfaces.LoggingServerPrxHelper;

/**
 * A log4j appender to log to a remote Ice server.
 * 
 * @author nah
 * 
 */
public class IceAppender extends AppenderSkeleton {

	private String m_logHost;
	private int m_logPort;
	private String m_logName;
	private String m_logCategory;
	private LoggingServerPrx m_logServer;

	public IceAppender() throws IOException {
		m_logHost = null;
		m_logPort = LoggingServer.DEFAULT_LOGGER_PORT;
		m_logName = LoggingServer.DEFAULT_LOGGER_NAME;
		m_logCategory = LoggingServer.DEFAULT_LOGGER_CATEGORY;

	}

	@Override
	public void activateOptions() {
		m_logServer = getLoggingServer(m_logHost, m_logPort);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.apache.log4j.Appender#close()
	 */
	@Override
	public void close() {
		// SocketAppender
	}

	public void setHost(String _host) {
		m_logHost = _host;
	}

	public void setPort(String _port) {
		m_logPort = Integer.parseInt(_port);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.apache.log4j.Appender#requiresLayout()
	 */
	@Override
	public boolean requiresLayout() {
		// TODO Auto-generated method stub
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.log4j.AppenderSkeleton#append(org.apache.log4j.spi.LoggingEvent
	 * )
	 */
	@Override
	protected void append(LoggingEvent _event) {
		// LogLog.debug("message: " + _event.getMessage());

		try {
			
			// TODO check efficiency concerns about doing this
			// every time

			ByteArrayOutputStream os = new ByteArrayOutputStream();
			ObjectOutputStream oos = new ObjectOutputStream(os);
			
			oos.writeObject(_event);
			oos.flush();
			m_logServer.logSerialisedEvent(os.toByteArray());

			
			
		} catch (IOException e) {
			LogLog.error("can't create output event data", e);
		}

	}

	private LoggingServerPrx getLoggingServer(String _host, int _port) {

		Identity id = new Identity(m_logName, m_logCategory);

		ObjectPrx base;
		if (_host != null) {
			base = communicator().stringToProxy(
					communicator().identityToString(id) + ":default -h "
							+ _host + " -p " + _port);
		} else {
			base = communicator().stringToProxy(
					communicator().identityToString(id) + ":default -p "
							+ _port);
		}

		if (base == null) {
			throw new RuntimeException(
					"Cannot create proxy to logging server on " + _host);
		}

		LoggingServerPrx prx = LoggingServerPrxHelper.checkedCast(base);
		assert (prx != null);
		return prx;
	}

	private Communicator m_communicator = null;

	private Communicator communicator() {
		if (m_communicator == null) {
			m_communicator = Ice.Util.initialize();
		}
		return m_communicator;
	}

}
