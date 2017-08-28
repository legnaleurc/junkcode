class CommandHandler(object):

    def __init__(self, fs, channel_handler):
        self._fs = fs
        self._current_folder = '/'
        self._current_mode = 'I'
        self._channel_handler = channel_handler
        self._data_channel = None

    async def handle_command_line(self, line):
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
        return await actor(self, args)

    def done(self, code, **kwargs):
        if code == 200:
            return '{0} Command OK.'.format(code)
        if code == 215:
            return '{0} {1} Type: {2}'.format(code, kwargs['id'],
                                              kwargs['type'])
        if code == 220:
            return '{0} Service ready for new user.'.format(code)
        if code == 227:
            return '{0} Entering Passive Mode (h1,h2,h3,h4,p1,p2).'.format(code)
        if code == 230:
            return '{0} User logged in, proceed.'.format(code)
        if code == 250:
            return '{0} Requested file action okay, completed.'.format(code)
        if code == 257:
            return '{0} "{1}" Folder operation.'.format(code, kwargs['path'])
        if code == 502:
            return '{0} Command not implemented.'.format(code)
        if code == 550:
            return '{0} Requested action not taken.'.format(code)
        raise ValueError('undefined code: {0}'.format(code))

    async def do_user(self, args):
        if args == 'anonymous':
            return self.done(230)
        # TODO support authentication
        return self.done(502)

    async def do_syst(self, args):
        return self.done(215, id='UNIX', type='L8')

    async def do_pwd(self, args):
        return self.down(257, path=self._current_folder)

    async def do_type(self, args):
        if args not in ('A', 'I'):
            return self.done(502)
        self._current_mode = args
        return self.done(200)

    async def do_cwd(self, args):
        if not await self._fs.is_folder(args):
            return self.done(550)
        self._current_folder = args
        return self.done(250)

    async def do_pasv(self, args):
        self._data_channel = self._channel_handler.create_passive_listener()
        return self.done(227, self._data_channel.get_address())
