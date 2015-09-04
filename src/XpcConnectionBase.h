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
#include <string>

#include <xpc/xpc.h>
#include <dispatch/dispatch.h>

class XpcConnectionBase {

public:
    XpcConnectionBase(std::string target);

    void sendMessage(boost::python::dict message);
    
    virtual void handler(boost::python::object args) = 0;

private:
    void _sendMessage(xpc_object_t message);

    static xpc_object_t ValueToXpcObject(boost::python::object obj);
    static xpc_object_t DictToXpcObject(boost::python::dict object);
    static xpc_object_t ListToXpcObject(boost::python::list array);

    static boost::python::object XpcObjectToValue(xpc_object_t xpcObject);
    static boost::python::dict   XpcDictToDict(xpc_object_t xpcDictionary);
    static boost::python::dict   XpcArrayToList(xpc_object_t xpcArray);

    void handleEvent(xpc_object_t event);

private:
    std::string serviceName;

    dispatch_queue_t dispatchQueue;
    xpc_connection_t xpcConnection;
};
