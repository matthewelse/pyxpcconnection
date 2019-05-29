#  pyxpcconnection: OS X XPC Bindings for Python
#
#  Copyright (c) 2015 Matthew Else
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#      http://www.apache.org/licenses/LICENSE-2.0
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

from setuptools import setup, Extension
import sys
import glob


def check_system(systems, message):
    import sys

    if sys.platform in systems:
        return
    print(message)
    sys.exit(1)


OTHER_OS_MESSAGE = """
        *****************************************************
        *      pyxpcconnection only works on Mac OS X       *
        *                                                   *
        *    if you're using Linux, then try pygattlib      *
        * instead: https://github.com/matthewelse/pygattlib *
        *                                                   *
        *  if you're using Windows, then raise an issue at  *
        *  https://github.com/matthewelse/bleep suggesting  *
        *            that I add Windows support.            *
        *****************************************************
    """

# check that this is being installed on Mac OS X.
check_system(['darwin'], OTHER_OS_MESSAGE)

if sys.version_info.major == 3:
    found_lib = None
    sub_version = sys.version_info.minor
    general_search = "*boost_python*3*.a"
    preferred_search = "*boost_python*3*{}*".format(sub_version)
    for search_term in [preferred_search, general_search]:
        results = glob.glob("/usr/local/lib/{}".format(search_term))
        if results:
            found_lib = results[0]
    if found_lib:
        boost_libs = [found_lib]
    else:
        raise Exception(
            "Boost library for python3 not found in /usr/local/lib. Please install boost-python3."
        )
else:
    boost_libs = ["boost_python"]

modules = [
    Extension(
        'xpcconnection',
        ['src/XpcConnectionBase.cpp',
         'src/bindings.cpp'],
        extra_compile_args=['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.9'],
        libraries = boost_libs
    )
]

setup(
    name="pyxpcconnection",
    version="0.0.1",
    author="Matthew Else",
    author_email="matthew.else@arm.com",
    description="Python XPC Library for Mac OSX",
    license="Apache-2.0",
    keywords="ble",
    url="about:blank",
    ext_modules=modules,
)
