
#include "JObject.h"

class t_JObject

    """t_JObject objects"""

    # "jcc.JObject" # tp_name

    #extern PyTypeObject PY_TYPE(JObject);

    def __new__(PyTypeObject *type, PyObject *args, PyObject *kwds)

        self = (t_JObject *) type->tp_alloc(type, 0);
        self.object = JObject(None)

        return self

    def __del__(self):

        self.object = JObject(None)

    @property
    def _jobject(self):

        return PyCapsule_New((void *) self.object.this$, "jobject", NULL);

    def hash(self):

        return env->hash(self.object.this$)

    def __eq__(self, other):

        if self is other:
            return True

        if not PyObject_TypeCheck(other, &PY_TYPE(JObject)):
            return False

        return self.object == other.object

    def __ne__(self, other):

        eq = self.__eq__(other)
        return NotImplemented if eq is NotImplemented else not eq

    def __lt__(self, other):

        raise NotImplementedError("<");

    def __gt__(self, other):

        raise NotImplementedError(">");

    def __le__(self, other):

        raise NotImplementedError("<=");

    def __ge__(self, other):

        raise NotImplementedError(">=");

    def __str__(self):

        if self.object.this$:
            char *utf = env->toString(self.object.this$);
            if utf == NULL:
                utf = env->getClassName(self.object.this$);
            if utf != NULL:
                return PyUnicode_DecodeUTF8(utf, strlen(utf), "strict")
        return "<null>"

    def __repr__(self):

        return "<{}: {}>".format(self.__class__.__name__, self.__str__());
