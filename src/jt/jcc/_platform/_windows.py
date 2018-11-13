# Copyright 2015-2018 Adam Karpierz
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

from __future__ import absolute_import

import sys
import os
from os import path
if sys.version_info.major >= 3:
    import winreg
else:
    import _winreg as winreg

from ...jvm.platform import _jvmfinder


class JVMFinder(_jvmfinder.JVMFinder):

    def __init__(self, java_version=None):

        super(JVMFinder, self).__init__(java_version)

        self._methods = (
            self._get_from_registry,
        )

    class WindowsRegistry(object):

        def __init__(self):

            hkey = winreg.HKEY_LOCAL_MACHINE
            self._handle = winreg.ConnectRegistry(None, hkey)

        def __del__(self):

            self.close()

        def get(self, key, name):

            handle = None
            try:
                handle = winreg.OpenKey(self._handle, key)
                return winreg.QueryValueEx(handle, name)[0]
            finally:
                if handle is not None:
                    handle.Close()

        def close(self):

            if self._handle:
                self._handle.Close()
            self._handle = None

        def __enter__(self):

            return self

        def __exit__(self, *exc_info):

            del exc_info
            self.close()

    def _get_from_registry(self, client_or_server="client"):

        with WindowsRegistry() as registry:
            for node in (r"SOFTWARE",):

                jre_key_path = node + r"\JavaSoft\Java Runtime Environment"
                jdk_key_path = node + r"\JavaSoft\Java Development Kit"

                for key_path, is_jre in ((jre_key_path, True), (jdk_key_path, False)):
                    try:
                        version = registry.get(key_path, "CurrentVersion")
                        looking_for = r"{}\{}".format(key_path, version)
                        java_home = registry.get(looking_for, "JavaHome")
                        if path.exists(java_home):
                            java_home = path.abspath(java_home)
                            jre_home  = java_home if is_jre else path.join(java_home, "jre")
                            break
                    except:
                        pass
                else:
                    jre_home = None

        if jre_home:
            jvm_path = path.join(path.abspath(path.join(jre_home, "bin", client_or_server)), "jvm.dll")
            if path.exists(jvm_path):
                return jvm_path

        return None


def add_jvm_dll_directory_to_path():

    jvm_path = JVMFinder().get_jvm_path()
    if jvm_path is None:
        raise ValueError("jvm.dll could not be found")

    env_path = os.environ["Path"].split(os.pathsep)
    env_path.append(path.dirname(jvm_path))
    os.environ["Path"] = os.pathsep.join(env_path)
    return True
