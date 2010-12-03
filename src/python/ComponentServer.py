# File:
# ComponentServer.py
#
# Author(s):
# Thomas Keller, Michael Brenner, Nick Hawes, Marko Mahnic
#
# Date:
# 06/2009

import sys
import Ice
import CDL_ice
import cast
import cast.pylog4cxx as pylog4cxx



class CASTComponentFactory(cast.interfaces.ComponentFactory):
 def newComponent(self, id, type, newProcess, current):
   #print "Creating new", type
   component = self._createBase(type)
   tmp = Ice.Identity(id,type)
   component.setID(id,None)
   component.setObjectAdapter(current.adapter)
   component.setIceID(tmp)
   base = current.adapter.add(component,tmp);
   prx = cast.interfaces.CASTComponentPrx.uncheckedCast(base)
   return prx

 def _createBase(self, path):
  """ Create an instance of the class specified by 'class_path'.
  Format for 'class_path' = package.subpackage.anything_else.classname
  where classname.py is the name of the file in which classname must
  be defined as a class (Java convention).
  Currently searches only PYTHONPATH, but this can be easily extended.
  Currently only constructors with no arguments are supported, but this is 
  also easy to extend. """

 # try:
  module = __import__(path, None, None, [path])
 # except ImportError:
 #   raise cast.ComponentCreationException(
 #     "Cannot find python module %s in this path:\n%s" % (path, sys.path))
  module_dict = module.__dict__
  class_name = path.rsplit(".", 1)[-1]
  try:
    cls = module_dict[class_name]
  except KeyError:
    raise cast.ComponentCreationException(
      "Unable to create component: Cannot find class %s in module %s %s" % (class_name, module_name, module_dict))
  inst = cls()
  assert isinstance(inst, cast.interfaces.CASTComponent), "The component created is not a CASTComponent"
  #print "created", type(inst)
  return inst



 def newManagedComponent(self, id, type, current=None):
   raise ComponentCreationException("Cannot create a Python ManagedComponent")

 def newUnmanagedComponent(self, id, type, current=None):
   raise ComponentCreationException("Cannot create a Python UnManagedComponent")

 def newWorkingMemory(self, id, type, current=None):
   raise ComponentCreationException("Cannot create a Python Working Memory")

 def newTaskManager(self, id, type, current=None):
   raise ComponentCreationException("Cannot create a Python Taskmanager")

 def getVersionString(self, current=None):
   return cast.cdl.CASTRELEASESTRING;

class ComponentServer(Ice.Application):
 def __init__(self):
   pass

 def run(self, argv):
   status = 0
   ic = self.communicator()

   if(not ic):
     return

   pylog4cxx.configure()
   logger = pylog4cxx.Logger("cast.server.python.ComponentServer", "", "")
   logger.info("Python server version: \"%s\"" % (cast.cdl.CASTRELEASESTRING))

   portArgs = "tcp -p %d" % cast.cdl.PYTHONSERVERPORT
   adapter = ic.createObjectAdapterWithEndpoints("ComponentServer3", portArgs)

   idty = Ice.Identity()
   idty.name = "ComponentFactory";
   idty.category = "ComponentFactory";

   factory = CASTComponentFactory()
   adapter.add(factory, idty)

   adapter.activate()
   # logger.trace("Python Server Running...")
   ic.waitForShutdown()

if __name__ == '__main__':
 try:
   ComponentServer().main(sys.argv)
 except SystemExit:
   print "Shutting down component server."
