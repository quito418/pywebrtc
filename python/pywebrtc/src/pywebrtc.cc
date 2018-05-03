#include <Python.h>

#include <string>
#include "libwebrtc_hl.hh"

extern const uint32_t WebRTCConnectionSize;

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
      delete self->connection;
      Py_TYPE(self)->tp_free((PyObject*)self);
    }
    
    static int
    PyWebRTCConnection_init(PyWebRTCConnection *self, PyObject *args, PyObject *kwargs) {

      if(sizeof(LibWebRTC::WebRTCConnection) == WebRTCConnectionSize) {
          self->connection = new LibWebRTC::WebRTCConnection{"server"};
	}

      else {
	  uint8_t *mem = new uint8_t[WebRTCConnectionSize];
	  self->connection = new (mem) LibWebRTC::WebRTCConnection{"server"};
	}
	
        return 0;
    }
    static PyObject*
    PyWebRTCConnection_getSDP(PyWebRTCConnection *self){

      PyObject *sdp = PyUnicode_FromString(self->connection->get_offer().c_str());
      return sdp;
    }

    static PyObject*
    PyWebRTCConnection_receiveAnswer(PyWebRTCConnection *self, PyObject *args){

      char *answer;
      if (!PyArg_ParseTuple(args, "s",
			    &answer)){
	        return 0;
      }
      
      self->connection->receiveAnswer(answer);
      Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_receiveOffer(PyWebRTCConnection *self, PyObject *args){
      char *offer;
      if (!PyArg_ParseTuple(args, "s",
			    &offer)){
        return 0;
      }

      PyObject *answer = PyUnicode_FromString(self->connection->receiveOffer(offer).c_str());
      return answer;
    }

   static PyObject*
    PyWebRTCConnection_getICEInformation(PyWebRTCConnection *self){
      PyObject *iceInfo = PyUnicode_FromString(self->connection->getICEInformation().c_str());
      return iceInfo;
    }

    static PyObject*
    PyWebRTCConnection_setICEInformation(PyWebRTCConnection *self, PyObject* py_ice_info){
      char *ice_info;
      if (!PyArg_ParseTuple(py_ice_info, "s",
			    &ice_info)){
          return 0;
      }

      self->connection->setICEInformation(ice_info);
      Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_sendString(PyWebRTCConnection *self, PyObject *args){
      char *message;
     if (!PyArg_ParseTuple(args, "s",
			    &message)){ 
        return 0;
      }

      self->connection->sendString(message);
      Py_RETURN_NONE;
    }

    static PyObject*
    PyWebRTCConnection_dataChannelOpen(PyWebRTCConnection *self){
      if(self->connection->dataChannelOpen()) {
        Py_RETURN_TRUE;
      }
      else {
        Py_RETURN_FALSE;
      }
    }

    static PyObject*
    PyWebRTCConnection_videoStreamOpen(PyWebRTCConnection *self){
      if(self->connection->videoStreamOpen()) {
        Py_RETURN_TRUE;
      }
      else {
        Py_RETURN_FALSE;
      }
    }

    static PyObject*
    PyWebRTCConnection_peerConnectionFailed(PyWebRTCConnection *self){
      if(self->connection->peerConnectionFailed()) {
        Py_RETURN_TRUE;
      }
      else {
        Py_RETURN_FALSE;
      }
    }

    static PyObject*
    PyWebRTCConnection_readFromDataChannel(PyWebRTCConnection *self){
      size_t size = self->connection->dataBuffer().size();
      Py_ssize_t py_size = size;
      PyObject *list = PyList_New(py_size);
      size_t i;
      for(i = 0; i < size; i++) {
          std::string message = self->connection->dataBuffer().at(i);
          PyObject *message_string = PyUnicode_FromString(message.c_str());
          Py_ssize_t index = i;
          PyList_SetItem(list, index, message_string);
      }
      self->connection->clearDataBuffer();
      return list;
    }

    static PyObject*
    PyWebRTCConnection_addStreams(PyWebRTCConnection *self){
      self->connection->addStreams();
      Py_RETURN_NONE;
    }

    static PyMethodDef PyWebRTCConnection_methods[] = {
        {"getSDP", (PyCFunction)PyWebRTCConnection_getSDP, METH_VARARGS,
              "Returns the SDP."
        },
        {"receiveAnswer", (PyCFunction)PyWebRTCConnection_receiveAnswer, METH_VARARGS,
              "Sets remote description using the answer."
        },
        {"receiveOffer", (PyCFunction)PyWebRTCConnection_receiveOffer, METH_VARARGS,
              "Sets remote description using the offer."
        },
        {"getICEInformation", (PyCFunction)PyWebRTCConnection_getICEInformation, METH_VARARGS,
              "Returns the ICE information."
        },
        {"setICEInformation", (PyCFunction)PyWebRTCConnection_setICEInformation, METH_VARARGS,
              "Adds the ICE Candidate to the peer connection."
        },
        {"sendString", (PyCFunction)PyWebRTCConnection_sendString, METH_VARARGS,
              "Sends a string through the data channel."
        },
        {"dataChannelOpen", (PyCFunction)PyWebRTCConnection_dataChannelOpen, METH_VARARGS,
              "Checks if the data channel is now open."
        },
        {"videoStreamOpen", (PyCFunction)PyWebRTCConnection_videoStreamOpen, METH_VARARGS,
              "Checks if the video stream is now open."
        },
        {"peerConnectionFailed", (PyCFunction)PyWebRTCConnection_peerConnectionFailed, METH_VARARGS,
              "Checks if establishing a peer connection has encountered a failure."
        },
        {"readFromDataChannel", (PyCFunction)PyWebRTCConnection_readFromDataChannel, METH_VARARGS,
              "Returns a list of strings received by the data channel in oldest to newest order. Clears the buffer after calling."
        },
        {"addStreams", (PyCFunction)PyWebRTCConnection_addStreams, METH_VARARGS,
              "Adds video streams to the peer connection"
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
