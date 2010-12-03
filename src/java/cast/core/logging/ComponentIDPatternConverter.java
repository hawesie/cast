package cast.core.logging;

import org.apache.log4j.helpers.FormattingInfo;
import org.apache.log4j.helpers.PatternConverter;
import org.apache.log4j.spi.LoggingEvent;

public class ComponentIDPatternConverter extends PatternConverter {

	public ComponentIDPatternConverter(FormattingInfo _formattingInfo) {
		super(_formattingInfo);
	}

	@Override
	protected String convert(LoggingEvent _event) {
		if(_event instanceof CASTLoggingEvent) {
			return ((CASTLoggingEvent)_event).getAdditions().getComponentID();
		}
		return "";
	}

}
