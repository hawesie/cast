package cast.core.logging;
import org.apache.log4j.helpers.FormattingInfo;
import org.apache.log4j.helpers.PatternConverter;
import org.apache.log4j.spi.LoggingEvent;

import cast.cdl.CASTTime;
import cast.core.CASTUtils;
import cast.interfaces.TimeServerPrx;

public class CASTTimePatternConverter extends PatternConverter {

		private static final TimeServerPrx m_timeServer;
		
		static {
			m_timeServer = CASTUtils.getTimeServer();
		}
	
		public CASTTimePatternConverter(FormattingInfo _formattingInfo) {
			super(_formattingInfo);
		}

		@Override
		protected String convert(LoggingEvent _event) {
			CASTTime time = m_timeServer.getCASTTime();
			StringBuilder sb = new StringBuilder();
			sb.append(time.s);
			sb.append(":");
			sb.append(time.us);
			return sb.toString();
		}

	}
