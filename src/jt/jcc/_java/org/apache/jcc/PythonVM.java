// Copyright 2015-2018 Adam Karpierz
// Licensed under the Apache License, Version 2.0
// http://www.apache.org/licenses/LICENSE-2.0

package org.apache.jcc;

public class PythonVM
{
    static protected PythonVM vm;

    static
    {
        System.loadLibrary("jcc");
    }

    static public PythonVM start(String programName)
    {
        return PythonVM.start(programName, null);
    }

    static public PythonVM start(String programName, String[] args)
    {
        if ( PythonVM.vm == null )
        {
            PythonVM.vm = new PythonVM();
            PythonVM.vm.init(programName, args);
        }
        return PythonVM.vm;
    }

    static public PythonVM get()
    {
        return PythonVM.vm;
    }

    protected PythonVM()
    {
    }

    protected native void init(String programName, String[] args);

    public native Object instantiate(String moduleName, String className) throws PythonException;

    public native int acquireThreadState();
    public native int releaseThreadState();
}
