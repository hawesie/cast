/*
 * CAST - The CoSy Architecture Schema Toolkit
 *
 * Copyright (C) 2006-2007 Nick Hawes, Henrik Jacobsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CAST_SUBARCHITECTURE_WORKING_MEMORY_H_
#define CAST_SUBARCHITECTURE_WORKING_MEMORY_H_

#include <cast/core/SubarchitectureComponent.hpp>
#include <cast/core/CASTWorkingMemory.hpp>
#include <cast/core/CASTWMPermissionsMap.hpp>
#include <cast/architecture/WorkingMemoryChangeFilterMap.hpp>
#include <cast/core/StringMap.hpp>


#include <vector>
#include <memory>
#include <tr1/unordered_set>

#include <boost/thread/shared_mutex.hpp>

namespace cast {

  typedef std::tr1::unordered_set<std::string> StringSet;
  typedef StringMap<interfaces::WorkingMemoryPrx>::map WMPrxMap;
  
  
  class SubarchitectureWorkingMemory: 
    public SubarchitectureComponent,
    public virtual interfaces::WorkingMemory {
    
  public:
    
    
    /**
     * Construct new object with a unique id. 
     * @param _id
     */
    SubarchitectureWorkingMemory();


    /**
     * Destructor. Cleans up container memory.
     */
    virtual ~SubarchitectureWorkingMemory();



    virtual 
    void 
    configureInternal(const std::map<std::string,std::string>& _config);

    virtual 
    bool 
    exists(const std::string & _id, 
	   const std::string & _subarch, 
	   const Ice::Current & _ctx) 
      throw (UnknownSubarchitectureException);

    virtual 
    ::Ice::Int 
    getVersionNumber(const std::string & _id, 
		     const std::string & _subarch, 
		     const Ice::Current & _ctx) 
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    cdl::WorkingMemoryPermissions 
    getPermissions(const std::string & _id, 
		   const std::string & _subarch, 
		   const Ice::Current & _ctx)
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    void
    lockEntry(const std::string & _id, 
	      const std::string & _subarch, 
	      const std::string & _component, 
	      cdl::WorkingMemoryPermissions _perm, 
	      const Ice::Current & _ctx) 
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    bool 
    tryLockEntry(const std::string & _id, 
		 const std::string & _subarch,
		 const std::string & _component,  
		 cdl::WorkingMemoryPermissions _perm, 
		 const Ice::Current & _ctx)
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual
    void
    unlockEntry(const std::string & _id, 
		const std::string & _subarch, 
		const std::string & _component, 
		const Ice::Current & _ctx) 
      throw (DoesNotExistOnWMException, ConsistencyException, UnknownSubarchitectureException);

    virtual 
    void 
    setWorkingMemory(const interfaces::WorkingMemoryPrx & _wm, 
		     const std::string & _subarch, 
		     const Ice::Current & _ctx) {

      debug("setting wm for subarch %s", _subarch.c_str());
      _wm->ice_ping();
      m_workingMemories[_subarch] = _wm;
      //has to be unchecked cast for the following, as the oneway proxy has no way to return from the check
      m_workingMemories_oneway[_subarch] = interfaces::WorkingMemoryPrx::uncheckedCast(_wm->ice_oneway());
    }

    interfaces::WorkingMemoryPrx &
    getWorkingMemory(const std::string & _subarch)
      throw (UnknownSubarchitectureException) {
      WMPrxMap::iterator i = m_workingMemories.find(_subarch);
      if(i == m_workingMemories.end()) {
	std::string message("Unknown subarchitecture: " + _subarch);
	throw UnknownSubarchitectureException(message,_subarch);
      }
      return i->second;
    }

    virtual 
    void 
    addToWorkingMemory(const std::string & _id, 
		       const std::string & _subarch, 
		       const std::string & _type, 
		       const std::string & _component, 
		       const Ice::ObjectPtr & _entry, 
		       const Ice::Current & _ctx)
      throw (AlreadyExistsOnWMException, UnknownSubarchitectureException);

    virtual 
    void 
    overwriteWorkingMemory(const std::string & _id, 
			   const std::string & _subarch, 
			   const std::string & _type, 
			   const std::string & _component, 
			   const Ice::ObjectPtr & _entry, 
			   const Ice::Current & _ctx)
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    void 
    deleteFromWorkingMemory(const std::string & _id, 
			    const std::string & _subarch, 
			    const std::string & _component,
			    const Ice::Current & _ctx)
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    cdl::WorkingMemoryEntryPtr 
    getWorkingMemoryEntry(const std::string & _id, 
			  const std::string & _subarch, 
			  const std::string & _component, 
			  const Ice::Current & _ctx) 
      throw (DoesNotExistOnWMException, UnknownSubarchitectureException);

    virtual 
    cdl::WorkingMemoryEntryPtr 
    getWorkingMemoryEntry(const std::string & _id, 
			  const std::string & _component) 
      throw (DoesNotExistOnWMException);
  

    virtual 
    void 
    getWorkingMemoryEntries(const std::string & _type, 
			    const std::string & _subarch, 
			    Ice::Int _count,
			    const std::string & _component, 
			    cast::cdl::WorkingMemoryEntrySeq & _entries, 
			    const Ice::Current & _ctx)
      throw (UnknownSubarchitectureException);


    virtual 
    void 
    getWorkingMemoryEntries(const std::string & _type, 
			    Ice::Int _count,
			    const std::string & _component, 
			    cast::cdl::WorkingMemoryEntrySeq & _entries);


    virtual 
    void 
    registerComponentFilter(const cdl::WorkingMemoryChangeFilter & _filter, 
			    Ice::Int priority,
			    const ::Ice::Current & _ctx);
 
    virtual 
    void 
    removeComponentFilter(const cdl::WorkingMemoryChangeFilter & _filter, 
			  const ::Ice::Current & _ctx);
    
    virtual 
    void 
    registerWorkingMemoryFilter(const cdl::WorkingMemoryChangeFilter & _filter, 
				const std::string & _wmid,
				Ice::Int priority,
				const ::Ice::Current & _ctx);
    virtual 
    void 
    removeWorkingMemoryFilter(const cdl::WorkingMemoryChangeFilter & _filter, 
			      const ::Ice::Current & _ctx);

    virtual 
    void 
    addReader(const interfaces::WorkingMemoryReaderComponentPrx & _reader, 
	      const Ice::Current& _ctx);

  
    virtual
    void 
    receiveChangeEvent(const cdl::WorkingMemoryChange& wmc, 
		       const Ice::Current & _ctx);


  protected: 
  
    /**
     * Add a subarchitecture which should be ignored for changes
     * 
     * @param _subarch
     */
    void ignoreChangesFromSubarchitecture(const std::string & _subarch);


    /**
     * Determines whether the wm is allowed to forward this change to the
     * attached reader processes.
     * 
     * @param _change
     * @return true if the change is not from an ignored sa and passes all the filters
     */
    bool isAllowedChange(const cdl::WorkingMemoryChange & _change) const;


    /**
     * Determines whether the wm is allowed to forward this change to the
     * wm with the given id
     * 
     * @param _change
     * @return true if the change is not from an ignored sa and passes all the filters
     */

    bool
    isAllowedChange(const std::string & _wmid,
		    const cdl::WorkingMemoryChange & _change) const;


    /**
     * Add some data to working memory. If the given id already exists
     * in the working memory then the data isn't added and false is
     * returned.
     * 
     * @param _id
     *            The id to use for the data.
     * @param _pData
     *            The data itself.
     * @return True if the data is added successfully, else false.
     */
    virtual 
    bool 
    addToWorkingMemory(const std::string & _id, 
		       cdl::WorkingMemoryEntryPtr _entry);


    /**
     * Overwrite some data to working memory. If the given id does not
     * exist in the working memory then the data isn't written and false
     * is returned.
     * 
     * @param _id
     *            The id to use for the data.
     * @param _data
     *            The data itself.
     * @return True if the data is overwritten successfully, else false.
     */
    virtual 
    bool 
    overwriteWorkingMemory(const std::string & _id, 
			   cdl::WorkingMemoryEntryPtr _data,
			   const std::string & _component)
      throw (DoesNotExistOnWMException);
  

    /**
     * Delete the specified entry from working memory. If the given id
     * does not exist in the working memory then nothing happens and
     * null is returned.
     * 
     * @param _id
     *            The id of the entry to delete.
     * @return The item that is deleted, or null if nothing is deleted.
     */
    virtual 
    cdl::WorkingMemoryEntryPtr 
    deleteFromWorkingMemory(const std::string & _id,
			    const std::string & _component)
      throw (DoesNotExistOnWMException);
  

  

    /**
     * Empty run method. Does nothing.
     * 
     */
    virtual void runComponent(){};

    /**
     * Determines whether to forward change notifications to other
     * subarchitecture working memories (i.e. its peers).
     * 
     * @param sendXarchChangeNotifications
     *            The sendXarchChangeNotifications to set.
     */
    void setSendXarchChangeNotifications(bool sendXarchChangeNotifications) {
      m_sendXarchChangeNotifications = sendXarchChangeNotifications;
    }


    /**
     * Determines whether to forward change notifications to other
     * subarchitecture working memories (i.e. its peers).
     * 
     * @return Returns the sendXarchChangeNotifications.
     */
    bool isSendingXarchChangeNotifications() {
      return m_sendXarchChangeNotifications;
    }
  
    bool m_sendXarchChangeNotifications;

    CASTWorkingMemory m_workingMemory;

    ///set for storing change filters
    WorkingMemoryChangeFilterMap<std::string> m_componentFilters;

    ///set for storing change filters for other wms
    WorkingMemoryChangeFilterMap<std::string> m_wmFilters;

    ///proxies for other working memories
    WMPrxMap m_workingMemories;

    ///oneway proxies for other working memories
    WMPrxMap m_workingMemories_oneway;

  private:

  
    cdl::WorkingMemoryEntryPtr
    createEntry(const std::string & _id, 	      
		const std::string & _type, 
		const Ice::ObjectPtr & _entry) 
      throw (AlreadyExistsOnWMException) {
      cdl::WorkingMemoryEntryPtr 
	ptr(new cdl::WorkingMemoryEntry(_id, _type, 0, _entry));
      return ptr;
    }


    void readBlock(const std::string & _id, 
		   const std::string & _component);


    /**
     * Signal that an operation has occurred to all connected
     * components.
     * 
     * @param _op
     *            The operation type to signal.
     * @param _src
     *            The component that caused the operation.
     * @param _id
     *            The id of the entry that was the subject of the
     *            operation.
     * @param _type
     *            The ontological type of the entry that was the subject
     *            of the operation.
     */
    void 
    signalChange(cdl::WorkingMemoryOperation _op, const std::string & _src,
		 const std::string &  _id,  const std::string &  _type, 
		 const std::vector<std::string> & _typeHierarchy);



    void buildIDLists(const std::map<std::string,std::string>& _config);


    /**
     * Determines whether to share wm filters
     */
    bool m_wmDistributedFiltering;
  
    /**
     * Stores subarchitectures to ignore changes from
     */ 
    StringSet m_ignoreList;

    /**
     * Stores ids of other working memories.
     */ 
    StringSet m_wmIDs;


    /**
     * Used for locks and permissions
     */
    CASTWMPermissionsMap m_permissions;
  
    std::vector<interfaces::WorkingMemoryReaderComponentPrx> m_readers;


    /**
     * Shared lock used to manage read/write synchronisation
     */
    boost::shared_mutex m_readWriteLock;


  };

} //namespace cast

#endif
