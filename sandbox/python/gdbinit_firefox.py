# ex: et ts=4 sts=4 sw=4 :

import gdb


class DereferenceCommand(gdb.Command):

    '''
    Dereference.
    '''

    def __init__(self):
        super(DereferenceCommand, self).__init__('pp', gdb.COMMAND_SUPPORT, gdb.COMPLETE_EXPRESSION, False)

    def invoke(self, argument, from_tty):
        args = gdb.string_to_argv(argument)
        name = args[0]

        symbol, is_member = gdb.lookup_symbol(name)
        if not symbol:
            print('`{0}` not found'.format(name))
            return

        if not symbol.is_valid():
            print('`{0}` is invalid'.format(name))
            return

        if symbol.is_function:
            print('`{0}` is a function'.format(name))
            return

        if symbol.needs_frame:
            value = symbol.value(gdb.selected_frame())
        else:
            value = symbol.value()

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

        print('no matching pointer, print raw')
        print(value)


DereferenceCommand()
