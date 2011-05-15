/**
 * 
 */
package cast.tests;

import cast.CASTException;
import cast.architecture.ChangeFilterFactory;
import cast.architecture.ManagedComponent;
import cast.architecture.WorkingMemoryChangeReceiver;
import cast.cdl.TestStructInt;
import cast.cdl.WorkingMemoryChange;
import cast.cdl.WorkingMemoryOperation;

/**
 * 
 * Reads a value from working memory then changes it locally without writing it
 * back to WM.
 * 
 * @author nah
 * 
 */
public class DirectAccessReader extends ManagedComponent implements
		WorkingMemoryChangeReceiver {

	private enum ReaderBehaviour {
		CHECK, ALTER
	};

	private ReaderBehaviour m_behaviour = ReaderBehaviour.CHECK;

	@Override
	protected void start() {
		addChangeFilter(ChangeFilterFactory.createLocalTypeFilter(
				TestStructInt.class, WorkingMemoryOperation.ADD), this);
	}

	public void workingMemoryChanged(WorkingMemoryChange _wmc)
			throws CASTException {
		TestStructInt tsi = getMemoryEntry(_wmc.address, TestStructInt.class);

		if (m_behaviour == ReaderBehaviour.ALTER) {
			tsi.dummy++;
		} else if (m_behaviour == ReaderBehaviour.CHECK) {
			int startingValue = tsi.dummy;
			while (isRunning()) {
				tsi = getMemoryEntry(_wmc.address, TestStructInt.class);
				if (tsi.dummy != startingValue) {
					throw new RuntimeException("Value has changed");
				} else {
					println("fine: " + tsi.dummy);
				}
				sleepComponent(1000);
			}

		}
	}

}
