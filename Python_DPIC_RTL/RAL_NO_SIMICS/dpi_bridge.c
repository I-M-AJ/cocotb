#include <Python.h>
#include <stdint.h>
#include "svdpi.h"

#ifdef _WIN32
  #define DPI_EXPORT __declspec(dllexport)
#else
  #define DPI_EXPORT
#endif

extern int sv_write_port(const char* port_name, uint64_t val);
extern int sv_read_port(const char* port_name, uint64_t* val);
extern int sv_step_time(int delay_ns);

DPI_EXPORT void c_write_port(const char* port_name, uint64_t val) {
    sv_write_port(port_name, val);
}

DPI_EXPORT uint64_t c_read_port(const char* port_name) {
    uint64_t val = 0;
    sv_read_port(port_name, &val);
    return val;
}

DPI_EXPORT void c_step_time(int delay_ns) {
    sv_step_time(delay_ns);
}

DPI_EXPORT int run_python_test(void) {
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append('.')");
    PyObject *pName = PyUnicode_DecodeFSDefault("driver");
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        PyObject *pFunc = PyObject_GetAttrString(pModule, "run_stimulus");
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pValue = PyObject_CallObject(pFunc, NULL);
            Py_XDECREF(pValue);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
    }
    Py_Finalize();
    return 0;
}
