/*
    pyxpcconnection: OS X XPC Bindings for Python

    Copyright (c) 2015 Matthew Else

    Based on node-xpc-connection: Copyright Sandeep Mistry 2015

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/raw_function.hpp>

#include "XpcConnection.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(pygattosx) {
    class_<XpcConnection, boost::noncopyable>("XpcConnection", init<std::string>())
        .def("sendMessage", &XpcConnection::sendMessage)
        .def("handler", pure_virtual(&XpcConnection::handler))
    ;

}
