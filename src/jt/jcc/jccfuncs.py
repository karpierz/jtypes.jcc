#ifdef PYTHON

PyMethodDef jcc_funcs[] =
{
    { "initVM",               __initialize__,       METH_VARARGS | METH_KEYWORDS, NULL },
    { getVMEnv,             ,
    { findClass,            ,
    { makeInterface,        ,
    { makeClass,            ,
    { "_set_exception_types", _set_exception_types, METH_VARARGS, NULL },
    { "_set_function_self",   _set_function_self,   METH_VARARGS, NULL },
    { "JArray",               JArray_Type,          METH_O,       NULL },
};


@annotate(JCCEnv)
def getVMEnv():

    if env.vm is None:
        return None

    jccenv = JCCEnv()
    jccenv.env = env

    return jccenv


@annotate(className=str)
def findClass(className):

    try:
        jcls = env->findClass(className)
        return t_Class::wrap_Object(Class(jcls)) if jcls else None
    catch ( int e ):
        if e == _EXC_PYTHON:
            return NULL;
        elif e == _EXC_JAVA:
            return PyErr_SetJavaError()
        else:
            raise


_interface_bytes = bytearray(
    b"\xca\xfe\xba\xbe",  # magic number: 0xcafebabe
    b"\x00\x00\x00\x32",  # version 50.0
    b"\x00\x07",          # constant pool max index: 6
    b"\x07\x00\x04",      # 1: class name at 4
    b"\x07\x00\x05",      # 2: class name at 5
    b"\x07\x00\x06",      # 3: class name at 6
    b"\x01\x00\x00",      # 4: empty string
    b"\x01\x00\x10",      # 5: 16-byte string: java/lang/Object
    b"java/lang/Object",
    b"\x01\x00\x00",      # 6: empty string
    b"\x06\x01",          # public abstract interface
    b"\x00\x01",          # this class at 1
    b"\x00\x02",          # superclass at 2
    b"\x00\x01",          # 1 interface
    b"\x00\x03",          # interface at 3
    b"\x00\x00",          # 0 fields
    b"\x00\x00",          # 0 methods
    b"\x00\x00"           # 0 attributes
)

_class_bytes = bytearray(
    b"\xca\xfe\xba\xbe",      # magic number: 0xcafebabe
    b"\x00\x00\x00\x32",      # version 50.0
    b"\x00\x0c",              # constant pool max index: 11
    b"\x0a\x00\x03\x00\x08",  # 1: method for class 3 at 8
    b"\x07\x00\x09",          # 2: class name at 9
    b"\x07\x00\x0a",          # 3: class name at 10
    b"\x07\x00\x0b",          # 4: class name at 11
    b"\x01\x00\x06",          # 5: 6-byte string: <init>
    b"<init>",
    b"\x01\x00\x03()V",       # 6: 3-byte string: ()V
    b"\x01\x00\x04",          # 7: 4-byte string: Code
    b"Code",
    b"\x0c\x00\x05\x00\x06",  # 8: name at 5, signature at 6
    b"\x01\x00\x00",          # 9: empty string
    b"\x01\x00\x00",          # 10: empty string
    b"\x01\x00\x00",          # 11: empty string
    b"\x00\x21",              # super public
    b"\x00\x02",              # this class at 2
    b"\x00\x03",              # superclass at 3
    b"\x00\x01",              # 1 interface
    b"\x00\x04",              # interface at 4
    b"\x00\x00",              # 0 fields
    b"\x00\x01",              # 1 method
    b"\x00\x01\x00\x05",      # public, name at 5
    b"\x00\x06\x00\x01",      # signature at 6, 1 attribute
    b"\x00\x07",              # attribute name at 7: Code
    b"\x00\x00\x00\x11",      # 17 bytes past 6 attribute bytes
    b"\x00\x01",              # max stack: 1
    b"\x00\x01",              # max locals: 1
    b"\x00\x00\x00\x05",      # code length: 5
    b"\x2a\xb7\x00\x01\xb1",  # actual code bytes
    b"\x00\x00",              # 0 method exceptions
    b"\x00\x00",              # 0 method attributes
    b"\x00\x00",              # 0 attributes
)


PyObject* makeInterface(name, extName):

    # make an empty interface that extends an interface

    char *name, *extName;
    int name_len, extName_len;

    if (!PyArg_ParseTuple(args, "s#s#",
                          &name, &name_len, &extName, &extName_len))
        return NULL;
    
    bytes_len = sizeof(_interface_bytes)
    const int len       = bytes_len + name_len + extName_len
    char*     buf       = (char *) malloc(len)
    if (buf == NULL)
        raise MemoryError()

    name_pos    = 22
    extName_pos = 44

    memcpy(buf,                                        _interface_bytes,               name_pos)
    memcpy(buf + name_pos    + name_len,               _interface_bytes + name_pos,    extName_pos - name_pos)
    memcpy(buf + extName_pos + name_len + extName_len, _interface_bytes + extName_pos, bytes_len   - extName_pos)
    extName_pos += name_len;

    *((unsigned short *) (buf + name_pos - 2)) = htons(name_len)
    memcpy(buf + name_pos, name, name_len)
 
    *((unsigned short *) (buf + extName_pos - 2)) = htons(extName_len)
    memcpy(buf + extName_pos, extName, extName_len)

    jenv  = env.get_vm_env() # jni.JNIEnv
    jucls = jenv.FindClass(b"java/net/URLClassLoader")
    jmid  = jenv.GetStaticMethodID(jucls, b"getSystemClassLoader", b"()Ljava/lang/ClassLoader;")
    classLoader = jenv.CallStaticObjectMethod(jucls, jmid) # jni.jobject
    jcls = jenv.DefineClass(name, classLoader, (const jbyte *) buf, len)
    if not jcls:
        return PyErr_SetJavaError()
        
    return t_Class::wrap_Object(Class(jcls))


PyObject* makeClass(name, extName, implName):

    # make an empty class that extends a class and implements an interface

    char *name, *extName, *implName;
    int name_len, extName_len, implName_len;

    if (!PyArg_ParseTuple(args, "s#s#s#",
                          &name, &name_len, &extName, &extName_len, &implName, &implName_len))
        return NULL;

    bytes_len = sizeof(_class_bytes)
    const int len       = bytes_len + name_len + extName_len + implName_len;
    char*     buf       = (char *) malloc(len)
    if (buf == NULL)
        raise MemoryError()

    name_pos     = 54
    extName_pos  = 57
    implName_pos = 60

    memcpy(buf,                                                        _class_bytes,                name_pos)
    memcpy(buf + name_pos     + name_len,                              _class_bytes + name_pos,     extName_pos - name_pos)
    memcpy(buf + extName_pos  + name_len + extName_len,                _class_bytes + extName_pos,  bytes_len   - extName_pos)
    memcpy(buf + implName_pos + name_len + extName_len + implName_len, _class_bytes + implName_pos, bytes_len   - implName_pos)

    extName_pos  += name_len
    implName_pos += name_len + extName_len

    *((unsigned short *) (buf + name_pos - 2)) = htons(name_len)
    memcpy(buf + name_pos, name, name_len)

    *((unsigned short *) (buf + extName_pos - 2)) = htons(extName_len)
    memcpy(buf + extName_pos, extName, extName_len)

    *((unsigned short *) (buf + implName_pos - 2)) = htons(implName_len)
    memcpy(buf + implName_pos, implName, implName_len)

    jenv  = env.get_vm_env() # jni.JNIEnv
    jucls = jenv.FindClass(b"java/net/URLClassLoader")
    jmid  = jenv.GetStaticMethodID(jucls, b"getSystemClassLoader", b"()Ljava/lang/ClassLoader;")
    classLoader = jenv.CallStaticObjectMethod(jucls, jmid) # jni.jobject
    jcls = jenv.DefineClass(name, classLoader, (const jbyte *) buf, len)
    if not jcls:
        return PyErr_SetJavaError()

    return t_Class::wrap_Object(Class(jcls))


@annotate(JavaError=object, InvalidArgsError=object)
def _set_exception_types(JavaError, InvalidArgsError):

    global PyExc_JavaError
    global PyExc_InvalidArgsError

    PyExc_JavaError        = JavaError
    PyExc_InvalidArgsError = InvalidArgsError


@annotate(object=object, module=object)
def _set_function_self(object, module):

    if (!PyCFunction_Check(object)):
        raise TypeError(object)

    PyCFunctionObject *cfn = (PyCFunctionObject *) object;
    cfn.m_self = module


#endif
