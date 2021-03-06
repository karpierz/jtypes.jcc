# Copyright 2015-2018 Adam Karpierz
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

from ...jvm.lib import platform

if platform.is_windows:
    from ._windows import JVMFinder
elif platform.is_linux:
    from ._linux   import JVMFinder
elif platform.is_osx:
    from ._osx     import JVMFinder
elif platform.is_android:
    from ._android import JVMFinder
elif platform.is_posix:
    from ._linux   import JVMFinder
else:
    raise ImportError("unsupported platform")

del platform
