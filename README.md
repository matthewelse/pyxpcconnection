# pyxpcconnection

A Python wrapper for XPC in OS X. 

pyxpcconnection is a port of node-xpc-connection by Sandeep Mistry to Python.

## Installation

> Most of the time, you won't need to install this package directly - you should install [bleep](https://github.com/matthewelse/bleep) instead.

All of pyxpcconnection's dependencies are included with OS X, so installation is simple:

```bash
git clone https://github.com/matthewelse/pyxpcconnection
cd pyxpcconnection
sudo python setup.py install
```

If you are compiling and running using Python3 installed via HomeBrew run:

```bash
brew install boost-python --with-python3
ln -s /usr/local/lib/libboost_python3.a /usr/local/lib/libboost_python-py34.a
``` 

If you want to work on the development of pyxpcconnection, use this instead:

```bash
git clone https://github.com/matthewelse/pyxpcconnection
cd pyxpcconnection
sudo python setup.py develop
```
