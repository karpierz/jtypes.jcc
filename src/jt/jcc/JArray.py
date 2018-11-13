
#include <jni.h>
#include <Python.h>
#include "structmember.h"

#include "JArray.h"
#include "functions.h"
#include "java/lang/Class.h"

using namespace java::lang;


template<typename T> class _t_JArray : public t_JArray<T>
{
public:

    static PyObject* format;
};

template<typename U> static PyObject* get(U *self, Py_ssize_t n):

    return self.array.get(n)

template<typename U> static PyObject* toSequence(U *self):

    return self.array.toSequence()

template<typename U> static PyObject* toSequence(U *self, Py_ssize_t lo, Py_ssize_t hi):

    return self.array.toSequence(lo, hi)

template<typename U> class _t_iterator
{
public:

    PyObject_HEAD
    U *obj;
    Py_ssize_t position;

    static void dealloc(_t_iterator *self):

        Py_XDECREF(self.obj);
        self.ob_type->tp_free((PyObject *) self);

    static PyObject* iternext(_t_iterator *self):

        if (self.position < (Py_ssize_t) self.obj->array.length)
            return get<U>(self.obj, self.position++)
        else:
            PyErr_SetNone(PyExc_StopIteration)
            return NULL;

    static PyTypeObject *JArrayIterator;
};

template<typename T, typename U> static int init(U *self, PyObject* args, PyObject* kwds):

    PyObject* obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return -1;

    if PySequence_Check(obj):
        self.array = JArray<T>(obj)
    elif PyGen_Check(obj):
        PyObject* tuple = PyObject_CallFunctionObjArgs((PyObject *) &PyTuple_Type, obj, NULL)
        if (!tuple)
            return -1;
        self.array = JArray<T>(tuple)
    elif isinstance(obj, int):
        n = obj
        if n < 0:
            raise ValueError(obj)
        self.array = JArray<T>(n)
    else:
        raise TypeError(obj)

    return 0;

template<typename T, typename U> static void dealloc(U *self):

    self.array = JArray<T>((jobject) NULL)
    self.ob_type->tp_free((PyObject *) self)

template<typename U> static PyObject* _format(U *self, PyObject* (*fn)(PyObject *)):

    if (self.array.this$):

        PyObject* list = self.toSequence<U>()
        if (list):
            PyObject* result = (*fn)(list)
            Py_DECREF(list)
            if (result):
                PyObject* args = PyTuple_New(1)
                PyTuple_SET_ITEM(args, 0, result)
                result = PyString_Format(U::format, args)
                Py_DECREF(args)
                return result;

        return NULL;

    return "<null>"

template<typename U> static PyObject* repr(U *self):

    return _format(self, (PyObject* (*)(PyObject *)) PyObject_Repr)

template<typename U> static PyObject* str(U *self):

    return _format(self, (PyObject* (*)(PyObject *)) PyObject_Str)

template<typename U> static int _compare(U *self, PyObject* value, int i0, int i1, int op, int *cmp):

    PyObject* v0 = get<U>(self, i0)
    PyObject* v1 = PySequence_Fast_GET_ITEM(value, i1)

    if (!v0)
        return -1;

    if (!v1)
        return -1;

    *cmp = PyObject_RichCompareBool(v0, v1, op)

    if (*cmp < 0)
        return -1;

    return 0;

template<typename U> static PyObject* richcompare(U *self, PyObject* value, int op):

    PyObject* result = NULL;
    int s0, s1;

    if ! PySequence_Check(value):
        return Py_NotImplemented;

    value = PySequence_Fast(value, "not a sequence")
    if (!value)
        return NULL;

    s0 = PySequence_Fast_GET_SIZE(value)
    s1 = self.array.length;

    if s1 < 0:
        return NULL;

    if (s0 != s1):
        switch (op)
        {
          case Py_EQ: result = Py_False; break;
          case Py_NE: result = Py_True;  break;
        }

    if !result:

        int i0, i1, cmp = 1;

        for (i0 = 0, i1 = 0; i0 < s0 && i1 < s1 && cmp; i0++, i1++):
            if (_compare(self, value, i0, i1, Py_EQ, &cmp) < 0):
                return NULL;

        if (cmp):
            switch (op)
            {
              case Py_LT: cmp = s0 <  s1; break;
              case Py_LE: cmp = s0 <= s1; break;
              case Py_EQ: cmp = s0 == s1; break;
              case Py_NE: cmp = s0 != s1; break;
              case Py_GT: cmp = s0 >  s1; break;
              case Py_GE: cmp = s0 >= s1; break;
              default: cmp = 0;
            }
            result = cmp ? Py_True : Py_False;
        elif (op == Py_EQ)
            result = Py_False;
        elif (op == Py_NE)
            result = Py_True;
        elif (_compare(self, value, i0, i1, op, &cmp) < 0):
            return NULL;
        else:
            result = cmp ? Py_True : Py_False;

    return result;

template<typename U> static PyObject* iter(U *self):

    _t_iterator<U> *it = PyObject_New(_t_iterator<U>, _t_iterator<U>::JArrayIterator);

    if (it):
        it->position = 0;
        it->obj      = self;

    return (PyObject *) it;

template<typename U> static Py_ssize_t seq_length(U *self):

    if (self.array.this$)
        return self.array.length;
    else:
        return 0;

template<typename U> static PyObject* seq_get(U *self, Py_ssize_t n):

    return get<U>(self, n)

template<typename U> static int seq_contains(U *self, PyObject* value):

    return 0

template<typename U> static PyObject* seq_concat(U *self, PyObject* arg):

    list = self.toSequence<U>() # PyObject*

    if (list != NULL && PyList_Type.tp_as_sequence->sq_inplace_concat(list, arg) < 0):
        return NULL;

    return list;

template<typename U> static PyObject* seq_repeat(U *self, Py_ssize_t n):

    list = self.toSequence<U>() # PyObject*

    if (list != NULL && PyList_Type.tp_as_sequence->sq_inplace_repeat(list, n) < 0):
        return NULL;

    return list;

template<typename U> static PyObject* seq_getslice(U *self, Py_ssize_t lo, Py_ssize_t hi):

    return toSequence<U>(self, lo, hi)

template<typename U> static int seq_set(U *self, Py_ssize_t n, PyObject* value):

    return self.array.set(n, value)

template<typename U> static int seq_setslice(U *self, Py_ssize_t lo, Py_ssize_t hi, PyObject* values):

    Py_ssize_t length = self.array.length

    if (values == NULL):
        raise ValueError("array size cannot change")

    if lo < 0: lo = length + lo
    if lo < 0: lo = 0
    elif lo > length: lo = length
    if hi < 0: hi = length + hi
    if hi < 0: hi = 0
    elif hi > length: hi = length
    if lo > hi: lo = hi

    PyObject* sequence = PySequence_Fast(values, "not a sequence");
    if (!sequence)
        return -1;

    Py_ssize_t size = PySequence_Fast_GET_SIZE(sequence);
    if size < 0:
        return -1;

    if size != hi - lo:
        raise ValueError("array size cannot change");

    for (Py_ssize_t i = lo; i < hi; i++):

        PyObject* value = PySequence_Fast_GET_ITEM(sequence, i - lo);

        if (value == NULL)
            return -1;

        if self.array.set(i, value) < 0:
            return -1;

    return 0;

template<typename T> static jclass initializeClass(bool getOnly):

    vm_env = env.get_vm_env() # jni.JNIEnv
    return vm_env.GetObjectClass(JArray<T>((Py_ssize_t) 0).this$);

template<typename T> static PyObject* cast_(PyTypeObject *type, PyObject* args, PyObject* kwds):

    PyObject *arg, *clsObj;

    if (!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if (!PyObject_TypeCheck(arg, &PY_TYPE(Object)))
        raise TypeError(arg);

    Class argCls = ((t_Object *) arg)->object.getClass()

    if not argCls.isArray():
        raise TypeError(arg);

    clsObj = PyObject_GetAttrString((PyObject *) type, "class_");
    if (!clsObj)
        return NULL;

    Class arrayCls = ((t_Class *) clsObj)->object;

    if not arrayCls.isAssignableFrom(argCls):
        raise TypeError(arg);

    return JArray<T>(((t_JObject *) arg)->object.this$).wrap()

template<typename T> static PyObject* wrapfn_(const jobject &object):

    return JArray<T>(object).wrap()

template<typename T> static PyObject* instance_(PyTypeObject *type, PyObject* args, PyObject* kwds):

    PyObject* arg, *clsObj;

    if (!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if (!PyObject_TypeCheck(arg, &PY_TYPE(Object)))
        return False

    Class argCls = ((t_Object *) arg)->object.getClass()

    if not argCls.isArray():
        return False

    clsObj = PyObject_GetAttrString((PyObject *) type, "class_");
    if (!clsObj)
        return NULL;

    Class arrayCls = ((t_Class *) clsObj)->object;

    return arrayCls.isAssignableFrom(argCls)

template<typename T> static PyObject* assignable_(PyTypeObject *type, PyObject* args, PyObject* kwds):

    return instance_<T>(type, args, kwds);

template< typename T, typename U = _t_JArray<T> > class jarray_type
{
public:

    PySequenceMethods seq_methods;
    PyTypeObject      type_object;

    class iterator_type
    {
        public:

        PyTypeObject type_object;

        iterator_type()
        {
            memset(&type_object, 0, sizeof(type_object));

            Py_REFCNT(&type_object) = 1;
            Py_TYPE(&type_object) = NULL;
            type_object.tp_basicsize = sizeof(_t_iterator<U>);
            type_object.tp_dealloc = (destructor) _t_iterator<U>::dealloc;
            type_object.tp_flags = Py_TPFLAGS_DEFAULT;
            type_object.tp_doc = "JArrayIterator<T> wrapper type";
            type_object.tp_iter = (getiterfunc) PyObject_SelfIter;
            type_object.tp_iternext = (iternextfunc) _t_iterator<U>::iternext;
        }

        def install(char *name, PyObject* module):
        {
            type_object.tp_name = name;

            if (PyType_Ready(&type_object) == 0):
                PyModule_AddObject(module, name, (PyObject *) &type_object);

            _t_iterator<U>::JArrayIterator = &type_object;
        }
    };

    iterator_type iterator_type_object;

    jarray_type()
    {
        memset(&seq_methods, 0, sizeof(seq_methods));
        memset(&type_object, 0, sizeof(type_object));

        static PyMethodDef methods[] =
        {
            { "cast_",       (PyCFunction) (PyObject* (*)(PyTypeObject*, PyObject*, PyObject*)) cast_<T>,       METH_VARARGS | METH_CLASS, NULL },
            { "instance_",   (PyCFunction) (PyObject* (*)(PyTypeObject*, PyObject*, PyObject*)) instance_<T>,   METH_VARARGS | METH_CLASS, NULL },
            { "assignable_", (PyCFunction) (PyObject* (*)(PyTypeObject*, PyObject*, PyObject*)) assignable_<T>, METH_VARARGS | METH_CLASS, NULL },
            { NULL, NULL, 0, NULL }
        };

        seq_methods.sq_length = (lenfunc) (Py_ssize_t (*)(U *)) seq_length<U>;
        seq_methods.sq_concat = (binaryfunc) (PyObject *(*)(U *, PyObject *)) seq_concat<U>;
        seq_methods.sq_repeat = (ssizeargfunc) (PyObject *(*)(U *, Py_ssize_t)) seq_repeat<U>;
        seq_methods.sq_item  = (ssizeargfunc) (PyObject *(*)(U *, Py_ssize_t)) seq_get<U>;
        seq_methods.sq_slice = (ssizessizeargfunc) (PyObject *(*)(U *, Py_ssize_t, Py_ssize_t)) seq_getslice<U>;
        seq_methods.sq_ass_item  = (ssizeobjargproc) (int (*)(U *, Py_ssize_t, PyObject *)) seq_set<U>;
        seq_methods.sq_ass_slice = (ssizessizeobjargproc) (int (*)(U *, Py_ssize_t, Py_ssize_t, PyObject *)) seq_setslice<U>;
        seq_methods.sq_contains = (objobjproc) (int (*)(U *, PyObject *)) seq_contains<U>;
        seq_methods.sq_inplace_concat = NULL;
        seq_methods.sq_inplace_repeat = NULL;

        Py_REFCNT(&type_object) = 1;
        type_object.tp_basicsize = sizeof(U);
        type_object.tp_dealloc = (destructor) (void (*)(U *)) dealloc<T,U>;
        type_object.tp_repr = (reprfunc) (PyObject *(*)(U *)) repr<U>;
        type_object.tp_as_sequence = &seq_methods;
        type_object.tp_str = (reprfunc) (PyObject *(*)(U *)) str<U>;
        type_object.tp_flags = Py_TPFLAGS_DEFAULT;
        type_object.tp_doc = "JArray<T> wrapper type";
        type_object.tp_richcompare = (richcmpfunc) (PyObject *(*)(U *, PyObject *, int)) richcompare<U>;
        type_object.tp_iter = (getiterfunc) (PyObject *(*)(U *)) iter<U>;
        type_object.tp_methods = methods;
        type_object.tp_base = &PY_TYPE(Object);
        type_object.tp_init = (initproc) (int (*)(U *, PyObject *, PyObject *)) init<T,U>;
        type_object.tp_new = (newfunc) _new;
    }

    def install(char *name, char *type_name, char *iterator_name, PyObject* module):

        type_object.tp_name = name;

        if (PyType_Ready(&type_object) == 0)
        {
            Py_INCREF((PyObject *) &type_object);
            PyDict_SetItemString(type_object.tp_dict, "class_",  make_descriptor(initializeClass<T>));
            PyDict_SetItemString(type_object.tp_dict, "wrapfn_", make_descriptor(wrapfn_<T>));

            PyModule_AddObject(module, name, (PyObject *) &type_object);
        }

        U::format = PyString_FromFormat("JArray<%s>%%s", type_name);
        iterator_type_object.install(iterator_name, module);

    static PyObject* _new(PyTypeObject *type, PyObject* args, PyObject* kwds):

        U *self = (U *) type->tp_alloc(type, 0);

        if (self)
            self.array = JArray<T>((jobject) NULL);

        return (PyObject *) self;
};

template<typename T> class _t_jobjectarray : public _t_JArray<T>
{
public:

    PyObject *(*wrapfn)(const T&);
};

template<> PyObject* get(_t_jobjectarray<jobject> *self, Py_ssize_t n):

    return self.array.get(n, self.wrapfn)

template<> PyObject* toSequence(_t_jobjectarray<jobject> *self):

    return self.array.toSequence(self.wrapfn)

template<> PyObject* toSequence(_t_jobjectarray<jobject> *self, Py_ssize_t lo, Py_ssize_t hi):

    return self.array.toSequence(lo, hi, self.wrapfn)

template<> int init< jobject,_t_jobjectarray<jobject> >(_t_jobjectarray<jobject> *self, PyObject *args, PyObject *kwds):

    PyObject *obj, *clsObj = NULL;
    PyObject *(*wrapfn)(const jobject &) = NULL;
    jclass jcls;

    if (!PyArg_ParseTuple(args, "O|O", &obj, &clsObj))
        return -1;

    if (clsObj == NULL)
        jcls = env.findClass(b"java/lang/Object")
    elif (PyObject_TypeCheck(clsObj, &PY_TYPE(Class)))
        jcls = (jclass) ((t_Class *) clsObj)->object.this$;
    elif isinstance(clsObj, type):

        if (PyType_IsSubtype((PyTypeObject *) clsObj, &PY_TYPE(JObject))):

            PyObject *cobj = PyObject_GetAttrString(clsObj, "wrapfn_");

            if (cobj == NULL)
                PyErr_Clear()
            else:
                wrapfn = (PyObject *(*)(const jobject &)) PyCapsule_GetPointer(cobj, "wrapfn");
                Py_DECREF(cobj);

            clsObj = PyObject_GetAttrString(clsObj, "class_");
            if (clsObj == NULL)
                return -1;

            jcls = (jclass) ((t_Class *) clsObj)->object.this$;
            Py_DECREF(clsObj);

        else:
            raise ValueError(clsObj)
    else:
        raise TypeError(clsObj)

    if PySequence_Check(obj):
        self.array = JArray<jobject>(jcls, obj);
    elif PyGen_Check(obj):
        PyObject *tuple = PyObject_CallFunctionObjArgs((PyObject *) &PyTuple_Type, obj, NULL);
        if (!tuple)
            return -1;
        self.array = JArray<jobject>(jcls, tuple);
    elif isinstance(obj, int):
        n = obj
        if n < 0:
            raise ValueError(obj)
        self.array = JArray<jobject>(jcls, n);
    else:
        raise TypeError(obj)

    self.wrapfn = wrapfn;

    return 0;

template<> jclass initializeClass<jobject>(bool getOnly):

    jclass jcls = env.findClass(b"java/lang/Object")
    vm_env = env.get_vm_env() # jni.JNIEnv
    return vm_env.GetObjectClass(JArray<jobject>(jcls, (Py_ssize_t) 0).this$);

template<> PyObject* cast_<jobject>(PyTypeObject *type, PyObject *args, PyObject *kwds):

    PyObject *arg, *clsArg = NULL;
    PyObject *(*wrapfn)(const jobject&) = NULL;
    jclass elementCls;

    if (!PyArg_ParseTuple(args, "O|O", &arg, &clsArg))
        return NULL;

    if (!PyObject_TypeCheck(arg, &PY_TYPE(Object))):
        raise TypeError(arg);

    Class argCls = ((t_Object *) arg)->object.getClass()

    if not argCls.isArray():
        raise TypeError(arg);

    if (clsArg != NULL):

        if not isinstance(clsArg, type):
            raise TypeError(clsArg)
        elif (!PyType_IsSubtype((PyTypeObject *) clsArg, &PY_TYPE(JObject)))
            raise ValueError(clsArg)

        PyObject *cobj = PyObject_GetAttrString(clsArg, "wrapfn_");

        if (cobj == NULL):
            PyErr_Clear()
        else:
            wrapfn = (PyObject *(*)(const jobject &)) PyCapsule_GetPointer(cobj, "wrapfn");
            Py_DECREF(cobj);

        clsArg = clsArg.class_
        if (clsArg == NULL)
            return NULL;

        elementCls = (jclass) ((t_Class *) clsArg)->object.this$;
        Py_DECREF(clsArg);

    else:
        elementCls = env.findClass(b"java/lang/Object")

    vm_env = env.get_vm_env() # jni.JNIEnv
    jobjectArray array = vm_env.NewObjectArray(0, elementCls)
    Class arrayCls = vm_env.GetObjectClass((jobject) array)

    if not arrayCls.isAssignableFrom(argCls):
        raise TypeError(arg);

    return JArray<jobject>(((t_JObject *) arg)->object.this$).wrap(wrapfn);

template<> PyObject* wrapfn_<jobject>(const jobject &object):

    PyObject *cobj = PyObject_GetAttrString((PyObject *) &PY_TYPE(Object), "wrapfn_");
    PyObject *(*wrapfn)(const jobject&) = NULL;

    if (cobj == NULL)
        PyErr_Clear()
    else:
        wrapfn = (PyObject *(*)(const jobject &)) PyCapsule_GetPointer(cobj, "wrapfn");
        Py_DECREF(cobj);

    return JArray<jobject>(object).wrap(wrapfn);

template<> PyObject* instance_<jobject>(PyTypeObject *type, PyObject *args, PyObject *kwds):

    PyObject *arg, *clsArg = NULL;
    jclass elementCls;

    if (!PyArg_ParseTuple(args, "O|O", &arg, &clsArg))
        return NULL;

    if (!PyObject_TypeCheck(arg, &PY_TYPE(Object)))
        return False

    Class argCls = ((t_Object *) arg)->object.getClass()

    if not argCls.isArray():
        return False

    if (clsArg != NULL):

        if not isinstance(clsArg, type):
            raise TypeError(clsArg)
        elif (!PyType_IsSubtype((PyTypeObject *) clsArg, &PY_TYPE(JObject)))
            raise ValueError(clsArg)

        clsArg = clsArg.class_
        if (clsArg == NULL)
            return NULL;

        elementCls = (jclass) ((t_Class *) clsArg)->object.this$;
        Py_DECREF(clsArg);

    else:
        elementCls = env.findClass(b"java/lang/Object")

    vm_env = env.get_vm_env() # jni.JNIEnv
    jobjectArray array = vm_env.NewObjectArray(0, elementCls)
    Class arrayCls = vm_env.GetObjectClass((jobject) array)

    return arrayCls.isAssignableFrom(argCls)

template<> PyObject* assignable_<jobject>(PyTypeObject *type, PyObject *args, PyObject *kwds):

    PyObject *arg, *clsArg = NULL;
    jclass elementCls;

    if (!PyArg_ParseTuple(args, "O|O", &arg, &clsArg))
        return NULL;

    if (!PyObject_TypeCheck(arg, &PY_TYPE(Object)))
        return False

    Class argCls = ((t_Object *) arg)->object.getClass()

    if not argCls.isArray():
        return False

    if (clsArg != NULL):

        if not isinstance(clsArg, type):
            raise TypeError(clsArg)
        elif (!PyType_IsSubtype((PyTypeObject *) clsArg, &PY_TYPE(JObject)))
            raise ValueError(clsArg)

        clsArg = clsArg.class_
        if (clsArg == NULL)
            return NULL;

        elementCls = (jclass) ((t_Class *) clsArg)->object.this$;
        Py_DECREF(clsArg);

    else:
        elementCls = env.findClass(b"java/lang/Object")

    vm_env = env.get_vm_env() # jni.JNIEnv
    jobjectArray array = vm_env.NewObjectArray(0, elementCls)
    Class arrayCls = vm_env.GetObjectClass((jobject) array)

    return argCls.isAssignableFrom(arrayCls)


template<typename T> PyTypeObject* _t_iterator<T>::JArrayIterator;
template<typename T> PyObject*     _t_JArray<T>::format;

static jarray_jobject  = jarray_type<jobject, _t_jobjectarray<jobject>>()

static jarray_jstring  = jarray_type<jstring>()
static jarray_jboolean = jarray_type<jboolean>()
static jarray_jbyte    = jarray_type<jbyte>()
static jarray_jchar    = jarray_type<jchar>()
static jarray_jdouble  = jarray_type<jdouble>()
static jarray_jfloat   = jarray_type<jfloat>()
static jarray_jint     = jarray_type<jint>()
static jarray_jlong    = jarray_type<jlong>()
static jarray_jshort   = jarray_type<jshort>()


PyObject* JArray<jobject>::wrap(PyObject* (*wrapfn)(const jobject&)):

    if (this$ == NULL)
        return None

    _t_jobjectarray<jobject>* obj = PyObject_New(_t_jobjectarray<jobject>, &jarray_jobject.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jobject>));
    obj.array = *this
    obj->wrapfn = wrapfn;

    return (PyObject*) obj

PyObject* JArray<jstring>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jstring>* obj = PyObject_New(_t_JArray<jstring>, &jarray_jstring.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jstring>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jboolean>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jboolean>* obj = PyObject_New(_t_JArray<jboolean>, &jarray_jboolean.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jboolean>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jbyte>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jbyte>* obj = PyObject_New(_t_JArray<jbyte>, &jarray_jbyte.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jbyte>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jchar>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jchar>* obj = PyObject_New(_t_JArray<jchar>, &jarray_jchar.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jchar>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jdouble>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jdouble>* obj = PyObject_New(_t_JArray<jdouble>, &jarray_jdouble.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jdouble>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jfloat>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jfloat>* obj = PyObject_New(_t_JArray<jfloat>, &jarray_jfloat.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jfloat>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jint>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jint>* obj = PyObject_New(_t_JArray<jint>, &jarray_jint.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jint>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jlong>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jlong>* obj = PyObject_New(_t_JArray<jlong>, &jarray_jlong.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jlong>));
    obj.array = *this

    return (PyObject*) obj

PyObject* JArray<jshort>::wrap():

    if (this$ == NULL)
        return None

    _t_JArray<jshort>* obj = PyObject_New(_t_JArray<jshort>, &jarray_jshort.type_object);
    memset((void *) &(obj->array), 0, sizeof(JArray<jshort>));
    obj.array = *this

    return (PyObject*) obj


PyObject *JArray_Type(PyObject *self, PyObject *arg):

    PyObject *type_name = NULL, *type;

    char const *name = NULL;
    if isinstance(arg, type):
        type_name = arg.__name__
    elif isinstance(arg, builtins.str):
        type_name = arg
    elif isinstance(arg, float):
        type_name = NULL;
        name = "double"
    else:
        arg_type  = (PyObject *) arg->ob_type;
        type_name = arg_type.__name__

    if type_name != NULL:
        name = PyUnicode_AsUTF8(type_name) if PY3 else PyString_AsString(type_name)
        if (!name):
            return NULL;

    if   name == "object": type = (PyObject *) &jarray_jobject.type_object;
    elif name == "string": type = (PyObject *) &jarray_jstring.type_object;
    elif name == "bool":   type = (PyObject *) &jarray_jboolean.type_object;
    elif name == "byte":   type = (PyObject *) &jarray_jbyte.type_object;
    elif name == "char":   type = (PyObject *) &jarray_jchar.type_object;
    elif name == "double": type = (PyObject *) &jarray_jdouble.type_object;
    elif name == "float":  type = (PyObject *) &jarray_jfloat.type_object;
    elif name == "int":    type = (PyObject *) &jarray_jint.type_object;
    elif name == "long":   type = (PyObject *) &jarray_jlong.type_object;
    elif name == "short":  type = (PyObject *) &jarray_jshort.type_object;
    else:
        raise ValueError(arg)

    return type

static PyObject *t_JArray_jbyte__get_string_(t_JArray<jbyte> *self, void *data)
{
    return self.array.to_string_()
}

#ifdef PY3
static PyObject *t_JArray_jbyte__get_bytes_(t_JArray<jbyte> *self, void *data)
{
    return self.array.to_bytes_()
}
#endif

static PyGetSetDef t_JArray_jbyte__fields[] = {
    { "string_", (getter) t_JArray_jbyte__get_string_, NULL, "", NULL },
#ifdef PY3
    { "bytes_",  (getter) t_JArray_jbyte__get_bytes_,  NULL, "", NULL },
#endif
    { NULL, NULL, NULL, NULL, NULL }
};


PyTypeObject *PY_TYPE(JArrayObject);
PyTypeObject *PY_TYPE(JArrayString);
PyTypeObject *PY_TYPE(JArrayBool);
PyTypeObject *PY_TYPE(JArrayByte);
PyTypeObject *PY_TYPE(JArrayChar);
PyTypeObject *PY_TYPE(JArrayDouble);
PyTypeObject *PY_TYPE(JArrayFloat);
PyTypeObject *PY_TYPE(JArrayInt);
PyTypeObject *PY_TYPE(JArrayLong);
PyTypeObject *PY_TYPE(JArrayShort);


void _install_jarray(PyObject *module)
{
    jarray_jobject.install("JArray_object", "object", "__JArray_object_iterator", module);
    PY_TYPE(JArrayObject) = &jarray_jobject.type_object;

    jarray_jstring.install("JArray_string", "string", "__JArray_string_iterator", module);
    PY_TYPE(JArrayString) = &jarray_jstring.type_object;

    jarray_jboolean.install("JArray_bool", "bool", "__JArray_bool_iterator", module);
    PY_TYPE(JArrayBool) = &jarray_jboolean.type_object;

    jarray_jbyte.type_object.tp_getset = t_JArray_jbyte__fields;
    jarray_jbyte.install("JArray_byte", "byte", "__JArray_byte_iterator", module);
    PY_TYPE(JArrayByte) = &jarray_jbyte.type_object;

    jarray_jchar.install("JArray_char", "char", "__JArray_char_iterator", module);
    PY_TYPE(JArrayChar) = &jarray_jchar.type_object;

    jarray_jdouble.install("JArray_double", "double", "__JArray_double_iterator", module);
    PY_TYPE(JArrayDouble) = &jarray_jdouble.type_object;

    jarray_jfloat.install("JArray_float", "float", "__JArray_float_iterator", module);
    PY_TYPE(JArrayFloat) = &jarray_jfloat.type_object;

    jarray_jint.install("JArray_int", "int", "__JArray_int_iterator", module);
    PY_TYPE(JArrayInt) = &jarray_jint.type_object;

    jarray_jlong.install("JArray_long", "long", "__JArray_long_iterator", module);
    PY_TYPE(JArrayLong) = &jarray_jlong.type_object;

    jarray_jshort.install("JArray_short", "short", "__JArray_short_iterator", module);
    PY_TYPE(JArrayShort) = &jarray_jshort.type_object;
}
