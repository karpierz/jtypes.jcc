
#ifdef _jcc_lib

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* vm_env;

    if ( ! vm->GetEnv((void **) &vm_env, JNI_VERSION_1_6) )
        env = new JCCEnv(vm, vm_env);

    _PythonException_registerNatives(vm_env)

    return JNI_VERSION_1_6;
}

// Class:  org.apache.jcc.PythonVM
// Method: native void init(String programName, String[] args);

JNIEXPORT void JNICALL Java_org_apache_jcc_PythonVM_init(JNIEnv* vm_env, jobject self,
                                                         jstring programName, jobjectArray args)
{
    return _PythonVM_init(vm_env, self, programName, args);
}

};

#endif /* _jcc_lib */

#ifdef _jcc_lib

static void _PythonVM_init(JNIEnv* vm_env, jobject self,
                           jstring programName, jobjectArray args)
{
    const char* utf8 = vm_env->GetStringUTFChars(programName, JNI_FALSE);
#ifdef linux
    // load python runtime for other .so modules to link (such as _time.so)
    char buf[32];
    sprintf(buf, "libpython%d.%d.so", PY_MAJOR_VERSION, PY_MINOR_VERSION);
    dlopen(buf, RTLD_NOW | RTLD_GLOBAL);
#endif

    Py_SetProgramName((char*) utf8);

    PyEval_InitThreads();
    Py_Initialize();

    if ( ! args )
        PySys_SetArgv(1, (char **) &utf8);
    else
    {
        int argc = vm_env->GetArrayLength(args);
        char** argv = (char**) calloc(argc + 1, sizeof(char*));

        argv[0] = (char*) utf8;
        for ( int i = 0; i < argc; ++i )
        {
            jstring arg = (jstring) vm_env->GetObjectArrayElement(args, i);
            argv[i + 1] = (char*)   vm_env->GetStringUTFChars(arg, JNI_FALSE);
        }

        PySys_SetArgv(argc + 1, argv);

        for ( int i = 0; i < argc; ++i )
        {
            jstring arg = (jstring) vm_env->GetObjectArrayElement(args, i);
            vm_env->ReleaseStringUTFChars(arg, argv[i + 1]);
        }

        free(argv);
    }

    vm_env->ReleaseStringUTFChars(programName, utf8);
    PyEval_ReleaseLock();
}

#endif /* _jcc_lib */
