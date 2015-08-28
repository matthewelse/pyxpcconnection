"""
 pygattosx: BLE GATT Library for Mac OS X

 Copyright (c) 2015 Matthew Else

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

from __future__ import print_function

import time
import objc
from PyObjCTools import AppHelper

objc.loadBundle('CoreBluetooth', globals(), bundle_path=objc.pathForFramework(u'/System/Library/Frameworks/IOBluetooth.framework/Versions/A/Frameworks/CoreBluetooth.framework'))

class CentralManagerDelegate:
    def __init__(self):
        self.manager = None
        self.start_time = time.time()
        self.discovered_peripherals = {}

    def centralManagerDidUpdateState_(self, manager):
        options = NSDictionary.dictionaryWithDictionary_({
            #0: CBCentralManagerScanOptionAllowDuplicatesKey
        })
        self.manager = manager
        self.manager.scanForPeripheralsWithServices_options_(None, None)

    def centralManager_didDiscoverPeripheral_advertisementData_RSSI_(self, manager, peripheral, data, rssi):
        advertised_uuids = list(data.objectForKey_('kCBAdvDataServiceUUIDs') or [])
        advertised_uuids = [uuid.UUIDString() for uuid in advertised_uuids]

        name = peripheral.name()
        uuids = advertised_uuids

        device = {
            'uuids': uuids,
            'name': name,
            'flags': 0,
            'appearance': 0
        }

        self.discovered_peripherals[peripheral.identifier().UUIDString()] = device

        if time.time() - self.start_time > 5:
            # timeout
            print(self.discovered_peripherals)
            self.manager.stopScan()

delegate = CentralManagerDelegate()
manager = CBCentralManager.alloc()
manager.initWithDelegate_queue_(delegate, None)

AppHelper.runConsoleEventLoop()
