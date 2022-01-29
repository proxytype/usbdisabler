# USB-Disabler
Disable USB devices for protection in user-space.
![alt text](https://raw.githubusercontent.com/proxytype/usbdisabler/main/usbdisabler.gif)
Use WIN32 api to get and set devices using specific hardware id for USB devices like storage device, hubs and more,
in our tests usb keyboards and mouses are not affected because they defined in different hardware class guid.

## Win32 Objects And Structures
[- SP_DEVINFO_DATA](https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_devinfo_data)<br>
[- HDEVINFO](https://docs.microsoft.com/en-us/windows-hardware/drivers/install/device-information-sets)<br>
[- DEVPROPTYPE](https://docs.microsoft.com/en-us/previous-versions/ff543546(v=vs.85))<br>
[- SP_PROPCHANGE_PARAMS](https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_propchange_params)<br>

## Win32 Functions
 - SetupDiGetClassDevs
 - SetupDiEnumDeviceInfo
 - SetupDiSetClassInstallParams
 - SetupDiCallClassInstaller
 - SetupDiGetDeviceRegistryPropertyA

TODO:
- add support to other classes guid.
