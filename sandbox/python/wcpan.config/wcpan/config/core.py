import os.path as op


class Configuration(object):

    def __init__(self, name, model_class):
        self._name = name
        self._model_class = model_class
        # TODO not only YAML
        self._main_file_name = '{}.yaml'.format(name)
        self._base_path = None
        self._file_path = None
        # possible path
        self._root_list = [
            op.expanduser('~/.config'),
        ]

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


class FileField(wm.BaseField):

    def to_python(self):
        return FileFieldHelper(self.data)


class ModelFileField(wm.BaseField):

    def __init__(self, wrapped_class, **kwargs):
        super(ModelFileField, self).__init__(**kwargs)

        self._wrapped_class = wrapped_class

    def to_python(self):
        raise NotImplementedError()


class FileFieldHelper(object):

    def __init__(self, file_path):
        self._file_path = file_path
        self._base_path = None

    def _set_base_path(self, base_path):
        self._base_path = base_path

    def _get_path(self):
        if op.isabs(self._file_path):
            path = self._file_path
        elif self._base_path is None:
            # TODO show path candidate
            raise ValueError('`{0}` not found'.format(self._file_path))
        else:
            path = op.join(self._base_path, self._file_path)
        return path

    def open_(self, mode):
        path = self._get_path()
        return open(path, mode)

    @property
    def is_file(self):
        path = self._get_path()
        return op.isfile(path)

    @property
    def path(self):
        path = self._get_path()
        return path


def set_base_path(model, base_path):
    fields = ((key, getattr(model, key))
              for key in model._fields.keys()
              if hasattr(model, key))
    for key, value in fields:
        if isinstance(value, wm.Model):
            set_base_path(value, base_path)
        elif isinstance(value, FileFieldHelper):
            value._set_base_path(base_path)
