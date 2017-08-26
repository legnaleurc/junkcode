class CommandHandler(object):

    def __init__(self):
        self._current_folder = '/'
        self._current_mode = 'L'

    def handle_command_line(self, line):
        command = line.split(' ', 1)
        if len(command) == 1:
            command = command[0]
            args = ''
        else:
            command, args = command

        method = 'do_{0}'.format(command.lower())
        actor = getattr(self, method, None)
        if not actor:
            return self.done(502)
        return actor(self, args)

    def done(self, code, **kwargs):
        if code == 215:
            return '{0} {1} Type: {2}'.format(code, kwargs['id'],
                                              kwargs['type'])
        if code == 220:
            return '{0} Service ready for new user.'.format(code)
        if code == 230:
            return '{0} User logged in, proceed.'.format(code)
        if code == 257:
            return '{0} "{1}" Folder operation.'.format(code, kwargs['path'])
        if code == 502:
            return '{0} Command not implemented.'.format(code)
        raise ValueError('undefined code: {0}'.format(code))

    def do_user(self, args):
        if args == 'anonymous':
            return self.done(230)
        # TODO support authentication
        return self.done(502)

    def do_syst(self, args):
        return self.done(215, id='UNIX', type='L8')

    def do_pwd(self, args):
        return self.down(257, path=self._current_folder)
