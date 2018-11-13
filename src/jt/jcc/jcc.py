
#include <jni.h>

#ifdef linux
#include <dlfcn.h>
#endif

#include "structmember.h"

#include "JObject.h"
#include "JCCEnv.h"
#include "macros.h"


env = JCCEnv() # _DLL_EXPORT


class JCCEnv(object):

    """JCCEnv"""

    # "jcc.JCCEnv" # tp_name

    JCCEnv* env;

    @property
    @annotate(int)
    def jni_version(self):

        return self.env.getJNIVersion()

    @property
    def java_version(self):

        jver = self.env.getJavaVersion()
        ver = self.env.fromJString(jver)
        self.env->get_vm_env().DeleteLocalRef((jobject) jver)
        return ver

    @property
    @annotate(Optional[builtins.str])
    def classpath(self):

        cpath = self.env.getClassPath()
        return cpath if cpath else None

    @annotate(int, name=Optional[builtins.str], asDaemon=bool)
    def attachCurrentThread(self, name=None, asDaemon=False):

        return self.env.attachCurrentThread(name, asDaemon)

    @annotate(int)
    def detachCurrentThread(self):

        result = self.env.vm.DetachCurrentThread()
        self.env.set_vm_env(None)
        return result

    @annotate(bool)
    def isCurrentThreadAttached(self):

        return (self.env.get_vm_env() is not None)

    @annotate(bool)
    def isShared(self):

        #ifdef _jcc_lib
        return True
        #else
        return False
        #endif

    @annotate(builtins.str)
    def strhash(self, obj):

        return "%08x" % hash(obj)

    @annotate(classpath=builtins.str)
    def _addClassPath(self, classpath):

        self.env.setClassPath(classpath)

    @annotate(classes=bool, values=bool)
    def _dumpRefs(self, classes=False, values=False):

        result = {} if classes else []

        std::multimap<int, countedRef>::iterator iter;
        for ( iter = self.env.refs.begin(); iter != self.env.refs.end(); ++iter ):

            if classes:

                # return dict of { class name: instance count }

                class_name = self.env.getClassName(iter->second.global)
                if class_name not in result:
                    result[class_name] = 1
                else:
                    result[class_name] += 1

            elif values:

                # return list of (value string, ref count)

                val_str = self.env.toString(iter->second.global)
                count   = PyInt_FromLong(iter->second.count)
                result.append((val_str, count))

            else:

                # return list of (id hash code, ref count)

                hcode = PyInt_FromLong(iter->first)
                count = PyInt_FromLong(iter->second.count)
                result.append((hcode, count))

        return result

    def getVMEnv(self): # _DLL_EXPORT

        if self.env.vm is None:
            return None

        jccenv = JCCEnv()
        jccenv.env = self.env

        return jccenv


_DLL_EXPORT PyObject* initJCC(PyObject* module):

    static int _once_only = 1;

    PyObject* ver = PyString_FromString(JCC_VER);
    PyObject_SetAttrString(module, "JCC_VERSION", ver); Py_DECREF(ver);

    if (_once_only)
    {
        PyEval_InitThreads();
        INSTALL_TYPE(JCCEnv, module);

        if (env == NULL)
            env = new JCCEnv(NULL, NULL);

        _once_only = 0;
        Py_RETURN_TRUE;
    }

    Py_RETURN_FALSE;


_DLL_EXPORT extern "C++" PyObject *initVM(PyObject *self, PyObject *args, PyObject *kwds)
{
    return // tu do przekazac wynik wywolania ponizszej pythonowej initVM
}

def initVM(self, classpath=None, initialheap=None, maxheap=None, maxstack=None, vmargs):

    static char *kwnames[] = {};
    char *classpath = NULL, *initialheap = NULL, *maxheap = NULL, *maxstack = NULL;
    PyObject* vmargs  = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|zzzzO", kwnames,
                                     &classpath, &initialheap, &maxheap, &maxstack, &vmargs))
        return NULL;

    if env->vm:

        if (initialheap is not None or
            maxheap     is not None or
            maxstack    is not None or
            vmargs      is not None):
            raise ValueError("JVM is already running, options are ineffective")

        if ( classpath is None and self != NULL ):
            try:
                classpath = self.CLASSPATH
            except AttributeError:
                pass

        if ( classpath and classpath[0] ):
            env->setClassPath(classpath)

        return getVMEnv(self)

    else:

        JavaVMOption vm_options[32];
        JNIEnv* vm_env;
        JavaVM *vm;
        unsigned int nOptions = 0;

        vm_args = jni.JavaVMInitArgs()
        vm_args.version = jni.JNI_VERSION_1_6
        JNI_GetDefaultJavaVMInitArgs(vm_args)

        if ( classpath is None and self != NULL ):
            try:
                classpath = self.CLASSPATH
            except AttributeError:
                pass

#ifdef _jcc_lib
        import jcc
        if (classpath):
            _add_paths("-Djava.class.path=", jcc.CLASSPATH, classpath, &vm_options[nOptions++])
        else:
            _add_option("-Djava.class.path=", jcc.CLASSPATH, &vm_options[nOptions++])
        del jcc
#else
        if (classpath):
            _add_option("-Djava.class.path=", classpath, &vm_options[nOptions++])
#endif

        if (initialheap): _add_option("-Xms", initialheap, &vm_options[nOptions++])
        if (maxheap):     _add_option("-Xmx", maxheap,     &vm_options[nOptions++])
        if (maxstack):    _add_option("-Xss", maxstack,    &vm_options[nOptions++])

        if vmargs != NULL and PyString_Check(vmargs):

#ifdef _MSC_VER
            char *buf = _strdup(PyString_AS_STRING(vmargs))
#else
            char *buf = strdup(PyString_AS_STRING(vmargs))
#endif
            char *sep = ",";
            char *option;
            for ( option = strtok(buf, sep); option != NULL; option = strtok(NULL, sep) ):
                if nOptions < sizeof(vm_options) / sizeof(JavaVMOption):
                    _add_option("", option, &vm_options[nOptions++])
                else:
                    raise ValueError("Too many options (> {})".format(nOptions))

        elif vmargs != NULL and issequence(vmargs):

            PyObject* fast = PySequence_Fast(vmargs, "error converting vmargs to a tuple")
            if (fast == NULL):
                return NULL;

            for ( int i = 0; i < PySequence_Fast_GET_SIZE(fast); ++i ):
                option = PySequence_Fast_GET_ITEM(fast, i)

                if not isinstance(option, builtins.str):
                    raise TypeError("vmargs arg {} is not a string".format(i))

                if nOptions < sizeof(vm_options) / sizeof(JavaVMOption):
                    _add_option("", option, &vm_options[nOptions++])
                else:
                    raise ValueError("Too many options (> {})".format(nOptions))

        elif (vmargs != NULL):
            raise TypeError("vmargs is not a string or sequence")

        #vm_options[nOptions++].optionString = "-verbose:gc"
        #vm_options[nOptions++].optionString = "-Xcheck:jni"

        vm_args.nOptions           = nOptions
        vm_args.ignoreUnrecognized = jni.JNI_FALSE
        vm_args.options            = vm_options

        if JNI_CreateJavaVM(&vm, (void **) &vm_env, &vm_args) < 0:
            raise ValueError("An error occurred while creating Java VM")

        env->set_vm(vm, vm_env)

        jccenv = JCCEnv()
        jccenv.env = env

#ifdef _jcc_lib
        _PythonException_registerNatives(vm_env)
#endif
        return jccenv


#ifdef _jcc_lib

@annotate(name=builtins.str, p0=builtins.str, p1=builtins.str, option=jni.JavaVMOption)
def _add_paths(name, p0, p1, option):

    option.optionString = "{}{}{}{}".format(name, p0, os.pathsep, p1)

#endif


@annotate(name=builtins.str, value=builtins.str, option=jni.JavaVMOption)
def _add_option(name, value, option):

    option.optionString = "{}{}".format(name, value)


@annotate(module=object, parent=builtins.str, name=builtins.str)
def getJavaModule(module, parent, name): # _DLL_EXPORT

    # returns borrowed reference

    modules = sys.modules

    if parent == "":
        parent_module = None
        full_name     = name
    elif parent in modules:
        parent_module = modules[parent]
        full_name     = "{}.{}".format(parent, name)
    else:
        raise ValueError("Parent module '{}' not found".format(parent))

    try:
        child_module = modules[full_name]
    except KeyError:
        child_module = PyModule_New(full_name)
        if (child_module != NULL):
            if (parent_module != NULL):
                parent_module.__dict__[name] = child_module
                # moze lepiej setattr(parent_module, name, child_module) ???
            modules[full_name] = child_module

    # During __install__    pass, __file__ is not yet set on module.
    # During __initialize__ pass, __file__ is passed down to child_module.
    if (child_module != NULL):
        try:
            file = module.__dict__["__file__"]
        except:
            pass
        else:
            child_module.__dict__["__file__"] = file

    return child_module
