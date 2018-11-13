# Copyright 2015-2018 Adam Karpierz
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

from . import __config__ ; del __config__
from .__about__ import * ; del __about__

import os, sys

if sys.platform == 'win32':

    if '--find-jvm-dll' in sys.argv:
        from ._platform import add_jvm_dll_directory_to_path
        add_jvm_dll_directory_to_path()

    from jcc.config import SHARED
    if SHARED:
        path = os.environ['Path'].split(os.pathsep)
        eggpath = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        if eggpath not in path:
            path.insert(0, eggpath)
            os.environ['Path'] = os.pathsep.join(path)

from jcc import _jcc

# used when jcc is invoked with -m
if __name__ == '__main__':
    import jcc.__main__
else:
    from ._jcc import initVM

CLASSPATH = os.path.join(os.path.dirname(os.path.abspath(__file__)),"classes")
_jcc.CLASSPATH = CLASSPATH
