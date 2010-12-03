/*
 * CAST - The CoSy Architecture Schema Toolkit
 *
 * Copyright (C) 2006-2010 Nick Hawes
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

#ifndef CAST_LOGGING_HPP_
#define CAST_LOGGING_HPP_


#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/file.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/stream.h>


namespace cast {
//super namespacing to match java declaration and thus config file details
namespace core {
namespace logging {

  bool configureFromFile(const std::string & _filename);

  bool configureFromEnvironment();
  
  bool configureFromCWD();

  bool configureFromHome();

  void configureDefault();

  /**
   * Rules for finding the file to use for configuring logging properties.
   * 
   * 1. Use the file indicated by the environment CAST_LOGGING_PROPERTIES
   * 
   * 2. If the env var or the file it specifies does not exist, use
   * log4j.properties in the cwd.
   * 
   * 3. If that file does not exist, use ~/.log4j.properties instead.
   * 
   * 4. And if that fails, fall back to default (stdout, ComponentLayout)
   * config.
   * 
   */
  void initLogging();
}
}
}


#endif
