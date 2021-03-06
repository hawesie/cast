/*
 * CAST - The CoSy Architecture Schema Toolkit Copyright (C) 2006-2007
 * Nick Hawes This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version
 * 2.1 of the License, or (at your option) any later version. This
 * library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details. You should have
 * received a copy of the GNU Lesser General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * 
 */
package cast.architecture;

import java.util.Map;

import org.apache.log4j.Level;

import Ice.Current;
import Ice.Object;
import Ice.ObjectImpl;
import cast.AlreadyExistsOnWMException;
import cast.ConsistencyException;
import cast.DoesNotExistOnWMException;
import cast.PermissionException;
import cast.UnknownSubarchitectureException;
import cast.cdl.COMPONENTNUMBERKEY;
import cast.cdl.WorkingMemoryAddress;
import cast.core.CASTUtils;
import cast.core.logging.ComponentLogger;
import cast.interfaces.WorkingMemoryPrx;
import cast.interfaces.WorkingMemoryPrxHelper;

/**
 * Defines a class of process that can both read from and write to a working
 * memory.
 * 
 * @author nah
 */
public abstract class WorkingMemoryWriterComponent extends
		WorkingMemoryAttachedComponent {

	protected int m_dataCount;

	private int m_componentNumber;

	private String m_componentNumberString;

	private ComponentLogger m_loggerForAdditions;
	private ComponentLogger m_loggerForOverwrites;
	private ComponentLogger m_loggerForDeletes;

	/**
	 * Working memory proxy just for writing. Allows choice of serialisation or
	 * not for local connections.
	 */
	private WorkingMemoryPrx m_workingMemoryForWrite;

	/**
	 * @param _id
	 */
	public WorkingMemoryWriterComponent() {
		m_dataCount = 0;
		m_componentNumber = -1;
	}

	@Override
	protected void startInternal() {
		super.startInternal();
		m_loggerForAdditions = getLogger(".wm.rw.add");
		m_loggerForOverwrites = getLogger(".wm.rw.ovr");
		m_loggerForDeletes = getLogger(".wm.rw.del");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * cast.core.components.CASTProcessingComponent#configure(java.util.Properties
	 * )
	 */
	@Override
	public void configureInternal(Map<String, String> _config) {
		super.configureInternal(_config);
		String componentNumber = _config.get(COMPONENTNUMBERKEY.value);
		assert (componentNumber != null);
		m_componentNumber = Integer.parseInt(componentNumber);
		m_componentNumberString = stringify(m_componentNumber);
	}

	/**
	 * Add new data to working memory. The data will be stored with the given
	 * id.
	 * 
	 * @param _id
	 *            The id the data will be stored with.
	 * @param _data
	 *            The data itself
	 * 
	 * @throws AlreadyExistsOnWMException
	 *             If an entry exists at the given id.
	 * @throws DoesNotExistOnWMException
	 */
	public <T extends ObjectImpl> void addToWorkingMemory(String _id, T _data)
			throws AlreadyExistsOnWMException {
		try {
			addToWorkingMemory(_id, getSubarchitectureID(), _data);
		} catch (UnknownSubarchitectureException e) {
			throw new RuntimeException(
					"Shouldn't happen on own subarchitecture", e);
		}
	}

	/**
	 * Add new data to working memory. The data will be stored with the given
	 * id.
	 * 
	 * @param _id
	 *            The id the data will be stored with.
	 * @param _data
	 *            The data itself
	 * 
	 * @throws AlreadyExistsOnWMException
	 *             If an entry exists at the given id.
	 * @throws DoesNotExistOnWMException
	 * @throws UnknownSubarchitectureException
	 */
	public <T extends Object> void addToWorkingMemory(
			WorkingMemoryAddress _wma, T _data)
			throws AlreadyExistsOnWMException, DoesNotExistOnWMException,
			UnknownSubarchitectureException {
		addToWorkingMemory(_wma.id, _wma.subarchitecture, _data);
	}

	/**
	 * Add new data to working memory. The data will be stored with the given
	 * id.
	 * 
	 * @param _id
	 *            The id the data will be stored with.
	 * @param _subarchitectureID
	 *            The subarchitecture to write to.
	 * @param _data
	 *            The data itself
	 * @throws AlreadyExistsOnWMException
	 *             If an entry exists at the given id.
	 * @throws UnknownSubarchitectureException
	 */
	public <T extends Ice.Object> void addToWorkingMemory(String _id,
			String _subarch, T _data) throws AlreadyExistsOnWMException,
			UnknownSubarchitectureException {

		assert _id.length() > 0 : "id must not be empty";
		assert _subarch.length() > 0 : "subarchitecture id must not be empty";

		// UPGRADE not sure what to do with this now
		// checkPrivileges(_subarch);
		String type = CASTUtils.typeName(_data);

		// #bug 52, testcase 2: If we are already versioning this data
		// it's ok. This means we had previously written to this address.

		int versionWhichWillEndUpOnWM = 0;

		// if not versioned, start doing so
		if (!isVersioned(_id)) {
			debug("is not versioned: " + _id);
			startVersioning(_id);
		}
		// if it's already versioned, then we need to update our numbers
		else {
			debug("re-adding to working memory with id " + _id);
			// get the last known version number, which we store + 1
			try {
				versionWhichWillEndUpOnWM = getVersionNumber(_id, _subarch) + 1;
				storeVersionNumber(_id, versionWhichWillEndUpOnWM);
			} catch (DoesNotExistOnWMException e) {
				throw new RuntimeException(
						"Error in versioning for data re-added to working memory",
						e);
			}
		}
		m_workingMemoryForWrite.addToWorkingMemory(_id, _subarch, type,
				getComponentID(), _data);

		logAdd(_id, _subarch, type, versionWhichWillEndUpOnWM);
	}

	/**
	 * Log the add to the logger defined to received adds. The logger must be
	 * TRACE enabled to receive events.
	 * 
	 * @param _id
	 * @param _subarch
	 * @param _type
	 * @param _version
	 */
	private final void logAdd(String _id, String _subarch, String _type,
			int _version) {

		if (m_loggerForAdditions.isTraceEnabled()) {
			m_loggerForAdditions.trace(CASTUtils.concatenate("add,",
					CASTUtils.toString(getCASTTime()), ",", _id, ",", _subarch,
					",", _type, ",", _version), getLogAdditions());
		}
	}

	/**
	 * Log the overwrite to the logger defined to received overwrite. The logger
	 * must be TRACE enabled to receive events.
	 * 
	 * @param _id
	 * @param _subarch
	 * @param _type
	 */
	private final void logOverwrite(String _id, String _subarch, String _type,
			int _version) {

		if (m_loggerForOverwrites.isTraceEnabled()) {
			m_loggerForOverwrites.trace(CASTUtils.concatenate("ovr,",
					CASTUtils.toString(getCASTTime()), ",", _id, ",", _subarch,
					",", _type, ",", _version), getLogAdditions());
		}
	}

	/**
	 * Log the delete to the logger defined to received deletes. The logger must
	 * be TRACE enabled to receive events.
	 * 
	 * @param _id
	 * @param _subarch
	 * @param _type
	 */
	private final void logDelete(String _id, String _subarch) {

		if (m_loggerForDeletes.isTraceEnabled()) {
			m_loggerForDeletes.trace(
					CASTUtils.concatenate("del,",
							CASTUtils.toString(getCASTTime()), ",", _id, ",",
							_subarch), getLogAdditions());
		}
	}

	/**
	 * Delete data from working memory with given id.
	 * 
	 * @param _id
	 *            The id of the working memory entry to be deleted.
	 * @throws PermissionException
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist on working memory.
	 */
	public void deleteFromWorkingMemory(String _id)
			throws DoesNotExistOnWMException, PermissionException {
		try {
			deleteFromWorkingMemory(_id, getSubarchitectureID());
		} catch (UnknownSubarchitectureException e) {
			throw new RuntimeException(
					"Shouldn't happen on own subarchitecture", e);
		}
	}

	/**
	 * Delete data from working memory with given id.
	 * 
	 * @param _id
	 *            The id of the working memory entry to be deleted.
	 * @throws PermissionException
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist on working memory.
	 * @throws UnknownSubarchitectureException
	 */
	public void deleteFromWorkingMemory(WorkingMemoryAddress _wma)
			throws DoesNotExistOnWMException, PermissionException,
			UnknownSubarchitectureException {
		deleteFromWorkingMemory(_wma.id, _wma.subarchitecture);
	}

	/**
	 * Delete data from working memory with given id.
	 * 
	 * @param _id
	 *            The id of the working memory entry to be deleted.
	 * @param _subarch
	 *            The subarchitecture to write to.
	 * @param _sync
	 *            Whether to block until the operation completes.
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist on working memory.
	 * @throws PermissionException
	 * @throws UnknownSubarchitectureException
	 */
	public void deleteFromWorkingMemory(String _id, String _subarch)
			throws DoesNotExistOnWMException, PermissionException,
			UnknownSubarchitectureException {

		assert _id.length() > 0 : "id must not be empty";
		assert _subarch.length() > 0 : "subarchitecture id must not be empty";

		// UPGRADE still needs thought
		// checkPrivileges(_subarch);

		// do the check here as it may save a lot of hassle later
		if (!existsOnWorkingMemory(_id, _subarch)) {
			throw new cast.DoesNotExistOnWMException(
					"Entry does not exist for deleting. Was looking for id "
							+ _id + " in sa " + _subarch,
					new WorkingMemoryAddress(_subarch, _id));
		}

		// if we don't currently hold a lock on this item
		if (!holdsDeleteLock(_id, _subarch)) {
			// check that we can delete it
			if (!isDeletable(_id, _subarch)) {
				throw new cast.PermissionException(
						"Delete not allowed on locked item: " + _id + ":"
								+ _subarch, new WorkingMemoryAddress(_subarch,
								_id));
			}
		}

		// always keep versioning...
		// stopVersioning(_id);

		m_workingMemory
				.deleteFromWorkingMemory(_id, _subarch, getComponentID());

		logDelete(_id, _subarch);
	}

	private static final char[] id_table = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
			'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
			'x', 'y', 'z' };

	private static final int ID_TABLE_SIZE = 62;

	/**
	 * generates short unique strings from ints
	 */
	private final String stringify(int _n) {
		if (_n == 0)
			return "0";

		String ret = new String();

		// int i = 0;
		while (_n != 0) {
			int m = _n % ID_TABLE_SIZE;
			ret += id_table[m];
			_n -= m;
			_n /= ID_TABLE_SIZE;
		}
		return ret;
	}

	public final String newDataID() {

		StringBuffer sb = new StringBuffer(stringify(m_dataCount++));
		sb.append(":");
		if (m_logger.getLevel() == Level.TRACE) {
			sb.append(getComponentID());
			sb.append(":data");
		} else {
			sb.append(m_componentNumberString);
		}
		return sb.toString();

	}

	/**
	 * Overwrite data in working memory. The data to be overwritten is indicated
	 * by the id, which is then used for the id of the new data.
	 * 
	 * @param _id
	 *            The id the data will be stored with
	 * @param _data
	 *            The data itself
	 * 
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist to be overwritten.
	 * @throws ConsistencyException
	 *             if this component does not have the most recent version of
	 *             the data at the given id.
	 * @throws PermissionException
	 */
	public <T extends Object> void overwriteWorkingMemory(String _id, T _data)
			throws DoesNotExistOnWMException, ConsistencyException,
			PermissionException {

		try {
			overwriteWorkingMemory(_id, getSubarchitectureID(), _data);
		} catch (UnknownSubarchitectureException e) {
			throw new RuntimeException(
					"Shouldn't happen on own subarchitecture", e);
		}
	}

	/**
	 * Overwrite data in working memory. The data to be overwritten is indicated
	 * by the id, which is then used for the id of the new data.
	 * 
	 * @param _wma
	 *            The id the data will be stored with
	 * @param _data
	 *            The data itself
	 * 
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist to be overwritten.
	 * @throws ConsistencyException
	 *             if this component does not have the most recent version of
	 *             the data at the given id.
	 * @throws PermissionException
	 * @throws UnknownSubarchitectureException
	 */
	public <T extends Object> void overwriteWorkingMemory(
			WorkingMemoryAddress _wma, T _data)
			throws DoesNotExistOnWMException, ConsistencyException,
			PermissionException, UnknownSubarchitectureException {
		overwriteWorkingMemory(_wma.id, _wma.subarchitecture, _data);
	}

	/**
	 * Overwrite data in working memory. The data to be overwritten is indicated
	 * by the id, which is then used for the id of the new data.
	 * 
	 * @param _id
	 *            The id the data will be stored with
	 * @param _subarch
	 *            The subarchitecture to write to.
	 * @param _data
	 *            The data itself
	 * 
	 * @throws DoesNotExistOnWMException
	 *             if the given id does not exist to be overwritten.
	 * @throws PermissionException
	 * @throws ConsistencyException
	 * @throws UnknownSubarchitectureException
	 */

	public <T extends Object> void overwriteWorkingMemory(String _id,
			String _subarch, T _data) throws DoesNotExistOnWMException,
			PermissionException, ConsistencyException,
			UnknownSubarchitectureException {

		assert _id.length() > 0 : "id must not be empty";
		assert _subarch.length() > 0 : "subarchitecture id must not be empty";

		// checkPrivileges(_subarch);

		// do the check here as it may save a lot of hassle later
		if (!existsOnWorkingMemory(_id, _subarch)) {
			throw new DoesNotExistOnWMException(
					"Entry does not exist for overwriting. Was looking for id "
							+ _id + " in sa " + _subarch,
					new WorkingMemoryAddress(_subarch, _id));
		}

		// if we don't have an overwrite lock
		if (!holdsOverwriteLock(_id, _subarch)) {

			if (!isOverwritable(_id, _subarch)) {
				throw new PermissionException(
						"Overwrite not allowed on locked item: " + _id + ":"
								+ _subarch, new WorkingMemoryAddress(_subarch,
								_id));
			}

			// then check the consistency of the overwrite
			checkConsistency(_id, _subarch);
		} else {

			// if we need to do a one-time check of the consistecny
			if (needsConsistencyCheck(_id, _subarch)) {
				debug("one-time consistency check for locked item: " + _id
						+ ":" + _subarch);

				// then check the consistency of the overwrite
				checkConsistency(_id, _subarch);
				consistencyChecked(_id, _subarch);

			} else {
				debug("skipping consistency check for locked item: " + _id
						+ ":" + _subarch);
			}
		}

		String type = CASTUtils.typeName(_data);

		// logMemoryOverwrite(_id,_subarch,type);

		m_workingMemoryForWrite.overwriteWorkingMemory(_id, _subarch, type,
				getComponentID(), _data);

		// if we got this far, then we're allowed to update our local
		// version number for the id
		increaseStoredVersion(_id);

		logOverwrite(_id, _subarch, type, getStoredVersionNumber(_id));
	}

	/**
	 * Overrides setWorkingMemory to create a local copy of the wm pro
	 */
	@Override
	public void setWorkingMemory(WorkingMemoryPrx _wm, Current __current) {
		super.setWorkingMemory(_wm, __current);

		// Check whether the WM is language and network local
		if (m_workingMemory.ice_isCollocationOptimized()) {
			// if it is, create a new WM proxy that forces serialisation to
			// ensure a true deep copy is created
			m_workingMemoryForWrite = WorkingMemoryPrxHelper
					.uncheckedCast(m_workingMemory
							.ice_collocationOptimized(false));
		} else {
			m_workingMemoryForWrite = m_workingMemory;
		}
	}

	/**
	 * Sets WM proxy for WM writes back to original state. If the original proxy
	 * was collocation optimised then writing will now be collocation optimised
	 * too (and local changes to written objects will be immediately reflected
	 * on WM).
	 * 
	 * @return true if WM writes will now be collocation optimised.
	 */
	protected boolean resetWriteCollocationOptimisation() {
		m_workingMemoryForWrite = m_workingMemory;
		return m_workingMemoryForWrite.ice_isCollocationOptimized();
	}

	/**
	 * Sets WM proxy for WM writes to a un-collocation optimised state.
	 * Guarantees that local changes to written objects will not appear on WM.
	 * This is done automatically on start-up, so users do not need to call it.
	 * 
	 */
	protected void turnOffWriteCollocationOptimisation() {
		// Check whether the WM is language and network local
		if (m_workingMemoryForWrite.ice_isCollocationOptimized()) {
			m_workingMemoryForWrite = WorkingMemoryPrxHelper
					.uncheckedCast(m_workingMemory
							.ice_collocationOptimized(false));
		}
	}

}
