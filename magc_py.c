#include <Python.h>

static PyObject* hello(PyObject* self, PyObject* args) {
    printf("Hello from C! *testfunc*\n");
    Py_RETURN_NONE;
}

static PyMethodDef Methods[] = {
    {"hello", hello, METH_NOARGS, "Print hello"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef testmodule = {
    PyModuleDef_HEAD_INIT,
    "testmodule",
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_magc_py(void) {
    return PyModule_Create(&testmodule);
}

/*

clang -shared -undefined dynamic_lookup -I$(python3-config --includes) magc_py.c -o magc_py.so
python3 -c "import magc_py; magc_py.hello()"

*/