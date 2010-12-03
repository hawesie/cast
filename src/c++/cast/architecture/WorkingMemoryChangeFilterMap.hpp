/*
 * CAST - The CoSy Architecture Schema Toolkit
 *
 * Copyright (C) 2006-2007 Nick Hawes
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

#ifndef CAST_WORKING_MEMORY_CHANGE_FILTER_MAP_H_
#define CAST_WORKING_MEMORY_CHANGE_FILTER_MAP_H_

#include <map>
#include <vector>
#include <utility>

#include <cast/architecture/WorkingMemoryChangeFilterComparator.hpp>

namespace cast {

  /**
   * Map to store change filters against objects.
   * 
   * @author nah
   */
  template <class Stored>
  class WorkingMemoryChangeFilterMap {

  private:
    
    typedef std::pair< Stored,int > PairType;

    typedef std::vector<PairType> PairVector;
    
    typedef typename std::map
    <
      cdl::WorkingMemoryChangeFilter,
      PairVector,
      WorkingMemoryChangeFilterComparator
      > 
    InternalFilterMap;
  
  public:

    typedef typename InternalFilterMap::iterator iterator;
   
    typedef typename InternalFilterMap::const_iterator const_iterator;

    typedef typename InternalFilterMap::size_type size_type;

  private:
   
    InternalFilterMap m_map;

    bool m_localOnly;

    void 
    updateFilters() {
      m_localOnly = true;
      //cout<<"updateFilters"<<endl;
      for(iterator i = m_map.begin();
	  i != m_map.end();
	  ++i) {
	if (i->first.restriction != cdl::LOCALSA) {
	  m_localOnly = false;
	  return;
	}
      }           
    }


  public:
    
    /**
     * Default ctor.
     */
    WorkingMemoryChangeFilterMap() : m_localOnly(true) {}

    /**
     * Determines whether the filter set contains only local filters on
     * whether it needs xarch changes too.
     * 
     * @return True if all filters match on local changes only.
     */
    bool 
    localFiltersOnly() {
      return m_localOnly;
    }


    /**
     * Whether this filter set allows a change to pass
     */
    bool allowsChange(const cdl::WorkingMemoryChange & _wmc) const {
      for(const_iterator i = m_map.begin();
	  i != m_map.end();
	  ++i) {
	if (WorkingMemoryChangeFilterComparator::allowsChange(i->first,_wmc)) {
	  return true;
	}
      }
      return false;
      
    }
    



    /**
     * Find all the receivers that will accept the given change.
     * 
     * @param _wmc The change to check.  @return An iterator to the
     * receiver list, or NULL if one not found.
     */
    void get(const cdl::WorkingMemoryChange & _wmc, 
	     std::vector<Stored> & _receivers) const {
      //cout<<"get"<<endl;      
      for(const_iterator i = m_map.begin();
	  i != m_map.end();
	  ++i) {
	if (WorkingMemoryChangeFilterComparator::allowsChange(i->first, _wmc)) {
	  PairVector pReceiver = i->second;
	  assert(pReceiver.size() > 0);
	  
	  //insert all receivers from stored vector into input vector
	  //_receivers.insert(_receivers.begin(), pReceiver.begin(), pReceiver.end());
	  
	  for(typename PairVector::const_iterator j = pReceiver.begin();
	      j < pReceiver.end(); ++j) {
	    _receivers.push_back(j->first);
	  }
	}
      }
      //cout<<"done"<<endl;           
    }
    

    void remove(const cdl::WorkingMemoryChangeFilter & _filter, 
		std::vector<Stored> & _removed) {
      iterator i = m_map.find(_filter);
      assert(i != m_map.end());
      for(typename PairVector::const_iterator j = i->second.begin();
	  j < i->second.end(); ++j) {
	_removed.push_back(j->first);
      }
      m_map.erase(i);
      updateFilters();
    }

    void remove(const Stored & _receiver,
		std::vector<cdl::WorkingMemoryChangeFilter> & _removed)  {

      for(iterator i = m_map.begin(); i != m_map.end(); ++i) {

	cdl::WorkingMemoryChangeFilter filter = i->first;;
	//std::cout<<"in remove before: "<<i->second.size()<<std::endl;
	      
	typename PairVector::iterator j = i->second.begin();
 	while(j < i->second.end()) {	  
	  if(j->first == _receiver) {
	    j = i->second.erase(j);   
	  }
	  else {
	    ++j;
	  }
	} 
	//std::cout<<"in remove after: "<<i->second.size()<<std::endl;

	//if the list ends up empty, then the filter can be removed too
	if(i->second.empty()) {
	  _removed.push_back(filter);
	}
      }
      
      
      if(!_removed.empty()) {


	//std::cout<<"deleting keys "<<std::endl;
	
	//now deleted stored things
	for(std::vector<cdl::WorkingMemoryChangeFilter>::iterator i = _removed.begin();
	    i < _removed.end(); ++i){ 
	  m_map.erase(*i);
	}

	updateFilters();


      }
      

      //std::cout<<"out remove: "<<size()<<std::endl;
    }


    size_type
    size() const {
      int size = 0;
      //ugly way of doing it cos the rest of the code is fugly
      for(const_iterator i = m_map.begin();
	  i != m_map.end();
	  ++i) {
	size += i->second.size();
      }
      return size;
    }

    bool
    empty() const {
      return m_map.empty();
    }


    struct PrioritySort
    {
      bool operator()(PairType const &a , const PairType &b) { 
	return a.second > b.second; 
      }
    };
    
    void
    put(const cdl::WorkingMemoryChangeFilter & _key, const Stored & _value, const int & _priority) {
      
      //see if a list exists for the key
      iterator i = m_map.find(_key);
      
      //if it doesn't add one in.
      if(i == m_map.end()) {
	PairVector receiverList;
	m_map[_key] = receiverList;       
	
	//update local flag on new filter
	m_localOnly = m_localOnly && (_key.restriction == cdl::LOCALSA);
      }

      m_map[_key].push_back(PairType(_value,_priority));
      stable_sort(m_map[_key].begin(), m_map[_key].end(), PrioritySort());

    }



  };

}

#endif
