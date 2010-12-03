package cast.examples;

import java.util.ArrayList;

import cast.CASTException;
import cast.architecture.ChangeFilterFactory;
import cast.architecture.ManagedComponent;
import cast.architecture.WorkingMemoryChangeReceiver;
import cast.cdl.WorkingMemoryChange;
import cast.cdl.WorkingMemoryChangeFilter;

import comedyarch.autogen.Joke;

public class PriorityTest extends ManagedComponent {

	private class PrinterReceiver implements WorkingMemoryChangeReceiver {
		private final String m_say;
		public PrinterReceiver(String _say) {
			m_say = _say;
		}
		public void workingMemoryChanged(WorkingMemoryChange _wmc)
				throws CASTException {
			println(m_say);
			removeChangeFilter(this);
		}
	}
	
	
	private ArrayList<WorkingMemoryChangeReceiver> m_receivers = new ArrayList<WorkingMemoryChangeReceiver>(3);
	private WorkingMemoryChangeFilter m_filter = ChangeFilterFactory.createGlobalTypeFilter(Joke.class);
	
	@Override
	public void start() {

		m_receivers.add(new PrinterReceiver("default"));
		m_receivers.add(new PrinterReceiver("high"));
		m_receivers.add(new PrinterReceiver("low"));
		
		
		
		int i = 0; 
		for (WorkingMemoryChangeReceiver receiver : m_receivers) {
			ChangeReceiverPriority prio;
			if(i % 3 == 0) {
				prio = ChangeReceiverPriority.MEDIUM;
			}
			else if(i % 3 == 1) {
				prio = ChangeReceiverPriority.HIGH;
			} 
			else {
				prio = ChangeReceiverPriority.LOW;
			}
			addChangeFilter(m_filter, receiver,prio);
			i++;
		}
		println(getFilterCount());
		
	}
	
	@Override
	protected void runComponent() {
		while(isRunning()) {
			sleepComponent(1000);
			log("log here");
			debug("debug here");
			println(getFilterCount());
		}
	}
}
