# BigSurface
The name comes from macOS Big Sur.
Big Sur + Surface -> Big Surface (LOL)

PS : If you have a better name, please let me know.

**A proposition for a fully intergrated kext for all Surface related hardwares**

## How to install

You will need to first **DELETE** all the original **VoodooI2C** kexts including its plugins (all of them), **SMCBatteryManager.kext**, **SMCLightSensor.kext** and **SSDT_ALS0.aml**, then add BigSurface and SSDT-SURFACE.aml into opencore's `config.plist` in the order specified as below<img width="897" alt="截屏2023-02-09 23 22 47" src="https://user-images.githubusercontent.com/18528518/217953005-a00e42ff-39c5-44f8-90e9-4e8efba6b1ea.png">


## Which Surface series are supported ?
Check your UART device id in Windows, currently device id `34a8` is supported perfectly, `9d27` still needs some ameliorations but usable
- Surface Pro 7 & Laptop 3 & Book 3 & Laptop Go 1     -> 34a8
- Surface Pro 4, 5, 6 & Book 2 & Laptop 1, 2    -> 9d27

See my sub-repo `VoodooSerial` for details

## What works
- Surface Type Cover
  > The code is based on VoodooI2CHID.kext, but added **integrated and hot pluggable touchpad&keyboard support**.
- Buttons
  > Power/VolumeUp/VolumeDown buttons all works properly
- Ambient Light Sensor
  > ACPI device name: ACSD, attached under I2C4. **ONLY SP7 and SL3 devices use this hardware** for other devices, you need to write your own driver and then make a pull request. I am happy to merge your driver in :)
- Battery status--Surface Serial Hub
  > UART driver as well as MS's SAM module driver are implemented. 
  > **TODO**: support dual batteries for SB3.
- Performance mode
  > Right now it is set by `PerformanceMode` in `SurfaceBattery` (default 0x01), changing it to other values is not observed to have any effects. If you find any difference (fan speed or battery life), please let me know
  > 
  > Right now it can only be set by changing the plist or using `ioio`
  > We need a userspace software to control it if it actually has something useful.
- Surface Laptop3's keyboard & touchpad
  > Works now, all keys and gestures are recognised properly.
  > 
  > Known issue: neither keyboard nor touchpad can wake up the system.
- Touch Screen & Stylus Yes, this also works :)
  > The code is ported from linux, including `mei` and surface-linux's `ipts` & `iptsd` drivers.
  > 
  > 10 finger touch & stylus
  > 
  > **Important**: need a SSDT patch (SSDT-SURFACE) and a user-space daemon ([IPTSDaemon](https://github.com/Xiashangning/IPTSDaemon)) to work with.
  
Possible values for Performance mode are:

      State              Value
      
    Recommended          0x01
    
    Battery Saver        0x02 (Only in battery mode)
    
    Better Performance   0x03
    
    Best Performance     0x04
    
## TODO
- Cameras                            Impossible so far
  > ACPI devices: CAMR,CAMF,CAM3(infrared camera)
  > 
  > Corresponding device id: OV8865,OV5693,OV7251
  > 
  > Even Linux failed to drive the cameras on SP7 (IPU4), SP6 and before (IPU3) might be possible but I do not have the device.

## Important
Surface Pro 7 and Book 3 seem to have issues for macOS to recognize properly the battery in System Preferences. PM Profile in FACP is tablet not laptop.

Thus, an ACPI patch is needed:

```
Table Signature: 46414350
Find: 00080900 B2000000 F0F1
Replace: 00020900 B2000000 F0F1
```

Thanks to @he1833

## Debugging

BigSurface already uses kext logging through the macros in `BigSurface/BigSurface/helpers.hpp`:

- `LOG(...)` always writes through `IOLog`.
- `DBG_LOG(...)` only writes when the kext is built with `DEBUG=1`, which is enabled by the Xcode Debug configuration.

When debugging an issue, install a Debug build when possible so `DBG_LOG(...)` messages are included. Release builds hide these debug-only messages.

To watch live kernel logs while reproducing the problem:

```sh
sudo log stream --style syslog --level debug --predicate 'process == "kernel" AND (eventMessage CONTAINS "BigSurface" OR eventMessage CONTAINS "Surface")'
```

To save logs from the current boot after reproducing the problem:

```sh
sudo log show --last boot --style syslog --predicate 'process == "kernel" AND (eventMessage CONTAINS "BigSurface" OR eventMessage CONTAINS "Surface")' > ~/Desktop/bigsurface.log
```

Confirm that the kext is loaded:

```sh
kmutil showloaded | grep -i BigSurface
```

Confirm that the expected devices are matched in IORegistry:

```sh
ioreg -l -w0 | grep -i -E 'BigSurface|SurfaceSerialHub|SurfaceBattery|SurfaceButton'
```

If the machine panics or reboots, add these OpenCore boot arguments before reproducing the issue:

```text
-v keepsyms=1 debug=0x100
```

Use the affected subsystem to narrow the logs:

- Keyboard or touchpad: `SurfaceSerialHub` and `SurfaceHIDNub`.
- Battery: `SurfaceBattery`, `SurfaceBatteryNub`, and `SurfaceSMBusController`.
- Buttons: `SurfaceButton`.
- Ambient light sensor: `SurfaceAmbientLightSensor`.
- Touch or stylus/IPTS: `SurfaceManagementEngine`.

If the existing logs are not enough, add temporary `DBG_LOG(...)` calls around attach, probe, start, interrupt handlers, command send/receive paths, and error returns. Prefer logging return codes, target category, command ID, instance ID, sequence ID, buffer length, and event type.

## If you like my project, please consider to star this project, thanks!
### If you appreciate my effort and would like to pay me a coffee, here is my PayPal address: `ritchiexia@163.com`
