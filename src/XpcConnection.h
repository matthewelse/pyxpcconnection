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
#include "XpcConnectionBase.h"

#include <iostream>

class XpcConnection : public XpcConnectionBase, public boost::python::wrapper<XpcConnectionBase> {
public:
    XpcConnection(std::string target) : XpcConnectionBase(target) {

    };

    void handler(boost::python::object args) {
        this->get_override("handler")(args);
    }
};
