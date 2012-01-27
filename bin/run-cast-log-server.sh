#!/bin/bash

# sanity check
if [ $# -ne 1 ] ; then
  echo "Usage: `basename $0` <cast install prefix, e.g. /usr/local>"
  exit $E_BADARGS
fi

# startup the CAST servers. 

# where things are for runtime
CAST_DIR=$1
CAST_INSTALL_ROOT=${CAST_DIR}

CAST_BIN_PREFIX=bin
CAST_BIN_DIR=${CAST_INSTALL_ROOT}/${CAST_BIN_PREFIX}

CAST_LIB_PREFIX=lib/cast
CAST_LIB_DIR=${CAST_INSTALL_ROOT}/${CAST_LIB_PREFIX}

CAST_JAR=${CAST_INSTALL_ROOT}/share/java/cast.jar

java -ea -classpath ${CLASSPATH}:${CAST_JAR} cast.core.logging.LoggingServer



