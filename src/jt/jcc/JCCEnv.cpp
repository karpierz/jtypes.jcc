
#include <stdarg.h>
#if defined(_MSC_VER) || defined(__WIN32)
#define _DLL_IMPORT __declspec(dllimport)
#define _DLL_EXPORT __declspec(dllexport)
#include <windows.h>
#undef MAX_PRIORITY
#undef MIN_PRIORITY
#else
#include <pthread.h>
#define _DLL_IMPORT
#define _DLL_EXPORT
#endif

#ifdef __SUNPRO_CC
#undef DEFAULT_TYPE
#endif

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#include <map>

#ifdef PYTHON
#include <Python.h>
#endif

#ifdef PY3
#define PyInt_Check          PyLong_Check
#define PyInt_CheckExact     PyLong_CheckExact
#define PyInt_FromLong       PyLong_FromLong
#define PyInt_AsLong         PyLong_AsLong
#define PyString_Format      PyUnicode_Format
#define PyString_FromFormat  PyUnicode_FromFormat


/* Define some aliases for the removed PyInt_* functions */
#define PyInt_FromSize_t             PyLong_FromSize_t
#define PyInt_FromSsize_t            PyLong_FromSsize_t
#define PyInt_AsSsize_t              PyLong_AsSsize_t
#define PyInt_AsUnsignedLongMask     PyLong_AsUnsignedLongMask
#define PyInt_AsUnsignedLongLongMask PyLong_AsUnsignedLongLongMask
#define PyInt_AS_LONG                PyLong_AS_LONG
#define PyNumber_Int                 PyNumber_Long

#define PyString_FromString         PyUnicode_FromString
#define PyString_FromStringAndSize  PyUnicode_FromStringAndSize
#endif

#undef EOF

class JCCEnv;

#if defined(_MSC_VER) || defined(__WIN32)

#ifdef _jcc_shared
_DLL_IMPORT extern JCCEnv *env;
_DLL_IMPORT extern DWORD VM_ENV;
#else
_DLL_EXPORT extern JCCEnv *env;
_DLL_EXPORT extern DWORD VM_ENV;
#endif

#else

extern JCCEnv *env;

#endif

#define _EXC_PYTHON ((int) 0)
#define _EXC_JAVA   ((int) 1)

typedef jclass (*getclassfn)(bool);

class countedRef {
public:
    jobject global;
    int count;
};

class JCCEnv(object): # _DLL_EXPORT

protected:

    jclass _sys, _obj, _thr;
    jclass _boo, _byt, _cha, _dou, _flo, _int, _lon, _sho;
    jmethodID *_mids;

    enum
    {
        mid_sys_identityHashCode,
        mid_sys_setProperty,
        mid_sys_getProperty,
        mid_obj_toString,
        mid_obj_hashCode,
        mid_obj_getClass,
        mid_iterator,
        mid_iterator_next,
        mid_enumeration_nextElement,
        mid_Boolean_booleanValue,
        mid_Byte_byteValue,
        mid_Character_charValue,
        mid_Double_doubleValue,
        mid_Float_floatValue,
        mid_Integer_intValue,
        mid_Long_longValue,
        mid_Short_shortValue,
        mid_Boolean_init,
        mid_Byte_init,
        mid_Character_init,
        mid_Double_init,
        mid_Float_init,
        mid_Integer_init,
        mid_Long_init,
        mid_Short_init,
        max_mid
    };

public:

    JavaVM *vm;
    std::multimap<int, countedRef> refs;
    int handlers;

    explicit JCCEnv(JavaVM *vm, JNIEnv *env);

    int attachCurrentThread(char *name, int asDaemon);

    void set_vm(JavaVM *vm, JNIEnv *vm_env);

    JNIEnv* get_vm_env() const
    void    set_vm_env(JNIEnv *vm_env);

    jobject newGlobalRef   (jobject obj, int id);
    jobject deleteGlobalRef(jobject obj, int id);

    void handleException() const;

    jint    getJNIVersion() const;
    jstring getJavaVersion() const;

    jclass findClass(const char *className) const;
    jboolean isInstanceOf(jobject obj, getclassfn initializeClass) const;

    void registerNatives(jclass cls, JNINativeMethod *methods, int n) const;

    jobject iterator(jobject obj) const;
    jobject iteratorNext(jobject obj) const;
    jobject enumerationNext(jobject obj) const;

    jclass getClass(getclassfn initializeClass) const;
    jobject newObject(getclassfn initializeClass, jmethodID **mids, int m, ...);

    jobjectArray newObjectArray(jclass cls, int size);
    int getArrayLength(jarray a) const;
    jobject getObjectArrayElement(jobjectArray a, int n) const;
    void    setObjectArrayElement(jobjectArray a, int n, jobject obj) const;

    def getFieldID       (self, jcls: jni.jclass, const char* name, signature: bytes) -> jni.jfieldID 
    def getMethodID      (self, jcls: jni.jclass, const char* name, signature: bytes) -> jni.jmethodID
    def getStaticMethodID(self, jcls: jni.jclass, const char* name, signature: bytes) -> jni.jmethodID

    def getStaticObjectField (self, jcls: jni.jclass, const char* name, signature: bytes) -> jni.jobject
    def getStaticBooleanField(self, jcls: jni.jclass, const char* name) -> jni.jboolean
    def getStaticByteField   (self, jcls: jni.jclass, const char* name) -> jni.jbyte   
    def getStaticCharField   (self, jcls: jni.jclass, const char* name) -> jni.jchar   
    def getStaticDoubleField (self, jcls: jni.jclass, const char* name) -> jni.jdouble 
    def getStaticFloatField  (self, jcls: jni.jclass, const char* name) -> jni.jfloat  
    def getStaticIntField    (self, jcls: jni.jclass, const char* name) -> jni.jint    
    def getStaticLongField   (self, jcls: jni.jclass, const char* name) -> jni.jlong   
    def getStaticShortField  (self, jcls: jni.jclass, const char* name) -> jni.jshort  

    def getObjectField (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jobject 
    def getBooleanField(self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jboolean
    def getByteField   (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jbyte   
    def getCharField   (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jchar   
    def getDoubleField (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jdouble 
    def getFloatField  (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jfloat  
    def getIntField    (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jint    
    def getLongField   (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jlong   
    def getShortField  (self, this: jni.jobject, jfid: jni.jfieldID) -> jni.jshort  

    def setObjectField (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jobject)
    def setBooleanField(self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jboolean)
    def setByteField   (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jbyte)
    def setCharField   (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jchar)
    def setDoubleField (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jdouble)
    def setFloatField  (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jfloat)
    def setIntField    (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jint)
    def setLongField   (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jlong)
    def setShortField  (self, this: jni.jobject, jfid: jni.jfieldID, val: jni.jshort)
 
    def callObjectMethod (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jobject 
    def callBooleanMethod(self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jboolean
    def callByteMethod   (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jbyte   
    def callCharMethod   (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jchar   
    def callDoubleMethod (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jdouble 
    def callFloatMethod  (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jfloat  
    def callIntMethod    (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jint    
    def callLongMethod   (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jlong   
    def callShortMethod  (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.jshort  
    def callVoidMethod   (self, this: jni.jobject, jmid: jni.jmethodID, ...) -> jni.void    
    
    def callNonvirtualObjectMethod (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jobject 
    def callNonvirtualBooleanMethod(self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jboolean
    def callNonvirtualByteMethod   (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jbyte   
    def callNonvirtualCharMethod   (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jchar   
    def callNonvirtualDoubleMethod (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jdouble 
    def callNonvirtualFloatMethod  (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jfloat  
    def callNonvirtualIntMethod    (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jint    
    def callNonvirtualLongMethod   (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jlong   
    def callNonvirtualShortMethod  (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jshort  
    def callNonvirtualVoidMethod   (self, jobj: jni.jobject, cls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.void    

    def callStaticObjectMethod (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jobject 
    def callStaticBooleanMethod(self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jboolean
    def callStaticByteMethod   (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jbyte   
    def callStaticCharMethod   (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jchar   
    def callStaticDoubleMethod (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jdouble 
    def callStaticFloatMethod  (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jfloat  
    def callStaticIntMethod    (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jint    
    def callStaticLongMethod   (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jlong   
    def callStaticShortMethod  (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.jshort  
    def callStaticVoidMethod   (self, jcls: jni.jclass, jmid: jni.jmethodID, ...) -> jni.void    
 
    def booleanValue(self, jobj: jni.jobject) -> jni.jboolean
    def byteValue   (self, jobj: jni.jobject) -> jni.jbyte   
    def charValue   (self, jobj: jni.jobject) -> jni.jchar   
    def doubleValue (self, jobj: jni.jobject) -> jni.jdouble 
    def floatValue  (self, jobj: jni.jobject) -> jni.jfloat  
    def intValue    (self, jobj: jni.jobject) -> jni.jint    
    def longValue   (self, jobj: jni.jobject) -> jni.jlong   
    def shortValue  (self, jobj: jni.jobject) -> jni.jshort  

    def boxBoolean(self, val: jni.jboolean) -> jni.jobject
    def boxByte   (self, val: jni.jbyte)    -> jni.jobject
    def boxChar   (self, val: jni.jchar)    -> jni.jobject
    def boxDouble (self, val: jni.jdouble)  -> jni.jobject
    def boxFloat  (self, val: jni.jfloat)   -> jni.jobject
    def boxInteger(self, val: jni.jint)     -> jni.jobject
    def boxLong   (self, val: jni.jlong)    -> jni.jobject
    def boxShort  (self, val: jni.jshort)   -> jni.jobject

    int id(jobj: jni.jobject):
 
        return get_vm_env()->CallStaticIntMethod(_sys, _mids[mid_sys_identityHashCode], jobj) if jobj else 0

    int hash(jobj: jni.jobject):

        return get_vm_env()->CallIntMethod(jobj, _mids[mid_obj_hashCode]) if jobj else 0
        
    void  setClassPath(const char *classPath);
    char* getClassPath();

    jstring fromUTF(const char *bytes);
    char*   toUTF(jstring str);
    char*   toString(jobject obj);
    char*   getClassName(jobject obj);

#ifdef PYTHON

    jclass getPythonExceptionClass():

        return _thr

    bool restorePythonException(jthrowable throwable) const;

    jstring   fromPyString(PyObject *object) const;
    PyObject* fromJString(jstring js);
    void finalizeObject(JNIEnv *jenv, PyObject *obj);

#endif

    inline int isSame(jobject o1, jobject o2) const
    {
        return o1 == o2 || get_vm_env()->IsSameObject(o1, o2);
    }
};

#include <map>

#include <stdlib.h>
#include <string.h>
#include <jni.h>

#include "JCCEnv.h"

#if defined(_MSC_VER) || defined(__WIN32)
_DLL_EXPORT DWORD VM_ENV = 0;
#else
pthread_key_t JCCEnv::VM_ENV = (pthread_key_t) NULL;
#endif

#if defined(_MSC_VER) || defined(__WIN32)

static CRITICAL_SECTION *mutex = NULL;

class lock
{
public:

    lock()
    {
        EnterCriticalSection(mutex);
    }

    virtual ~lock()
    {
        LeaveCriticalSection(mutex);
    }
};

#else

static pthread_mutex_t *mutex = NULL;

class lock
{
public:

    lock()
    {
        pthread_mutex_lock(mutex);
    }

    virtual ~lock()
    {
        pthread_mutex_unlock(mutex);
    }
};

#endif

    @annotate(jvm=jni.JavaVM, jenv=jni.JNIEnv)
    JCCEnv::JCCEnv(jvm, jenv):

        #if defined(_MSC_VER) || defined(__WIN32):

        if (!mutex):
            mutex = new CRITICAL_SECTION();
            InitializeCriticalSection(mutex);  # recursive by default

        #else

        if (!mutex):
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            mutex = new pthread_mutex_t();
            pthread_mutex_init(mutex, &attr);

        #endif

        if (jvm):
            this->set_vm(jvm, jenv)
        else
            this->vm = NULL

    int attachCurrentThread(self, char *name=NULL, asDaemon=False):

        JNIEnv* jenv = NULL;
        JavaVMAttachArgs attach = { jni.JNI_VERSION_1_6, name, NULL };
        if asDaemon:
            result = self.vm.AttachCurrentThreadAsDaemon((void **) &jenv, &attach)
        else
            result = self.vm.AttachCurrentThread((void **) &jenv, &attach)
        self.set_vm_env(jenv)
        return result

    def set_vm(self, JavaVM *vm, JNIEnv* jenv):

        self.vm = vm
        self.set_vm_env(jenv)

        _mids = new jmethodID[max_mid];

        _sys = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/System")), jni.jclass)
        _obj = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Object")), jni.jclass)
        #ifdef _jcc_lib
        _thr = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"org/apache/jcc/PythonException")), jni.jclass)
        #else
        _thr = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/RuntimeException")), jni.jclass)
        #endif

        _boo = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Boolean")),   jni.jclass)
        _byt = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Byte")),      jni.jclass)
        _cha = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Character")), jni.jclass)
        _dou = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Double")),    jni.jclass)
        _flo = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Float")),     jni.jclass)
        _int = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Integer")),   jni.jclass)
        _lon = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Long")),      jni.jclass)
        _sho = jni.cast(jenv.NewGlobalRef(jenv.FindClass(b"java/lang/Short")),     jni.jclass)

        _mids[mid_sys_identityHashCode] = jenv.GetStaticMethodID(_sys, b"identityHashCode", b"(Ljava/lang/Object;)I")
        _mids[mid_sys_setProperty]      = jenv.GetStaticMethodID(_sys, b"setProperty",      b"(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;")
        _mids[mid_sys_getProperty]      = jenv.GetStaticMethodID(_sys, b"getProperty",      b"(Ljava/lang/String;)Ljava/lang/String;")
        _mids[mid_obj_toString]         = jenv.GetMethodID(_obj,       b"toString",         b"()Ljava/lang/String;")
        _mids[mid_obj_hashCode]         = jenv.GetMethodID(_obj,       b"hashCode",         b"()I")
        _mids[mid_obj_getClass]         = jenv.GetMethodID(_obj,       b"getClass",         b"()Ljava/lang/Class;")

        _mids[mid_iterator]                = jenv.GetMethodID(jenv.FindClass(b"java/lang/Iterable"),    b"iterator",    b"()Ljava/util/Iterator;")
        _mids[mid_iterator_next]           = jenv.GetMethodID(jenv.FindClass(b"java/util/Iterator"),    b"next",        b"()Ljava/lang/Object;")
        _mids[mid_enumeration_nextElement] = jenv.GetMethodID(jenv.FindClass(b"java/util/Enumeration"), b"nextElement", b"()Ljava/lang/Object;")

        _mids[mid_Boolean_booleanValue] = jenv.GetMethodID(_boo, b"booleanValue", b"()Z")
        _mids[mid_Byte_byteValue]       = jenv.GetMethodID(_byt, b"byteValue",    b"()B")
        _mids[mid_Character_charValue]  = jenv.GetMethodID(_cha, b"charValue",    b"()C")
        _mids[mid_Double_doubleValue]   = jenv.GetMethodID(_dou, b"doubleValue",  b"()D")
        _mids[mid_Float_floatValue]     = jenv.GetMethodID(_flo, b"floatValue",   b"()F")
        _mids[mid_Integer_intValue]     = jenv.GetMethodID(_int, b"intValue",     b"()I")
        _mids[mid_Long_longValue]       = jenv.GetMethodID(_lon, b"longValue",    b"()J")
        _mids[mid_Short_shortValue]     = jenv.GetMethodID(_sho, b"shortValue",   b"()S")
 
        _mids[mid_Boolean_init]   = jenv.GetMethodID(_boo, b"<init>", b"(Z)V")
        _mids[mid_Byte_init]      = jenv.GetMethodID(_byt, b"<init>", b"(B)V")
        _mids[mid_Character_init] = jenv.GetMethodID(_cha, b"<init>", b"(C)V")
        _mids[mid_Double_init]    = jenv.GetMethodID(_dou, b"<init>", b"(D)V")
        _mids[mid_Float_init]     = jenv.GetMethodID(_flo, b"<init>", b"(F)V")
        _mids[mid_Integer_init]   = jenv.GetMethodID(_int, b"<init>", b"(I)V")
        _mids[mid_Long_init]      = jenv.GetMethodID(_lon, b"<init>", b"(J)V")
        _mids[mid_Short_init]     = jenv.GetMethodID(_sho, b"<init>", b"(S)V")


#if not (defined(_MSC_VER) || defined(__WIN32)):
    static pthread_key_t VM_ENV;
#endif

    def get_vm_env(self): # JNIEnv

        #if defined(_MSC_VER) || defined(__WIN32)
            return (JNIEnv *) TlsGetValue(VM_ENV);
        #else
            return (JNIEnv *) pthread_getspecific(VM_ENV);
        #endif

    def set_vm_env(self, JNIEnv* jenv):

        #if defined(_MSC_VER) || defined(__WIN32):
            if (!VM_ENV)
                VM_ENV = TlsAlloc();
            TlsSetValue(VM_ENV, (LPVOID) jenv);
        #else
            if (!VM_ENV)
                pthread_key_create(&VM_ENV, NULL);
            pthread_setspecific(VM_ENV, (void *) jenv);
        #endif

    def __enter__(self):

        return self.get_vm_env()

    def __exit__(self, exc_type, exc, exc_tb):

        del exc_type, exc_tb
        if exc: self.handleException(exc)
        return True

    def handleException(self, exc):

        jenv = self.get_vm_env()

        jexc = jenv.ExceptionOccurred()
        if not jexc:
            if exc: raise exc
            return

        if not env->handlers:
            jenv.ExceptionDescribe()

        #ifdef PYTHON
            with PythonGIL():
                if (PyErr_Occurred()):
                    # _thr is PythonException ifdef _jcc_lib (shared mode)
                    # if not shared mode, _thr is RuntimeException
                    jobject cls = (jobject) jenv.GetObjectClass(jexc)
                    if jenv.IsSameObject(cls, _thr):
                        #ifndef _jcc_lib
                            # PythonException class is not available without shared mode.
                            # Python exception information thus gets lost and exception
                            # is reported via plain Java RuntimeException.
                            PyErr_Clear()
                            throw _EXC_JAVA;
                        #else
                            throw _EXC_PYTHON;
                        #endif
        #endif

        throw _EXC_JAVA;

    jint getJNIVersion(self):

        with self as jenv:
            return jenv.GetVersion()

    jstring getJavaVersion(self):
 
        with self as jenv:
            return jni.cast(self.callStaticObjectMethod(_sys, _mids[mid_sys_getProperty],
                                                        jenv.NewStringUTF(b"java.version")),
                                                        jni.jstring)

    jni.jobject newGlobalRef(self, jobject obj, int id):

        if not (obj)
            return NULL;

        if (id):

            lock locked;

            for ( std::multimap<int, countedRef>::iterator
                  iter = refs.find(id); iter != refs.end(); iter++ ):

                if (iter->first != id)
                    break;
                if isSame(obj, iter->second.global):
                    # If it's in the table but not the same reference,
                    # it must be a local reference and must be deleted.
                    if (obj != iter->second.global):
                        jenv = self.get_vm_env()
                        jenv.DeleteLocalRef(obj);

                    iter->second.count += 1;
                    return iter->second.global;

            jenv = self.get_vm_env()

            countedRef ref;
            ref.global = jenv.NewGlobalRef(obj);
            ref.count = 1;
            refs.insert(std::pair<const int, countedRef>(id, ref));
            jenv.DeleteLocalRef(obj);
            return ref.global;

        else: # zero when weak global ref is desired

            jenv = self.get_vm_env()
            return (jobject) jenv.NewWeakGlobalRef(obj)

        return NULL;

    jni.jobject deleteGlobalRef(self, jobject obj, int id):

        if not (obj)
            return NULL;

        if (id):

            lock locked;

            for ( std::multimap<int, countedRef>::iterator
                  iter = refs.find(id); iter != refs.end(); iter++ ):

                if (iter->first != id)
                    break;
                if isSame(obj, iter->second.global):

                    if (iter->second.count == 1):

                        jenv = self.get_vm_env()
                        if (!jenv):
                            # Python's cyclic garbage collector may remove
                            # an object inside a thread that is not attached
                            # to the JVM. This makes sure the JVM doesn't
                            # segfault.
                            self.attachCurrentThread(NULL, 0)
                            jenv = self.get_vm_env()

                        jenv.DeleteGlobalRef(iter->second.global);
                        refs.erase(iter);

                    else:
                        iter->second.count -= 1;

                    return NULL;

            printf("deleting non-existent ref: 0x%x\n", id);

        else: # zero when obj is weak global ref

            jenv = self.get_vm_env()
            jenv.DeleteWeakGlobalRef((jweak) obj);

        return NULL;

    jni.jobject iterator(self, jobject obj):

        return self.callObjectMethod(obj, _mids[mid_iterator]);

    jni.jobject iteratorNext(self, jobject obj):

        return self.callObjectMethod(obj, _mids[mid_iterator_next]);

    jni.jobject enumerationNext(self, jobject obj):

        return self.callObjectMethod(obj, _mids[mid_enumeration_nextElement]);

    def registerNatives(self, jclass cls, JNINativeMethod *methods, int n):

        with self as jenv:
            jenv.RegisterNatives(cls, methods, n);

jclass JCCEnv::getClass(getclassfn initializeClass):
{
    jclass jcls = (*initializeClass)(true);
    if not jcls:
        lock locked;
        jcls = (*initializeClass)(false);
    
    return jcls
}

    jni.jclass findClass(self, const char* className):

        if not (vm):
            #ifdef PYTHON
                with PythonGIL():
                    raise RuntimeError("initVM() must be called first")
                    throw _EXC_PYTHON;
            #else
                throw _EXC_JAVA;
            #endif

        jenv = self.get_vm_env()
        if not (jenv):
            #ifdef PYTHON
                with PythonGIL():
                    raise RuntimeError("attachCurrentThread() must be called first")
                    throw _EXC_PYTHON;
            #else
                throw _EXC_JAVA;
            #endif

        jcls = jenv.FindClass(className)
        self.handleException()
        return jcls

    jni.jboolean isInstanceOf(self, jobject obj, getclassfn initializeClass):

        with self as jenv:
            return jenv.IsInstanceOf(obj, getClass(initializeClass));

    jni.jobject newObject(self, getclassfn initializeClass, jmethodID **mids, int m, ...):

        jclass cls = getClass(initializeClass);

        with self as jenv:
            if not (jenv)
                #ifdef PYTHON
                    with PythonGIL():
                        raise RuntimeError("attachCurrentThread() must be called first")
                        throw _EXC_PYTHON;
                #else
                    throw _EXC_JAVA;
                #endif

            va_start(ap, m);
            jobj = jenv.NewObjectV(cls, (*mids)[m], ap);
            self.handleException()
            return jobj


    jni.jobjectArray newObjectArray(self, jclass cls, int size):

        with self as jenv:
            return jenv.NewObjectArray(size, cls)

    int getArrayLength(self, jarray array):

        with self as jenv:
            return jenv.GetArrayLength(array);

    jni.jobject getObjectArrayElement(self, jobjectArray array, int n):

        with self as jenv:
            return jenv.GetObjectArrayElement(array, n)

    def setObjectArrayElement(self, jobjectArray array, int n, jobject obj):

        with self as jenv:
            jenv.SetObjectArrayElement(array, n, obj)
            

    @annotate(jni.jobject, this=jni.jobject, jmid=jni.jmethodID)
    def callObjectMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallObjectMethodV(this, jmid, ap)
            
    @annotate(jni.jboolean, this=jni.jobject, jmid=jni.jmethodID)
    def callBooleanMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallBooleanMethodV(this, jmid, ap)

    @annotate(jni.jbyte, this=jni.jobject, jmid=jni.jmethodID)
    def callByteMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallByteMethodV(this, jmid, ap)

    @annotate(jni.jchar, this=jni.jobject, jmid=jni.jmethodID)
    def callCharMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallCharMethodV(this, jmid, ap)

    @annotate(jni.jdouble, this=jni.jobject, jmid=jni.jmethodID)
    def callDoubleMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallDoubleMethodV(this, jmid, ap)

    @annotate(jni.jfloat, this=jni.jobject, jmid=jni.jmethodID)
    def callFloatMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallFloatMethodV(this, jmid, ap)

    @annotate(jni.jint, this=jni.jobject, jmid=jni.jmethodID)
    def callIntMethod(self, this, jmid, ...):
 
        with self as jenv:
            return jenv.CallIntMethodV(this, jmid, ap)
 
    @annotate(jni.jlong, this=jni.jobject, jmid=jni.jmethodID)
    def callLongMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallLongMethodV(this, jmid, ap)
 
    @annotate(jni.jshort, this=jni.jobject, jmid=jni.jmethodID)
    def callShortMethod(self, this, jmid, ...):

        with self as jenv:
            return jenv.CallShortMethodV(this, jmid, ap)
        
    @annotate(None, this=jni.jobject, jmid=jni.jmethodID)
    def callVoidMethod(self, this, jmid, ...):
    
        with self as jenv:
            jenv.CallVoidMethodV(this, jmid, ap)
            return None


    @annotate(jni.jobject, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualObjectMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualObjectMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jboolean, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualBooleanMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualBooleanMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jbyte, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualByteMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualByteMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jchar, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualCharMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualCharMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jdouble, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualDoubleMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualDoubleMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jfloat, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualFloatMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualFloatMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jint, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualIntMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualIntMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jlong, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualLongMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallNonvirtualLongMethodV(obj, jcls, jmid, ap)

    @annotate(jni.jshort, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualShortMethod(self, obj, jcls, jmid, ...):
    
        with self as jenv:
            return jenv.CallNonvirtualShortMethodV(obj, jcls, jmid, ap)

    @annotate(None, obj=jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callNonvirtualVoidMethod(self, obj, jcls, jmid, ...):

        with self as jenv:
            jenv.CallNonvirtualVoidMethodV(obj, jcls, jmid, ap)
            return None

 
    @annotate(jni.jobject, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticObjectMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticObjectMethodV(jcls, jmid, ap)

    @annotate(jni.jboolean, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticBooleanMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticBooleanMethodV(jcls, jmid, ap)

    @annotate(jni.jbyte, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticByteMethod(self, jcls, jmid, ...):
     
        with self as jenv:
            return jenv.CallStaticByteMethodV(jcls, jmid, ap)

    @annotate(jni.jchar, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticCharMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticCharMethodV(jcls, jmid, ap)

    @annotate(jni.jdouble, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticDoubleMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticDoubleMethodV(jcls, jmid, ap)

    @annotate(jni.jfloat, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticFloatMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticFloatMethodV(jcls, jmid, ap)

    @annotate(jni.jint, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticIntMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticIntMethodV(jcls, jmid, ap)

    @annotate(jni.jlong, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticLongMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticLongMethodV(jcls, jmid, ap)

    @annotate(jni.jshort, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticShortMethod(self, jcls, jmid, ...):

        with self as jenv:
            return jenv.CallStaticShortMethodV(jcls, jmid, ap)
 
    @annotate(None, jcls=jni.jclass, jmid=jni.jmethodID)
    def callStaticVoidMethod(self, jcls, jmid, ...):

        with self as jenv:
            jenv.CallStaticVoidMethodV(jcls, jmid, ap)
            return None


    @annotate(jni.jboolean, jobj=jni.jobject)
    def booleanValue(self, jobj):

        with self as jenv:
            return jenv.CallBooleanMethod(jobj, _mids[mid_Boolean_booleanValue])

    @annotate(jni.jbyte, jobj=jni.jobject)
    def byteValue(self, jobj):
 
        with self as jenv:
            return jenv.CallByteMethod(jobj, _mids[mid_Byte_byteValue])

    @annotate(jni.jchar, jobj=jni.jobject)
    def charValue(self, jobj):

        with self as jenv:
            return jenv.CallCharMethod(jobj, _mids[mid_Character_charValue])

    @annotate(jni.jshort, jobj=jni.jobject)
    def shortValue(self, jobj):

        with self as jenv:
            return jenv.CallShortMethod(jobj, _mids[mid_Short_shortValue])

    @annotate(jni.jint, jobj=jni.jobject)
    def intValue(self, jobj):

        with self as jenv:
            return jenv.CallIntMethod(jobj, _mids[mid_Integer_intValue])

    @annotate(jni.jlong, jobj=jni.jobject)
    def longValue(self, jobj):

        with self as jenv:
            return jenv.CallLongMethod(jobj, _mids[mid_Long_longValue])

    @annotate(jni.jfloat, jobj=jni.jobject)
    def floatValue(self, jobj):

        with self as jenv:
            return jenv.CallFloatMethod(jobj, _mids[mid_Float_floatValue])

    @annotate(jni.jdouble, jobj=jni.jobject)
    def doubleValue(self, jobj):

        with self as jenv:
            return jenv.CallDoubleMethod(jobj, _mids[mid_Double_doubleValue])


    @annotate(jni.jobject, val=jni.jboolean)
    def boxBoolean(self, val):

        with self as jenv:
            return jenv.NewObject(_boo, _mids[mid_Boolean_init], val)

    @annotate(jni.jobject, val=jni.jbyte)
    def boxByte(self, val):

        with self as jenv:
            return jenv.NewObject(_byt, _mids[mid_Byte_init], val)

    @annotate(jni.jobject, val=jni.jchar)
    def boxChar(self, val):

        with self as jenv:
            return jenv.NewObject(_cha, _mids[mid_Character_init], val)

    @annotate(jni.jobject, val=jni.jshort)
    def boxShort(self, val):

        with self as jenv:
            return jenv.NewObject(_sho, _mids[mid_Short_init], val)

    @annotate(jni.jobject, val=jni.jint)
    def boxInteger(self, val):

        with self as jenv:
            return jenv.NewObject(_int, _mids[mid_Integer_init], val)

    @annotate(jni.jobject, val=jni.jlong)
    def boxLong(self, val):

        with self as jenv:
            return jenv.NewObject(_lon, _mids[mid_Long_init], val)

    @annotate(jni.jobject, val=jni.jfloat)
    def boxFloat(self, val):

        with self as jenv:
            return jenv.NewObject(_flo, _mids[mid_Float_init], val)

    @annotate(jni.jobject, val=jni.jdouble)
    def boxDouble(self, val):
 
        with self as jenv:
            return jenv.NewObject(_dou, _mids[mid_Double_init], val)


    @annotate(jcls=jni.jclass, name=bytes, signature=bytes)
    jni.jfieldID getFieldID(self, jcls, name, signature):

        with self as jenv:
            return jenv.GetFieldID(jcls, name, signature)

    @annotate(jcls=jni.jclass, name=bytes, signature=bytes)
    jni.jmethodID getMethodID(self, jcls, name, signature):

        with self as jenv:
            return jenv.GetMethodID(jcls, name, signature)

    @annotate(jcls=jni.jclass, name=bytes, signature=bytes)
    jni.jmethodID getStaticMethodID(self, jcls, name, signature):

        with self as jenv:
            return jenv.GetStaticMethodID(jcls, name, signature)


    @annotate(jni.jobject, jcls=jni.jclass, name=bytes, signature=bytes)
    def getStaticObjectField(self, jcls, name, signature):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, signature)
            return jenv.GetStaticObjectField(jcls, jfid)

    @annotate(jni.jboolean, jcls=jni.jclass, name=bytes)
    def getStaticBooleanField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"Z")
            return jenv.GetStaticBooleanField(jcls, jfid)

    @annotate(jni.jbyte, jcls=jni.jclass, name=bytes)
    def getStaticByteField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"B")
            return jenv.GetStaticByteField(jcls, jfid)

    @annotate(jni.jchar, jcls=jni.jclass, name=bytes)
    def getStaticCharField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"C")
            return jenv.GetStaticCharField(jcls, jfid)

    @annotate(jni.jdouble, jcls=jni.jclass, name=bytes)
    def getStaticDoubleField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"D")
            return jenv.GetStaticDoubleField(jcls, jfid)

    @annotate(jni.jfloat, jcls=jni.jclass, name=bytes)
    def getStaticFloatField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"F")
            return jenv.GetStaticFloatField(jcls, jfid)

    @annotate(jni.jint, jcls=jni.jclass, name=bytes)
    def getStaticIntField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"I")
            return jenv.GetStaticIntField(jcls, jfid)

    @annotate(jni.jlong, jcls=jni.jclass, name=bytes)
    def getStaticLongField(self, jcls, name):
 
        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"J")
            return jenv.GetStaticLongField(jcls, jfid)

    @annotate(jni.jshort, jcls=jni.jclass, name=bytes)
    def getStaticShortField(self, jcls, name):

        with self as jenv:
            jfid = jenv.GetStaticFieldID(jcls, name, b"S")
            return jenv.GetStaticShortField(jcls, jfid)


    @annotate(jni.jobject, this=jni.jobject, jfid=jni.jfieldID)
    def getObjectField(self, this, jfid):

        with self as jenv:
            return jenv.GetObjectField(this, jfid)
         
    @annotate(jni.jboolean, this=jni.jobject, jfid=jni.jfieldID)
    def getBooleanField(self, this, jfid):

        with self as jenv:
            return jenv.GetBooleanField(this, jfid)

    @annotate(jni.jbyte, this=jni.jobject, jfid=jni.jfieldID)
    def getByteField(self, this, jfid):

        with self as jenv:
            return jenv.GetByteField(this, jfid)

    @annotate(jni.jchar, this=jni.jobject, jfid=jni.jfieldID)
    def getCharField(self, this, jfid):

        with self as jenv:
            return jenv.GetCharField(this, jfid)

    @annotate(jni.jdouble, this=jni.jobject, jfid=jni.jfieldID)
    def getDoubleField(self, this, jfid):

        with self as jenv:
            return jenv.GetDoubleField(this, jfid)

    @annotate(jni.jfloat, this=jni.jobject, jfid=jni.jfieldID)
    def getFloatField(self, this, jfid):

        with self as jenv:
            return jenv.GetFloatField(this, jfid)

    @annotate(jni.jint, this=jni.jobject, jfid=jni.jfieldID)
    def getIntField(self, this, jfid):

        with self as jenv:
            return jenv.GetIntField(this, jfid)

    @annotate(jni.jlong, this=jni.jobject, jfid=jni.jfieldID)
    def getLongField(self, this, jfid):

        with self as jenv:
            return jenv.GetLongField(this, jfid)

    @annotate(jni.jshort, this=jni.jobject, jfid=jni.jfieldID)
    def getShortField(self, this, jfid):

        with self as jenv:
            return jenv.GetShortField(this, jfid)


    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jobject)
    def setObjectField(self, this, jfid, val):

        with self as jenv:
            jenv.SetObjectField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jboolean)
    def setBooleanField(self, this, jfid, val):

        with self as jenv:
            jenv.SetBooleanField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jbyte)
    def setByteField(self, this, jfid, val):

        with self as jenv:
            jenv.SetByteField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jdouble)
    def setCharField(self, this, jfid, val):

        with self as jenv:
            jenv.SetCharField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jchar)
    def setDoubleField(self, this, jfid, val):

        with self as jenv:
            jenv.SetDoubleField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=)
    def setFloatField(self, this, jfid, jni.jfloat val):

        with self as jenv:
            jenv.SetFloatField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jint)
    def setIntField(self, this, jfid, val):
    
        with self as jenv:
            jenv.SetIntField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jlong)
    def setLongField(self, this, jfid, val):

        with self as jenv:
            jenv.SetLongField(this, jfid, val)

    @annotate(this=jni.jobject, jfid=jni.jfieldID, val=jni.jshort)
    def setShortField(self, this, jfid, val):

        with self as jenv:
            jenv.SetShortField(this, jfid, val)

    @annotate(int, jobj=jni.jobject)
    def id(self, jobj):
 
        with self as jenv:
            return jenv.CallStaticIntMethod(_sys, _mids[mid_sys_identityHashCode], jobj) if jobj else 0

    @annotate(int, jobj=jni.jobject)
    def hash(self, jobj):

        with self as jenv:
            return jenv.CallIntMethod(jobj, _mids[mid_obj_hashCode]) if jobj else 0

    def setClassPath(self, const char* classPath):

        jenv = self.get_vm_env()
        _ucl = (jclass) jenv.FindClass(b"java/net/URLClassLoader")
        _fil = (jclass) jenv.FindClass(b"java/io/File")
        jmid = jenv.GetStaticMethodID(_ucl, b"getSystemClassLoader",
                                            b"()Ljava/lang/ClassLoader;")
        jobject classLoader = jenv.CallStaticObjectMethod(_ucl, jmid)
        mf = jenv.GetMethodID(_fil, b"<init>", b"(Ljava/lang/String;)V")
        mu = jenv.GetMethodID(_fil, b"toURL",  b"()Ljava/net/URL;")
        ma = jenv.GetMethodID(_ucl, b"addURL", b"(Ljava/net/URL;)V")
        #if defined(_MSC_VER) || defined(__WIN32)
        const char* pathsep = ";";
        char *path = _strdup(classPath);
        #else
        const char* pathsep = ":";
        char *path = strdup(classPath);
        #endif

        for (char *cp = strtok(path, pathsep); cp != NULL; cp = strtok(NULL, pathsep)):
            jstring string = jenv.NewStringUTF(cp);
            jobject file = jenv.NewObject(_fil, mf, string);
            jobject url  = jenv.CallObjectMethod(file, mu);
            jenv.CallVoidMethod(classLoader, ma, url);

    char* getClassPath(self):

        jenv = self.get_vm_env()
        _ucl = jenv.FindClass(b"java/net/URLClassLoader")
        _url = jenv.FindClass(b"java/net/URL")
        jmid = jenv.GetStaticMethodID(_ucl, b"getSystemClassLoader",
                                            b"()Ljava/lang/ClassLoader;")
        jobject classLoader = jenv.CallStaticObjectMethod(_ucl, jmid)
        gu = jenv.GetMethodID(_ucl, b"getURLs", b"()[Ljava/net/URL;")
        gp = jenv.GetMethodID(_url, b"getPath", b"()Ljava/lang/String;")
        #if defined(_MSC_VER) || defined(__WIN32)
        const char* pathsep = ";";
        #else
        const char* pathsep = ":";
        #endif
        jobjectArray jarr = (jobjectArray) jenv.CallObjectMethod(classLoader, gu);

        int first = 1;
        int total = 0;
        char *classpath = NULL;
        
        count = jenv.GetArrayLength(jarr) if jarr else 0
        for i in range(count):
            url  = jenv.GetObjectArrayElement(jarr, i) # jni.jobject
            path = jni.cast(jenv.CallObjectMethod(url, gp), jni.jstring)
            utf8_chars = jenv.GetStringUTFChars(path, None)
            size = jenv.GetStringUTFLength(path)

            total += size + 1;
            if (classpath == NULL)
                classpath = (char *) calloc(total, 1);
            else
                classpath = (char *) realloc(classpath, total);
            if (classpath == NULL)
                return NULL;

            if (first)
                first = 0;
            else
                strcat(classpath, pathsep);

            strcat(classpath, utf8_chars);

        return classpath;

    jni.jstring fromUTF(self, const char* utf8):

        with self as jenv:
            return jenv.NewStringUTF(utf8)

    char* toUTF(self, jstring jstr): # bytes

        with self as jenv:
            utf8_chars = jenv.GetStringUTFChars(jstr, None)
            try:
                return ct.cast(utf8_chars, ct.c_char_p).value
            finally:
                jenv.ReleaseStringUTFChars(jstr, utf8_chars)

    char* toString(self, jobject jobj):
 
        try:
            return self.toUTF(jni.cast(self.callObjectMethod(jobj, _mids[mid_obj_toString]),
                                       jni.jstring)) if obj else NULL
        except (int e):
            if e ==  _EXC_PYTHON:
                return NULL;
            elif e ==  _EXC_JAVA:
                with self as jenv:
                    jenv.ExceptionDescribe()
                    jenv.ExceptionClear()
                return NULL;
            else:
                throw;
 
    PyObject* toPyUnicode(self, jobject obj):
    {
    }

    char* getClassName(self, jobject jobj):
    
        return self.toString(self.callObjectMethod(jobj, _mids[mid_obj_getClass])) if jobj else NULL
 
