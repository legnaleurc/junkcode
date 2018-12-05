#include <stdio.h>

#include <Python.h>

#include "unpack.h"


static PyObject *
py_unpack_to(PyObject * self, PyObject * args) {
    int port = 0;
    const char * http_path = NULL;
    const char * output_path = NULL;

    if (!PyArg_ParseTuple(args, "iss", &port, &http_path, &output_path)) {
        return NULL;
    }

    unpack_to(port, http_path, output_path);

    Py_RETURN_NONE;
}


static PyMethodDef unpack_methods[] = {
    {"unpack_to",  py_unpack_to, METH_VARARGS,
     "Extract archive online."},
    {NULL, NULL, 0, NULL},
};


static struct PyModuleDef unpack_module = {
    PyModuleDef_HEAD_INIT,
    "_unpack",
    NULL,
    -1,
    unpack_methods,
};


PyMODINIT_FUNC
PyInit__unpack(void) {
    return PyModule_Create(&unpack_module);
}
