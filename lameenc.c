
#include <Python.h>
#include <lame.h>

/** The name of the module */
static char module_name[] = "lameenc";

/** The docstring description of the module */
static char module_docstring[] =
    "This module provides an interface to encode PCM data into MP3 data.";

/** No methods in the module, only a class */
static PyMethodDef module_methods[] = {
    { NULL, NULL, 0, NULL }
};

/** The module definition */
static struct PyModuleDef lameenc_module = {
    PyModuleDef_HEAD_INIT,
    module_name,
    module_docstring,
    -1,
    module_methods
};

typedef struct {
    PyObject_HEAD
    /* The LAME encoder */
    lame_global_flags* lame;
    /* Whether the encoder has been initialised */
    int initialised;
} EncoderObject;

/**
 * Instantiates the new Encoder class memory
 */
static PyObject* Encoder_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    EncoderObject* self = (EncoderObject*) type->tp_alloc(type, 0);
    if (self != NULL)
    {
        Py_BEGIN_ALLOW_THREADS
        self->lame = lame_init();
        if (self->lame == NULL)
        {
            Py_CLEAR(self);
        }
        lame_set_num_channels(self->lame, 2);
        lame_set_in_samplerate(self->lame, 44100);
        lame_set_brate(self->lame, 128);
        lame_set_quality(self->lame, 2);
        // We aren't providing a file interface, so don't output a blank frame
        lame_set_bWriteVbrTag(self->lame, 0);
        Py_END_ALLOW_THREADS
        self->initialised = 0;
    }
    return (PyObject*) self;
}

/**
 * Destroy the Encoder class
 */
static void Encoder_dealloc(EncoderObject* self)
{
    lame_close(self->lame);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

/**
 * Initialises a new encoder
 */
static int Encoder_init(EncoderObject* self, PyObject* args, PyObject* kwds)
{
    /* Nothing to do */
    return 0;
}

/**
 * Set the number of channels for the encoder
 */
static PyObject* setChannels(EncoderObject* self, PyObject* args)
{
    int channels;

    if (!PyArg_ParseTuple(args, "i", &channels))
    {
        return NULL;
    }

    if (lame_set_num_channels(self->lame, channels) < 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to set the channels");
        return NULL;
    }

    Py_RETURN_NONE;
}

/**
 * Set the bitrate
 */
static PyObject* setBitRate(EncoderObject* self, PyObject* args)
{
    int bitrate;

    if (!PyArg_ParseTuple(args, "i", &bitrate))
    {
        return NULL;
    }

    if (lame_set_brate(self->lame, bitrate) < 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to set the bit rate");
        return NULL;
    }

    Py_RETURN_NONE;
}

/**
 * Set the sample rate
 */
static PyObject* setInSampleRate(EncoderObject* self, PyObject* args)
{
    int insamplerate;

    if (!PyArg_ParseTuple(args, "i", &insamplerate))
    {
        return NULL;
    }

    if (lame_set_in_samplerate(self->lame, insamplerate) < 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to set the input sample rate");
        return NULL;
    }

    Py_RETURN_NONE;
}

/**
 * Set the number of channels for the encoder
 */
static PyObject* setQuality(EncoderObject* self, PyObject* args)
{
    int quality;

    if (!PyArg_ParseTuple(args, "i", &quality))
    {
        return NULL;
    }

    if (lame_set_quality(self->lame, quality) < 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to set the quality");
        return NULL;
    }

    Py_RETURN_NONE;
}

/**
 * Encode a block of PCM data into MP3
 */
static PyObject* encode(EncoderObject* self, PyObject* args)
{
    short int* inputSamplesArray;
    int inputSamplesLength;
    int sampleCount;
    int requiredOutputBytes;
    PyObject *outputArray;
    int channels;

    if (!PyArg_ParseTuple(args, "s#", &inputSamplesArray, &inputSamplesLength))
    {
        return NULL;
    }
    /* inputSamplesArray is a 16-bit PCM integer, but s gives the length in bytes */
    if (inputSamplesLength % 2 != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Input data must be 16-bit PCM data");
        return NULL;
    }
    inputSamplesLength /= 2;

    /* Initialise the encoder if this is our first call */
    if (self->initialised == 0)
    {
        int ret;

        Py_BEGIN_ALLOW_THREADS
        ret = lame_init_params(self->lame);
        Py_END_ALLOW_THREADS

        if (ret >= 0)
        {
            self->initialised = 1;
        }
        else
        {
            PyErr_SetString(PyExc_RuntimeError, "Error initialising the encoder");
            self->initialised = -1;
            return NULL;
        }
    }

    /* The encoder is in an erroneous state */
    if (self->initialised != 1)
    {
        PyErr_SetString(PyExc_RuntimeError, "Encoder not initialised");
        return NULL;
    }
    
    /* Do the encoding */
    channels = lame_get_num_channels(self->lame);
    sampleCount = inputSamplesLength / channels;
    if (inputSamplesLength % channels != 0)
    {
        PyErr_SetString(
            PyExc_RuntimeError, "The input data must be interleaved 16-bit PCM"
        );
        return NULL;
    }
    requiredOutputBytes = sampleCount + (sampleCount / 4) + 7200;
    outputArray = PyByteArray_FromStringAndSize(NULL, requiredOutputBytes);
    if (outputArray != NULL)
    {
        int outputBytes;
        Py_BEGIN_ALLOW_THREADS
        if (channels > 1)
        {
            outputBytes = lame_encode_buffer_interleaved(
                self->lame,
                inputSamplesArray, sampleCount,
                (unsigned char*) PyByteArray_AsString(outputArray), requiredOutputBytes
            );
        }
        else
        {
            outputBytes = lame_encode_buffer(
                self->lame,
                inputSamplesArray, inputSamplesArray, sampleCount,
                (unsigned char*) PyByteArray_AsString(outputArray), requiredOutputBytes
            );
        }
        Py_END_ALLOW_THREADS
        if (outputBytes < 0)
        {
            Py_CLEAR(outputArray);
        }
        else
        {
            if (PyByteArray_Resize(outputArray, outputBytes) == -1)
            {
                Py_CLEAR(outputArray);
            }
        }
    }

    return outputArray;
}

static void silentOutput(const char *format, va_list ap)
{
    return;
}

static PyObject* silence(EncoderObject* self, PyObject* args)
{
    if (lame_set_errorf(self->lame, &silentOutput) < 0 ||
        lame_set_debugf(self->lame, &silentOutput) < 0 ||
        lame_set_msgf(self->lame, &silentOutput) < 0)
    {
        PyErr_SetString(
            PyExc_RuntimeError, "Unable to redirect output to silent function"
        );
        return NULL;
    }
    Py_RETURN_NONE;
}

/**
 * Finalise the the MP3 encoder
 */
static PyObject* flush(EncoderObject* self, PyObject* args)
{
    static const int blockSize = 8 * 1024;
    PyObject* outputArray = NULL;
    if (self->initialised == 1)
    {
        int bytes = 0;
        outputArray = PyByteArray_FromStringAndSize(NULL, blockSize);
        if (outputArray != NULL)
        {
            Py_BEGIN_ALLOW_THREADS
            bytes = lame_encode_flush(
                self->lame,
                (unsigned char*) PyByteArray_AsString(outputArray),
                blockSize
            );
            Py_END_ALLOW_THREADS
            if (bytes > 0)
            {
                if (PyByteArray_Resize(outputArray, bytes) == -1)
                {
                    Py_CLEAR(outputArray);
                }
            }
            self->initialised = 2;            
        }
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, "Not currently encoding");
    }
    return outputArray;
}

/** The methods in the Encoder class */
static PyMethodDef Encoder_methods[] = {
    { "set_channels", (PyCFunction) &setChannels, METH_VARARGS, "Set the number of channels" },
    { "set_quality", (PyCFunction) &setQuality, METH_VARARGS, "Set the encoder quality" },
    { "set_bit_rate", (PyCFunction) &setBitRate, METH_VARARGS, "Set the constant bit rate" },
    { "set_in_sample_rate", (PyCFunction) &setInSampleRate, METH_VARARGS, "Set the input sample rate" },
    { "encode", (PyCFunction) &encode, METH_VARARGS, "Encode a block of PCM data" },
    { "flush", (PyCFunction) &flush, METH_NOARGS, "Flush the last block of MP3 data" },
    { "silence", (PyCFunction) &silence, METH_NOARGS, "Silence the stdout from LAME" },
    { NULL, NULL, 0, NULL }
};

/** The Encoder class type */
static PyTypeObject EncoderType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lameenc.Encoder",             /* tp_name */
    sizeof(EncoderObject),         /* tp_basicsize */
    0,                             /* tp_itemsize */
    (destructor) Encoder_dealloc,  /* tp_dealloc */
    0,                             /* tp_print */
    0,                             /* tp_getattr */
    0,                             /* tp_setattr */
    0,                             /* tp_compare */
    0,                             /* tp_repr */
    0,                             /* tp_as_number */
    0,                             /* tp_as_sequence */
    0,                             /* tp_as_mapping */
    0,                             /* tp_hash */
    0,                             /* tp_call */
    0,                             /* tp_str */
    0,                             /* tp_getattro */
    0,                             /* tp_setattro */
    0,                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,            /* tp_flags */
    "A class that provides access to the LAME MP3 encoder",  /* tp_doc */
    0,                             /* tp_traverse */
    0,                             /* tp_clear */
    0,                             /* tp_richcompare */
    0,                             /* tp_weaklistoffset */
    0,                             /* tp_iter */
    0,                             /* tp_iternext */
    Encoder_methods,               /* tp_methods */
    0,                             /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    (initproc) Encoder_init,       /* tp_init */
    0,                             /* tp_alloc */
    Encoder_new,                   /* tp_new */
};

/**
 * Module initialisation function
 *
 * \return  The created module
 */
PyMODINIT_FUNC PyInit_lameenc(void)
{
    static char EncoderClassName[] = "Encoder";

    /* Create the module with no methods in it */
    PyObject *module = PyModule_Create(&lameenc_module);

    /* Initialise the class */
    if (PyType_Ready(&EncoderType) < 0)
    {
        Py_CLEAR(module);
    }
    else
    {
        /* Add the class to the module */
        Py_INCREF(&EncoderType);
        if (PyModule_AddObject(module, EncoderClassName, (PyObject*) &EncoderType) == -1)
        {
            Py_CLEAR(module);
        }
    }

    return module;
}
