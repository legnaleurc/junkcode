from __future__ import print_function
from __future__ import unicode_literals

import lldb


def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand(
        b'type summary add -x -v -F'
        b' lldbinit_qt.qstring_summary "^QString$"')
    debugger.HandleCommand(
        b'type synthetic add -x -l'
        b' lldbinit_qt.QVectorSyntheticProvider "^QVector<.+>$"')
    debugger.HandleCommand(
        b'type synthetic add -x -l'
        b' lldbinit_qt.QListSyntheticProvider "^QList<.+>$"')


class QVectorSyntheticProvider(object):

    def __init__(self, value, internal_dict):
        self.valobj = value

    def num_children(self):
        try:
            s = self.valobj.GetChildMemberWithName(b'd').GetChildMemberWithName(b'size').GetValueAsUnsigned()
            return s
        except:
            return 0

    def get_child_index(self, name):
        try:
            return int(name.lstrip(b'[').rstrip(b']'))
        except:
            return None

    def get_child_at_index(self, index):
        if index < 0:
            return None
        if index >= self.num_children():
            return None
        if self.valobj.IsValid() == False:
            return None
        try:
            doffset = self.valobj.GetChildMemberWithName(b'd').GetChildMemberWithName(b'offset').GetValueAsUnsigned()
            type_ = self.valobj.GetType().GetTemplateArgumentType(0)
            elementSize = type_.GetByteSize()
            return self.valobj.GetChildMemberWithName(b'd').CreateChildAtOffset(b'[' + str(index) + b']', doffset + index * elementSize, type_)
        except:
            return None


class QListSyntheticProvider(object):

    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        try:
            listDataD = self.valobj.GetChildMemberWithName('p').GetChildMemberWithName('d')
            begin = listDataD.GetChildMemberWithName('begin').GetValueAsUnsigned()
            end = listDataD.GetChildMemberWithName('end').GetValueAsUnsigned()
            return (end - begin)
        except:
            return 0

    def get_child_index(self, name):
        try:
            return int(name.lstrip('[').rstrip(']'))
        except:
            return None

    def get_child_at_index(self, index):
        if index < 0:
            return None
        if index >= self.num_children():
            return None
        if self.valobj.IsValid() == False:
            return None
        try:
            pD = self.valobj.GetChildMemberWithName('p').GetChildMemberWithName('d')
            pBegin = pD.GetChildMemberWithName('begin').GetValueAsUnsigned()
            pArray = pD.GetChildMemberWithName('array').GetValueAsUnsigned()
            pAt = pArray + pBegin + index
            type_ = self.valobj.GetType().GetTemplateArgumentType(0)
            elementSize = type_.GetByteSize()
            voidSize = pD.GetChildMemberWithName('array').GetType().GetByteSize()
            return self.valobj.GetChildMemberWithName('p').GetChildMemberWithName('d').GetChildMemberWithName('array').CreateChildAtOffset('[' + str(index) + ']', pBegin + index * voidSize, type_)
        except:
            print("boned getchild")
        return None


def qstring_summary(value, unused):
    try:
        d = value.GetChildMemberWithName(b'd')
        # have to divide by 2 (size of unsigned short = 2)
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
