/*
 *   pyxpcconnection: OS X XPC Bindings for Python
 *
 *   Copyright (c) 2015 Matthew Else
 *
 *   Based on node-xpc-connection: Copyright Sandeep Mistry 2015
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#include <iostream>
#include <vector>

#include "XpcConnectionBase.h"

// define these so that prettyprint can do it's magic :)
std::ostream& operator<<(std::ostream& os, const boost::python::object& o)
{
    return os << boost::python::extract<std::string>(boost::python::str(o))();
}

std::ostream& operator<<(std::ostream& os, const boost::python::dict& o)
{
    return os << boost::python::extract<std::string>(boost::python::str(o))();
}

#include "prettyprint.hpp"

XpcConnectionBase::XpcConnectionBase(std::string target) :
    serviceName(target) {
    dispatchQueue = dispatch_queue_create(serviceName.c_str(), 0);
    xpcConnection = xpc_connection_create_mach_service(serviceName.c_str(), dispatchQueue, XPC_CONNECTION_MACH_SERVICE_PRIVILEGED);

    xpc_connection_set_event_handler(xpcConnection, ^(xpc_object_t event) {
        // this seems to fix a segfault (it's never released, so this should probably be a problem...)
        auto gstate = PyGILState_Ensure();

        xpc_retain(event);

        // handle the event
        handleEvent(event);
        PyGILState_Release(gstate);
    });

    xpc_connection_resume(xpcConnection);

}

xpc_object_t XpcConnectionBase::ValueToXpcObject(boost::python::object obj) {
    auto gstate = PyGILState_Ensure();

    xpc_object_t xpcObject = NULL;

    std::string type = boost::python::extract<std::string>(obj.attr("__class__").attr("__name__"));

    //std::cout << "unpacking: " << type << std::endl;

    if (type == "UUID") {
        boost::python::object s = obj.attr("bytes");

        std::string str = boost::python::extract<std::string>(s);

        std::vector<uint8_t> data;

        for (int i = 0; i < str.size(); i++) {
            data.push_back(str.data()[i] & 0xff);
        }

        uuid_t *uuid = (uuid_t *)str.data();

        xpcObject = xpc_uuid_create(*uuid);
    } else if (type == "str") {
        std::string s = boost::python::extract<std::string>(obj);

        xpcObject = xpc_string_create(s.c_str());
    } else if (type == "int") {
        long int_val = boost::python::extract<int>(obj);

        xpcObject = xpc_int64_create(int_val);
    } else if (type == "dict") {
        boost::python::dict dict = boost::python::extract<boost::python::dict>(obj);
        xpcObject = XpcConnectionBase::DictToXpcObject(dict);
    } else if (type == "list") {
        boost::python::list list = boost::python::extract<boost::python::list>(obj);

        xpcObject = XpcConnectionBase::ListToXpcObject(list);
    } else if (type == (PY_MAJOR_VERSION == 3 ? "memoryview" : "buffer")) {
        std::string s = boost::python::extract<std::string>(obj);

        xpcObject = xpc_data_create(s.c_str(), s.size());
    } else if (obj.is_none()) {
        /* empty */
    } else {
        //std::cout << "[xpc] unexpected data type: " << type << std::endl;
    }

    PyGILState_Release(gstate);

    return xpcObject;
}

xpc_object_t XpcConnectionBase::ListToXpcObject(boost::python::list list) {
    xpc_object_t xpcArray = xpc_array_create(NULL, 0);

    for(uint32_t i = 0; i < boost::python::len(list); i++) {
        boost::python::object value = list[i];

        xpc_object_t xpcValue = XpcConnectionBase::ValueToXpcObject(value);

        xpc_array_append_value(xpcArray, xpcValue);
        if (xpcValue) {
            xpc_release(xpcValue);
        }
    }

    return xpcArray;
}

xpc_object_t XpcConnectionBase::DictToXpcObject(boost::python::dict object) {
    xpc_object_t xpcObject = xpc_dictionary_create(NULL, NULL, 0);

    boost::python::object iterator = object.keys();

    boost::python::ssize_t n = boost::python::len(iterator);
    
    for (boost::python::ssize_t i = 0; i < n; i++) {
        boost::python::object key = iterator[i];

        std::string key_s = boost::python::extract<std::string>(key);

        boost::python::object value = object[key];

        xpc_object_t xpcValue = XpcConnectionBase::ValueToXpcObject(value);
        xpc_dictionary_set_value(xpcObject, key_s.c_str(), xpcValue);

        if (xpcValue) {
            xpc_release(xpcValue);
        }
    }

    return xpcObject;
}

boost::python::object XpcConnectionBase::XpcObjectToValue(xpc_object_t xpcObject) {
    auto gstate = PyGILState_Ensure();
    xpc_type_t valueType = xpc_get_type(xpcObject);

    boost::python::object obj;

    if (valueType == XPC_TYPE_INT64) {
        long value = xpc_int64_get_value(xpcObject);
        obj = boost::python::object(value);
    }
    else if (valueType == XPC_TYPE_STRING) {
        std::string value(xpc_string_get_string_ptr(xpcObject));
        obj = boost::python::object(value);
    }
    else if (valueType == XPC_TYPE_DATA) {
        std::string value((char *)xpc_data_get_bytes_ptr(xpcObject), xpc_data_get_length(xpcObject));
        obj = boost::python::object(value);
    }
    else if (valueType == XPC_TYPE_DICTIONARY) {
        obj = XpcConnectionBase::XpcDictToDict(xpcObject);
    }
    else if (valueType == XPC_TYPE_ARRAY) {
        obj = XpcConnectionBase::XpcArrayToList(xpcObject);
    }
    else if (valueType == XPC_TYPE_UUID) {
        std::string buffer((char *)xpc_uuid_get_bytes(xpcObject), sizeof(uuid_t));
        std::string new_buffer;
        new_buffer.assign(buffer);
        obj = boost::python::object(new_buffer);
    }

    PyGILState_Release(gstate);
    return obj;
}

boost::python::dict XpcConnectionBase::XpcDictToDict(xpc_object_t xpcDictionary) {
    __block boost::python::dict d;

    xpc_dictionary_apply(xpcDictionary, ^bool(const char *key, xpc_object_t value) {
        std::string s(key);
        boost::python::object val = XpcConnectionBase::XpcObjectToValue(value);

        d[s] = val;

        return true;
    });

    return d;
}

boost::python::dict XpcConnectionBase::XpcArrayToList(xpc_object_t xpcArray) {
    __block boost::python::dict array;

    xpc_array_apply(xpcArray, ^bool(size_t index, xpc_object_t value) {
        array[index] = XpcConnectionBase::XpcObjectToValue(value);

        return true;
    });

    return array;
}

void XpcConnectionBase::handleEvent(xpc_object_t event) {
    try {
        auto gstate = PyGILState_Ensure();
        // return a tuple containing type, and details
        xpc_type_t eventType = xpc_get_type(event);

        if (eventType == XPC_TYPE_ERROR) {
            std::string message = "unknown";

            if (event == XPC_ERROR_CONNECTION_INTERRUPTED) {
                message = "connection interrupted";
            } else if (event == XPC_ERROR_CONNECTION_INVALID) {
                message = "connection invalid";
            }

            //std::cout << "[xpc] error: " << message << std::endl;

            boost::python::tuple args = boost::python::make_tuple(std::string("error"), message);
            //handler(args);
        } else if (eventType == XPC_TYPE_DICTIONARY) {
            boost::python::object obj = XpcConnectionBase::XpcObjectToValue(event);
            boost::python::tuple args = boost::python::make_tuple(std::string("event"), obj);

            //std::cout << "[xpc] received XPC event: " << obj.attr("__getitem__")("kCBMsgId") << std::endl;

            handler(args);
        } else {
            //std::cout << "[xpc] received something strange..." << std::endl;
        }

        PyGILState_Release(gstate);
        xpc_release(event);
    } catch(boost::python::error_already_set const&) {
        PyErr_Print();
    }
}

void XpcConnectionBase::sendMessage(boost::python::dict obj) {
    auto gstate = PyGILState_Ensure();

    xpc_object_t message = XpcConnectionBase::DictToXpcObject(obj);

    xpc_connection_send_message(xpcConnection, message);

    xpc_release(message);

    PyGILState_Release(gstate);
}
