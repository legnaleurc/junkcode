import socket
import subprocess
import os.path

import tornado.tcpserver
import tornado.ioloop
import tornado.iostream

import tornado.locks


class FTPServer(tornado.tcpserver.TCPServer):

    def __init__(self, path):
        super().__init__()
        self.path = path
        self._loop = tornado.ioloop.IOLoop.current()

    async def handle_stream(self, stream, address):
        session = ControlChannel(self, stream, address)
        self._loop.add_callback(session.start)


class PassiveListener(tornado.tcpserver.TCPServer):

    def __init__(self, client_ip):
        super().__init__()
        self.client_ip = client_ip
        self.stream = None
        self._ready_lock = tornado.locks.Condition()

        self.bind(0, family=socket.AF_INET)
        self.start()

    def get_socket(self):
        print(list(self._sockets.values()))
        return list(self._sockets.values())[0]

    def format_host(self):
        addr = socket.gethostbyname(socket.gethostname())
        port = self.get_socket().getsockname()[1]
        result = addr.replace(".", ",")
        result += "," + str(port // 256)
        result += "," + str(port % 256)
        return "(" + result + ")"

    async def handle_stream(self, stream, addr):
        print("New data connection", addr, self.client_ip)
        self.stream = stream
        self._ready_lock.notify()
        self._ready_lock = None

    async def send(self, data):
        self.stream.write(data)
        self.stream.close()
        self.stop()
        print("Data sent, closing data connection")

    async def wait_for_ready(self):
        await self._ready_lock.wait()


class ControlChannel(object):

    def __init__(self, server, stream, address):
        self.server = server
        self.stream = stream
        self.address = address
        self.encoding = "utf-8"
        self._cwd = '/'
        self._transfer_mode = 'binary'
        self.start_position = 0

    async def writeline(self, value):
        value += "\r\n"
        await self.stream.write(value.encode(self.encoding))
        print('->', value)

    async def readline(self):
        value = await self.stream.read_until(b"\r\n")
        value = value.decode(self.encoding)
        value = value.rstrip("\r\n")
        return value

    async def start(self):
        print("Incoming connection from {}".format(self.address))
        await self.writeline("220 Service ready for new user.")
        self.running = True
        while self.running:
            try:
                await self.handle_command()
            except tornado.iostream.StreamClosedError:
                self.stop()

    def stop(self):
        print("Closing connection from {}".format(self.address))
        self.running = False
        self.stream.close()

    async def handle_command(self):
        command = await self.readline()
        print("Received command: " + command)
        command = command.split(" ", 1)
        if len(command) == 1:
            command = command[0]
            parameters = ""
        else:
            command, parameters = command

        if command == "USER":
            await self.writeline("230 User logged in, proceed.")

        elif command == "SYST":
            await self.writeline("215 UNIX Type: L8")

        elif command == "FEAT":
            await self.writeline("211-")
            await self.writeline(" PASV")
            await self.writeline(" REST")
            await self.writeline("211 ")

        elif command == "PWD":
            await self.writeline('257 "{}"'.format(self._cwd))

        elif command == "CWD":
            self._cwd = parameters
            await self.writeline('250 Requested file action okay, completed.')

        elif command == "TYPE":
            await self.writeline('200 __TYPE_OK__')

        elif command == "PASV":
            self.data_connection = PassiveListener(self.address[0])
            await self.writeline("227 Entering Passive Mode " + self.data_connection.format_host() + ".")
            await self.data_connection.wait_for_ready()

        elif command == "LIST":
            await self.writeline("150 File status okay; about to open data connection.")
            await self.data_connection.send(
                subprocess.check_output(["ls", "-l", self.server.path]))
            await self.writeline("226 Closing data connection.")

        elif command == "RETR":
            await self.writeline("150")
            filename = os.path.basename(parameters)
            # Wait for opened data connection ?
            fh = open(os.path.join(self.server.path, filename), "rb")
            fh.seek(self.start_position)
            await self.data_connection.send(fh.read())
            self.start_position = 0
            await self.writeline("226")

        elif command == "REST":
            self.start_position = int(parameters)
            await self.writeline("350")

        elif command == "QUIT":
            await self.writeline("221 Service closing control connection.")
            self.close()

        else:
            await self.writeline("502 Command not implemented.")
