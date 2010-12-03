package cast.core.logging;

import org.apache.log4j.PatternLayout;
import org.apache.log4j.helpers.PatternParser;
import org.apache.log4j.spi.LoggingEvent;

import cast.cdl.RETROCONVERSIONPATTERN;

/**
 * Layout used for CAST logging lines. 
 * 
 * @author nah
 * 
 */
public class ComponentLayout extends PatternLayout {

	private final static String CAST_RETRO_CONVERSION_PATTERN = RETROCONVERSIONPATTERN.value;

	public ComponentLayout() {
		super(CAST_RETRO_CONVERSION_PATTERN);
	}

	@Override
	protected PatternParser createPatternParser(String _pattern) {
		return new CASTPatternParser(_pattern);
	}

	@Override
	public String format(LoggingEvent _event) {
		StringBuffer buf = new StringBuffer();
		buf.append(super.format(_event));
		return buf.toString();
	}
}
