/*
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include <jni.h>
#include <stdarg.h>
#ifdef _MSC_VER
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "java/lang/Object.h"
#include "java/lang/Class.h"
#include "java/lang/String.h"
#include "java/lang/Throwable.h"
#include "java/lang/Boolean.h"
#include "java/lang/Byte.h"
#include "java/lang/Character.h"
#include "java/lang/Double.h"
#include "java/lang/Float.h"
#include "java/lang/Integer.h"
#include "java/lang/Long.h"
#include "java/lang/Short.h"
#include "java/util/Iterator.h"
#include "JArray.h"
#include "functions.h"
#include "macros.h"

using namespace java::lang;
using namespace java::util;


PyExc_JavaError        = ValueError
PyExc_InvalidArgsError = ValueError


static boxfn get_boxfn(PyTypeObject *type):

    cobj = PyObject_GetAttr((PyObject*) type, "boxfn_")
    if cobj == NULL:
        return NULL;

    boxfn fn = (boxfn) PyCapsule_GetPointer(cobj, "boxfn");
    return fn


static int is_instance_of(PyObject* arg, PyTypeObject *type):

    clsObj = PyObject_GetAttr((PyObject*) type, "class_")
    if clsObj == NULL:
        return -1;

    int result = env->get_vm_env()->IsInstanceOf(((t_Object *) arg)->object.this$,
                                                 (jclass) ((t_Object *) clsObj)->object.this$);
    return result


#if defined(_MSC_VER) || defined(__SUNPRO_CC)
int __parseArgs(PyObject* args, char* types, ...)
{
    int count = ((PyTupleObject *) args)->ob_size;

    va_list list, check;
    va_start(list, types);
    va_start(check, types);

    return _parseArgs(((PyTupleObject *) args)->ob_item, count, types, list, check);
}

int __parseArg(PyObject* arg, char* types, ...)
{
    va_list list, check;
    va_start(list, types);
    va_start(check, types);

    return _parseArgs(&arg, 1, types, list, check);
}

int _parseArgs(PyObject** args, unsigned int count, char* types, va_list list, va_list check)
{
    unsigned int typeCount = strlen(types);

    if count > typeCount:
        return -1;
#else

int _parseArgs(PyObject** args, unsigned int count, char* types, ...)
{
    unsigned int typeCount = strlen(types);

    va_list list, check;
    va_start(list, types);
    va_start(check, types);
#endif

    if ! env->vm:
        raise RuntimeError("initVM() must be called first")

    vm_env = env->get_vm_env()  # JNIEnv*
    if ! vm_env:
        raise RuntimeError("attachCurrentThread() must be called first")

    bool last    = false;   /* true if last expected parameter */
    bool varargs = false;   /* true if in varargs mode */
    bool empty   = false;   /* true if in varargs mode and no params passed */
    int  array   = 0;       /* > 0 if expecting an array, its nesting level */
    unsigned int pos = 0;

    for (unsigned int a = 0; a < count; a++, pos++)
    {
        PyObject* arg = args[a];
        char tc = types[pos];

        last = last || types[pos + 1] == '\0';

        if (array > 1 && tc != '[')
          tc = 'o';

        switch (tc)
        {
          case '[':
          {
              if (++array > 1 && !PyObject_TypeCheck(arg, PY_TYPE(JArrayObject)))
                return -1;

              a -= 1;
              break;
          }

          case 'j':           /* Java object, with class$    */
          case 'k':           /* Java object, with initializeClass */
          case 'K':           /* Java object, with initializeClass and params */
          {
              jclass cls = NULL;

              switch (tc) {
                case 'j':
                  cls = (jclass) va_arg(list, Class *)->this$;
                  break;
                case 'k':
                case 'K':
                  try {
                      getclassfn initializeClass = va_arg(list, getclassfn);
                      cls = env->getClass(initializeClass);
                  } catch (int e) {
                      switch (e) {
                        case _EXC_PYTHON:
                          return -1;
                        case _EXC_JAVA:
                          PyErr_SetJavaError();
                          return -1;
                        default:
                          throw;
                      }
                  }
                  break;
              }

              if arg is None:
                  break;

              /* ensure that class Class is initialized (which may not be the
               * case because of earlier recursion avoidance (JObject(cls)).
               */
              if (!Class::class$)
                  env->getClass(Class::initializeClass);

              if array:
              {
                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayObject)))
                      break;

                  if PySequence_Check(arg) and not isinstance(arg, (bytes, unicode)):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          int ok = 0;

                          if obj is None:
                              ok = 1;
                          elif (PyObject_TypeCheck(obj, &PY_TYPE(Object)) &&
                                   vm_env->IsInstanceOf(((t_Object *) obj)->object.this$, cls))
                              ok = 1;
                          elif (PyObject_TypeCheck(obj, &PY_TYPE(FinalizerProxy)))
                          {
                              PyObject* o = ((t_fp *) obj)->object;

                              if (PyObject_TypeCheck(o, &PY_TYPE(Object)) &&
                                  vm_env->IsInstanceOf(((t_Object *) o)->object.this$, cls))
                                  ok = 1;
                          }

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if (last)
                  {
                      int ok = 0;

                      if arg is None:
                          ok = 1;
                      elif (PyObject_TypeCheck(arg, &PY_TYPE(Object)) &&
                               vm_env->IsInstanceOf(((t_Object *) arg)->object.this$, cls))
                          ok = 1;
                      elif (PyObject_TypeCheck(arg, &PY_TYPE(FinalizerProxy)))
                      {
                          PyObject* o = ((t_fp *) arg)->object;

                          if (PyObject_TypeCheck(o, &PY_TYPE(Object)) &&
                              vm_env->IsInstanceOf(((t_Object *) o)->object.this$, cls))
                              ok = 1;
                      }
                      if ok:
                      {
                          varargs = true;
                          break;
                      }
                  }
              }
              elif (PyObject_TypeCheck(arg, &PY_TYPE(Object)) &&
                       vm_env->IsInstanceOf(((t_Object *) arg)->object.this$, cls))
                  break;
              elif (PyObject_TypeCheck(arg, &PY_TYPE(FinalizerProxy)))
              {
                  arg = ((t_fp *) arg)->object;
                  if (PyObject_TypeCheck(arg, &PY_TYPE(Object)) &&
                      vm_env->IsInstanceOf(((t_Object *) arg)->object.this$, cls))
                      break;
              }

              return -1;
          }

          case 'Z':           /* boolean, strict */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayBool)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = (obj == Py_True || obj == Py_False)

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if (last && (arg == Py_True || arg == Py_False))
                  {
                      varargs = true;
                      break;
                  }
              }
              elif (arg == Py_True || arg == Py_False)
                  break;

              return -1;
          }

          case 'B':           /* byte */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayByte)))
                      break;

                  if (last)
                  {
                      if ((isinstance(arg, bytes) && len(arg) == 1) ||
                          PyInt_CheckExact(arg)):
                      {
                          varargs = true;
                          break;
                      }
                  }
              }
              elif (isinstance(arg, bytes) && len(arg) == 1)
                  break;
              elif PyInt_CheckExact(arg):
                  break;

              return -1;
          }

          case 'C':           /* char */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayChar)))
                      break;

                  if (last)
                  {
                      if  isinstance(arg, unicode) and len(arg) == 1:
                      {
                          varargs = true;
                          break;
                      }
                  }
              }
              elif  isinstance(arg, unicode) and len(arg) == 1:
                  break;
              return -1;
          }

          case 'I':           /* int */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayInt)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = PyInt_CheckExact(obj)

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if last && PyInt_CheckExact(arg):
                  {
                      varargs = true;
                      break;
                  }
              }
              elif PyInt_CheckExact(arg):
                  break;

              return -1;
          }

          case 'S':           /* short */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayShort)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = PyInt_CheckExact(obj)

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if last && PyInt_CheckExact(arg):
                  {
                      varargs = true;
                      break;
                  }
              }
              elif PyInt_CheckExact(arg):
                  break;

              return -1;
          }

          case 'D':           /* double */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayDouble)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = PyFloat_CheckExact(obj);

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if (last && PyFloat_CheckExact(arg))
                  {
                      varargs = true;
                      break;
                  }
              }
              elif (PyFloat_CheckExact(arg))
                  break;

              return -1;
          }

          case 'F':           /* float */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayFloat)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = PyFloat_CheckExact(obj);

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if (last && PyFloat_CheckExact(arg))
                  {
                      varargs = true;
                      break;
                  }
              }
              elif (PyFloat_CheckExact(arg))
                  break;

              return -1;
          }

          case 'J':           /* long long */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayLong)))
                      break;

                  if PySequence_Check(arg):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = PyLong_CheckExact(obj)

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  }

                  if (last && PyLong_CheckExact(arg))
                  {
                      varargs = true;
                      break;
                  }
              }
              elif (PyLong_CheckExact(arg))
                  break;

              return -1;
          }

          case 's':           /* string  */
          {
              if array:
              {
                  if arg is None:
                      break;

                  if (PyObject_TypeCheck(arg, PY_TYPE(JArrayString)))
                      break;

                  if PySequence_Check(arg) and not isinstance(arg, (bytes, unicode)):
                  {
                      if len(arg) > 0:
                      {
                          obj = PySequence_GetItem(arg, 0)
                          ok = (obj is None || isinstance(obj, (bytes, unicode)))

                          Py_DECREF(obj);
                          if ok:
                              break;
                      }
                      else
                          break;
                  } 

                  if last and (arg is None or isinstance(arg, (bytes, unicode))):
                  {
                      varargs = true;
                      break;
                  }
              }
              elif arg is None or isinstance(arg, (bytes, unicode)):
                  break;

              return -1;
          }

          case 'o':         /* java.lang.Object */
            break;

          case 'O':         /* java.lang.Object with type param */
          {
              PyTypeObject *type = va_arg(list, PyTypeObject *);

              if (type != NULL)
              {
                  boxfn fn = get_boxfn(type);
    
                  if (fn == NULL || fn(type, arg, NULL) < 0)
                      return -1;
              }
              break;
          }

          case 'T':         /* tuple of python types with wrapfn_ */
          {
              static wrapfn_ = "wrapfn_"
              int len = va_arg(list, int);

              if isinstance(arg, tuple):
              {
                  if len(arg) != len:
                      return -1;

                  for (int i = 0; i < len; i++)
                  {
                      PyObject* type = PyTuple_GET_ITEM(arg, i);

                      if (!(type is None || (PyType_Check(type) && PyObject_HasAttr(type, wrapfn_))))
                          return -1;
                  }
                  break;
              }
              return -1;
          }

          default:
            return -1;
        }

        if (tc != '[')
            array = 0;

        if (varargs)
        {
            pos = typeCount;
            break;
        }
    }

    if array:
        return -1;

    if (pos == typeCount - 2 && types[pos] == '[' && types[pos + 1] != '[')
    {
        varargs = true;
        empty = true;
        pos = typeCount;
    }

    if (pos != typeCount)
      return -1;

    pos = 0;
    last = false;

    for (unsigned int a = 0; a <= count; a++, pos++)
    {
        char tc = types[pos];
        PyObject* arg;

        if (a == count)
        {
            if (empty)  /* empty varargs */
                arg = NULL;
            else
                break;
        }
        else
            arg = args[a];

        last = last || types[pos + 1] == '\0';

        if (array > 1 && tc != '[')
            tc = 'o';

        switch (tc) {
          case '[':
          {
              array += 1;
              a -= 1;

              break;
          }

          case 'j':           /* Java object except String and Object */
          case 'k':           /* Java object, with initializeClass    */
          case 'K':           /* Java object, with initializeClass and params */
          {
              jclass cls = NULL;

              switch (tc) {
                case 'j':
                  cls = (jclass) va_arg(check, Class *)->this$;
                  break;
                case 'k':
                case 'K':
                  getclassfn initializeClass = va_arg(check, getclassfn);
                  cls = env->getClass(initializeClass);
                  break;
              }

              if array:
              {
                  JArray<jobject> *array = va_arg(list, JArray<jobject> *);

#ifdef _java_generics
                  if (tc == 'K')
                  {
                      PyTypeObject ***tp = va_arg(list, PyTypeObject ***);

                      va_arg(list, getparametersfn);
                      *tp = NULL;
                  }
#endif
                  if arg is None:
                      *array = JArray<jobject>((jobject) NULL);
                  elif last and varargs:
                      *array = fromPySequence(cls, args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayObject)))
                      *array = ((t_JArray<jobject> *) arg)->array;
                  else
                      *array = JArray<jobject>(cls, arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  Object *obj = va_arg(list, Object *);

                  if (PyObject_TypeCheck(arg, &PY_TYPE(FinalizerProxy)))
                      arg = ((t_fp *) arg)->object;

#ifdef _java_generics
                  if (tc == 'K')
                  {
                      PyTypeObject ***tp = va_arg(list, PyTypeObject ***);
                      PyTypeObject **(*parameters_)(void *) =
                          va_arg(list, getparametersfn);

                      if arg is None:
                          *tp = NULL;
                      else
                          *tp = (*parameters_)(arg);
                  }
#endif

                  *obj = arg is None ? Object(NULL) : ((t_Object *) arg)->object;
              }
              break;
          }

          case 'Z':           /* boolean, strict */
          {
              if array:
              {
                  JArray<jboolean> *array = va_arg(list, JArray<jboolean> *);

                  if arg is None:
                      *array = JArray<jboolean>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jboolean>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayBool)))
                      *array = ((t_JArray<jboolean> *) arg)->array;
                  else
                      *array = JArray<jboolean>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else:
              {
                  jboolean *b = va_arg(list, jboolean *);
                  *b = arg == Py_True;
              }
              break;
          }

          case 'B':           /* byte */
          {
              if array:
              {
                  JArray<jbyte> *array = va_arg(list, JArray<jbyte> *);

                  if arg is None:
                      *array = JArray<jbyte>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jbyte>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayByte)))
                      *array = ((t_JArray<jbyte> *) arg)->array;
                  else
                      *array = JArray<jbyte>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              elif isinstance(arg, bytes):
              {
                  jbyte *a = va_arg(list, jbyte *);
                  *a = (jbyte) PyString_AS_STRING(arg)[0];
              }
              else:
              {
                  jbyte *a = va_arg(list, jbyte *);
                  *a = (jbyte) PyInt_AsLong(arg);
              }
              break;
          }

          case 'C':           /* char */
          {
              if array:
              {
                  JArray<jchar> *array = va_arg(list, JArray<jchar> *);

                  if arg is None:
                      *array = JArray<jchar>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jchar>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayChar)))
                      *array = ((t_JArray<jchar> *) arg)->array;
                  else
                      *array = JArray<jchar>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jchar *c = va_arg(list, jchar *);
                  *c = (jchar) PyUnicode_AS_UNICODE(arg)[0];
              }
              break;
          }

          case 'I':           /* int */
          {
              if array:
              {
                  JArray<jint> *array = va_arg(list, JArray<jint> *);

                  if arg is None:
                      *array = JArray<jint>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jint>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayInt)))
                      *array = ((t_JArray<jint> *) arg)->array;
                  else
                      *array = JArray<jint>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jint *n = va_arg(list, jint *);
                  *n = (jint) PyInt_AsLong(arg);
              }
              break;
          }

          case 'S':           /* short */
          {
              if array:
              {
                  JArray<jshort> *array = va_arg(list, JArray<jshort> *);

                  if arg is None:
                      *array = JArray<jshort>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jshort>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayShort)))
                      *array = ((t_JArray<jshort> *) arg)->array;
                  else
                      *array = JArray<jshort>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jshort *n = va_arg(list, jshort *);
                  *n = (jshort) PyInt_AsLong(arg);
              }
              break;
          }

          case 'D':           /* double */
          {
              if array:
              {
                  JArray<jdouble> *array = va_arg(list, JArray<jdouble> *);

                  if arg is None:
                      *array = JArray<jdouble>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jdouble>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayDouble)))
                      *array = ((t_JArray<jdouble> *) arg)->array;
                  else
                      *array = JArray<jdouble>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jdouble *d = va_arg(list, jdouble *);
                  *d = (jdouble) PyFloat_AsDouble(arg);
              }
              break;
          }

          case 'F':           /* float */
          {
              if array:
              {
                  JArray<jfloat> *array = va_arg(list, JArray<jfloat> *);

                  if arg is None:
                      *array = JArray<jfloat>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jfloat>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayFloat)))
                      *array = ((t_JArray<jfloat> *) arg)->array;
                  else
                      *array = JArray<jfloat>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jfloat *d = va_arg(list, jfloat *);
                  *d = (jfloat) (float) PyFloat_AsDouble(arg);
              }
              break;
          }

          case 'J':           /* long long */
          {
              if array:
              {
                  JArray<jlong> *array = va_arg(list, JArray<jlong> *);

                  if arg is None:
                      *array = JArray<jlong>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<jlong>(args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayLong)))
                      *array = ((t_JArray<jlong> *) arg)->array;
                  else
                      *array = JArray<jlong>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  jlong *l = va_arg(list, jlong *);
                  *l = (jlong) PyLong_AsLongLong(arg);
              }
              break;
          }

          case 's':           /* string  */
          {
              if array:
              {
                  JArray<jstring> *array = va_arg(list, JArray<jstring> *);

                  if arg is None:
                      *array = JArray<jstring>((jobject) NULL);
                  elif last and varargs:
                      *array = fromPySequence(env->getClass(String::initializeClass),
                                              args + a, count - a);
                  elif (PyObject_TypeCheck(arg, PY_TYPE(JArrayString)))
                      *array = ((t_JArray<jstring> *) arg)->array;
                  else
                      *array = JArray<jstring>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  String *str = va_arg(list, String *);

                  if arg is None:
                      *str = String(NULL);
                  else
                  {
                      *str = p2j(arg);
                      if PyErr_Occurred():
                          return -1;
                  }
              }
              break;
          }

          case 'o':           /* java.lang.Object  */
          case 'O':           /* java.lang.Object with type param */
          {
              if array:
              {
                  JArray<Object> *array = va_arg(list, JArray<Object> *);

                  if arg is None:
                      *array = JArray<Object>((jobject) NULL);
                  elif last and varargs:
                      *array = JArray<Object>(env->getClass(Object::initializeClass),
                                              args + a, count - a);
                  else
                      *array = JArray<Object>(arg);

                  if PyErr_Occurred():
                      return -1;
              }
              else
              {
                  Object *obj = va_arg(list, Object *);

                  if (tc == 'O')
                  {
                      PyTypeObject *type = va_arg(check, PyTypeObject *);

                      if (type != NULL)
                      {
                          boxfn fn = get_boxfn(type);

                          if (fn == NULL || fn(type, arg, obj) < 0)
                              return -1;

                          break;
                      }
                  }

                  if (boxObject(NULL, arg, obj) < 0)
                      return -1;
              }
              break;
          }

          case 'T':         /* tuple of python types with wrapfn_ */
          {
              int len = va_arg(check, int);
              PyTypeObject **types = va_arg(list, PyTypeObject **);

              for (int i = 0; i < len; i++)
              {
                  PyObject* type = PyTuple_GET_ITEM(arg, i);

                  if type is None:
                      types[i] = NULL;
                  else
                      types[i] = (PyTypeObject *) type;
              }
              break;
          }

          default:
            return -1;
        }

        if (tc != '[')
            array = 0;

        if (last && varargs)
        {
            pos = typeCount;
            break;
        }
    }

    if (pos == typeCount)
        return 0;

    return -1;
}


String p2j(PyObject* object)
{
    return String(env->fromPyString(object));
}

PyObject* j2p(const String& js)
{
    return env.fromJString((jstring) js.this$)
}

PyObject *c2p(jchar c)
{
#ifdef PY3
    PyObject *result = PyUnicode_New(1, c);
    PyUnicode_WriteChar(result, 0, c);
#else
    PyObject *result = PyUnicode_FromUnicode((Py_UNICODE *) &c, 1);
#endif

    return result;
}

PyObject* PyErr_SetArgsError(char *name, PyObject* args):

    if PyErr_Occurred():
        return NULL;

    PyObject* err = Py_BuildValue("(sO)", name, args);
    raise InvalidArgsError(err)

PyObject* PyErr_SetArgsError(PyObject* self, char *name, PyObject* args):

    if PyErr_Occurred():
        return NULL;

    PyObject* type = (PyObject*) self->ob_type;
    PyObject* err  = Py_BuildValue("(OsO)", type, name, args);
    raise InvalidArgsError(err)

PyObject* PyErr_SetArgsError(PyTypeObject *type, char *name, PyObject* args):

    if PyErr_Occurred():
        return NULL;

    PyObject* err = Py_BuildValue("(OsO)", type, name, args);
    raise InvalidArgsError(err)

PyObject* PyErr_SetJavaError():

    vm_env = env->get_vm_env();  # JNIEnv*
    jthrowable throwable = vm_env->ExceptionOccurred();
    vm_env->ExceptionClear();

    if env->restorePythonException(throwable):
        return NULL;

    PyObject* err = t_Throwable::wrap_Object(Throwable(throwable));
    raise JavaError(err)
    return NULL;

void throwPythonError(void)
{
    PyObject* exc = PyErr_Occurred();

    if (exc && PyErr_GivenExceptionMatches(exc, PyExc_JavaError)):
    {
        PyObject *value, *traceback;

        PyErr_Fetch(&exc, &value, &traceback);
        if (value)
        {
            PyObject* je = value.getJavaException()

            if (!je)
                PyErr_Restore(exc, value, traceback);
            else
            {
                Py_DECREF(exc);
                Py_DECREF(value);
                Py_XDECREF(traceback);
                exc = je;

                if (exc && PyObject_TypeCheck(exc, &PY_TYPE(Throwable)))
                {
                    jobject jobj = ((t_Throwable *) exc)->object.this$;

                    env->get_vm_env()->Throw((jthrowable) jobj);
                    Py_DECREF(exc);

                    return;
                }
            }
        }
        else
        {
            Py_DECREF(exc);
            Py_XDECREF(traceback);
        }
    }
    elif (exc && PyErr_GivenExceptionMatches(exc, PyExc_StopIteration))
    {
        PyErr_Clear();
        return;
    }

    if (exc)
    {
        PyObject* name = PyObject_GetAttrString(exc, "__name__");

        env->get_vm_env()->ThrowNew(env->getPythonExceptionClass(), PyString_AS_STRING(name));
        Py_DECREF(name);
    }
    else
        env->get_vm_env()->ThrowNew(env->getPythonExceptionClass(), "python error");
}

def throwTypeError(const char *name, PyObject* object):

    tuple = Py_BuildValue("(ssO)", "while calling", name, object);
    env->get_vm_env()->ThrowNew(env->getPythonExceptionClass(), "type error");
    raise TypeError(tuple)

int abstract_init(PyObject* self, PyObject* args, PyObject* kwds):

    err = Py_BuildValue("(sO)", "instantiating java class", self->ob_type);
    raise NotImplementedError(err)


PyObject* callSuper(PyTypeObject *type, const char *name, PyObject* args, int cardinality):

    PyObject* super  = (PyObject*) type->tp_base;
    PyObject* method = PyObject_GetAttrString(super, name)
    PyObject* value;

    if (!method)
        return NULL;

    if cardinality > 1:
        value = PyObject_Call(method, args, NULL);
    else:
        tuple = PyTuple_Pack(1, args)
        value = PyObject_Call(method, tuple, NULL);
    return value

PyObject* callSuper(PyTypeObject *type, PyObject* self, const char *name, PyObject* args, int cardinality):

    PyObject* tuple = PyTuple_Pack(2, type, self);
    PyObject* super = PyObject_Call((PyObject*) &PySuper_Type, tuple, NULL);
    PyObject* method, *value;

    Py_DECREF(tuple);
    if (!super)
        return NULL;

    method = PyObject_GetAttrString(super, name)
    Py_DECREF(super);
    if (!method)
        return NULL;

    if cardinality > 1:
        value = PyObject_Call(method, args, NULL);
    else:
        tuple = PyTuple_Pack(1, args)
        value = PyObject_Call(method, tuple, NULL);
    return value

PyObject* castCheck(PyObject* obj, getclassfn initializeClass, int reportError)
{
    if (PyObject_TypeCheck(obj, &PY_TYPE(FinalizerProxy)))
        obj = ((t_fp *) obj)->object;

    if (!PyObject_TypeCheck(obj, &PY_TYPE(Object)))
    {
        if reportError:
            raise TypeError(obj)
        return NULL;
    }

    jobject jobj = ((t_Object *) obj)->object.this$;

    if (jobj && !env->isInstanceOf(jobj, initializeClass))
    {
        if reportError:
            raise TypeError(obj)
        return NULL;
    }

    return obj;
}

    def get_extension_iterator(self):

        return self.iterator()

    def get_extension_next(self):

        return self.next()

    def get_extension_nextElement(self):

        return self.nextElement()

static bool setArrayObj(jobjectArray array, int index, PyObject* obj):

    jobject jobj;

    delete_local = False

    if obj is None:

      jobj = NULL;

    elif isinstance(obj, (bytes, unicode)):

        jobj = env->fromPyString(obj);
        delete_local = True

    elif PyObject_TypeCheck(obj, &PY_TYPE(JObject)):

        jobj = ((t_JObject *) obj)->object.this$;

    elif PyObject_TypeCheck(obj, &PY_TYPE(FinalizerProxy)):

        jobj = ((t_JObject *) ((t_fp *) obj)->object)->object.this$;

    elif (obj == Py_True || obj == Py_False):

        jobj = env->boxBoolean(obj == Py_True);
        delete_local = True

    elif isinstance(obj, float):

        jobj = env->boxDouble(PyFloat_AS_DOUBLE(obj));
        delete_local = True

    elif isinstance(obj, int):

        jobj = env->boxInteger(PyInt_AS_LONG(obj));
        delete_local = True

    elif isinstance(obj, long):

        jobj = env->boxLong(PyLong_AsLongLong(obj));
        delete_local = True

    else:
        raise TypeError(obj)

    try:
        env->setObjectArrayElement(array, index, jobj);
        if delete_local:
            env->get_vm_env()->DeleteLocalRef(jobj)
        Py_DECREF(obj);
    catch (int e):
    {
        Py_DECREF(obj);
        switch (e) {
          case _EXC_JAVA:
            PyErr_SetJavaError();
            return false;
          default:
            throw;
        }
    }

    return true;

jobjectArray fromPySequence(jclass cls, PyObject* sequence)
{
    if sequence is None:
        return NULL;

    if not PySequence_Check(sequence):
        raise TypeError(sequence)

    int length = PySequence_Length(sequence);
    jobjectArray array;

    try:
        array = env->newObjectArray(cls, length);
    catch (int e)
    {
        switch (e) {
          case _EXC_PYTHON:
            return NULL;
          case _EXC_JAVA:
            PyErr_SetJavaError();
            return NULL;
          default:
            throw;
        }
    }

    for (int i = 0; i < length; i++)
    {
        PyObject* obj = PySequence_GetItem(sequence, i);

        if (!obj)
            break;

        if (!setArrayObj(array, i, obj))
            return NULL;
    }

    return array;
}

jobjectArray fromPySequence(jclass cls, PyObject** args, int length)
{
    jobjectArray array;

    try:
        array = env->newObjectArray(cls, length);
    catch (int e)
    {
        switch (e) {
          case _EXC_PYTHON:
            return NULL;
          case _EXC_JAVA:
            PyErr_SetJavaError();
            return NULL;
          default:
            throw;
        }
    }

    for (int i = 0; i < length; i++)
    {
        PyObject* obj = args[i];

        if (!obj)
            break;

        Py_INCREF(obj);
        if (!setArrayObj(array, i, obj))
            return NULL;
    }

    return array;
}

def installType(PyTypeObject *type, PyObject* module, char *name, int isExtension):
{
    if (PyType_Ready(type) == 0)
    {
        Py_INCREF(type);
        if (isExtension)
        {
            type->ob_type = &PY_TYPE(FinalizerClass);
            Py_INCREF(&PY_TYPE(FinalizerClass));
        }
        PyModule_AddObject(module, name, (PyObject*) type);
    }
}

PyObject* wrapType(PyTypeObject *type, const jobject& jobj):

    cobj = PyObject_GetAttr((PyObject*) type, "wrapfn_")
    if cobj == NULL:
        return NULL;

    PyObject* (*wrapfn)(const jobject&);
    wrapfn = (PyObject* (*)(const jobject &)) PyCapsule_GetPointer(cobj, "wrapfn")
    return wrapfn(jobj)


@annotate(bool, jobj=jni.jobject)
def unboxBoolean(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Boolean::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Boolean))

    return bool(env.booleanValue(jobj))

@annotate(int, jobj=jni.jobject)
def unboxByte(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Byte::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Byte))

    return int(env.byteValue(jobj))

@annotate(jobj=jni.jobject)
def unboxCharacter(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Character::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Character))
    
    jchar c = env.charValue(jobj)
    return PyUnicode_FromUnicode((Py_UNICODE *) &c, 1);

@annotate(int, jobj=jni.jobject)
def unboxShort(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Short::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Short))
    
    return int(env.shortValue(jobj))

@annotate(int, jobj=jni.jobject)
def unboxInteger(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Integer::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Integer))
    
    return int(env.intValue(jobj))

@annotate(long, jobj=jni.jobject)
def unboxLong(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Long::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Long))
    
    return long(env.longValue(jobj))

@annotate(float, jobj=jni.jobject)
def unboxFloat(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Float::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Float))
    
    return float(env.floatValue(jobj))

@annotate(float, jobj=jni.jobject)
def unboxDouble(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::Double::initializeClass):
        raise TypeError(java::lang::PY_TYPE(Double))
    
    return float(env.doubleValue(jobj))

@annotate(jobj=jni.jobject)
def unboxString(jobj):

    if not jobj:
        return None

    if not env.isInstanceOf(jobj, java::lang::String::initializeClass):
        raise TypeError(java::lang::PY_TYPE(String))
    
    return env.fromJString((jstring) jobj)


static int boxJObject(PyTypeObject *type, PyObject* arg, java::lang::Object* obj):

    if arg is None:

        if obj != NULL:
            *obj = Object(NULL);

    elif PyObject_TypeCheck(arg, &PY_TYPE(Object)):

        if type != NULL && ! is_instance_of(arg, type):
            return -1;

        if obj != NULL:
            *obj = ((t_Object *) arg)->object;

    elif PyObject_TypeCheck(arg, &PY_TYPE(FinalizerProxy)):

        arg = ((t_fp *) arg)->object;
        if PyObject_TypeCheck(arg, &PY_TYPE(Object)):

            if type != NULL && ! is_instance_of(arg, type):
                return -1;

            if obj != NULL:
                *obj = ((t_Object *) arg)->object;

        else
            return -1;

    else
        return 1;

    return 0;

int boxBoolean(PyTypeObject *type, PyObject* arg, java::lang::Object* obj):

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if arg == Py_True:
        if obj != NULL:
            *obj = *Boolean::TRUE;
    elif arg == Py_False:
        if obj != NULL:
            *obj = *Boolean::FALSE;
    else
        return -1;

    return 0;

int boxByte(PyTypeObject *type, PyObject* arg, java::lang::Object* obj):

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        int n = PyInt_AS_LONG(arg);
        jbyte b = (jbyte) n;

        if (b == n)
            if obj != NULL:
                *obj = Byte(b);
        else
            return -1;

    elif isinstance(arg, long):

        PY_LONG_LONG ln = PyLong_AsLongLong(arg);
        jbyte b = (jbyte) ln;

        if (b == ln)
            if obj != NULL:
                *obj = Byte(b);
        else
            return -1;

    elif isinstance(arg, float):

        double d = PyFloat_AS_DOUBLE(arg);
        jbyte b = (jbyte) d;

        if (b == d)
            if obj != NULL:
                *obj = Byte(b);
        else
            return -1;

    else
        return -1;

    return 0;

int boxCharacter(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, bytes):

        if len(arg) != 1:
            return -1;

        char *c;
        Py_ssize_t len;
        if PyBytes_AsStringAndSize(arg, &c, &len) < 0:
            return -1;

        if obj != NULL:
            *obj = Character((jchar) c[0]);

    elif isinstance(arg, unicode):

        if len(arg) != 1:
            return -1;

        if obj != NULL:
            *obj = Character((jchar) PyUnicode_AsUnicode(arg)[0]);

    else
        return -1;

    return 0;

int boxCharSequence(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, (bytes, unicode)):

        if obj != NULL:
            *obj = p2j(arg);
            if PyErr_Occurred():
                return -1;

    else
        return -1;
    
    return 0;

int boxDouble(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        if obj != NULL:
            *obj = Double((jdouble) PyInt_AS_LONG(arg));

    elif isinstance(arg, long):

        if obj != NULL:
            *obj = Double((jdouble) PyLong_AsLongLong(arg));

    elif isinstance(arg, float):

        if obj != NULL:
            *obj = Double(PyFloat_AS_DOUBLE(arg));

    else
        return -1;

    return 0;

int boxFloat(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        if obj != NULL:
            *obj = Float((jfloat) PyInt_AS_LONG(arg));

    elif isinstance(arg, long):

        PY_LONG_LONG ln = PyLong_AsLongLong(arg);
        float f = (float) ln;

        if ((PY_LONG_LONG) f == ln)
            if obj != NULL:
                *obj = Float(f);
        else
            return -1;

    elif isinstance(arg, float):

        double d = PyFloat_AS_DOUBLE(arg);
        float f = (float) d;

        if ((double) f == d)
            if obj != NULL:
                *obj = Float(f);
        else
            return -1;

    else
        return -1;

    return 0;

int boxInteger(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        if obj != NULL:
            *obj = Integer((jint) PyInt_AS_LONG(arg));

    elif isinstance(arg, long):

        PY_LONG_LONG ln = PyLong_AsLongLong(arg);
        int n = (int) ln;

        if (n == ln)
            if obj != NULL:
                *obj = Integer(n);
        else
            return -1;

    elif isinstance(arg, float):

        double d = PyFloat_AS_DOUBLE(arg);
        int n = (int) d;

        if ((double) n == d)
            if obj != NULL:
                *obj = Integer(n);
        else
            return -1;

    else
        return -1;

    return 0;

int boxLong(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        if obj != NULL:
            *obj = Long((jlong) PyInt_AS_LONG(arg));

    elif isinstance(arg, long):

        if obj != NULL:
            *obj = Long((jlong) PyLong_AsLongLong(arg));

    elif isinstance(arg, float):

        double d = PyFloat_AS_DOUBLE(arg);
        PY_LONG_LONG n = (PY_LONG_LONG) d;

        if ((double) n == d)
            if obj != NULL:
                *obj = Long((jlong) n);
        else
            return -1;

    else
        return -1;

    return 0;

int boxNumber(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        if obj != NULL:
            *obj = Integer((jint) PyInt_AS_LONG(arg));

    elif isinstance(arg, long):

        if obj != NULL:
            *obj = Long((jlong) PyLong_AsLongLong(arg));

    elif isinstance(arg, float):

        if obj != NULL:
            *obj = Double((jdouble) PyFloat_AS_DOUBLE(arg));

    else
        return -1;

    return 0;

int boxShort(PyTypeObject *type, PyObject* arg, java::lang::Object* obj)

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, int):

        int n = (int) PyInt_AS_LONG(arg);
        short sn = (short) n;

        if (sn == n)
            if obj != NULL:
                *obj = Short((jshort) sn);
        else
            return -1;

    elif isinstance(arg, long):

        PY_LONG_LONG ln = PyLong_AsLongLong(arg);
        short sn = (short) ln;

        if (sn == ln)
            if obj != NULL:
                *obj = Short((jshort) sn);
        else
            return -1;

    elif isinstance(arg, float):

        double d = PyFloat_AS_DOUBLE(arg);
        short sn = (short) (int) d;

        if ((double) sn == d)
            if obj != NULL:
                *obj = Short((jshort) sn);
        else
            return -1;

    else
        return -1;

    return 0;

int boxString(PyTypeObject *type, PyObject* arg, java::lang::Object* obj):

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if isinstance(arg, (bytes, unicode)):

        if obj != NULL:
            *obj = p2j(arg);
            if PyErr_Occurred():
                return -1;

    else
        return -1;

    return 0;

int boxObject(PyTypeObject *type, PyObject* arg, java::lang::Object* obj):

    result = boxJObject(type, arg, obj)  # int
    if result <= 0:
        return result

    if obj != NULL:

        if isinstance(arg, (bytes, unicode)):

            *obj = p2j(arg);
            if PyErr_Occurred():
                return -1;

        elif arg == Py_True:
            *obj = *Boolean::TRUE;
        elif arg == Py_False:
            *obj = *Boolean::FALSE;

        elif isinstance(arg, int):

            long ln = PyInt_AS_LONG(arg);
            int  n  = (int) ln;

            if ln != (long) n:
                *obj = Long((jlong) ln)
            else
                *obj = Integer((jint) n)

        elif isinstance(arg, long):

            *obj = Long((jlong) PyLong_AsLongLong(arg));

        elif isinstance(arg, float):

            *obj = Double((jdouble) PyFloat_AS_DOUBLE(arg));

        else
            return -1;

    elif not (isinstance(arg, (bytes, unicode))   or
              (arg == Py_True or arg == Py_False) or
              isinstance(arg, (int, long, float))):
        return -1;

    return 0;


#ifdef _java_generics
PyObject* typeParameters(PyTypeObject *types[], size_t size)
{
    size_t count = size / sizeof(PyTypeObject *);

    PyObject* tuple = PyTuple_New(count);
    for (size_t i = 0; i < count; i++):

        PyObject* type = (PyObject*) types[i];
        if (type == NULL)
            type = None

        PyTuple_SET_ITEM(tuple, i, type);

    return tuple;
}
#endif
