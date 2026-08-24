# Kernel Driver

this kernel driver communicates with an usermode app via ICOTL.

## Prerequisites TODO: links

1. KDMapper (used to load our driver)
2. Windows SDK/WDK
3. WinDbg
4. a Windows 11 VM

(make sure to disable `Enforce Unsigned Driver Blocklist` in `Device Integrity` section of Windows Security)

## Overview

we expose a device and the usermode app takes this device, sends CTL codes, and from these codes we do certain operations.

the operations include:

* Attaching to a process via its PID
* Enumerating process modules (some apps block usermode enumeration with `MODULEENTRY32` / `TH32CS_SNAPMODULE`)
* Reading memory
* Writing memory

currently this is detected in apex legends because this is the most primitive method EVER.

What we should do in the future is use a KVM (linux), because EAC runs usermode on linux
