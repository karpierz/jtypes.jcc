
#include <jni.h>
#include <Python.h>
#include "structmember.h"

#include "java/lang/Object.h"
#include "java/lang/Class.h"
#include "functions.h"

using namespace java::lang;

/* FinalizerProxy */

PyTypeObject PY_TYPE(FinalizerClass) =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "jcc.FinalizerClass",                /* tp_name */
    PyType_Type.tp_basicsize,            /* tp_basicsize */
    (ternaryfunc) t_fc_call,             /* tp_call */
    Py_TPFLAGS_DEFAULT,                  /* tp_flags */
    "FinalizerClass",                    /* tp_doc */
    &PyType_Type,                        /* tp_base */
};

static PyMappingMethods t_fp_as_mapping = {
    (lenfunc)t_fp.map_length,            /* mp_length          */
    (binaryfunc)t_fp.map_get,            /* mp_subscript       */
    (objobjargproc)t_fp.map_set,         /* mp_ass_subscript   */
};

static PySequenceMethods t_fp_as_sequence = {
    (lenfunc)t_fp.seq_length,                 /* sq_length */
    (binaryfunc)t_fp.seq_concat,              /* sq_concat */
    (ssizeargfunc)t_fp.seq_repeat,            /* sq_repeat */
    (ssizeargfunc)t_fp.seq_get,               /* sq_item */
    (ssizessizeargfunc)t_fp.seq_getslice,     /* sq_slice */
    (ssizeobjargproc)t_fp.seq_set,            /* sq_ass_item */
    (ssizessizeobjargproc)t_fp.seq_setslice,  /* sq_ass_slice */
    (objobjproc)t_fp.seq_contains,            /* sq_contains */
    (binaryfunc)t_fp.seq_inplace_concat,      /* sq_inplace_concat */
    (ssizeargfunc)t_fp.seq_inplace_repeat,    /* sq_inplace_repeat */
};

PyTypeObject PY_TYPE(FinalizerProxy) =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "jcc.FinalizerProxy",                      /* tp_name */
    sizeof(t_fp),                              /* tp_basicsize */
    (destructor)t_fp.dealloc,                  /* tp_dealloc */
    (reprfunc)t_fp.repr,                       /* tp_repr */
    &t_fp.as_sequence,                         /* tp_as_sequence */
    &t_fp.as_mapping,                          /* tp_as_mapping */
    (getattrofunc)t_fp.getattro,               /* tp_getattro */
    (setattrofunc)t_fp.setattro,               /* tp_setattro */
    (Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC), /* tp_flags */
    "FinalizerProxy",                          /* tp_doc */
    (traverseproc)t_fp.traverse,               /* tp_traverse */
    (inquiry)t_fp.clear,                       /* tp_clear */
    (getiterfunc)t_fp.iter,                    /* tp_iter */
};

static def t_fc_call(PyObject* self, PyObject* args, PyObject* kwds): # PyObject*

    PyObject* obj = PyType_Type.tp_call(self, args, kwds);
    if (obj):
        t_fp* fp = (t_fp *) PY_TYPE(FinalizerProxy).tp_alloc(&PY_TYPE(FinalizerProxy), 0);
        fp->object = obj;      /* released by t_fp.clear() */
        obj = (PyObject*) fp;

    return obj

static def t_fp.dealloc(self):

    if self.object:
        ((t_JObject *) self.object)->object.weaken$();

    t_fp.clear(self);
    self.ob_type->tp_free((PyObject*) self);

static int t_fp.traverse(self, visitproc visit, void *arg):

    Py_VISIT(self.object);
    return 0;

static int t_fp.clear(self):

    Py_CLEAR(self.object);
    return 0;

static def t_fp.repr(self): # PyObject*

    return PyObject_Repr(self.object);

static def t_fp.iter(self): # PyObject*

    return PyObject_GetIter(self.object);

static def t_fp.getattro(self, PyObject* name): # PyObject*

    return PyObject_GetAttr(self.object, name);

static int t_fp.setattro(self, PyObject* name, PyObject* value):

    return PyObject_SetAttr(self.object, name, value);

static Py_ssize_t t_fp.map_length(self):

    return PyMapping_Size(self.object);

static def t_fp.map_get(self, PyObject* key): # PyObject*

    return PyObject_GetItem(self.object, key);

static int t_fp.map_set(self, PyObject* key, PyObject* value):

    if (value == NULL)
        return PyObject_DelItem(self.object, key);
    else:
        return PyObject_SetItem(self.object, key, value);

static Py_ssize_t t_fp.seq_length(self):

    return PySequence_Length(self.object);

static def t_fp.seq_get(self, Py_ssize_t n): # PyObject*

    return PySequence_GetItem(self.object, n);

static int t_fp.seq_contains(self, PyObject* value):

    return PySequence_Contains(self.object, value);

static def t_fp.seq_concat(self, PyObject* arg): # PyObject*

    return PySequence_Concat(self.object, arg);

static def t_fp.seq_repeat(self, Py_ssize_t n): # PyObject*

    return PySequence_Repeat(self.object, n);

static def t_fp.seq_getslice(self, Py_ssize_t low, Py_ssize_t high): # PyObject*

    return PySequence_GetSlice(self.object, low, high);

static int t_fp.seq_set(self, Py_ssize_t i, PyObject* value):

    return PySequence_SetItem(self.object, i, value);

static int t_fp.seq_setslice(self, Py_ssize_t low, Py_ssize_t high, PyObject* arg):

    return PySequence_SetSlice(self.object, low, high, arg);

static def t_fp.seq_inplace_concat(self, PyObject* arg): # PyObject*

    return PySequence_InPlaceConcat(self.object, arg);

static def t_fp.seq_inplace_repeat(self, Py_ssize_t n): # PyObject*

    return PySequence_InPlaceRepeat(self.object, n);

/* const variable descriptor */

class t_descriptor
{
public:
    PyObject_HEAD
    int flags;
    union
    {
        PyObject* value;
        getclassfn initializeClass;
    } access;
};

DESCRIPTOR_VALUE   = 0x0001
DESCRIPTOR_CLASS   = 0x0002
DESCRIPTOR_GETFN   = 0x0004
DESCRIPTOR_GENERIC = 0x0008

static PyMethodDef t_descriptor_methods[] = {
    { NULL, NULL, 0, NULL }
};

PyTypeObject PY_TYPE(ConstVariableDescriptor) =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "jcc.ConstVariableDescriptor",       /* tp_name */
    sizeof(t_descriptor),                /* tp_basicsize */
    (destructor)t_descriptor.dealloc,    /* tp_dealloc */
    Py_TPFLAGS_DEFAULT,                  /* tp_flags */
    "const variable descriptor",         /* tp_doc */
    t_descriptor_methods,                /* tp_methods */
    (descrgetfunc)t_descriptor.___get__, /* tp_descr_get */
};

static def t_descriptor.dealloc(self):

    if self.flags & DESCRIPTOR_VALUE:
        Py_DECREF(self.access.value);

    self.ob_type->tp_free((PyObject*) self);

def make_descriptor(PyTypeObject* value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = (PyObject*) value
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(getclassfn initializeClass): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.initializeClass = initializeClass
    self.flags                  = DESCRIPTOR_CLASS

    return self

def make_descriptor(getclassfn initializeClass, int generics): # ConstVariableDescriptor

    t_descriptor* self = (t_descriptor *) make_descriptor(initializeClass);
    if generics:
        self.flags |= DESCRIPTOR_GENERIC

    return self

def make_descriptor(PyObject* value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    if self:
        self.access.value = value;
        self.flags        = DESCRIPTOR_VALUE
    else
        Py_DECREF(value);

    return self

def make_descriptor(PyObject* (*wrapfn)(const jobject&)): # ConstVariableDescriptor

    return make_descriptor(PyCapsule_New((void *) wrapfn, "wrapfn", NULL));

def make_descriptor(boxfn fn): # ConstVariableDescriptor

    return make_descriptor(PyCapsule_New((void *) fn, "boxfn", NULL));

def make_descriptor(jboolean b): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = (b ? Py_True : Py_False);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jbyte value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyInt_FromLong(value);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jchar value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    Py_UNICODE pchar = (Py_UNICODE) value;
    self.access.value = PyUnicode_FromUnicode(&pchar, 1);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jdouble value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyFloat_FromDouble(value);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jfloat value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyFloat_FromDouble((double) value);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jint value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyInt_FromLong(value);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jlong value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyLong_FromLongLong((long long) value);
    self.flags        = DESCRIPTOR_VALUE

    return self

def make_descriptor(jshort value): # ConstVariableDescriptor

    self = ConstVariableDescriptor()
    self.access.value = PyInt_FromLong((short) value);
    self.flags        = DESCRIPTOR_VALUE

    return self

static def t_descriptor.___get__(self, PyObject* obj, PyObject* type): # PyObject*

    if self.flags & DESCRIPTOR_VALUE:
        return self.access.value

    if self.flags & DESCRIPTOR_CLASS:
#ifdef _java_generics
        if self.flags & DESCRIPTOR_GENERIC:
            return t_Class::wrap_Object(Class(env->getClass(self.access.initializeClass)), (PyTypeObject *) type);
        else:
#endif
            return t_Class::wrap_Object(Class(env->getClass(self.access.initializeClass)));

    return None
