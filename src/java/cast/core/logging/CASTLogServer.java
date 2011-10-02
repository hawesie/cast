package cast.core.logging;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;

import org.apache.log4j.helpers.LogLog;
import org.apache.log4j.spi.LoggingEvent;

import Ice.Current;
import cast.interfaces._LoggingServerDisp;

public class CASTLogServer extends _LoggingServerDisp {

	private static final long serialVersionUID = 1L;
	
	private ComponentLogger m_logger = ComponentLogger
			.getLogger(CASTLogServer.class.getCanonicalName());

	@Override
	public void logMessage(String message, Current __current) {
		System.out.println("message: " + message);

	}


	@Override
	public void logSerialisedEvent(byte[] event, Current __current) {
		try {
			ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(event));
			LoggingEvent le = (LoggingEvent) ois.readObject();
			m_logger.callAppenders(le);
		} catch (IOException e) {
			LogLog.error("Error deserialising LoggingEvent", e);
		} catch (ClassNotFoundException e) {
			LogLog.error("Error deserialising LoggingEvent", e);
		}
	}

}
