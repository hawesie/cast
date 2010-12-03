package cast.core.logging;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

/**
 * CAST logger that extends the basic log mechanisms to add extra CAST-specific
 * detail to log events.
 * 
 * @author nah
 * 
 */
public class ComponentLogger extends Logger {

	/**
	 * The fully qualified name of the Logger class. See also the getFQCN
	 * method.
	 */
	private static final String FQCN = ComponentLogger.class.getName();
	private static final ComponentLoggerFactory m_factory = new ComponentLoggerFactory();

	ComponentLogger(String _name) {
		super(_name);
	}

	static public ComponentLogger getLogger(String _name) {
		return (ComponentLogger) Logger.getLogger(_name, m_factory);
	}

	public void info(Object _message, LogAdditions _addy) {
		if (repository.isDisabled(Level.INFO_INT)) {
			return;
		}
		if (Level.INFO.isGreaterOrEqual(this.getEffectiveLevel())) {
			forcedLog(FQCN, Level.INFO, _message, null, _addy);
		}
	}

	public void info(Object _message, Throwable _t, LogAdditions _addy) {
		if (repository.isDisabled(Level.INFO_INT)) {
			return;
		}
		if (Level.INFO.isGreaterOrEqual(this.getEffectiveLevel())) {
			forcedLog(FQCN, Level.INFO, _message, _t, _addy);
		}
	}

	/**
	 * Log a message object with the {@link org.apache.log4j.Level#TRACE TRACE}
	 * level.
	 * 
	 * @param message
	 *            the message object to log.
	 * @see #debug(Object) for an explanation of the logic applied.
	 */
	public void trace(Object message, LogAdditions _addy) {
		if (repository.isDisabled(Level.TRACE_INT)) {
			return;
		}

		if (Level.TRACE.isGreaterOrEqual(this.getEffectiveLevel())) {
			forcedLog(FQCN, Level.TRACE, message, null, _addy);
		}
	}

	/**
	 * Log a message object with the <code>TRACE</code> level including the
	 * stack trace of the {@link Throwable}<code>t</code> passed as parameter.
	 * 
	 * <p>
	 * See {@link #debug(Object)} form for more detailed information.
	 * </p>
	 * 
	 * @param message
	 *            the message object to log.
	 * @param t
	 *            the exception to log, including its stack trace.
	 */
	public void trace(Object message, Throwable t, LogAdditions _addy) {
		if (repository.isDisabled(Level.TRACE_INT)) {
			return;
		}

		if (Level.TRACE.isGreaterOrEqual(this.getEffectiveLevel())) {
			forcedLog(FQCN, Level.TRACE, message, t, _addy);
		}
	}

	private void forcedLog(String _fqcn, Level _info, Object _message,
			Throwable _t, LogAdditions _addy) {
		callAppenders(new CASTLoggingEvent(_fqcn, this, _info, _message, _t,
				_addy));
	}

	/**
	 * Log a message object with the {@link Level#DEBUG DEBUG} level.
	 * 
	 * <p>
	 * This method first checks if this category is <code>DEBUG</code> enabled
	 * by comparing the level of this category with the {@link Level#DEBUG
	 * DEBUG} level. If this category is <code>DEBUG</code> enabled, then it
	 * converts the message object (passed as parameter) to a string by invoking
	 * the appropriate {@link org.apache.log4j.or.ObjectRenderer}. It then
	 * proceeds to call all the registered appenders in this category and also
	 * higher in the hierarchy depending on the value of the additivity flag.
	 * 
	 * <p>
	 * <b>WARNING</b> Note that passing a {@link Throwable} to this method will
	 * print the name of the <code>Throwable</code> but no stack trace. To print
	 * a stack trace use the {@link #debug(Object, Throwable)} form instead.
	 * 
	 * @param message
	 *            the message object to log.
	 */
	public void debug(Object message, LogAdditions _addy) {
		if (repository.isDisabled(Level.DEBUG_INT))
			return;
		if (Level.DEBUG.isGreaterOrEqual(this.getEffectiveLevel())) {
			forcedLog(FQCN, Level.DEBUG, message, null, _addy);
		}
	}

	/**
	 * Log a message object with the <code>DEBUG</code> level including the
	 * stack trace of the {@link Throwable} <code>t</code> passed as parameter.
	 * 
	 * <p>
	 * See {@link #debug(Object)} form for more detailed information.
	 * 
	 * @param message
	 *            the message object to log.
	 * @param t
	 *            the exception to log, including its stack trace.
	 */
	public void debug(Object message, Throwable t, LogAdditions _addy) {
		if (repository.isDisabled(Level.DEBUG_INT))
			return;
		if (Level.DEBUG.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.DEBUG, message, t, _addy);
	}

	/**
	 * Log a message object with the {@link Level#WARN WARN} Level.
	 * 
	 * <p>
	 * This method first checks if this category is <code>WARN</code> enabled by
	 * comparing the level of this category with {@link Level#WARN WARN} Level.
	 * If the category is <code>WARN</code> enabled, then it converts the
	 * message object passed as parameter to a string by invoking the
	 * appropriate {@link org.apache.log4j.or.ObjectRenderer}. It proceeds to
	 * call all the registered appenders in this category and also higher in the
	 * hieararchy depending on the value of the additivity flag.
	 * 
	 * <p>
	 * <b>WARNING</b> Note that passing a {@link Throwable} to this method will
	 * print the name of the Throwable but no stack trace. To print a stack
	 * trace use the {@link #warn(Object, Throwable)} form instead.
	 * <p>
	 * 
	 * @param message
	 *            the message object to log.
	 */
	public void warn(Object message, LogAdditions _addy) {
		if (repository.isDisabled(Level.WARN_INT))
			return;

		if (Level.WARN.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.WARN, message, null, _addy);
	}

	/**
	 * Log a message with the <code>WARN</code> level including the stack trace
	 * of the {@link Throwable} <code>t</code> passed as parameter.
	 * 
	 * <p>
	 * See {@link #warn(Object)} for more detailed information.
	 * 
	 * @param message
	 *            the message object to log.
	 * @param t
	 *            the exception to log, including its stack trace.
	 */
	public void warn(Object message, Throwable t, LogAdditions _addy) {
		if (repository.isDisabled(Level.WARN_INT))
			return;
		if (Level.WARN.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.WARN, message, t, _addy);
	}

	/**
	 * Log a message object with the {@link Level#FATAL FATAL} Level.
	 * 
	 * <p>
	 * This method first checks if this category is <code>FATAL</code> enabled
	 * by comparing the level of this category with {@link Level#FATAL FATAL}
	 * Level. If the category is <code>FATAL</code> enabled, then it converts
	 * the message object passed as parameter to a string by invoking the
	 * appropriate {@link org.apache.log4j.or.ObjectRenderer}. It proceeds to
	 * call all the registered appenders in this category and also higher in the
	 * hierarchy depending on the value of the additivity flag.
	 * 
	 * <p>
	 * <b>WARNING</b> Note that passing a {@link Throwable} to this method will
	 * print the name of the Throwable but no stack trace. To print a stack
	 * trace use the {@link #fatal(Object, Throwable)} form instead.
	 * 
	 * @param message
	 *            the message object to log
	 */
	public void fatal(Object message, LogAdditions _addy) {
		if (repository.isDisabled(Level.FATAL_INT))
			return;
		if (Level.FATAL.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.FATAL, message, null, _addy);
	}

	public void error(Object _message, LogAdditions _addy) {
		if (repository.isDisabled(Level.ERROR_INT))
			return;
		if (Level.ERROR.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.ERROR, _message, null, _addy);
	}

	public void error(Object _message, Throwable _t, LogAdditions _addy) {
		if (repository.isDisabled(Level.ERROR_INT))
			return;
		if (Level.ERROR.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.ERROR, _message, _t, _addy);
	}

	/**
	 * Log a message object with the <code>FATAL</code> level including the
	 * stack trace of the {@link Throwable} <code>t</code> passed as parameter.
	 * 
	 * <p>
	 * See {@link #fatal(Object)} for more detailed information.
	 * 
	 * @param message
	 *            the message object to log.
	 * @param t
	 *            the exception to log, including its stack trace.
	 */
	public void fatal(Object message, Throwable t, LogAdditions _addy) {
		if (repository.isDisabled(Level.FATAL_INT))
			return;
		if (Level.FATAL.isGreaterOrEqual(this.getEffectiveLevel()))
			forcedLog(FQCN, Level.FATAL, message, t, _addy);
	}

}
