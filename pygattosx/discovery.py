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

import objc

objc.loadBundle('CoreBluetooth', globals(), bundle_path=objc.pathForFramework(u'/System/Library/Frameworks/IOBluetooth.framework/Versions/A/Frameworks/CoreBluetooth.framework'))

# TODO: make this into an Enum
CBCentralManagerStateUnknown        = 0
CBCentralManagerStateResetting      = 1
CBCentralManagerStateUnsupported    = 2
CBCentralManagerStateUnauthorized   = 3
CBCentralManagerStatePoweredOff     = 4
CBCentralManagerStatePoweredOn      = 5
CBCentralManagerStatePoweredOn      = 6

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
            self.manager.stopScan()

class DiscoveryService:
    def __init__(self, device):
        # ignore the device, we don't really care about that
        self.delegate = CentralManagerDelegate()

        self.manager = CBCentralManager.alloc()
        self.manager.initWithDelegate_queue_(self.delegate, None)

    def discover(self, timeout):
        # start scanning
        if self.manager.state == CBCentralManagerStatePoweredOn:
            self.manager.scanForPeripheralsWithServices_options_(None, None)

            # TODO: wait for timeout

            return self.delegate.discovered_peripherals
        else:
            raise RuntimeError("BLE Device is not ready")
