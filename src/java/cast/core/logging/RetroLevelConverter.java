package cast.core.logging;

import org.apache.log4j.Level;
import org.apache.log4j.helpers.FormattingInfo;
import org.apache.log4j.helpers.PatternConverter;
import org.apache.log4j.spi.LoggingEvent;

/**
 * Output debug levels in the way CAST used to.
 * 
 * @author nah
 * 
 */
public class RetroLevelConverter extends PatternConverter {

	public RetroLevelConverter(FormattingInfo _formattingInfo) {
		super(_formattingInfo);
	}

	@Override
	protected String convert(LoggingEvent _event) {
		switch (_event.getLevel().toInt()) {
		case Level.INFO_INT:
			return "";
		case Level.TRACE_INT:
			return "DEBUG ";
		case Level.DEBUG_INT:
			return "LOG ";
		default:
			return _event.getLevel().toString() + " ";
		}
	}

}
