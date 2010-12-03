package cast.core.logging;

import org.apache.log4j.helpers.PatternConverter;
import org.apache.log4j.helpers.PatternParser;

/**
 * Parser used to add CAST-specific pattern options. With thanks to
 * http://www.jajakarta
 * .org/log4j/jakarta-log4j-1.1.3/docs/deepExtension.html#PatternLayout
 * 
 * @author nah
 * 
 */
public class CASTPatternParser extends PatternParser {

	static final char CAST_TIME_CHAR = 'a';

	static final char ID_CHAR = 'i';

	static final char SUBARCH_CHAR = 's';
	
	static final char COLOUR_START_CHAR = 'S';

	static final char COLOUR_END_CHAR = 'E';

	static final char LEVEL_CHAR = 'P';


	public CASTPatternParser(String _pattern) {
		super(_pattern);
	}

	@Override
	protected void finalizeConverter(char _c) {

		PatternConverter pc = null;
		switch (_c) {
		case ID_CHAR:
			pc = new ComponentIDPatternConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		case SUBARCH_CHAR:
			pc = new SubarchitectureIDPatternConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		case CAST_TIME_CHAR:
			pc = new CASTTimePatternConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		case COLOUR_START_CHAR:
			pc = new ColourStartConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		case COLOUR_END_CHAR:
			pc = new ColourEndConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		case LEVEL_CHAR:
			pc = new RetroLevelConverter(formattingInfo);
			currentLiteral.setLength(0);
			addConverter(pc);
			break;
		default:
			super.finalizeConverter(_c);
		}
	}

}