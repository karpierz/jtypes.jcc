@echo off
setlocal
set JAVA8_HOME=C:\Program Files\Java\jdk1.8.0_181
if not defined JAVA_HOME (set JAVA_HOME=%JAVA8_HOME%)
set javac="%JAVA_HOME%"\bin\javac -encoding UTF-8 -g:none -deprecation -Xlint:unchecked ^
    -source 1.8 -target 1.8 -bootclasspath "%JAVA8_HOME%\jre\lib\rt.jar"
set py=C:\Windows\py.exe -3.6 -B
pushd "%~dp0"\src\jt\jcc\_java
%javac% org\apache\jcc\*.java
%py% -m class2py org\apache\jcc\PythonException.class
%py% -m class2py org\apache\jcc\PythonVM.class
del /F/Q org\apache\jcc\*.class
popd
pushd "%~dp0"\tests
rmdir /Q/S java\classes 2> nul & mkdir java\classes
dir /S/B/O:N ^
    java\org\apache\jcc\*.java ^
    2> nul > build.fil
%javac% -d java/classes -classpath java/lib/* @build.fil
del /F/Q build.fil
popd
endlocal
