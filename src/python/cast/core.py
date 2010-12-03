import Ice
import cast
import cast.cdl
import threading
import pylog4cxx

def languageToPort(lang):
  if lang == cast.cdl.ComponentLanguage.CPP:
    return cast.cdl.CPPSERVERPORT
  if lang == cast.cdl.ComponentLanguage.JAVA:
    return cast.cdl.JAVASERVERPORT
  if lang == cast.cdl.ComponentLanguage.PYTHON:
    return cast.cdl.PYTHONSERVERPORT
  else:
    raise RuntimeError('Unknown language in enum: %s' % (lang))

def getLogColorEscape(index):
  printNumber = (index % 7)
  bold = (index / 7) % 2
  if bold: bold = ";1"
  else: bold = ""
  if printNumber == 0: return "\033[0m"
  else: return "\033[3%d%sm" % (printNumber, bold)
  return co

class ComponentRunThread ( threading.Thread ):
  """ Class used to separate control for runComponent. """
  def __init__(self, _component):
    self.m_component = _component
    threading.Thread.__init__(self)

  def run ( self ):
    self.m_component.runComponent()


class CASTComponent(cast.interfaces.CASTComponent):
  """ Superclass for all user-defined CASTComponents. """

  def __init__(self):
    self.m_componentID = ''
    self.m_componentNumber = 0
    self.m_adapter = None
    self.m_iceIdentity = None
    self.m_startCalled = False
    self.m_runThread = None
    self.m_serverIdentities = []
    self.m_manager = None
    self.m_timeServer = None
    self._logger = None
    self.m_loglevel = "info"

  def setObjectAdapter(self, _adapter):
    self.m_adapter = _adapter

  def getObjectAdapter(self):
    return self.m_adapter

  def getCommunicator(self):
    return self.getObjectAdapter().getCommunicator()

  def setIceID(self, _id):
    self.m_iceIdentity = _id

  def beat(self, _current):
    pass

  def setID(self, _id, _current):
    self.m_componentID = _id
    # the logger will be recreated with new id on next use
    self._logger = None

  def getComponentID(self):
    return self.m_componentID

  def getID(self, _current):
    return self.getComponentID()

  @property
  def m_logger(self):
    if self._logger == None:
      cid = self.getComponentID()
      if cid == "": cid = "cast.init.%d" % self.m_componentNumber
      sa = "" # TODO
      self._logger = pylog4cxx.Logger(cid, sa, getLogColorEscape(self.m_componentNumber))
      self._logger.setLevel(self.m_loglevel)
    return self._logger

  def debug(self, message):
    self.m_logger.trace(message)

  def log(self, message):
    self.m_logger.debug(message)

  def println(self, message):
    self.m_logger.info(message)

  def error(self, message):
    self.m_logger.error(message)

  def configureComponent(self, _config):
    """ Send configuration details to user-defined component """
    pass

  def configure(self, _config, _current):
    def istrue(key): return key=='' or key.lower() == "true"

    self.m_loglevel = "info"

    if cast.cdl.LOGKEY in _config and istrue(_config[cast.cdl.LOGKEY]):
      self.m_loglevel = "debug"

    if cast.cdl.DEBUGKEY in _config and istrue(_config[cast.cdl.DEBUGKEY]):
      self.m_loglevel = "trace"

    if self._logger != None:
      self._logger.setLevel(self.m_loglevel)

    if cast.cdl.COMPONENTNUMBERKEY in _config:
      self.m_componentNumber = int(_config[cast.cdl.COMPONENTNUMBERKEY])
    else: self.m_componentNumber = 0

    self.configureComponent(_config)

  def startComponent(self):
    """ Start user-defined component """
    pass

  def start(self, _current):
    self.m_startCalled = True
    self.startComponent()

  def runComponent(self):
    """ Run user-defined component. This is where you do the work. """
    pass

  def run(self, _current):
    """ self.runComponent() """
    self.m_runThread = ComponentRunThread(self)
    self.m_runThread.start()


  def stopComponent(self):
    """ Stop user-defined component. """
    pass

  def stop(self, _current):
    self.m_startCalled = False
    self.stopComponent()
    if self.m_runThread != None:
      self.m_runThread.join()

  def isRunning(self):
    return self.m_startCalled


  def destroy(self, _current):
    for idty in self.m_serverIdentities:
      self.getObjectAdapter().remove(idty)
    _current.adapter.remove(_current.id);


  def setComponentManager(self, man, _current):
    self.m_manager = man

  def getComponentManager(self):
    return self.m_manager


  def setTimeServer(self, ts, _current):
    self.m_timeServer = ts

  def registerIceServerManual(self, name, category, servant):
    ident = Ice.Identity(name, category)
    self.getObjectAdapter().add(servant, ident)
    self.m_serverIdentities.append(ident);

  def registerIceServer(self, interface, servant):
    """ registers the servant as supplying the given interface """
    category = "%s.%s" % (interface.__module__, interface.__name__)
    self.registerIceServerManual(self.getComponentID(), category, servant)

  def getIceServerManual(self, _name, _category, _host, _port):
    idty = Ice.Identity(_name,_category)
    serverAddr = "%s :default -h %s -p %s" % (self.getCommunicator().identityToString(idty), _host, _port)
    base = self.getCommunicator().stringToProxy(serverAddr)
    return base

  def getIceServer(self, _componentID, interface, _proxyType):
    """ Returns an ice server of the type specified by _proxyType resolved from _componentID with given interface"""
    desc = self.getComponentManager().getComponentDescription(_componentID)
    host = desc.hostName
    port = languageToPort(desc.language)
    category = "%s.%s" % (interface.__module__, interface.__name__)
    prx = self.getIceServerManual(_componentID, category, host, port);
    return _proxyType.checkedCast(prx)
