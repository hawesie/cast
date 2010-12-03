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

	public LogAdditions getAdditions() {
		return m_additions;
	}

}
