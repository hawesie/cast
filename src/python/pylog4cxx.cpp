/*
 * @author:  Marko Mahnič
 * @created: 2010-05-11
 */
   
// Python
#include <Python.h>
#include "structmember.h"

// include log4cxx header files.
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

// CAST
#include "cast/core/ComponentLogger.hpp"
#include "cast/core/PatternConverters.hpp"
#include "cast/core/CASTComponent.hpp"
#include "cast/core/Logging.hpp"

using namespace log4cxx;
using namespace log4cxx::helpers;
namespace castlog = cast::core::logging;

static void doex(Exception& ex)
{
   try {
     printf("*** PyLog4Cxx Error: %s\n", ex.what());
   }
   catch (...) {}
}

#if 0
cast::CASTComponent::ComponentLoggerPtr getLogger(const char* compId)
{
  return castlog::ComponentLogger::getLogger(compId);
}

PyObject* do_L4C_log(LevelPtr level, PyObject *self, PyObject *args)
{
   char empty = '\0';
   char *message = NULL;
   char *compid = NULL;
   char *subarch = &empty;
   char *color = &empty;
   if (!PyArg_ParseTuple(args, "ss|ss", &message, &compid, &subarch, &color))
      return NULL;

   //try {
   //   LoggerPtr logger(Logger::getLogger(logger_id));
   //   LOG4CXX_LOG(logger, level, message);
   //}
   try {
      cast::CASTComponent::ComponentLoggerPtr logger = getLogger(compid);
      castlog::LogAdditions additions(compid, subarch, color);
      CAST_LOG(logger, level, message, additions);
   }
   catch(Exception& ex) {
      doex(ex);
   }

   Py_INCREF(Py_None);
   return Py_None;
}

#define LOGFUNC(fnName, fnGetLevel)\
   PyObject* L4C_##fnName(PyObject *self, PyObject *args) { \
      return do_L4C_log(Level::fnGetLevel(), self, args); \
   }

LOGFUNC(fatal, getFatal)
LOGFUNC(error, getError)
LOGFUNC(warn, getWarn)
LOGFUNC(info, getInfo)
LOGFUNC(debug, getDebug)
LOGFUNC(trace, getTrace)

#undef LOGFUNC
#define LOGFUNC(fnName) \
   {#fnName, L4C_##fnName, METH_VARARGS, \
     #fnName "(strLogger, strMessage)\n" \
     "Add a '" #fnName "' message to a log." \
   },
#endif

PyObject* L4C_configure(PyObject *self, PyObject *args)
{
   char *fname = NULL;

   if (!PyArg_ParseTuple(args, "|s", &fname))
      return NULL;

   try {
//      if (fname == NULL || strlen(fname) < 1){
//        //BasicConfigurator::configure();
       
//     printf("*** initialising logging... ");
       castlog::initLogging();
       //   printf("*** DONE\n");

//      }
//      else {
//        PropertyConfigurator::configure(fname);
//      }
   }
   catch(Exception& ex) {
      doex(ex);
   }

   Py_INCREF(Py_None);
   return Py_None;
}


PyMethodDef module_methods[] = {
   {  "configure", L4C_configure, METH_VARARGS, 
      "configure(log4j_properties=None)\n"
      "Configure the logging subsystem."
   },
#if 0
   LOGFUNC(fatal)
   LOGFUNC(error)
   LOGFUNC(warn)
   LOGFUNC(info)
   LOGFUNC(debug)
   LOGFUNC(trace)
#endif
   {NULL, NULL, 0, NULL}
};
#undef LOGFUNC


/* Now a Logger object */
typedef struct {
   PyObject_HEAD
   /* Type-specific fields go here. */
   PyObject* id;
   PyObject* subarch;
   PyObject* color;
   cast::CASTComponent::ComponentLoggerPtr logger;
} LoggerObject;

static void
Logger_dealloc(LoggerObject* self)
{
    Py_XDECREF(self->id);
    Py_XDECREF(self->subarch);
    Py_XDECREF(self->color);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Logger_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LoggerObject *self;

    self = (LoggerObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
       self->id = PyString_FromString("");
       if (self->id == NULL) {
         Py_DECREF(self);
         return NULL;
       }

       self->subarch = PyString_FromString("");
       if (self->subarch == NULL) {
         Py_DECREF(self);
         return NULL;
       }

       self->color = PyString_FromString("");
       if (self->color == NULL) {
         Py_DECREF(self);
         return NULL;
       }
    }

    return (PyObject *)self;
}

static int
Logger_init(LoggerObject *self, PyObject *args, PyObject *kwds)
{
   static char *kwlist[] = {"id", "subarch", "color", NULL};
   PyObject *id=NULL, *subarch=NULL, *color=NULL, *tmp;

   if (! PyArg_ParseTupleAndKeywords(args, kwds, "|SSS", kwlist, &id, &subarch, &color))
      return -1; 

#define PYOBJ_REPLACE(pyobj, onew) \
   if (onew) { tmp = pyobj; Py_INCREF(onew); pyobj = onew; Py_DECREF(tmp); }
   PYOBJ_REPLACE(self->id, id);
   PYOBJ_REPLACE(self->subarch, subarch);
   PYOBJ_REPLACE(self->color, color);
#undef PYOBJ_REPLACE

   char* pid = PyString_AsString(self->id);
   try {
      self->logger = castlog::ComponentLogger::getLogger(pid);
      self->logger->setLevel(Level::getDebug());
   }
   catch(Exception& ex) {
      doex(ex);
   }
   return 0;
}

static PyMemberDef Logger_members[] = {
   {"id", T_OBJECT_EX, offsetof(LoggerObject, id), 0,
      "Logger Id. Usually the CAST component id."},
   {"subarch", T_OBJECT_EX, offsetof(LoggerObject, subarch), 0,
      "The subarchitecture of the component that owns the logger."},
   {"color", T_OBJECT_EX, offsetof(LoggerObject, color), 0,
      "ANSI escape code."},
   //{"number", T_INT, offsetof(Noddy, number), 0,
   // "noddy number"},
   {NULL}  /* Sentinel */
};


PyObject* Logger_do_log(LevelPtr level, PyObject *o_self, PyObject *args)
{
  //  printf("Logger_do_log\n");

   char empty = '\0';
   char *message = &empty;
   if (!PyArg_ParseTuple(args, "|s", &message))
      return NULL;

   try {
      LoggerObject* self = (LoggerObject*) o_self;
      cast::CASTComponent::ComponentLoggerPtr logger = self->logger;
      if (logger == NULL) logger = castlog::ComponentLogger::getLogger("*noname*");
      // castlog::LogAdditions additions(compid, subarch, color);
      castlog::LogAdditions additions(PyString_AsString(self->id),
            PyString_AsString(self->subarch), PyString_AsString(self->color));
      CAST_LOG(logger, level, message, additions);
   }
   catch(Exception& ex) {
      doex(ex);
   }

   Py_INCREF(Py_None);
   return Py_None;
}

#define LOGFUNC(fnName, fnGetLevel)\
   PyObject* Logger_##fnName(PyObject *self, PyObject *args) { \
      return Logger_do_log(Level::fnGetLevel(), self, args); \
   }
LOGFUNC(fatal, getFatal)
LOGFUNC(error, getError)
LOGFUNC(warn, getWarn)
LOGFUNC(info, getInfo)
LOGFUNC(debug, getDebug)
LOGFUNC(trace, getTrace)
#undef LOGFUNC

PyObject* Logger_setLevel(PyObject *o_self, PyObject *args)
{
   const char* debug = "DEBUG";
   const char *level = (char*)debug;
   if (!PyArg_ParseTuple(args, "s", &level))
      return NULL;
   
   if (level == NULL || strlen(level) < 1) level = debug;

   char buf[6];
   strncpy(buf, level, 6);
   buf[5] = '\0';
   for (char* pbuf = buf; *pbuf != '\0'; pbuf++) *pbuf = (char) toupper(*pbuf);
   // printf("Logger_setLevel: %s\n", buf);
   
   LoggerObject* self = (LoggerObject*) o_self;
   self->logger->setLevel(Level::toLevel(buf));

   Py_INCREF(Py_None);
   return Py_None;
}


#define LOGFUNC(fnName) \
   {#fnName, Logger_##fnName, METH_VARARGS, \
     "Logger." #fnName "(strMessage)\n" \
     "Add a '" #fnName "' message to a log." \
   },
static PyMethodDef Logger_methods[] = {
   // TODO: trace ... fatal; setLevel, getLevel
   LOGFUNC(fatal)
   LOGFUNC(error)
   LOGFUNC(warn)
   LOGFUNC(info)
   LOGFUNC(debug)
   LOGFUNC(trace)
   { "setLevel", Logger_setLevel, METH_VARARGS,
     "Logger.setLevel(strLevel)\n"
     "Set logging level to one of: all, trace, debug, info, warn, error, fatal, off.\n"
     "Any other string will set the level to debug."
   },
   {NULL}  /* Sentinel */
};
#undef LOGFUNC

static PyTypeObject LoggerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pylog4cxx.Logger",        /*tp_name*/
    sizeof(LoggerObject),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Logger_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Logger objects",          /* tp_doc */
    0,		                   /* tp_traverse */
    0,		                   /* tp_clear */
    0,		                   /* tp_richcompare */
    0,		                   /* tp_weaklistoffset */
    0,		                   /* tp_iter */
    0,		                   /* tp_iternext */
    Logger_methods,            /* tp_methods */
    Logger_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Logger_init,     /* tp_init */
    0,                         /* tp_alloc */
    Logger_new,                /* tp_new */
    
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC initpylog4cxx()
{
   PyObject* m;

   if (PyType_Ready(&LoggerType) < 0)
      return;

   // (void) Py_InitModule("pylog4cxx", methods);
   m = Py_InitModule3("pylog4cxx", module_methods,
         "pylog4cxx - Python interface to CoSy-CAST log4cxx logging.");

   if (m == NULL) return;

   Py_INCREF(&LoggerType);
   PyModule_AddObject(m, "Logger", (PyObject *)&LoggerType);
}

