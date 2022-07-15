
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "Globals.h"
#include "Map.h"
#include "Strategy.hpp"
#include <unordered_set>
#include "Gamestate.h"
#include <utility> 



/* Parse python object into c++ objects
   start_lib: 
*/
static PyObject* start_lib(PyObject* self, PyObject* args) {
    cerr << "Started c++ lib" << endl;
    PyObject * p1Units, * p2Units, * p1Stats, * p2Stats, * turnInfo;
    if (!PyArg_ParseTuple(args, "OOOOO", &p1Units, &p2Units, &p1Stats, &p2Stats, &turnInfo)) {
        PyErr_SetString(PyExc_TypeError, "Error parsing arguments");
        return NULL;
    }
    unordered_set<location> upgradedthings;
    Map mymap;
    location temp_loc;
    int x, y, health;
    bool upgraded;
    if (PyList_Check(p1Units) && PySequence_Size(p1Units) == 8) {
        PyObject* units_of_type = PyList_GET_ITEM(p1Units, 7);
        if (PyList_Check(units_of_type)) {
            Py_ssize_t size = PySequence_Size(units_of_type);
            for (int j = 0; j < size; j++) {
                PyObject* unit = PyList_GET_ITEM(units_of_type, j);
                if (PyList_Check(unit) && PySequence_Size(unit) == 4) {
                    x = PyLong_AsLong(PyList_GET_ITEM(unit, 0));
                    y = PyLong_AsLong(PyList_GET_ITEM(unit, 1));
                }
                upgradedthings.insert(loc_fromxy(x, y));
            }
        }
        for (int i = 0; i < 3; i++) {
            PyObject* units_of_type = PyList_GET_ITEM(p1Units, i);
            if (PyList_Check(units_of_type)) {
                Py_ssize_t size = PySequence_Size(units_of_type);
                for (int j = 0; j < size; j++) {
                    PyObject* unit = PyList_GET_ITEM(units_of_type, j);
                    if (PyList_Check(unit) && PySequence_Size(unit) == 4) {
                        x = PyLong_AsLong(PyList_GET_ITEM(unit, 0));
                        y = PyLong_AsLong(PyList_GET_ITEM(unit, 1));
                        health = (int)PyFloat_AsDouble(PyList_GET_ITEM(unit, 2));
                    }
                    temp_loc = loc_fromxy(x, y);
                    upgraded = upgradedthings.find(temp_loc) != upgradedthings.end();
                    mymap.add_unit(x, y, health, (unit_type)i, PLAYER1, upgraded, false);
                }
            }
        }
    }
    if (PyList_Check(p2Units) && PySequence_Size(p2Units) == 8) {
        PyObject* units_of_type = PyList_GET_ITEM(p2Units, 7);
        if (PyList_Check(units_of_type)) {
            Py_ssize_t size = PySequence_Size(units_of_type);
            for (int j = 0; j < size; j++) {
                PyObject* unit = PyList_GET_ITEM(units_of_type, j);
                if (PyList_Check(unit) && PySequence_Size(unit) == 4) {
                    x = PyLong_AsLong(PyList_GET_ITEM(unit, 0));
                    y = PyLong_AsLong(PyList_GET_ITEM(unit, 1));
                }
                upgradedthings.insert(loc_fromxy(x, y));
            }
        }

        for (int i = 0; i < 3; i++) {
            PyObject* units_of_type = PyList_GET_ITEM(p2Units, i);
            if (PyList_Check(units_of_type)) {
                Py_ssize_t size = PySequence_Size(units_of_type);
                for (int j = 0; j < size; j++) {
                    PyObject* unit = PyList_GET_ITEM(units_of_type, j);
                    if (PyList_Check(unit) && PySequence_Size(unit) == 4) {
                        x = PyLong_AsLong(PyList_GET_ITEM(unit, 0));
                        y = PyLong_AsLong(PyList_GET_ITEM(unit, 1));
                        health = (int)PyFloat_AsDouble(PyList_GET_ITEM(unit, 2));
                    }
                    temp_loc = loc_fromxy(x, y);
                    upgraded = upgradedthings.find(temp_loc) != upgradedthings.end();
                    mymap.add_unit(x, y, health, (unit_type)i, PLAYER2, upgraded, false);
                }
            }
        }
    }
    Gamestate mystate;
    if (PyList_Check(p1Stats) && PySequence_Size(p1Stats) == 4) {
        mystate.p1_health = (int)PyFloat_AsDouble(PyList_GET_ITEM(p1Stats, 0));
        mystate.p1_structP = PyFloat_AsDouble(PyList_GET_ITEM(p1Stats, 1));
        mystate.p1_mobileP = PyFloat_AsDouble(PyList_GET_ITEM(p1Stats, 2));
        mystate.p1_time_ms = PyLong_AsLong(PyList_GET_ITEM(p1Stats, 3));
    }
    if (PyList_Check(p2Stats) && PySequence_Size(p2Stats) == 4) {
        mystate.p2_health = (int)PyFloat_AsDouble(PyList_GET_ITEM(p2Stats, 0));
        mystate.p2_structP = PyFloat_AsDouble(PyList_GET_ITEM(p2Stats, 1));
        mystate.p2_mobileP = PyFloat_AsDouble(PyList_GET_ITEM(p2Stats, 2));
        mystate.p2_time_ms = PyLong_AsLong(PyList_GET_ITEM(p2Stats, 3));
    }
    if (PyList_Check(turnInfo) && PySequence_Size(turnInfo) == 4) {
        mystate.turnPhase = PyLong_AsLong(PyList_GET_ITEM(turnInfo, 0));
        mystate.turnNumber = PyLong_AsLong(PyList_GET_ITEM(turnInfo, 1));
        mystate.turnFrame = PyLong_AsLong(PyList_GET_ITEM(turnInfo, 2));
        mystate.turnTotalFrame = PyLong_AsLong(PyList_GET_ITEM(turnInfo, 3));
    }
    final_output main_output({}, {}); // two empty instruction lists
    // calculate strategy based on mymap and mystate and put it into main_output
    Main(&mymap, &mystate, &main_output);
    

    // convert strategy back to python objects
    instruction temp;
    
    Py_ssize_t py_size = main_output.first.size();
    PyObject* py_defend_list;
    py_defend_list = PyList_New(py_size);
    for (int i = 0; i < py_size; i++) {
        PyObject* py_temp_tuple;
        py_temp_tuple = PyTuple_New(3);
        PyObject* py_name, * py_x, * py_y;
        temp = (main_output.first)[i];
        py_name = PyUnicode_New(2, 127);
        py_name = PyUnicode_FromStringAndSize(get<0>(temp).c_str(), 2);
        py_x = PyLong_FromLong(get<1>(temp));
        py_y = PyLong_FromLong(get<2>(temp));
        PyTuple_SetItem(py_temp_tuple, 0, py_name);
        PyTuple_SetItem(py_temp_tuple, 1, py_x);
        PyTuple_SetItem(py_temp_tuple, 2, py_y);
        PyList_SetItem(py_defend_list, i, py_temp_tuple);
    }
    py_size = main_output.second.size();
    PyObject* py_attack_list;
    py_attack_list = PyList_New(py_size);
    for (int i = 0; i < py_size; i++) {
        PyObject* py_temp_tuple;
        py_temp_tuple = PyTuple_New(3);
        PyObject* py_name, * py_x, * py_y;
        temp = (main_output.second)[i];
        py_name = PyUnicode_New(2, 127);
        py_name = PyUnicode_FromStringAndSize(get<0>(temp).c_str(), 2);
        py_x = PyLong_FromLong(get<1>(temp));
        py_y = PyLong_FromLong(get<2>(temp));
        PyTuple_SetItem(py_temp_tuple, 0, py_name);
        PyTuple_SetItem(py_temp_tuple, 1, py_x);
        PyTuple_SetItem(py_temp_tuple, 2, py_y);
        PyList_SetItem(py_attack_list, i, py_temp_tuple);
    }
    PyObject* py_output = PyTuple_New(2);
    PyTuple_SetItem(py_output, 0, py_defend_list);
    PyTuple_SetItem(py_output, 1, py_attack_list);

    return py_output;
}

static PyMethodDef Methods[] = {
    {"main", start_lib, METH_VARARGS},
    {NULL, NULL}  /* Sentinel */
};

static struct PyModuleDef example_extension_module = {
    PyModuleDef_HEAD_INIT,
    "My very own library",
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_Mylib(void) {
    return PyModule_Create(&example_extension_module);
}


