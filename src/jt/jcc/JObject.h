#ifndef _JObject_H
#define _JObject_H

#include <stdio.h>
#include "JCCEnv.h"

class _DLL_EXPORT JObject
{
public:

    jobject this$;

    inline explicit JObject(jobject obj)
    {
        if (obj)
        {
            id = env->id(obj);
            this$ = env->newGlobalRef(obj, id);
        }
        else
        {
            id = 0;
            this$ = NULL;
        }
    }

    inline JObject(const JObject& obj)
    {
        id = obj.id ? obj.id : env->id(obj.this$);
        this$ = env->newGlobalRef(obj.this$, id);
    }

    virtual ~JObject()
    {
        this$ = env->deleteGlobalRef(this$, id);
    }

    JObject& weaken$()
    {
        if (id)
        {
            jobject ref = env->newGlobalRef(this$, 0);

            env->deleteGlobalRef(this$, id);
            id = 0;
            this$ = ref;
        }

        return *this;
    }

    inline int operator!() const
    {
        return env->isSame(this$, NULL);
    }

    inline int operator==(const JObject& obj) const
    {
        return env->isSame(this$, obj.this$);
    }

    JObject& operator=(const JObject& obj)
    {
        jobject prev = this$;
        int objid = obj.id ? obj.id : env->id(obj.this$);

        this$ = env->newGlobalRef(obj.this$, objid);
        env->deleteGlobalRef(prev, id);
        id = objid;

        return *this;
    }

private:

    int id;  // zero when this$ is a weak ref */
};

#endif /* _JObject_H */
