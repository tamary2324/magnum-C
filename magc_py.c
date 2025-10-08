#include <Python.h>

int Cfib(int n){
    if (n < 2)
        return n;
    else
        return Cfib(n-1)+Cfib(n-2);
}

static PyObject* hello(PyObject* self, PyObject* args) {
    printf("Hello from C! *testfunc*\n");
    Py_RETURN_NONE;
}

static PyObject* fib(PyObject* self, PyObject* args) {
    int n;
    if(!PyArg_ParseTuple(args, "i", &n)) // "i" tells Python.h to expect a single integer
        return NULL;
    return Py_BuildValue("i", Cfib(n));
}

static PyMethodDef Methods[] = {
    {"hello", hello, METH_NOARGS, "Print hello"},
    {"fib", fib, METH_VARARGS, "calculate fib"},
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