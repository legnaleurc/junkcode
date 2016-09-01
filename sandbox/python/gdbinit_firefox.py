# ex: et ts=4 sts=4 sw=4 :

import re

import gdb
import gdb.xmethod


def nested_get(value, property_list, default=None):
    try:
        return reduce(gdb.Value.__getitem__, property_list, value)
    except Exception:
        return default


class DereferenceCommand(gdb.Command):

    '''
    Dereference.
    '''

    def __init__(self):
        super(DereferenceCommand, self).__init__('pp', gdb.COMMAND_SUPPORT, gdb.COMPLETE_EXPRESSION, False)

    def invoke(self, argument, from_tty):
        args = gdb.string_to_argv(argument)
        name = args[0]

        value = gdb.parse_and_eval(name)

        if value.is_optimized_out:
            print('`{0}` has been optimized out'.format(name))
            return

        try:
            print(value.dereference())
            return gdb.Value(111)
        except Exception:
            pass

        try:
            print(value['mRawPtr'].dereference())
            return
        except Exception:
            pass

        try:
            print(value['mRawPtr'].dereference())
            return
        except Exception:
            pass

        if value.type.name == 'mozilla::StyleSheetHandle::RefPtr':
            tmp = nested_get(value, ['mHandle', 'mPtr', 'mValue'])
            if tmp:
                tmp = long(tmp)
                if tmp & 0x1:
                    tmp = tmp - 1
                    type_ = 'mozilla::ServoStyleSheet'
                else:
                    type_ = 'mozilla::CSSStyleSheet'

                tmp = gdb.Value(tmp)
                type_ = gdb.lookup_type(type_)
                tmp = tmp.cast(type_.pointer())

                print(tmp.dereference())
            else:
                print('(mozilla::StyleSheet *) 0x0')

            return

        print('no matching pointer')


class NetscapePointerMatcher(gdb.xmethod.XMethodMatcher):

    def __init__(self):
        super(NetscapePointerMatcher, self).__init__('NetscapePointerMatcher')

        self._methods = (
            NetscapePointer(),
        )

    def match(self, class_type, method_name):
        m = re.match(r'^nsCOMPtr<.*>$', class_type.tag)
        if not m:
            return None
        workers = []
        for method in self._methods:
            if method.enabled:
                worker = method.get_worker(method_name)
                if worker:
                    workers.append(worker)
        return workers


class NetscapePointer(gdb.xmethod.XMethod):

    def __init__(self):
        super(NetscapePointer, self).__init__('nsCOMPtr')

    def get_worker(self, method_name):
        if method_name == 'operator*':
            return NetscapePointerDereference()


class NetscapePointerDereference(gdb.xmethod.XMethodWorker):

    def __init__(self):
        super(NetscapePointerDereference, self).__init__()

'''
    def get_arg_types(self):
        return None

    def get_result_type(self, obj):
        return obj.type.template_argument(0)
'''

    def __call__(self, obj):
        return obj['mRawPtr'].dereference()


DereferenceCommand()
gdb.xmethod.register_xmethod_matcher(None, NetscapePointerMatcher())
