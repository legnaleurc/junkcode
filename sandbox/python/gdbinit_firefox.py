# ex: et ts=4 sts=4 sw=4 :

import re
import functools as ft

import gdb
import gdb.xmethod


def nested_get(value, property_list, default=None):
    try:
        return ft.reduce(gdb.Value.__getitem__, property_list, value)
    except Exception:
        return default


class SmartPointerMatcher(gdb.xmethod.XMethodMatcher):

    def __init__(self, name):
        super(SmartPointerMatcher, self).__init__(name)

        self.methods = [
            SmartPointerMethod('SmartPointerMethod'),
        ]

    # Override
    def match(self, class_type, method_name):
        rp = self._internal_match(class_type.tag)
        if not rp:
            return None
        workers = filter(lambda _: _.enabled, self.methods)
        workers = map(lambda _: _.get_worker(method_name, rp), workers)
        workers = filter(None, workers)
        workers = list(workers)
        return workers

    def _internal_match(self, class_name):
        m = re.match(r'^(Ref|nsCOM)Ptr<.*>$', class_name)
        if m:
            return ['mRawPtr']

        m = re.match(r'^mozilla::ArenaRefPtr<.*>$', class_name)
        if m:
            return ['mPtr', 'mRawPtr']

        return None


class SmartPointerMethod(gdb.xmethod.XMethod):

    def __init__(self, name):
        super(SmartPointerMethod, self).__init__(name)

    def get_worker(self, method_name, real_pointer):
        if method_name == 'operator*':
            return SmartPointerDereference(real_pointer)
        return None


class SmartPointerDereference(gdb.xmethod.XMethodWorker):

    def __init__(self, real_pointer):
        super(SmartPointerDereference, self).__init__()

        self._rp = real_pointer

    # Override
    def get_arg_types(self):
        return None

    # Override
    def __call__(self, obj):
        v = nested_get(obj, self._rp)
        return v.dereference()


gdb.xmethod.register_xmethod_matcher(None, SmartPointerMatcher('SmartPointerMatcher'))
