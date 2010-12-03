#include "PriorityTester.hpp"

#include <ChangeFilterFactory.hpp>

#include <ComedyEssentials.hpp>




using namespace std;
using namespace cast;
using namespace cast::cdl;
using namespace comedyarch::autogen;

/**
 * The function called to create a new instance of our component.
 */
extern "C" {
  cast::CASTComponentPtr 
  newComponent() {
    return new PriorityTester();
  }
}




void 
PriorityTester::start() {

  addChangeFilter(createLocalTypeFilter<Joke>(cdl::ADD),
		  new MemberFunctionChangeReceiver<PriorityTester>(this,
								   &PriorityTester::medium));  

  addChangeFilter(createLocalTypeFilter<Joke>(cdl::ADD),
		  new MemberFunctionChangeReceiver<PriorityTester>(this,
								   &PriorityTester::high),
		  cast::HIGH);  

  addChangeFilter(createLocalTypeFilter<Joke>(cdl::ADD),
		  new MemberFunctionChangeReceiver<PriorityTester>(this,
								   &PriorityTester::low),
		  cast::LOW);  
}

