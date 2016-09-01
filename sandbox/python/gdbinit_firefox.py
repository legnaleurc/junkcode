# ex: et ts=4 sts=4 sw=4 :

import gdb


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
            return
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


DereferenceCommand()
