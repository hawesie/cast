package cast.core.logging;

import org.apache.log4j.Category;
import org.apache.log4j.Priority;
import org.apache.log4j.spi.LoggingEvent;

public class CASTLoggingEvent extends LoggingEvent {

	/**
	 * 
	 */
	private static final long serialVersionUID = -5504787860886412037L;
	private final LogAdditions m_additions;
	
	public CASTLoggingEvent(String _fqnOfCategoryClass, Category _logger,
			Priority _level, Object _message, Throwable _throwable, LogAdditions _addy) {
		super(_fqnOfCategoryClass, _logger, _level, _message, _throwable);
		m_additions = _addy;
	}

	public CASTLoggingEvent(LoggingEvent _le, LogAdditions _logAdditions) {
		super(_le.getFQNOfLoggerClass(),_le.getLogger(), _le.getTimeStamp(), _le.getLevel(), _le.getMessage(), _le.getThreadName(), _le.getThrowableInformation(), _le.getNDC(), _le.getLocationInformation(), _le.getProperties());
		m_additions = _logAdditions;
	}

	public LogAdditions getAdditions() {
		return m_additions;
	}

}
