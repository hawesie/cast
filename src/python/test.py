#!/usr/bin/env python
# vim:set fileencoding=utf-8 sw=4 ts=8 et:vim

import pylog4cxx as log

f = open("log4j.properties", "w")
f.write("""
log4j.rootLogger=WARN, console
log4j.appender.console=org.apache.log4j.ConsoleAppender
log4j.appender.console.layout=cast.core.logging.ComponentLayout
log4j.appender.console.layout.ConversionPattern=%S[%P%i: %m]%E%n
""")
f.close()


print " --- Confiuring pylog4cxx"
log.configure("log4j.properties")

print "\n --- Logging with Logger objects"
l1 = log.Logger("logger.1", "other.sa", "cyan")

for level in ["off", "warn", "info", "debug", "all"]:
    l1.setLevel(level)
    print "\n --- Logging level set to", level
    l1.trace("Trace logged")
    l1.debug("Debug logged")
    l1.info("Info logged")
    l1.warn("Warning logged")
    l1.error("Error logged")
    l1.fatal("Fatal logged")
print "-------------------------------------------------------"

print " --- l1 -> DEBUG"
print " --- l2 -> ERROR"
l1.setLevel("debug")
l2 = log.Logger("logger.2", "custom.sa", "blue")
l2.setLevel("error")

for l in [l1, l2]:
    print l.id, "TRACE"
    l.trace("TRACE")
    print l.id, "INFO"
    l.info("INFO")
    print l.id, "WARN"
    l.warn("WARN")
    print l.id, "FATAL"
    l.fatal("FATAL")

#print "\n --- Logging with global logging functions (deprecated)"
#log.trace("Trace logged", "component", "my.sa", "blue")
#log.info("Info logged", "component", "my.sa", "blue")
#log.debug("Debug logged", "component", "my.sa", "green")
#log.warn("Warning logged", "component", "my.sa", "green")
#log.error("Error logged", "component", "my.sa", "red")
#log.fatal("Fatal logged", "component", "my.sa", "red")

