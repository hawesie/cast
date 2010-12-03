/**
 * 
 */
package cast.server;

import java.util.Hashtable;
import java.util.Map;

import Ice.Current;
import cast.CASTException;
import cast.cdl.ComponentDescription;
import cast.interfaces._ComponentManagerDisp;

/**
 * @author nah
 * 
 */
public class CASTComponentManager extends _ComponentManagerDisp {

	private final Hashtable<String, ComponentDescription> m_descriptions;

	/**
	 * Serial version.
	 */
	private static final long serialVersionUID = 1L;

	public CASTComponentManager() {
		m_descriptions = new Hashtable<String, ComponentDescription>();
	}

	public void addComponentDescription(ComponentDescription _description,
			Current __current) {
		m_descriptions.put(_description.componentName, _description);
	}

	public Map<String, ComponentDescription> getComponentDescriptions(
			Current __current) {
		return m_descriptions;
	}

	public ComponentDescription getComponentDescription(String _componentID,
			Current __current) throws CASTException {

		ComponentDescription desc = m_descriptions.get(_componentID);

		if (desc == null) {
			throw new CASTException("No description for: " + _componentID
					+ ": " + m_descriptions.keySet());
		}
		return desc;
	}

}
