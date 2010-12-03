import cast.core
import time

class ExampleComponent(cast.core.CASTComponent):
  def startComponent(self):
    # self.debug("starting/debug")
    self.log("starting")
    # self.println("starting/println")
    # self.error("starting/error")

  def stopComponent(self):
    self.log("stopping")

  def configureComponent(self, _config):
    self.log("configured")

  def runComponent(self):
    while self.isRunning():
      self.log("running")
      time.sleep(1)

