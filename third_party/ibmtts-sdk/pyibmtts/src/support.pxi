'''
Declares methods and macros found in stdlib.h and Python.h for use in Pyrex.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''

cdef extern from "stdlib.h":
  ctypedef int size_t
  void *malloc(size_t size)
  void free(void* ptr)
  int sizeof()

cdef extern from "Python.h":
  void Py_INCREF(object)
  void Py_DECREF(object)
  object PyBuffer_FromMemory(void *, int)
  object PyString_FromStringAndSize(char *, int)
  ctypedef void* PyGILState_STATE
  PyGILState_STATE PyGILState_Ensure()
  void PyGILState_Release(PyGILState_STATE)
