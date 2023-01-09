#include "usb_tool.h"
#include <stdbool.h>

// usb state manager
enum UsbState usb_state = USB_NOT_MOUNTED;
enum UsbState getUsbState(void) { return usb_state; }
void tud_mount_cb(void) { usb_state = USB_MOUNTED; }
void tud_unmount_cb(void) { usb_state = USB_NOT_MOUNTED; }
void tud_suspend_cb(bool remote_wakeup_en) { usb_state = USB_IDLE; }
void tud_resume_cb(void) { usb_state = USB_MOUNTED; }
