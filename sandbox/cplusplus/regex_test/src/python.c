#include "c.h"

#include <Python.h>


#ifdef __cplusplus
extern "C" {
#endif

PyObject * search_wrapper(PyObject * self, PyObject * args);
PyObject * PyInit_regex_import();

#ifdef __cplusplus
}
#endif


PyObject * c_regex_match_wrapper(PyObject * self, PyObject * args) {
    const char * utf8_pattern = NULL;
    const char * utf8_cell = NULL;
    int rv = PyArg_ParseTuple(args, "ss", &utf8_pattern, &utf8_cell);
    if (!rv) {
        Py_RETURN_FALSE;
    }

    rv = c_regex_match(utf8_pattern, utf8_cell);
    if (rv) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}


static PyMethodDef regex_import_methods[] = {
    {"c_regex_match", c_regex_match_wrapper, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL},
};


static PyModuleDef regex_import_module = {
    PyModuleDef_HEAD_INIT, "regex_import", NULL, -1, regex_import_methods,
    NULL, NULL, NULL, NULL,
};


PyObject * PyInit_regex_import() {
    return PyModule_Create(&regex_import_module);
}
