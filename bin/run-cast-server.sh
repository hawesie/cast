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
CAST_PY_DIR=${CAST_LIB_DIR}/python

CAST_JAR=${CAST_INSTALL_ROOT}/share/java/cast.jar

WE_SET_ICE_CONFIG=1
CAST_CONFIG_PATH=share/cast/config/cast_ice_config
CAST_ICE_CONFIG=${CAST_INSTALL_ROOT}/${CAST_CONFIG_PATH}


# check for ice config info
if [[ "$ICE_CONFIG" ]] ; then
    echo "--------------------------------------------------------------------------"
    echo "ICE_CONFIG is already set.";
    echo "You should also include the contents of ${CAST_ICE_CONFIG} in your config." 
    echo "--------------------------------------------------------------------------"
    WE_SET_ICE_CONFIG=0
else
    export ICE_CONFIG=${CAST_ICE_CONFIG}
fi 

java -ea -classpath ${CLASSPATH}:${CAST_JAR} cast.server.ComponentServer &
JAVA_SERVER_JOB=$!


SAVED_DYLIB_PATH=${DYLD_LIBRARY_PATH}
SAVED_LIB_PATH=${LD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${CAST_LIB_DIR}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CAST_LIB_DIR}

${CAST_BIN_DIR}/cast-server-c++ &
CPP_SERVER_JOB=$!


SAVED_PYTHONPATH=${PYTHONPATH}
export PYTHONPATH=${PYTHONPATH}:${CAST_PY_DIR}

python -m ComponentServer &
PY_SERVER_JOB=$!


echo Java server pid: ${JAVA_SERVER_JOB}
echo CPP server pid: ${CPP_SERVER_JOB}
echo Python server pid: ${PY_SERVER_JOB}


# if we're killed, take down everyone else with us
trap "kill -9 ${CPP_SERVER_JOB} ${JAVA_SERVER_JOB} ${PY_SERVER_JOB}" INT TERM EXIT

wait

# reset evn vars
export LD_LIBRARY_PATH=${SAVED_LIB_PATH}
export DYLD_LIBRARY_PATH=${SAVED_DYLIB_PATH}
export PYTHONPATH=${SAVED_PYTHONPATH}

if (( $WE_SET_ICE_CONFIG )) ; then
    unset ICE_CONFIG
fi