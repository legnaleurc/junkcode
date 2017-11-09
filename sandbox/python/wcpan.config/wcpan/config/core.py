import os.path as op


class Configuration(object):

    def __init__(self, name, model_class):
        self._name = name
        self._model_class = model_class
        # TODO not only YAML
        self._main_file_name = '{}.yaml'.format(name)
        self._root_list = []
        self._base_path = None
        self._file_path = None

        self._root_list.append(op.expanduser('~/.config'))

    def __call__(self):
        ok = self._find_path()
        if not ok:
            data = self._model_class.from_dict({})
        else:
            with open(self._file_path, 'r') as fin:
                data = yaml.safe_load(fin)
            data = self._model_class.from_dict(data)
            set_base_path(data, self._base_path)

        return data

    def _find_path(self):
        if self._base_path and self._file_path:
            return True

        for root in self._root_list:
            base_path = op.join(root, self._name)
            file_path = op.join(base_path, self._main_file_name)
            if op.isfile(file_path):
                self._base_path = base_path
                self._file_path = file_path
                return True

        return False
