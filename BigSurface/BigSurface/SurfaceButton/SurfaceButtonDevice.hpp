//
//  SurfaceButtonDevice.hpp
//  SurfaceButton
//
//  Copyright © 2018-2020 Le Bao Hiep. All rights reserved.
//

#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/IOMessage.h>

#include "HIDReport.hpp"

class SurfaceButtonDevice final : public IOHIDDevice {
    OSDeclareDefaultStructors(SurfaceButtonDevice);
private:
    consumer_input csmrreport;
    bool started {false};
    bool terminating {false};
    bool shutdown {false};
    bool events_enabled {false};
    bool client_open {false};

public:
    void setEventsEnabled(bool enabled);

    bool hasOpenClient() const;

    IOReturn simulateKeyboardEvent(UInt32 usagePage, UInt32 usage, bool status);
    
    bool handleStart(IOService *provider) override;

    bool handleOpen(IOService *client, IOOptionBits options, void *argument) override;

    void handleClose(IOService *client, IOOptionBits options) override;

    void handleStop(IOService *provider) override;

    bool willTerminate(IOService *provider, IOOptionBits options) override;

    IOReturn message(UInt32 type, IOService *provider, void *argument) override;

    IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const override;

    OSString *newManufacturerString() const override;
    OSString *newProductString() const override;
    OSNumber *newVendorIDNumber() const override;
    OSNumber *newProductIDNumber() const override;
    OSNumber *newLocationIDNumber() const override;
    OSNumber *newCountryCodeNumber() const override;
    OSNumber *newVersionNumber() const override;
    OSNumber *newPrimaryUsagePageNumber() const override;
    OSNumber *newPrimaryUsageNumber() const override;
};
