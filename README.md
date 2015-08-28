# pygattosx

A Python Bluetooth Low Energy wrapper for OSX. This is an abstraction layer, which is used by [bleep](https://github.com/matthewelse/bleep) to provide a high-level BLE interface.

## Installation

> Most of the time, you won't need to install this package directly - you should install [bleep](https://github.com/matthewelse/bleep) instead.

The only dependencies for this module are PyObjC, which should be installed by default on OS X, and CoreBluetooth.framework, which is also installed on OS X. As a result, installation is simple:

```bash
git clone https://github.com/matthewelse/pygattosx
cd pygattosx
sudo python setup.py install
```

If you want to work on the development of pygattosx, use this instead:

```bash
git clone https://github.com/matthewelse/pygattosx
cd pygattosx
sudo python setup.py develop
```
