from __future__ import print_function
from __future__ import unicode_literals

import lldb


def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand(
        b'type summary add -x -F'
        b' lldbinit_qt.qstring_summary "QString"')


def make_string_from_pointer_with_offset(d_ptr, offset, length):
    strval = 'u"'
    try:
        data_array = d_ptr.GetPointeeData(0, length).uint16
        for X in range(offset, length):
            V = data_array[X]
            if V == 0:
                break
            strval += unichr(V)
    except:
        pass
    strval = strval + '"'
    return strval.encode('utf-8')


#qt5
def qstring_summary(value, unused):
    try:
        d = value.GetChildMemberWithName(b'd')
        #have to divide by 2 (size of unsigned short = 2)
        offset = d.GetChildMemberWithName(b'offset').GetValueAsUnsigned() / 2
        size = get_max_size(value)
        return make_string_from_pointer_with_offset(d, offset, size)
    except Exception as e:
        print('error', e)
        return value


def get_max_size(value):
    max_size = None
    try:
        debugger = value.GetTarget().GetDebugger()
        v = lldb.SBDebugger.GetInternalVariableValue(b'target.max-string-summary-length', debugger.GetInstanceName())
        max_size = int(v.GetStringAtIndex(0))
    except:
        max_size = 512
    return max_size
