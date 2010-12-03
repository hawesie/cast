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

#ifndef PRIORITY_TESTER_HPP_
#define PRIORITY_TESTER_HPP_

#include <cast/architecture.hpp>
#include <cast/core/CASTUtils.hpp>

using namespace cast;
 
class PriorityTester : 
  public cast::ManagedComponent {
  

public:
  
  /**
   * Empty destructor.
   */
  virtual ~PriorityTester(){};

protected:
  virtual 
  void 
  start();

private:

  void high(const cast::cdl::WorkingMemoryChange & _wmc) {
    println("high");
    
    std::cout<<_wmc.timestamp<<std::endl;
  }
  void medium(const cast::cdl::WorkingMemoryChange & _wmc) {
    println("medium");
    std::cout<<_wmc.timestamp<<std::endl;

  }
  void low(const cast::cdl::WorkingMemoryChange & _wmc) {
    println("low");
    std::cout<<_wmc.timestamp<<std::endl;
  }

};

#endif
