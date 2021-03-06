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

/**
 * 
 */
package cast.architecture;

import cast.CASTException;
import cast.cdl.WorkingMemoryChange;

/**
 * @author nah
 */
public interface WorkingMemoryChangeReceiver {

	/**
	 * Receives a change that has occurred to connected working memories.
	 * 
	 * @param _wmc
	 *            A description of the change.
	 * 
	 * @throws Any
	 *             CASTExceptions thrown by a change receiver will be caught by
	 *             the receiving component. However, this is only a convenience
	 *             and should not replace your own error handling.
	 */
	public void workingMemoryChanged(WorkingMemoryChange _wmc)
			throws CASTException;

}
