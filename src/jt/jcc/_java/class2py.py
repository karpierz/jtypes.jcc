from jt.jvm.java.class2py import *

header = \
"""\
# Copyright 2015-2018 Adam Karpierz
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

"""

if __name__ == "__main__":
    import sys
    class2py(sys.argv[1], header=header)
