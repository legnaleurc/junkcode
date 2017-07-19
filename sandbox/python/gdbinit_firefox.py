# ex: et ts=4 sts=4 sw=4 :

import re

import gdb
import gdb.xmethod


def nested_get(value, property_list, default=None):
    try:
        return reduce(gdb.Value.__getitem__, property_list, value)
    except Exception:
        return default


class SmartPointerMatcher(gdb.xmethod.XMethodMatcher):

    def __init__(self, name):
        super(SmartPointerMatcher, self).__init__(name)

        self.methods = [
            SmartPointerMethod('SmartPointerMethod'),
        ]

    def match(self, class_type, method_name):
        m = re.match(r'^(Ref|nsCOM)Ptr<.*>$', class_type.tag)
        print(m)
        if not m:
            return None
        workers = []
        for method in self._methods:
            if method.enabled:
                worker = method.get_worker(method_name)
                if worker:
                    workers.append(worker)
        return workers


class SmartPointerMethod(gdb.xmethod.XMethod):

    def __init__(self, name):
        super(SmartPointerMethod, self).__init__(name)

    def get_worker(self, method_name):
        if method_name == 'operator*':
            return SmartPointerDereference()


class SmartPointerDereference(gdb.xmethod.XMethodWorker):

    def __init__(self):
        super(SmartPointerDereference, self).__init__()

    def get_arg_types(self):
        return None

    def __call__(self, obj):
        return obj['mRawPtr'].dereference()


gdb.xmethod.register_xmethod_matcher(None, SmartPointerMatcher('SmartPointerMatcher'))
