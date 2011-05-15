/*
 * Testing code.
 *
 * Copyright (C) 2006-2011 Nick Hawes
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

#ifndef CAST_DIRECT_ACCESS_READER_HPP
#define CAST_DIRECT_ACCESS_READER_HPP

#include <cast/architecture.hpp>

class DirectAccessReader : public cast::ManagedComponent {
  
public:
  virtual ~DirectAccessReader() {};
  
protected:
  void start();
  void testStructAdded(const cast::cdl::WorkingMemoryChange & _wmc);
};

#endif 