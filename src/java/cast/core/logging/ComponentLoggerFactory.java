package cast.core.logging;

import org.apache.log4j.Logger;
import org.apache.log4j.spi.LoggerFactory;

public class ComponentLoggerFactory implements LoggerFactory {

	public Logger makeNewLoggerInstance(String _name) {
		return new ComponentLogger(_name);
	}

}
