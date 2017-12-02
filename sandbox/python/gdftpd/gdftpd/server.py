import aioftp


class FtpServer(aioftp.Server):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    async def dispatcher(self, reader, writer):
        try:
            await super().dispatcher(reader, writer)
        except ConnectionResetError as e:
            print('client disconnected')
