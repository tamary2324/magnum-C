#include <Python.h>

static PyObject* hello(PyObject* self, PyObject* args) {
    printf("Hello from C!\n");
    Py_RETURN_NONE;
}

static PyMethodDef Methods[] = {
    {"hello", hello, METH_NOARGS, "Print hello"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef yourmodule = {
    PyModuleDef_HEAD_INIT,
    "your_module",
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_magc_py(void) {
    return PyModule_Create(&yourmodule);
}

/*

clang -shared -undefined dynamic_lookup -I$(python3-config --includes) your_module.c -o your_module.so
python3 -c "import your_module; your_module.hello()"

*/