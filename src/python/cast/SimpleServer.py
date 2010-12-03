import cast.core
import time
import Ice 

class SimpleServer(cast.examples.autogen.WordServer, cast.core.CASTComponent):
  def __init__(self):
    cast.core.CASTComponent.__init__(self)

  def startComponent(self):
    self.registerIceServer(cast.examples.autogen.WordServer,self)
 
  def getNewWord(self,_current):
    return "Octopus"

