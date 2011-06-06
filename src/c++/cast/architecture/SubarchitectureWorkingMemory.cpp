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

#include "SubarchitectureWorkingMemory.hpp"

#include <CASTWorkingMemory.hpp>
#include <CASTUtils.hpp>

#include <boost/thread/locks.hpp>

using namespace std;

/**
 * The function called to create a new instance of our component.
 */
extern "C" {
  cast::CASTComponentPtr newComponent() {
    return new cast::SubarchitectureWorkingMemory();
  }
}

namespace cast {
  
  using namespace cdl;
  using namespace interfaces;
  
  SubarchitectureWorkingMemory::SubarchitectureWorkingMemory() :
  m_wmDistributedFiltering(true) {
    
    setSendXarchChangeNotifications(true);
  }
  
  SubarchitectureWorkingMemory::~SubarchitectureWorkingMemory() {
    
  }
  
  void SubarchitectureWorkingMemory::addReader(
                                               const interfaces::WorkingMemoryReaderComponentPrx & _reader,
                                               const Ice::Current& _ctx) {
    //only have oneway connections, so now return signal or value
    //TODO see if errors from network are at all likely, and or replace with datagram proxies
    //
    m_readers.push_back(
                        interfaces::WorkingMemoryReaderComponentPrx::uncheckedCast(
                                                                                   _reader->ice_oneway()));
  }
  
  
  bool SubarchitectureWorkingMemory::isAllowedChange(
                                                     const cdl::WorkingMemoryChange & _change) const {
    
    string subarch(_change.address.subarchitecture);
    StringSet::const_iterator i = m_ignoreList.find(subarch);
    return i == m_ignoreList.end() && m_componentFilters.allowsChange(_change);
  }
  
  bool SubarchitectureWorkingMemory::isAllowedChange(
                                                     const std::string & _subarch, const cdl::WorkingMemoryChange & _change) const {
    
    vector<string> receivers;
    // get all receivers for this change
    
    //     cout<<"isAllowedChange: "<<m_wmFilters.size()<<endl;
    m_wmFilters.get(_change, receivers);
    // if the given id is a recevier, then go ahead
    
    //     cout<<"isAllowedChange: "<<receivers.size()<<endl;
    
    
    vector<string>::iterator found = find(receivers.begin(), receivers.end(),
                                          _subarch);
    
    //     for(vector<string>::const_iterator i = receivers.begin();
    // 	i < receivers.end(); ++i) {
    
    //       cout<<"isAllowedChange: "<<*i<<endl;
    //       cout<<"isAllowedChange: "<<_subarch<<endl;
    //     }
    
    //     cout<<"isAllowedChange: "<<(found != receivers.end())<<endl;
    
    return found != receivers.end();
  }
  
  void SubarchitectureWorkingMemory::overwriteWorkingMemory(
                                                            const std::string & _id, const std::string & _subarch,
                                                            const std::string & _type, const std::string & _component,
                                                            const Ice::ObjectPtr & _entry, const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if (getSubarchitectureID() == _subarch) {
      boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
      bool result = overwriteWorkingMemory(_id, createEntry(_id, _type,
                                                            _entry), _component);
      //sanity check
      assert(result);
      signalChange(cdl::OVERWRITE, _component, _id, _type, _entry->ice_ids());
    } else {
      //send on to the one that really cares
      getWorkingMemory(_subarch)->overwriteWorkingMemory(_id, _subarch,
                                                         _type, _component, _entry);
    }
  }
  
  bool SubarchitectureWorkingMemory::overwriteWorkingMemory(const string & _id,
                                                            WorkingMemoryEntryPtr _pData, const string & _component)
  throw (DoesNotExistOnWMException) {
    
    //first sanity check
    if (!m_workingMemory.contains(_id)) {
      throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,"Entry does not exist to overwrite. Was trying to overwrite id %s in subarchitecture %s" ,
                                                       _id.c_str(),getSubarchitectureID().c_str()),
                                      makeWorkingMemoryAddress(_id,getSubarchitectureID())));
    }
    
    //sanity check locking elsewhere
    if (m_permissions.isLocked(_id)) {
      const WorkingMemoryPermissions & permissions =
      m_permissions.getPermissions(_id);
      
      // if the item is delete locked
      if (!overwriteAllowed(permissions)) {
        // we're going to assume that client checking is up to scratch
        assert (m_permissions.isLockHolder(_id, _component));
      }
      else {
        //this is never going to happen as all locks are overwrite
        //locks
      }
    }
    
    bool result = m_workingMemory.overwrite(_id,_pData);
    
    return result;
  }
  
  void
  SubarchitectureWorkingMemory::deleteFromWorkingMemory(const std::string& _id,
                                                        const std::string & _subarch,
                                                        const std::string & _component,
                                                        const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
      WorkingMemoryEntryPtr entry(deleteFromWorkingMemory(_id, _component));
      //sanity check
      assert(entry);
      signalChange(cdl::DELETE,_component,_id,entry->type, entry->entry->ice_ids());
    }
    else {
      //send on to the one that really cares
      getWorkingMemory(_subarch)->deleteFromWorkingMemory(_id,_subarch,_component);
    }
  }
  
  WorkingMemoryEntryPtr
  SubarchitectureWorkingMemory::deleteFromWorkingMemory(const string & _id,
                                                        const string & _component)
  throw (DoesNotExistOnWMException) {
    
    //first sanity check
    if(!m_workingMemory.contains(_id)) {
      throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                       "Entry does not exist to delete. Was trying to delete id %s in subarchitecture %s",
                                                       _id.c_str(),getSubarchitectureID().c_str()),
                                      makeWorkingMemoryAddress(_id,getSubarchitectureID())));
    }
    
    bool isLocked = false;
    
    if (m_permissions.isLocked(_id)) {
      
      const WorkingMemoryPermissions & permissions =
      m_permissions.getPermissions(_id);
      
      // if the item is delete locked
      if (!deleteAllowed(permissions)) {
        // we're going to assume that client checking is up to scratch
        assert (m_permissions.isLockHolder(_id, _component));
      }
      else {
        // could just be an overwrite lock, in which case it doesn't
        // matter who is the lock holder
      }
      
      isLocked = true;
    }
    
    WorkingMemoryEntryPtr pResult(m_workingMemory.remove(_id));
    
    if (isLocked) {
      // unlock on deletion
      debug("unlocking on deletion: %s %s",_id.c_str(),_component.c_str());
      m_permissions.unlock(_id, _component);
    }
    
    debug("unlocking on deletion before permission removal: %s %s",_id.c_str(),_component.c_str());
    
    //must unlock around remove as it may block
    m_readWriteLock.unlock();
    m_permissions.remove(_id);
    m_readWriteLock.lock();
    
    debug("unlocking on deletion after permission removal: %s %s",_id.c_str(),_component.c_str());
    
    return pResult;
  }
  
  cdl::WorkingMemoryEntryPtr
  SubarchitectureWorkingMemory::getWorkingMemoryEntry(const std::string & _id,
                                                      const std::string & _subarch,
                                                      const std::string & _component,
                                                      const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);
      cdl::WorkingMemoryEntryPtr entry = getWorkingMemoryEntry(_id,_component);
      return entry;
    }
    else {
      //      println("remote query");
      //send on to the one that really cares
      return getWorkingMemory(_subarch)->getWorkingMemoryEntry(_id,_subarch,_component);
    }
    
  }
  
  cdl::WorkingMemoryEntryPtr
  SubarchitectureWorkingMemory::getWorkingMemoryEntry(const std::string & _id,
                                                      const std::string & _component)
  throw (DoesNotExistOnWMException) {
    
    
    if (m_workingMemory.contains(_id)) {
      
      
      //block access if necessary
      readBlock(_id,_component);
      
      WorkingMemoryEntryPtr entry = m_workingMemory.get(_id);
      
      
      //the entry might not be there if something changed during the
      //readBlock
      if(entry) {
        return entry;
      }
      else {
        debug("Entry was deleted during readBlock: %s", _id.c_str());
      }
      
    }
    
    
    
    //if we get this far nothing exists to return
    throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                     "Entry does not exist for reading. Was looking in subarch %s for id %s",
                                                     getSubarchitectureID().c_str(),_id.c_str()),
                                    makeWorkingMemoryAddress(_id,getSubarchitectureID())));
    
  }
  
  
  void
  SubarchitectureWorkingMemory::getWorkingMemoryEntries(const std::string & _type,
                                                        const std::string & _subarch,
                                                        Ice::Int _count,
                                                        const std::string & _component,
                                                        cast::cdl::WorkingMemoryEntrySeq & _entries,
                                                        const Ice::Current & _ctx)
  
  throw (UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {      
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);
      getWorkingMemoryEntries(_type,_count, _component, _entries);
    }
    else {
      //send on to the one that really cares
      getWorkingMemory(_subarch)->getWorkingMemoryEntries(_type,_subarch,_count,_component, _entries);
    }
    
    
  }
  
  
  void
  SubarchitectureWorkingMemory::getWorkingMemoryEntries(const std::string & _type,
                                                        Ice::Int _count,
                                                        const std::string & _component,
                                                        cast::cdl::WorkingMemoryEntrySeq & _entries) {
    
    
    //get ids of the entries to return
    vector<string> ids;
    m_workingMemory.getIDsByType(_type,_count, ids);
    
    //now get each item in turn
    for(vector<string>::const_iterator i = ids.begin();
        i < ids.end(); ++i) {
      
      
      //check whether we need to block
      readBlock(*i, _component);
      
      WorkingMemoryEntryPtr entry(m_workingMemory.get(*i));
      
      //if deletion during read block then things might not be there
      //any more
      if(entry) {
        _entries.push_back(entry);
      }
    }
    
  }
  
  
  void
  SubarchitectureWorkingMemory::receiveChangeEvent(const cdl::WorkingMemoryChange& wmc,
                                                   const Ice::Current & _ctx) {
    
    boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
    
    // if the filters require external changes, allow them to be
    // forwarded
    if (!m_componentFilters.localFiltersOnly()) {
      
      if(m_bDebugOutput) {
        ostringstream outStream;
        outStream<<"forwarding change: "<<wmc;
        debug(outStream.str());
      }
      
      for(vector<WorkingMemoryReaderComponentPrx>::iterator reader = m_readers.begin();
          reader < m_readers.end(); ++ reader) {
        (*reader)->receiveChangeEvent(wmc);
      }
      
    }
    
  }
  
  
  
  
  void
  SubarchitectureWorkingMemory::signalChange(cdl::WorkingMemoryOperation _op,
                                             const string & _src,
                                             const string &  _id,
                                             const string &  _type,
                                             const vector<string> & _typeHierarchy) {
	  
    
    cdl::WorkingMemoryChange wmc;
    wmc.operation = _op;
    wmc.src = _src;
    wmc.address.id = _id;
    wmc.address.subarchitecture = getSubarchitectureID();
    wmc.type = _type;
    wmc.superTypes = _typeHierarchy;
    wmc.timestamp = getCASTTime();
    
    if(m_bDebugOutput) {
      ostringstream outStream;
      outStream<<"SubarchitectureWorkingMemory::signalChange: "<<wmc<<endl;
      debug(outStream.str());
    }
    
    //signal change locally if allowed
    if (isAllowedChange(wmc)) {
      //send locally
      for(vector<WorkingMemoryReaderComponentPrx>::iterator reader = m_readers.begin();
          reader < m_readers.end(); ++ reader) {
        (*reader)->receiveChangeEvent(wmc);
      }
    }
    
    // signal change across sub-architectures where appropriate
    if (isSendingXarchChangeNotifications()) {
      for(WMPrxMap::iterator i = m_workingMemories_oneway.begin();
          i != m_workingMemories_oneway.end(); ++i) {
        
        if(isAllowedChange(i->first,wmc)) {
          
          i->second->receiveChangeEvent(wmc);
        }
      }
    }
    
  }
  
  
  void
  SubarchitectureWorkingMemory::readBlock(const std::string & _id,
                                          const std::string & _component) {
    
    
    debug("start readBlock: %s %s", _id.c_str(), _component.c_str());
    
    WorkingMemoryPermissions perms = m_permissions.getPermissions(_id);
    
    while(m_permissions.isLocked(_id) &&
          !m_permissions.isLockHolder(_id, _component) &&
          !readAllowed(perms)) {
      
      //debug("blocking read by " + _component + " because " + _id
      //    + " is locked by " + m_permissions.getLockHolder(_id));
      // block
      
      // unlock before we block to allow other things to happen
      debug("locking readBlock: %s %s", _id.c_str(), _component.c_str());
      
      m_readWriteLock.unlock_shared();
      
      //lock entry, this will block
      m_permissions.lock(_id,_component, cdl::LOCKEDODR);
      
      debug("locked readBlock: %s %s", _id.c_str(), _component.c_str());
      
      // relock to finish the operation
      m_readWriteLock.lock_shared();
      
      
      debug("and inside locked readBlock: %s %s", _id.c_str(), _component.c_str());
      
      
      m_permissions.unlock(_id,_component);
      
      debug("unlocked readBlock: %s %s", _id.c_str(), _component.c_str());
      
      //now check if entry still exists as it could've been deleted
      //while we block... so we don't even hold a lock
      if(!m_workingMemory.contains(_id)) {
        debug("deletion during readBlock, returning");
        return;
      }
      
      //update permissions
      perms = m_permissions.getPermissions(_id);
    }
    
    debug("end readBlock: %s %s", _id.c_str(), _component.c_str());
  }
  
  
  void SubarchitectureWorkingMemory::configureInternal(const map<string,string>& _config) {
    SubarchitectureComponent::configureInternal(_config);
    
    map<string,string>::const_iterator key = _config.find(cdl::IGNORESAKEY);
    if(key != _config.end()) {
      const string & ignore = key->second;
      vector<string> ignoreList;
      tokenizeString(ignore,
                     ignoreList,
                     ",");
      for(vector<string>::iterator i = ignoreList.begin();
          i < ignoreList.end();
          ++i) {
        ignoreChangesFromSubarchitecture(*i);
      }
      
    }
    
    buildIDLists(_config);
  }
  
  void SubarchitectureWorkingMemory::ignoreChangesFromSubarchitecture(const string & _subarch) {
    log("ignoring changes from: %s",_subarch.c_str());
    m_ignoreList.insert(_subarch);
  }
  
  void SubarchitectureWorkingMemory::buildIDLists(const std::map<std::string,std::string>& _config) {
    
    map<string,string>::const_iterator key = _config.find(cdl::WMIDSKEY);
    assert(key != _config.end());
    const string & wmIDs = key->second;
    vector<string> ids;
    tokenizeString(wmIDs, ids, ",");
    for(vector<string>::iterator id = ids.begin();
        id < ids.end();
        ++id) {
      //debug(*id);
      m_wmIDs.insert(*id);
    }
  }
  
  
  
  bool
  SubarchitectureWorkingMemory::exists(const std::string & _id,
                                       const std::string & _subarch,
                                       const Ice::Current & _ctx)
  throw (UnknownSubarchitectureException) {
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);
      return m_workingMemory.contains(_id);
    }
    else {
      //send on to the one that really cares
      return getWorkingMemory(_subarch)->exists(_id,_subarch);
    }
  }
  
  
  Ice::Int
  SubarchitectureWorkingMemory::getVersionNumber(const std::string & _id,
                                                 const std::string & _subarch,
                                                 const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);
      //if it has once existed, then return a value
      if(m_workingMemory.hasContained(_id)) {
        Ice::Int versionNumber = m_workingMemory.getOverwriteCount(_id);
        return versionNumber;
      }
      //else kick up a fuss
      else {
        throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                         "Entry has never existed on wm. Was looking in subarch %s for id %s",
                                                         _subarch.c_str(),_id.c_str()),
                                        makeWorkingMemoryAddress(_id,_subarch)));
      }
    }
    else {
      //send on to the one that really cares
      return getWorkingMemory(_subarch)->getVersionNumber(_id,_subarch);
    }
  }
  
  
  cdl::WorkingMemoryPermissions
  SubarchitectureWorkingMemory::getPermissions(const std::string & _id,
                                               const std::string & _subarch,
                                               const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      //if it exists, then return a value
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);
      
      if(m_workingMemory.contains(_id)) {
        WorkingMemoryPermissions permissions = m_permissions.getPermissions(_id);
        return permissions;
      }
      //else kick up a fuss
      else {
        throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                         "Entry does not exist on wm  . Was looking in subarch %s for id %s",
                                                         _subarch.c_str(),_id.c_str()),
                                        makeWorkingMemoryAddress(_id,_subarch)));
      }
    }
    else {
      //get the correct wm and query that instead
      //send on to the one that really cares
      return getWorkingMemory(_subarch)->getPermissions(_id,_subarch);
    }
    
  }
  
  
  void
  SubarchitectureWorkingMemory::lockEntry(const std::string & _id,
                                          const std::string & _subarch,
                                          const std::string & _component,
                                          cdl::WorkingMemoryPermissions _perm,
                                          const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      //
      
      m_readWriteLock.lock_shared();
      
      if (m_workingMemory.contains(_id)) {
        
        debug("%s locking: %s",_component.c_str(),_id.c_str());
        
        
        // now unlock incase the WM locking blocks
        m_readWriteLock.unlock_shared();

        m_permissions.lock(_id, _component, _perm);
        
        
        // relock so we're back in control
        m_readWriteLock.lock_shared();
        
        
        //now check that it still exists, because it could've been
        //deleted before the lock was released
        
        
        
        if (!m_workingMemory.contains(_id)) {
          m_permissions.unlock(_id, _component);
          m_readWriteLock.unlock_shared();
          throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                           "Entry deleted whiile waiting for lock. Component %s was looking in subarch %s for id %s",
                                                           _component.c_str(),
                                                           _subarch.c_str(),
                                                           _id.c_str()),
                                          makeWorkingMemoryAddress(_id,_subarch)));
        }
        else {          
          assert(m_permissions.isLockHolder(_id,_component));
          assert(m_permissions.getPermissions(_id) == _perm);
//          debug("%s locked: ok",_component.c_str());
        }
      }
      //else kick up a fuss
      else {
        m_readWriteLock.unlock_shared();
        throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                         "Entry does not exist for locking. Was looking in subarch %s for id %s",
                                                         _subarch.c_str(),_id.c_str()),
                                        makeWorkingMemoryAddress(_id,_subarch)));
      }
      
      m_readWriteLock.unlock_shared();
    }
    else {
      getWorkingMemory(_subarch)->lockEntry(_id,_subarch, _component, _perm);
    }
  }
  
  
  
  
  bool
  SubarchitectureWorkingMemory::tryLockEntry(const std::string & _id,
                                             const std::string & _subarch,
                                             const std::string & _component,
                                             cdl::WorkingMemoryPermissions _perm,
                                             const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);

      if (m_workingMemory.contains(_id)) {
        if (m_permissions.tryLock(_id, _component, _perm)) {
          debug("%s try locked: %s",_component.c_str(),_id.c_str());
          assert(m_permissions.isLockHolder(_id,_component));
          assert(m_permissions.getPermissions(_id) == _perm);
          debug("%s try locked: ok",_component.c_str());
          return true;
        }
        else {
          return false;
        }
      }
      //else kick up a fuss
      else {
        throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                         "Entry does not exist for try-locking. Was looking in subarch %s for id %s",
                                                         _subarch.c_str(),_id.c_str()),
                                        makeWorkingMemoryAddress(_id,_subarch)));
      }
    }
    else {
      //get the correct wm and query that instead
      return getWorkingMemory(_subarch)->tryLockEntry(_id,_subarch, _component, _perm);
    }
  }
  
  
  void
  SubarchitectureWorkingMemory::unlockEntry(const std::string & _id,
                                            const std::string & _subarch,
                                            const std::string & _component,
                                            const Ice::Current & _ctx)
  throw (DoesNotExistOnWMException, ConsistencyException, UnknownSubarchitectureException) {
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
 
      boost::shared_lock<boost::shared_mutex> locker(m_readWriteLock);

      if (m_workingMemory.contains(_id)) {
        
        //this wasn't in the original design, but seems necessary now
        //we can do this at wm level
        if(!m_permissions.isLocked(_id)) {
          throw(ConsistencyException(exceptionMessage(__HERE__,
                                                      "Entry is not locked. %s was looking in subarch %s for id %s",
                                                      _component.c_str(),_subarch.c_str(),_id.c_str()),
                                     makeWorkingMemoryAddress(_id,_subarch)));
        }
        else if(m_permissions.getLockHolder(_id) != _component) {
          throw(ConsistencyException(exceptionMessage(__HERE__,
                                                      "Entry is not locked, but not by %s, who was looking in subarch %s for id %s",
                                                      _component.c_str(),_subarch.c_str(),_id.c_str()),
                                     makeWorkingMemoryAddress(_id,_subarch)));
        }
        
        m_permissions.unlock(_id, _component);
        debug("%s unlocked %s",_component.c_str(),_id.c_str());
      }
      //else kick up a fuss
      else {
        throw(DoesNotExistOnWMException(exceptionMessage(__HERE__,
                                                         "Entry does not exist for unlocking. Was looking in subarch %s for id %s",
                                                         _subarch.c_str(),_id.c_str()),
                                        makeWorkingMemoryAddress(_id,_subarch)));
      }
    }
    else {
      //get the correct wm and query that instead
      getWorkingMemory(_subarch)->unlockEntry(_id,_subarch, _component);
    }
    
  }
  
  
  bool
  SubarchitectureWorkingMemory::addToWorkingMemory(const string & _id,
                                                   WorkingMemoryEntryPtr _entry) {
    bool result = m_workingMemory.add(_id,_entry);
    if (result) {
      m_permissions.add(_id);
    }
    return result;
  }
  
  
  
  
  void
  SubarchitectureWorkingMemory::addToWorkingMemory(const std::string & _id,
                                                   const std::string & _subarch,
                                                   const std::string & _type,
                                                   const std::string & _component,
                                                   const Ice::ObjectPtr & _entry,
                                                   const Ice::Current & _ctx)
  throw (AlreadyExistsOnWMException, UnknownSubarchitectureException) {
    
    
    //if this is for me
    if(getSubarchitectureID() == _subarch) {
      //if it already exists complain bitterly
      
      boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);	
      
  	  if (m_workingMemory.contains(_id)) {		
        throw(AlreadyExistsOnWMException(exceptionMessage(__HERE__,
                                                          "Entry already exists on WM. Was trying to write id %s in subarchitecture %s",
                                                          _id.c_str(),_subarch.c_str()),
                                         makeWorkingMemoryAddress(_id,_subarch)));
        
      }
      //else get stuck in
      else {
        bool result = addToWorkingMemory(_id, createEntry(_id,_type,_entry));
        //sanity check
        assert(result);
        signalChange(cdl::ADD,_component,_id,_type, _entry->ice_ids());
      }
    }
    else {
      //get the correct wm and query that instead
      getWorkingMemory(_subarch)->addToWorkingMemory(_id,_subarch, _type, _component, _entry);
    }
  }
  
  
  
  void
  SubarchitectureWorkingMemory::registerComponentFilter(const cdl::WorkingMemoryChangeFilter & _filter,
                                                        Ice::Int priority,
                                                        const ::Ice::Current & _ctx) {
    
    
    boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
    
    debug("SubarchitectureWorkingMemory::registerComponentFilter()");
    ostringstream outStream;
    outStream<<_filter;
    debug(outStream.str());
    
    int prio = priority;
    m_componentFilters.put(_filter,_filter.origin,prio);
    
    //cout<<"new filters length: "<<m_componentFilters.size()<<endl;
    //cout<<"only local: "<<m_componentFilters.localFiltersOnly()<<endl;
    
    
    if (m_wmDistributedFiltering) {
      debug("fowarding filter");
      //sendFilter(_filter);
      for(WMPrxMap::iterator i = m_workingMemories.begin();
          i != m_workingMemories.end(); ++i) {
        debug("forwarding to %s", i->first.c_str());
        debug("forwarding to %s", i->second->getID().c_str());
        i->second->registerWorkingMemoryFilter(_filter, getSubarchitectureID(), priority);
      }
    }
    
    
  }
  
  void
  SubarchitectureWorkingMemory::registerWorkingMemoryFilter(const cdl::WorkingMemoryChangeFilter & _filter,
                                                            const std::string & _subarch,
                                                            Ice::Int priority,
                                                            const ::Ice::Current & _ctx) {
    
    boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
    
    debug("SubarchitectureWorkingMemory::registerWorkingMemoryFilter()");
    ostringstream outStream;
    outStream<<_filter;
    debug(outStream.str());
    
    m_wmFilters.put(_filter,_subarch,(int)priority);
  }
  
  
  void
  SubarchitectureWorkingMemory::removeComponentFilter(const cdl::WorkingMemoryChangeFilter & _filter,
                                                      const ::Ice::Current & _ctx) {
    
    
    boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
    
    //     debug("SubarchitectureWorkingMemory::deleteComponentChangeFilter()");
    //     debug(_src);
    //     ostringstream outStream;
    //     outStream<<_filter;
    //    debug(outStream.str());
    
    vector<string> removed;
    m_componentFilters.remove(_filter, removed);
    
    
    for(WMPrxMap::iterator i = m_workingMemories.begin();
        i != m_workingMemories.end(); ++i) {
      i->second->removeWorkingMemoryFilter(_filter);
    }
    
  }
  
  
  void
  SubarchitectureWorkingMemory::removeWorkingMemoryFilter(const cdl::WorkingMemoryChangeFilter & _filter,
                                                          const ::Ice::Current & _ctx) {
    
    
    boost::lock_guard<boost::shared_mutex> locker(m_readWriteLock);
    
    
    //     debug("SubarchitectureWorkingMemory::deleteWMChangeFilter()");
    //     debug(_src);
    //     ostringstream outStream;
    //     outStream<<_filter;
    //     debug(outStream.str());
    
    vector<string> removed;
    m_wmFilters.remove(_filter, removed);
    
    //cout<<"new filters length: "<<m_wmFilters.size()<<endl;
    //cout<<"only local: "<<m_wmFilters.localFiltersOnly()<<endl;
    
  }
  
  
  
  
} //namespace cast
