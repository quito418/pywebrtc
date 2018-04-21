#include <Python.h>

#include <string>
#include "libwebrtc_hl.hh"

extern "C" {

    typedef struct {

        PyObject_HEAD
	LibWebRTC::WebRTCConnection *connection;
        
    } PyWebRTCConnection;

    static PyObject*
    PyWebRTCConnection_new(PyTypeObject *type, PyObject *, PyObject *) {

        PyWebRTCConnection *self;

        self = (PyWebRTCConnection*)type->tp_alloc(type, 0);
        if(self == NULL) {
            PyErr_SetString(PyExc_ValueError, "Could not alloc a new PyWebRTCConnection.");
            return 0;
        }
	
        return (PyObject*) self;
    }

    static void
    PyWebRTCConnection_dealloc(PyWebRTCConnection* self) {

        //delete self->connection;
        Py_TYPE(self)->tp_free((PyObject*)self);
    }
    
    static int
    PyWebRTCConnection_init(PyWebRTCConnection *self, PyObject *args, PyObject *kwargs) {

      
        LibWebRTC::WebRTCConnection my_connection{"server"};
      
        // char *counter_name = NULL;

        // static char *kwlist[] = { "counter_name", NULL };
        // if (!PyArg_ParseTupleAndKeywords(args,
        //                                  kwargs,
        //                                  "s",
        //                                  kwlist,
        //                                  &counter_name)){

        //     PyErr_SetString(PyExc_ValueError, "PyWebRTCConnection failed while parsing constructor args/kwargs.");
        //     return 0;
        // }

        // if(counter_name == NULL) {

        //     PyErr_SetString(PyExc_ValueError, "PyWebRTCConnection requires `counter_name` to be specified.");
        //     return 0;
        // }

        // // std::cerr << "got it: '" << counter_name << "'\n";
        // try {

        //     libperf::PyWebRTCConnection *p = new libperf::PyWebRTCConnection{std::string(counter_name)};
        //     self->counter = p;
        // }
        // catch(const std::exception& e) {

        //     std::string error_message = std::string(e.what());
        //     error_message += std::string(" Try running `pywebrtc.get_available_counters()` to list the available counters on your system.");
        //     PyErr_SetString(PyExc_ValueError, error_message.c_str());
        //     return 0;
        // }
        
        return 0;
    }

    static PyObject*
    PyWebRTCConnection_start(PyWebRTCConnection *self){

        // try {
        //     self->counter->start();
        // }
        // catch(const std::exception& e) {
        //     PyErr_SetString(PyExc_ValueError, e.what());
        //     return 0;
        // }        

        Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_stop(PyWebRTCConnection *self){

        // try {
        //     self->counter->stop();
        // }
        // catch(const std::exception& e) {
        //     PyErr_SetString(PyExc_ValueError, e.what());
        //     return 0;
        // }

        Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_reset(PyWebRTCConnection *self){

        // try {
        //     self->counter->reset();       
        // }
        // catch(const std::exception& e) {
        //     PyErr_SetString(PyExc_ValueError, e.what());
        //     return 0;
        // }

        Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_getSDP(PyWebRTCConnection *self){

        // uint64_t counter_val;
        // try {
        //     counter_val = self->counter->getval();
        // }
        // catch(const std::exception& e) {
        //     PyErr_SetString(PyExc_ValueError, e.what());
        //     return 0;
        // }

        // std::string sdp_response = self->my_object.get_sdp();
        // PyObject *sdp = PyUnicode_FromString(sdp_response.c_str());
       
        PyObject *sdp = PyUnicode_FromString("you guys will put the SDP info here...");
        
        return sdp;
    }

    
    static PyObject*
    PyWebRTCConnection_getval(PyWebRTCConnection *self){

        // uint64_t counter_val;
        // try {
        //     counter_val = self->counter->getval();
        // }
        // catch(const std::exception& e) {
        //     PyErr_SetString(PyExc_ValueError, e.what());
        //     return 0;
        // }

        static_assert(sizeof(uint64_t) <= sizeof(unsigned long long), "sizeof(uint64_t) <= sizeof(long long) must be true");
        PyObject *val = PyLong_FromUnsignedLongLong(static_cast<unsigned long long>(42));
        
        return val;
    }

    static PyMethodDef PyWebRTCConnection_methods[] = {
        {"start", (PyCFunction)PyWebRTCConnection_start, METH_VARARGS,
              "Starts the counter."
        },
        {"stop", (PyCFunction)PyWebRTCConnection_stop, METH_VARARGS,
              "Stops the counter."
        },
        {"reset", (PyCFunction)PyWebRTCConnection_reset, METH_VARARGS,
              "Resets the counter that the subsequent call to `start` will begin at zero."
        },
        {"getval", (PyCFunction)PyWebRTCConnection_getval, METH_VARARGS,
              "Returns current value of the counter."
        },
        {"getSDP", (PyCFunction)PyWebRTCConnection_getSDP, METH_VARARGS,
              "Returen the SDP."
        },
        {NULL, NULL, METH_VARARGS, ""}  /* Sentinel */
    };
    
    static PyTypeObject PyWebRTCConnectionType = {
        PyObject_HEAD_INIT(NULL)
        .tp_name = "pywebrtc.PyWebRTCConnection",
        .tp_basicsize = sizeof(PyWebRTCConnection),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor)PyWebRTCConnection_dealloc,
        .tp_print = 0,
        .tp_getattr = 0,
        .tp_setattr = 0,
        .tp_as_async = 0,
        .tp_repr = 0,
        .tp_as_number = 0,
        .tp_as_sequence = 0,
        .tp_as_mapping = 0,
        .tp_hash = 0,
        .tp_call = 0,
        .tp_str = 0,
        .tp_getattro = 0,
        .tp_setattro = 0,
        .tp_as_buffer = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_doc = "PyWebRTCConnection object",
        .tp_traverse = 0,
        .tp_clear = 0,
        .tp_richcompare = 0,
        .tp_weaklistoffset = 0,
        .tp_iter = 0,
        .tp_iternext = 0,
        .tp_methods = PyWebRTCConnection_methods,
        .tp_members = 0,
        .tp_getset = 0,
        .tp_base = 0,
        .tp_dict = 0,
        .tp_descr_get = 0,
        .tp_descr_set = 0,
        .tp_dictoffset = 0,
        .tp_init = (initproc)PyWebRTCConnection_init,
        .tp_alloc = 0,
        .tp_new = PyWebRTCConnection_new,
        .tp_free = 0,
        .tp_is_gc = 0,
        .tp_bases = 0,
        .tp_mro = 0,
        .tp_cache = 0,
        .tp_subclasses = 0,
        .tp_weaklist = 0,
        .tp_del = 0,
        .tp_version_tag = 0,
        .tp_finalize = 0,
    };

    struct module_state {
        PyObject *error;
    };

    // static PyObject* pywebrtc_get_available_counters(void) {

    //     std::vector<std::string> available_counters;
    //     size_t num_counters;
    //     try {
    //         available_counters = libperf::get_counters_available();
    //         num_counters = available_counters.size();
    //     }
    //     catch(const std::exception& e) {
    //         PyErr_SetString(PyExc_ValueError, e.what());
    //         return 0;
    //     }

    //     if(num_counters == 0){
    //         PyErr_SetString(PyExc_ValueError, "Your system does not expose any performance counters to userspace programs! If you're on Ubuntu, try:\n\n    sudo apt-get install linux-tools-generic\n");
    //         return 0;            
    //     }
        
    //     static_assert(sizeof(Py_ssize_t) >= sizeof(size_t), "sizeof(Py_ssize_t) >= sizeof(size_t) must be true.");
    //     PyObject* available_counters_list = PyList_New(static_cast<Py_ssize_t>(num_counters));
    //     if(available_counters_list == NULL){
    //         PyErr_SetString(PyExc_ValueError, "could not create a new python list to put the available counters into.");
    //         return 0;
    //     }

    //     for(size_t i = 0; i < num_counters; i++) {

    //         PyObject* counter_name = PyUnicode_FromString(available_counters[i].c_str());
    //         if(counter_name == NULL){
    //             PyErr_SetString(PyExc_ValueError, "could not insert counter_name string into python list.");
    //             return 0;
    //         }
    //         PyList_SET_ITEM(available_counters_list, i, counter_name);

    //     }

    //     return available_counters_list;
    // }
    
    static PyMethodDef module_methods[] = {
        // { "get_available_counters", (PyCFunction)pywebrtc_get_available_counters, METH_NOARGS, NULL },
        {NULL, NULL, 0, NULL}  /* Sentinel */
    };
    
    static struct PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pywebrtc",
        .m_doc = "a python library for accessing CPU performance counters on linux.",
        .m_size = sizeof(struct module_state),
        .m_methods = module_methods,
        .m_slots = 0,
        .m_traverse = 0,
        .m_clear = 0,
        .m_free = 0
    };
    
    PyMODINIT_FUNC
    PyInit_pywebrtc(void)
    {

        if (PyType_Ready(&PyWebRTCConnectionType) < 0){
            PyErr_SetString(PyExc_ValueError, "could not intialize PyWebRTCConnection object.");
            return 0;
        }

        PyObject* module = PyModule_Create(&module_def);

        if (module == NULL){
            PyErr_SetString(PyExc_ValueError, "could not create perlib module.");
            return 0;
        }

        Py_INCREF(&PyWebRTCConnectionType);
        PyModule_AddObject(module, "PyWebRTCConnection", (PyObject *)&PyWebRTCConnectionType);

        PyObject *module_namespace = PyModule_GetDict(module);
        PyObject *version = PyUnicode_FromString(PYWEBRTC_VERSION);

        if(PyDict_SetItemString(module_namespace, "__version__", version)){
            PyErr_SetString(PyExc_ValueError, "could not set __version__ of pywebrtc module.");
            return 0;
        }

        return module;
    }

}
