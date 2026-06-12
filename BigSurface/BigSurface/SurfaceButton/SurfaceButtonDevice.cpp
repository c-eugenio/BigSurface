//
//  SurfaceButtonDevice.cpp
//  SurfaceButton
//
//  Copyright © 2018-2020 Le Bao Hiep. All rights reserved.
//

#include "SurfaceButtonDevice.hpp"

#define super IOHIDDevice
OSDefineMetaClassAndStructors(SurfaceButtonDevice, super);

const UInt8 virt_reportDescriptor[] = {
    0x05, 0x0c,       // Usage Page (Consumer)
    0x09, 0x01,       // Usage 1 (kHIDUsage_Csmr_ConsumerControl)
    0xa1, 0x01,       // Collection (Application)
    0x85, 0x02,       //   Report Id (2)
    0x05, 0x0c,       //   Usage Page (Consumer)
    0x95, 0x20,       //   Report Count............ (32)
    0x75, 0x08,       //   Report Size............. (8)
    0x15, 0x00,       //   Logical Minimum......... (0)
    0x26, 0xff, 0x00, //   Logical Maximum......... (255)
    0x19, 0x00,       //   Usage Minimum........... (0)
    0x29, 0xff,       //   Usage Maximum........... (255)
    0x81, 0x00,       //   Input...................(Data, Array, Absolute)
    0xc0,             // End Collection
};

void SurfaceButtonDevice::setEventsEnabled(bool enabled) {
    events_enabled = enabled && started && !terminating && !shutdown;
}

bool SurfaceButtonDevice::hasOpenClient() const {
    return client_open;
}

IOReturn SurfaceButtonDevice::simulateKeyboardEvent(UInt32 usagePage, UInt32 usage, bool status) {
    IOReturn result = kIOReturnError;

    if (!started || terminating || shutdown || !events_enabled)
        return kIOReturnOffline;
    
    if (usagePage == kHIDPage_Consumer) {
        if (status)
            csmrreport.keys.insert(usage);
        else
            csmrreport.keys.erase(usage);
        
        if (auto buffer = IOBufferMemoryDescriptor::withBytes(&csmrreport, sizeof(csmrreport), kIODirectionNone)) {
            result = handleReport(buffer, kIOHIDReportTypeInput, kIOHIDOptionsTypeNone);
            OSSafeReleaseNULL(buffer);
        }
    } else
        result = kIOReturnBadArgument;
    
    return result;
}

bool SurfaceButtonDevice::handleStart(IOService *provider) {
    if (!super::handleStart(provider)) {
        return false;
    }

    started = true;
    terminating = false;
    shutdown = false;
    events_enabled = true;
    client_open = false;

    setProperty("Built-In", kOSBooleanTrue);
    setProperty("HIDDefaultBehavior", kOSBooleanTrue);

    return true;
}

bool SurfaceButtonDevice::handleOpen(IOService *client, IOOptionBits options, void *argument) {
    if (!super::handleOpen(client, options, argument))
        return false;

    client_open = true;
    setEventsEnabled(true);
    return true;
}

void SurfaceButtonDevice::handleClose(IOService *client, IOOptionBits options) {
    client_open = false;
    events_enabled = false;
    super::handleClose(client, options);
}

void SurfaceButtonDevice::handleStop(IOService *provider) {
    terminating = true;
    events_enabled = false;
    client_open = false;
    started = false;
    super::handleStop(provider);
}

bool SurfaceButtonDevice::willTerminate(IOService *provider, IOOptionBits options) {
    terminating = true;
    events_enabled = false;
    client_open = false;
    return super::willTerminate(provider, options);
}

IOReturn SurfaceButtonDevice::message(UInt32 type, IOService *provider, void *argument) {
    switch (type) {
        case kIOMessageSystemWillPowerOff:
        case kIOMessageSystemWillRestart:
            shutdown = true;
            events_enabled = false;
            break;
        default:
            break;
    }
    return super::message(type, provider, argument);
}

IOReturn SurfaceButtonDevice::newReportDescriptor(IOMemoryDescriptor **descriptor) const {
    *descriptor = IOBufferMemoryDescriptor::withBytes(virt_reportDescriptor, sizeof(virt_reportDescriptor), kIODirectionNone);
    return kIOReturnSuccess;
}

OSString *SurfaceButtonDevice::newManufacturerString() const {
    return OSString::withCString("Microsoft Inc.");
}

OSString *SurfaceButtonDevice::newProductString() const {
    return OSString::withCString("Surface Button HID Device");
}

OSNumber *SurfaceButtonDevice::newVendorIDNumber() const {
    return OSNumber::withNumber(0x5ac, 32);
}

OSNumber *SurfaceButtonDevice::newProductIDNumber() const {
    return OSNumber::withNumber(0x276, 32);
}

OSNumber *SurfaceButtonDevice::newLocationIDNumber() const {
    return OSNumber::withNumber(0x14400000, 32);
}

OSNumber *SurfaceButtonDevice::newCountryCodeNumber() const {
    return OSNumber::withNumber(0x21, 32);
}

OSNumber *SurfaceButtonDevice::newVersionNumber() const {
    return OSNumber::withNumber(0x895, 32);
}

OSNumber *SurfaceButtonDevice::newPrimaryUsagePageNumber() const {
    return OSNumber::withNumber(kHIDPage_Consumer, 32);
}

OSNumber *SurfaceButtonDevice::newPrimaryUsageNumber() const {
    return OSNumber::withNumber(kHIDUsage_Csmr_ConsumerControl, 32);
}
