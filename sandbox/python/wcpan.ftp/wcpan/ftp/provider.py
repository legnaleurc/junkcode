class Provider(object):

    def access(self, path, mode):
        raise NotImplementedError()

    def is_folder(self, path):
        raise NotImplementedError()


class LocalFileSystemProvider(Provider):

    def __init__(self, root):
        pass
