import cast.core
import time

class WordServerClient(cast.core.CASTComponent):
  def runComponent(self):
    agg = self.getIceServer("aggregate.server", cast.examples.autogen.WordServer, cast.examples.autogen.WordServerPrx)
    print "new word: ", agg.getNewWord()
    impl = self.getIceServer("implements.server", cast.examples.autogen.WordServer, cast.examples.autogen.WordServerPrx)
    print "new word: ", impl.getNewWord()
