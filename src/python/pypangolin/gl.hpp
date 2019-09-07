#pragma once

#include <pybind11/pybind11.h>

namespace py_pangolin {

  void bind_gl(pybind11::module & m);

}  // py_pangolin