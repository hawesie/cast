import cast.core
import time
import Ice 


class WordServer(cast.examples.autogen.WordServer):
  def getNewWord(self,_current):
    return "Cod"

class SimpleAggregateServer(cast.core.CASTComponent):
  def startComponent(self):
    self.registerIceServer(cast.examples.autogen.WordServer,WordServer())




