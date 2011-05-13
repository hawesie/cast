/**
 * 
 */
package cast.tests;

import cast.AlreadyExistsOnWMException;
import cast.architecture.ManagedComponent;
import cast.cdl.TestStructInt;

/**
 * 
 * Writes a known integer value to WM and complains if anyone changes it.
 * 
 * @author nah
 * 
 */
public class DirectAccessWriter extends ManagedComponent {

	private enum WriterBehaviour {
		CHECK, ALTER
	};

	private WriterBehaviour m_behaviour = WriterBehaviour.ALTER;

	@Override
	protected void runComponent() {

		try {
			// Create a struct with a known value
			int startingValue = 0;
			TestStructInt tsi = new TestStructInt(startingValue);

			// and add it to WM
			addToWorkingMemory(newDataID(), tsi);

			while (isRunning()) {

				if (m_behaviour == WriterBehaviour.CHECK) {
					// If the value has changed, kick up a fuss
					if (tsi.dummy != startingValue) {
						throw new RuntimeException("Value has changed");
					} else {
						println("fine");
					}
				} else if (m_behaviour == WriterBehaviour.ALTER) {
					
					tsi.dummy++;
					println("altered: " + tsi.dummy);
				}

				sleepComponent(1000);
			}

		} catch (AlreadyExistsOnWMException e) {
			logException(e);
		}

	}

}
