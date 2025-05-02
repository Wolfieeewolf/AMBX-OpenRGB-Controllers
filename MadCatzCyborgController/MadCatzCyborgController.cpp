/*---------------------------------------------------------*\
| MadCatzCyborgController.cpp                                 |
|                                                           |
|   Driver for MadCatz Cyborg Gaming Light                  |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "MadCatzCyborgController.h"
#include "LogManager.h"
#include <cstring>

// Device constants
#define MADCATZ_CYBORG_CONFIGURATION 1

// Static position constants definition
const unsigned char MadCatzCyborgController::Position::CENTER[2] = { 0x00, 0x01 };
const unsigned char MadCatzCyborgController::Position::N[2]      = { 0x01, 0x00 };
const unsigned char MadCatzCyborgController::Position::NE[2]     = { 0x02, 0x00 };
const unsigned char MadCatzCyborgController::Position::E[2]      = { 0x04, 0x00 };
const unsigned char MadCatzCyborgController::Position::SE[2]     = { 0x08, 0x00 };
const unsigned char MadCatzCyborgController::Position::S[2]      = { 0x10, 0x00 };
const unsigned char MadCatzCyborgController::Position::SW[2]     = { 0x20, 0x00 };
const unsigned char MadCatzCyborgController::Position::W[2]      = { 0x40, 0x00 };
const unsigned char MadCatzCyborgController::Position::NW[2]     = { 0x80, 0x00 };

MadCatzCyborgController::MadCatzCyborgController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;
}

MadCatzCyborgController::~MadCatzCyborgController()
{
    hid_close(dev);
}

std::string MadCatzCyborgController::GetDeviceLocation()
{
    return(location);
}

std::string MadCatzCyborgController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

/*---------------------------------------------------------*\
| Function: Initialize                                       |
|                                                           |
| Description: Initializes the device, making it ready for  |
|              color commands and turning off all lights    |
|                                                           |
| Parameters: None                                          |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::Initialize()
{
    // Initialize the device
    hid_set_nonblocking(dev, 0);
    
    // USB idle request
    UsbIdleRequest();
    
    // Reset request
    UsbResetRequest();
    
    // Get initial device state
    UsbGetReport();
    
    // Turn off lights initially
    SetLEDColor(0, 0, 0);
}

/*---------------------------------------------------------*\
| Function: UsbIdleRequest                                   |
|                                                           |
| Description: Sends an idle request to the device          |
|              bmRequestType=0x21, bRequest=0x0a           |
|              wValue=0x00, wIndex=0                        |
|                                                           |
| Parameters: None                                          |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::UsbIdleRequest()
{
    unsigned char usb_buf[1] = { 0x00 };
    hid_send_feature_report(dev, usb_buf, 1);
}

/*---------------------------------------------------------*\
| Function: UsbResetRequest                                  |
|                                                           |
| Description: Sends a reset request to the device          |
|              bmRequestType=0x21, bRequest=0x09           |
|              wValue=0x03a7, wIndex=0                      |
|                                                           |
| Parameters: None                                          |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::UsbResetRequest()
{
    unsigned char usb_buf[2] = { CMD_RESET, 0x00 };
    
    hid_send_feature_report(dev, usb_buf, 2);
}

/*---------------------------------------------------------*\
| Function: UsbGetReport                                     |
|                                                           |
| Description: Gets a feature report from the device        |
|              bmRequestType=0xa1, bRequest=0x01           |
|              wValue=0x03b0, wIndex=0                      |
|                                                           |
| Parameters: None                                          |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::UsbGetReport()
{
    unsigned char usb_buf[9] = {0};
    
    hid_get_feature_report(dev, usb_buf, 9);
}

/*---------------------------------------------------------*\
| Function: SetLEDColor                                      |
|                                                           |
| Description: Sets the LED color                           |
|              bmRequestType=0x21, bRequest=0x09           |
|              wValue=0x03a2, wIndex=0                      |
|                                                           |
| Parameters:                                               |
|   red   - Red color component (0-255)                     |
|   green - Green color component (0-255)                   |
|   blue  - Blue color component (0-255)                    |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::SetLEDColor(unsigned char red, unsigned char green, unsigned char blue)
{
    if(dev == nullptr)
    {
        LOG_ERROR("Cannot set LED color - device not initialized");
        return;
    }
    
    LOG_DEBUG("Setting Cyborg LED color to RGB: %d,%d,%d", red, green, blue);
    
    // Create the color command packet
    // Format: [CMD_COLOR][0x00][R][G][B][0x00][0x00][0x00][0x00]
    unsigned char usb_buf[9] = { CMD_COLOR, 0x00, red, green, blue, 0x00, 0x00, 0x00, 0x00 };
    
    // Send feature report to the device
    int result = hid_send_feature_report(dev, usb_buf, 9);
    
    // Verify the command was sent successfully
    if(result < 0)
    {
        LOG_ERROR("Failed to set Cyborg LED color - HID error: %ls", hid_error(dev));
    }
    else if(result != 9)
    {
        LOG_WARNING("Incomplete HID report sent for Cyborg LED color: %d of 9 bytes", result);
    }
}

/*---------------------------------------------------------*\
| Function: SetIntensity                                     |
|                                                           |
| Description: Sets the brightness/intensity of the light   |
|              bmRequestType=0x21, bRequest=0x09           |
|              wValue=0x03a6, wIndex=0                      |
|                                                           |
| Parameters:                                               |
|   intensity - Brightness level (0-100)                    |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::SetIntensity(unsigned char intensity)
{
    if(dev == nullptr)
    {
        LOG_ERROR("Cannot set intensity - device not initialized");
        return;
    }
    
    // Ensure intensity is in range 0-100
    if(intensity > 100)
    {
        LOG_WARNING("Intensity value %d out of range, clamping to 100", intensity);
        intensity = 100;
    }
    
    LOG_DEBUG("Setting Cyborg light intensity to %d%%", intensity);
    
    // Create the intensity command packet
    // Format: [CMD_INTENSITY][0x00][intensity_value]
    unsigned char usb_buf[3] = { CMD_INTENSITY, 0x00, intensity };
    
    // Send feature report to the device
    int result = hid_send_feature_report(dev, usb_buf, 3);
    
    // Verify the command was sent successfully
    if(result < 0)
    {
        LOG_ERROR("Failed to set Cyborg light intensity - HID error: %ls", hid_error(dev));
    }
    else if(result != 3)
    {
        LOG_WARNING("Incomplete HID report sent for Cyborg light intensity: %d of 3 bytes", result);
    }
}

/*---------------------------------------------------------*\
| Function: SetPosition                                      |
|                                                           |
| Description: Sets the position of the light               |
|              bmRequestType=0x21, bRequest=0x09           |
|              wValue=0x03a4, wIndex=0                      |
|                                                           |
| Parameters:                                               |
|   position_byte1 - First byte of position data            |
|   position_byte2 - Second byte of position data           |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::SetPosition(unsigned char position_byte1, unsigned char position_byte2)
{
    unsigned char usb_buf[4] = { CMD_POSITION, 0x00, position_byte1, position_byte2 };
    
    hid_send_feature_report(dev, usb_buf, 4);
}

/*---------------------------------------------------------*\
| Function: SetVerticalPosition                             |
|                                                           |
| Description: Sets the vertical position of the light      |
|              bmRequestType=0x21, bRequest=0x09           |
|              wValue=0x03a5, wIndex=0                      |
|                                                           |
| Parameters:                                               |
|   v_pos - Vertical position (use VerticalPosition enum)   |
|                                                           |
| Returns: None                                             |
\*---------------------------------------------------------*/
void MadCatzCyborgController::SetVerticalPosition(unsigned char v_pos)
{
    unsigned char usb_buf[3] = { CMD_V_POS, 0x00, v_pos };
    
    hid_send_feature_report(dev, usb_buf, 3);
}
