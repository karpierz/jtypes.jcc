// Copyright 2015-2018 Adam Karpierz
// Licensed under the Apache License, Version 2.0
// http://www.apache.org/licenses/LICENSE-2.0

package org.apache.jcc;

public class PythonException extends RuntimeException
{
    public boolean withTrace      = true;
    protected long py_error_state = 0L;

    public PythonException(String message)
    {
        super(message);
        this.saveErrorState();
    }

    public void finalize() throws Throwable
    {
        this.pythonDecRef();
    }

    public String getMessage()
    {
        return this.getMessage(this.withTrace);
    }

    public String getMessage(boolean trace)
    {
        if ( this.py_error_state == 0L )
            return super.getMessage();
        else
        {
            String message = this.getErrorMessage();
            if ( trace )
            {
                message += "\n";
                message += this.getErrorTraceback();
            }
            return message;
        }
    }

    public native void pythonDecRef();

    public native String getErrorName();
    public native String getErrorMessage();
    public native String getErrorTraceback();

    protected native void saveErrorState();
}
