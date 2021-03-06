#ifndef CAST_ICE
#define CAST_ICE

module cast
{

  module cdl
  {

    const string CASTRELEASESTRING = "2.1.17 beta (Free The Pterodactyl 3)";

    const int JAVASERVERPORT = 10111;
    const int CPPSERVERPORT = 10211;
    const int JAVACLIENTSERVERPORT = 10311;
    const int PYTHONSERVERPORT = 10411;
	const int LOGGINGPORT = 10611;

 
    const string SUBARCHIDKEY = "org.cognitivesystem.cast.subarchID"; 
    const string COMPONENTNUMBERKEY = "org.cognitivesystem.cast.componentNumber";
    const string CONFIGFILEKEY = "org.cognitivesystem.cast.config";
    const string WMIDSKEY = "org.cognitivesystem.ast.wmids";
    const string COMPONENTIDSKEY = "org.cognitivesystem.cast.compids";
    const string LOGGINGENVVAR = "CAST_LOGGING_PROPERTIES";
    const string LOGGINGPROPERTIESFILE = "log4j.properties";

    ///Default conversion pattern used for logging layout
    const string RETROCONVERSIONPATTERN = "%S[%P%i: %m]%E%n";

 
    const string LOGKEY = "--log"; 
    const string DEBUGKEY = "--debug"; 
    const string DEBUGEVENTSKEY =  "--debug-events";
    const string IGNORESAKEY =  "--ignore";

    dictionary<string,string> StringMap;


    const int RECEIVERPRIORITYHIGH = 90;
    const int RECEIVERPRIORITYMEDIUM = 50;
    const int RECEIVERPRIORITYLOW = 10;

    /**
     * Definition of component languages.
     */
    enum ComponentLanguage {
      CPP,
      JAVA,
      PYTHON
    };
    

    /**
       The information necessary to start up a component in the framework.
    **/
    struct ComponentDescription {
      string componentName;
      string className;
      ComponentLanguage language;
      string hostName;
      StringMap configuration; 
      ///Whether to create component in its own process, currently this will only be supported by C++
      bool newProcess;
    };


    ["java:array"] sequence<byte> ByteSeq;

    class TestStructString {
      string dummy;
    };

    class TestStructInt {
      int dummy;
    };

    struct WorkingMemoryAddress {
      string id;
      string subarchitecture;
    };

    //life is easier with it as a class. more stuff generated for us
    class WorkingMemoryEntry {
      string id;
      string type;
      int version;
      Object entry;
    };

    ["java:type:java.util.LinkedList<cast.cdl.WorkingMemoryEntry>:java.util.List<cast.cdl.WorkingMemoryEntry>"]	
    sequence<WorkingMemoryEntry> WorkingMemoryEntrySeq;

    enum WorkingMemoryOperation {
      ADD,
      OVERWRITE,
      DELETE,
      GET,
      ///wildcard operation for use in filtering
      WILDCARD
    };

    enum WorkingMemoryChangeQueueBehaviour {
      DISCARD,
      QUEUE
    };


    /*
     * Enum indicating the result of a wm lock query.
     */
    enum WorkingMemoryPermissions {      
      LOCKEDO,
      LOCKEDOD,
      LOCKEDODR,      
      UNLOCKED,
      DOESNOTEXIST,

      /**
       * Result of a failed tryLock
       */
      ALREADYLOCKED
    };

    /*
     * Enum indicating the result of a wm lock query.
     */
    enum WorkingMemoryLockRequest {
      REQUESTLOCKO,
      REQUESTLOCKOD,
      REQUESTLOCKODR,
      REQUESTTRYLOCKO,
      REQUESTTRYLOCKOD,
      REQUESTTRYLOCKODR,
      REQUESTUNLOCK,
      REQUESTSTATUS
    };
    

    /*
    * Enum indicating whether a change filter is applicable to just
    * the locally attached wm or all wms.
    */
    enum FilterRestriction {
      /**
       * This change filter is applicable only to change events
       * referring to data on the components local working memory.
       */
      LOCALSA,
      /**
       * This change filter is applicable only to change events from
       * all working memories
       */
      ALLSA
    };


    enum ReceiverDeleteCondition {
      DELETERECEIVER,
      DONOTDELETERECEIVER
    };



    /**
     * Basic typed "pointer" for indicated working memory entries.
     */
    class WorkingMemoryPointer {
      ///The address of the entry
      WorkingMemoryAddress address;
      ///The type of the entry
      string type;    
    };


    /**
     * Time in seconds and microseconds since the current CAST run started.
     */
    struct CASTTime {
      // seconds
      long s;    
      // microseconds
      long us;   
    };


    sequence<string> StringSeq;
        
   
    struct WorkingMemoryChange {
      WorkingMemoryOperation operation;
      string src;
      WorkingMemoryAddress address;

      ///The type of the instantiated object
      string type;
      ///The super types of the object, taken from the static ids of the ice class
      StringSeq superTypes;      

      ///The (approximate) time the change occurred on working memory
      CASTTime timestamp;
    };

    /**
     * An object that represents a filter for filtering in changes from
     * working memory.
     */
    struct WorkingMemoryChangeFilter {
      /**
       * 
       */
      WorkingMemoryOperation operation;


      /**
       * Source of change. If not "" then adhered to.
       */
      string src;


      /**
       * If either of these are not "" then the filter will adhere to
       * them.
       */
      WorkingMemoryAddress address;

      string type;

      /**
       * Whether this filter should only accept changes from local
       * sources.
       */
      FilterRestriction restriction;


      /**
       * The component that originally created the change. Used to
       * distinguish otherwise identical filters when aggregated.
       */
      string origin;
      
    };



    enum TaskOutcome {
      ProcessingIncomplete,
      ProcessingComplete,
      ProcessingCompleteSuccess,
      ProcessingCompleteFailure
    };

    enum TaskManagementDecision {
      TaskAdopted,
      TaskRejected,
      TaskWaiting
    };



    /***
     * A module containing code purely for test CAST and CAST systems.
     ***/
    module testing {

      //moderately arbitrary values

      const long CASTTESTPASS = 29;
      const long CASTTESTFAIL = 30;

      ///Struct used for basic testing
      class CASTTestStruct {
	long count;	
	WorkingMemoryChange change;
      };

      /// used only for testing WM interaction
      class TestDummyStruct {
	string dummy;
      };      
    };




  };
  
  
  //CAST exceptions, in main cast namespce to match old design
  
  exception CASTException {
    string message;
  };

  exception ComponentCreationException extends CASTException {};

  exception SubarchitectureComponentException extends CASTException {};
 
  exception UnknownSubarchitectureException extends SubarchitectureComponentException {	    	    
   string subarchitecture;	   
  };

  exception WMException extends SubarchitectureComponentException {
    cdl::WorkingMemoryAddress wma;
  };

  exception DoesNotExistOnWMException extends WMException {};

  exception AlreadyExistsOnWMException extends WMException {};

  exception ConsistencyException extends WMException {};

  exception PermissionException extends WMException {};


  module interfaces
  {

    //fwd decls
    interface WorkingMemory;
    interface TaskManager;
    interface ComponentManager;


	/**
	* Server to collect logging output and send to single sink.
	*/
	interface LoggingServer {
		void logMessage(string message);
		/**
		* Log an event in a serialised form compatible with Java object serialisation. This works for Java and C++ with the latter using the Log4CXX output stream classes.
		*/
		void logSerialisedEvent(cast::cdl::ByteSeq event);

		/**
		* Log an event in a serialised form compatible with Java object serialisation. This works for Java and C++ with the latter using the Log4CXX output stream classes. The C++ serialisation doesn't maintain the extra info added by CAST, so this allows them to be send additionally.
		*/
			void logSerialisedEventWithAdditions(cast::cdl::ByteSeq event, string id, string saID, string colourStart);
	};

	/**
     * Servers to sync time across languages and machines. Currently wraps a simple monotonic timer.
     */
     interface TimeServer {
         ["cpp:const"] cdl::CASTTime getCASTTime();		

		/**
		* Convert the give double into a CASTTime. This creates a CASTTime representing the difference between the given time and the time server's zero time. 
		*/
		["cpp:const"] cdl::CASTTime fromTimeOfDayDouble(double todsecs);	
        
		/**
		* Convert the given secs and microsecs into a CASTTime. This creates a CASTTime representing the difference between the given time and the time server's zero time. 
		*/
		["cpp:const"] cdl::CASTTime fromTimeOfDay(long secs, long usecs);	
            	

       // /**
//        * Sync the time server to this time.
//        */
//        void syncTo(long secs, long usecs);

        /**
        * Sync the time server to 0. Should be equivalent to syncTo(0,0)
        */
        void reset();

     };   

    interface CASTComponent {
      ["cpp:const"] idempotent void beat();
      void setID(string id);

      /**
      * Get the id of this component.
      */
      idempotent string getID();

      void configure(cdl::StringMap config);
      void start();
      void run();                    
      void stop();
      
	  void setComponentManager(ComponentManager* man);
	  void setTimeServer(TimeServer* ts);

      /**
       * Destroy the component. This removes it from CAST server.
       **/
      void destroy();
    };




    interface WorkingMemoryAttachedComponent extends CASTComponent {
      void setWorkingMemory(WorkingMemory* wm);
    };
        

    interface WorkingMemoryReaderComponent extends WorkingMemoryAttachedComponent {
      void receiveChangeEvent(cdl::WorkingMemoryChange wmc);
    };

    interface ManagedComponent extends WorkingMemoryReaderComponent {
      void setTaskManager(TaskManager* tm);
      void taskDecision(string id, cdl::TaskManagementDecision decision);
    };

    interface UnmanagedComponent extends WorkingMemoryAttachedComponent {
    };

    interface TaskManager extends WorkingMemoryReaderComponent {
      /**
       * Propose a new task for component.
       */
      void proposeTask(string component, string taskID, string taskName);           
    
      /**
       * Retract a proposed task.
       */
      void retractTask(string component, string taskID);           

      /**
       * A task is finished
       */
      void taskComplete(string component, string taskID, cdl::TaskOutcome outcome);           
    
      /**
       * Add managed component to manager.
       */
      void addManagedComponent(ManagedComponent* comp);

    };

    
    
    interface WorkingMemory extends CASTComponent {

      idempotent bool exists(string id, string subarch) 
	throws UnknownSubarchitectureException;

      idempotent int getVersionNumber(string id, string subarch) 
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      idempotent cdl::WorkingMemoryPermissions getPermissions(string id, string subarch) 
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;
      
      void lockEntry(string id, string subarch, string component,
		     cdl::WorkingMemoryPermissions permissions)
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      bool tryLockEntry(string id, string subarch, string component,
			cdl::WorkingMemoryPermissions permissions) 
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      void unlockEntry(string id, string subarch, string component) 
	throws DoesNotExistOnWMException, ConsistencyException, UnknownSubarchitectureException;

      void setWorkingMemory(WorkingMemory* wm, string subarch);

       void addToWorkingMemory(string id, string subarch,
	    		      string type, string component,
			      Object entry)
	throws AlreadyExistsOnWMException, UnknownSubarchitectureException;

      void overwriteWorkingMemory(string id, string subarch,
				  string type, string component,
				  Object entry)
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      void deleteFromWorkingMemory(string id, string subarch, 
				   string component)
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      cdl::WorkingMemoryEntry getWorkingMemoryEntry(string id, 
						    string subarch,
						    string component)
	throws DoesNotExistOnWMException, UnknownSubarchitectureException;

      void getWorkingMemoryEntries(string type, 
				   string subarch, 
				   int count,
				   string component,
				   out cdl::WorkingMemoryEntrySeq entries)
	throws UnknownSubarchitectureException;
      

      void registerComponentFilter(cdl::WorkingMemoryChangeFilter filter, int priority);
      void removeComponentFilter(cdl::WorkingMemoryChangeFilter filter);
      
      void registerWorkingMemoryFilter(cdl::WorkingMemoryChangeFilter filter, 
				       string subarch,
				       int priority);
      void removeWorkingMemoryFilter(cdl::WorkingMemoryChangeFilter filter);
      

      void addReader(WorkingMemoryReaderComponent* reader);

      void receiveChangeEvent(cdl::WorkingMemoryChange wmc);


    };
    

    dictionary<string,cdl::ComponentDescription> ComponentDescriptionMap;

    /**
     * Interface to manage all components
     */
    interface ComponentManager {

      /**
       * Store the descirption of a component with the manager.
       */
      idempotent void addComponentDescription(cdl::ComponentDescription description);
    
      /**
       * Get the description of a particular component.
       */
      ["cpp:const"] idempotent cdl::ComponentDescription getComponentDescription(string componentID) throws CASTException;

      /**
       * Descriptions of all components indexed by component id.
       */
      ["cpp:const"] idempotent ComponentDescriptionMap getComponentDescriptions();

 

   };

  



    interface ComponentFactory {

      CASTComponent* newComponent(string id, string type, bool newProcess) throws ComponentCreationException;
      ManagedComponent* newManagedComponent(string id, string type, bool newProcess) throws ComponentCreationException;
      UnmanagedComponent* newUnmanagedComponent(string id, string type, bool newProcess) throws ComponentCreationException;
      WorkingMemory* newWorkingMemory(string id, string type, bool newProcess) throws ComponentCreationException;
      TaskManager* newTaskManager(string id, string type, bool newProcess) throws ComponentCreationException;

      /** Returns version information for this factory **/
      string getVersionString();

    };

  };

  module examples {
    module autogen {
    /**
    * Test interface to demonstrate standard server connections.
    */
      interface WordServer {
	    string getNewWord();
      };
      
      
      /**
      * Interface needed to directly inherit in C++. Ignore for Java.
      */
      interface WordServerAsComponent extends cast::interfaces::CASTComponent {
	    string getNewWord();
      };

    };
  };	 


};
#endif
