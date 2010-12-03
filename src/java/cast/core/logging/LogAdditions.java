package cast.core.logging;

import java.io.Serializable;

/**
 * Class to hold the extra information a log event for CAST could have.
 * 
 * @author nah
 * 
 */
public class LogAdditions implements Serializable {

	private static final long serialVersionUID = 1L;

	private final String m_id;
	private final String m_saID;
	private final String m_colourStart;


	public LogAdditions(String _id, String _colourStart) {
		this(_id, "", _colourStart);
	}

	public LogAdditions(String _id, String _saID, String _colourStart) {
		m_id = _id;
		m_saID = _saID;
		m_colourStart = _colourStart;
	}

	public  String getComponentID() {
		return m_id;
	}

	public  String getSubarchitectureID() {
		return m_saID;
	}
	
	public String getColourStart() {
		return m_colourStart;
	}
}
