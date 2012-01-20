#include "ExampleComponent.hpp"

using namespace std;
using namespace cast;
using namespace cast::cdl;


/**
 * The function called to create a new instance of our component.
 */
extern "C" {
  cast::CASTComponentPtr 
  newComponent() {
    printf("Creating unnamed component\n");
    return new ExampleComponent();
  }

  cast::CASTComponentPtr 
  newNamedComponent(const char* _name) {
    printf("Creating component named '%s'\n", _name);
    std::string name(_name);
    if (name == "Example1")
      return new ExampleComponent();
    if (name == "Example2")
      return new ExampleComponent();
    printf("Error: unknown component '%s'\n", _name);
    return 0;
  }
}




void
ExampleComponent::runComponent() {

  //CASTTime justasec(castTimeMicros(1000000));
  CASTTime justasec(castTimeMillis(1000));
  //CASTTime justasec(castTimeSeconds(1));

  for (unsigned int i = 0; i < 50 && isRunning(); ++i) {
    if(isRunning()) {
      println("running, but not really doing anything");
      
      CASTTime now(getCASTTime());

      cout<<(now)<<endl;
      cout<<(justasec)<<endl;
      cout<<(now - justasec)<<endl;
      cout<<(justasec - now)<<endl;

      sleepComponent(5000);
    }
  }
}
