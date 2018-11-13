#ifdef PYTHON

    bool JCCEnv::restorePythonException(jthrowable throwable):

        # returns true if Python exception instance was successfully restored

#ifdef _jcc_lib  # PythonException is only available in shared mode

        pycls = getPythonExceptionClass()  # jni.jclass

        jenv = self.get_vm_env()

        # Support through-layer exceptions by taking the active PythonException
        # and making the enclosed exception visible to Python again.

        if jenv.IsSameObject(jenv.GetObjectClass(throwable), pycls):

            jfid = jenv.GetFieldID(pycls, b"py_error_state", b"J")  # jni.jfieldID
            state = (PyObject *) jenv.GetLongField(throwable, jfid)

            if state != NULL:

                type  = state[0]
                value = state[1]
                tb    = state[2]

                Py_INCREF(type);
                if value is None:
                    value = NULL;
                else
                    Py_INCREF(value);
                if tb is None:
                    tb = NULL;
                else
                    Py_INCREF(tb);

                PyErr_Restore(type, value, tb);

                return True
#endif

        return False

    jstring JCCEnv::fromPyString(self, PyObject* object):

        if object is None:
            return NULL;
        elif PyUnicode_Check(object):

            jenv = self.get_vm_env()

            size = len(object)
            if sizeof(Py_UNICODE) == sizeof(jchar):
                jchars = (jchar*) PyUnicode_AS_UNICODE(object) # jchar*
            else:
                pchars = PyUnicode_AS_UNICODE(object)  # Py_UNICODE*
                jchars = new jchar[size]  # jchar*
                for (int i = 0; i < size; i++):
                    jchars[i] = jni.new(jni.jchar, pchars[i])
            return jenv->NewString(jchars, size)
        elif PyString_Check(object):
            return fromUTF(PyString_AS_STRING(object))
        else:
            raise TypeError("expected a string", object)

    def JCCEnv::fromJString(self, jstring jstr):

        jenv = self.get_vm_env()

        if sizeof(Py_UNICODE) == sizeof(jni.jchar):
            return JString(jenv, jstr, borrowed=True).str if jstr else None
        else:
            if jstr:
                size = jenv.GetStringLength(jstr)
                pstr = PyUnicode_FromUnicode(NULL, size) # PyObject*
                if pstr:
                    jboolean isCopy;
                    jchars = jenv.GetStringChars(jstr, &isCopy) # const jchar*
                    try:
                        pchars = PyUnicode_AS_UNICODE(pstr)  # Py_UNICODE*
                        for (int i = 0; i < size; i++):
                            pchars[i] = (Py_UNICODE) jchars[i]
                    finally:
                        jenv.ReleaseStringChars(jstr, jchars)
                return pstr
            else:
                return None

    def JCCEnv::finalizeObject(JNIEnv *jenv, PyObject *obj):

        # may be called from finalizer thread which has no vm_env thread local

        with PythonGIL():
            set_vm_env(jenv)
            Py_DECREF(obj);


class PythonGIL(object):

    def __init__(self, jenv=None):

        self._state = None # PyGILState_STATE
        self._jenv  = jenv # JNIEnv*

    def __enter__(self):

        self._state = PyGILState_Ensure()
        if self._jenv is not None:
            env->set_vm_env(self._jenv)
            self._jenv = None

    def __exit__(self, *exc_info):

        del exc_info
        PyGILState_Release(self._state)


class PythonThreadState(object):

    def __init__(self, handler=0):

        self._state   = None    # PyThreadState*
        self._handler = handler # int

    def __enter__(self):

        self._state = PyEval_SaveThread()
        env->handlers += self._handler

    def __exit__(self, *exc_info):

        del exc_info
        PyEval_RestoreThread(self._state)
        env->handlers -= self._handler


#endif
