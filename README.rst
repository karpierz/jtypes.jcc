**Currently only as placeholder (because a base package jtypes.jvm is still in development)**

jtypes.jcc
==========

PyLucene's Python to Java bridge.

Overview
========

  **jtypes.jcc** provides a bridge necessary to call into Java classes from Python via
  Java's Native Invocation Interface (JNI).

  `PyPI record`_.

  | **jtypes.jcc** is a lightweight Python package, based on the *ctypes* or *cffi* library.
  | It is an almost fully compliant implementation of PyLucene's *JCC* package
    by reimplementing its functionality in a clean Python instead of C++.

About JCC:
----------

Borrowed from the `original website`_:

  What is **JCC** ?

  **JCC** is a C++ code generator that produces a C++ object interface wrapping a Java
  library via Java's Native Interface (JNI). **JCC** also generates C++ wrappers that
  conform to Python's C type system making the instances of Java classes directly
  available to a Python interpreter.
  ...

  **JCC** is a Python extension written in Python and C++.
  It requires a Java Runtime Environment (JRE) to operate as it uses Java's reflection
  APIs to do its work.

Requirements
============

- Various Java Runtime Environments such as Oracle Java 1.7+, Apple's Java 1.6+
  on Mac OS X as well as open source Java OpenJDK 1.7+ builds.

Installation
============

Prerequisites:

+ Python 2.7 or higher or 3.4 or higher

  * http://www.python.org/
  * 2.7 and 3.6 are primary test environments.

+ pip and setuptools

  * http://pypi.python.org/pypi/pip
  * http://pypi.python.org/pypi/setuptools

To install run::

    python -m pip install --upgrade jtypes.jcc

To ensure everything is running correctly you can run the tests using::

    python -m jt.jcc.tests

Development
===========

Visit `development page`_

Installation from sources:

Clone the `sources`_ and run::

    python -m pip install ./jtypes.jcc

or on development mode::

    python -m pip install --editable ./jtypes.jcc

Prerequisites:

+ Development is strictly based on *tox*. To install it run::

    python -m pip install tox

License
=======

  | Copyright 2015-2018 Adam Karpierz
  |
  | Licensed under the Apache License, Version 2.0
  | http://www.apache.org/licenses/LICENSE-2.0
  | Please refer to the accompanying LICENSE file.

Authors
=======

* Adam Karpierz <adam@karpierz.net>

.. _PyPI record: https://pypi.python.org/pypi/jtypes.jcc
.. _original website: http://lucene.apache.org/pylucene/jcc
.. _development page: https://github.com/karpierz/jtypes.jcc
.. _sources: https://github.com/karpierz/jtypes.jcc
