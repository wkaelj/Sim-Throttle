#ifndef USB_TOOL_H
#define USB_TOOL_H

enum UsbState
{
    USB_NOT_MOUNTED,
    USB_MOUNTED,
    USB_IDLE,
};

enum UsbState getUsbState(void);

#endif