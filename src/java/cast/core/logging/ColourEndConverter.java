package cast.core.logging;

import org.apache.log4j.helpers.FormattingInfo;
import org.apache.log4j.helpers.PatternConverter;
import org.apache.log4j.spi.LoggingEvent;

public class ColourEndConverter extends PatternConverter {

	public ColourEndConverter(FormattingInfo _formattingInfo) {
	}

	@Override
	protected String convert(LoggingEvent _event) {
		return "\033[0m";
	}

}
