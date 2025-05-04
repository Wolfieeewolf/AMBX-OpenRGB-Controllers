/*---------------------------------------------------------*\
| AMBXController.cpp                                        |
|                                                           |
|   Driver for Philips amBX Gaming lights                   |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "AMBXController.h"
#include "LogManager.h"
#include <cstring>
#include <thread>
#include <chrono>

AMBXController::AMBXController(const char* path)
{
    initialized = false;
    interface_claimed = false;
    usb_context = nullptr;
    dev_handle = nullptr;
    
    location = "USB amBX: ";
    location += path;
    
    int libusb_result = libusb_init(&usb_context);
    if(libusb_result != LIBUSB_SUCCESS)
    {
        return;
    }
    
    libusb_device** device_list;
    ssize_t device_count = libusb_get_device_list(usb_context, &device_list);
    
    if(device_count < 0)
    {
        return;
    }
    
    for(ssize_t i = 0; i < device_count; i++)
    {
        libusb_device* device = device_list[i];
        struct libusb_device_descriptor desc;
        
        if(libusb_get_device_descriptor(device, &desc) != LIBUSB_SUCCESS)
        {
            continue;
        }
        
        if(desc.idVendor == AMBX_VID && desc.idProduct == AMBX_PID)
        {
            uint8_t bus = libusb_get_bus_number(device);
            uint8_t address = libusb_get_device_address(device);
            
            char device_id[32];
            snprintf(device_id, sizeof(device_id), "Bus %d Addr %d", bus, address);
            location = std::string("USB amBX: ") + device_id;
            
            int result = libusb_open(device, &dev_handle);
            if(result != LIBUSB_SUCCESS)
            {
                continue;
            }
            
            if(libusb_kernel_driver_active(dev_handle, 0))
            {
                libusb_detach_kernel_driver(dev_handle, 0);
            }
            
            libusb_set_auto_detach_kernel_driver(dev_handle, 1);
            
            result = libusb_claim_interface(dev_handle, 0);
            if(result != LIBUSB_SUCCESS)
            {
                libusb_close(dev_handle);
                dev_handle = nullptr;
                continue;
            }
            
            interface_claimed = true;
            
            if(desc.iSerialNumber != 0)
            {
                unsigned char serial_str[256];
                int serial_result = libusb_get_string_descriptor_ascii(dev_handle, desc.iSerialNumber, 
                                                                       serial_str, sizeof(serial_str));
                if(serial_result > 0)
                {
                    serial = std::string(reinterpret_cast<char*>(serial_str), serial_result);
                }
            }
            
            initialized = true;
            break;
        }
    }
    
    libusb_free_device_list(device_list, 1);
    
    if(initialized)
    {
        // Initialize all LEDs to off
        unsigned int leds[] = 
        { 
            AMBX_LIGHT_LEFT,
            AMBX_LIGHT_RIGHT,
            AMBX_LIGHT_WALL_LEFT,
            AMBX_LIGHT_WALL_CENTER,
            AMBX_LIGHT_WALL_RIGHT
        };
        
        RGBColor colors[] = { 0, 0, 0, 0, 0 };
        SetLEDColors(leds, colors, 5);
    }
}

AMBXController::~AMBXController()
{
    if(initialized)
    {
        try
        {
            unsigned int leds[] = 
            { 
                AMBX_LIGHT_LEFT,
                AMBX_LIGHT_RIGHT,
                AMBX_LIGHT_WALL_LEFT,
                AMBX_LIGHT_WALL_CENTER,
                AMBX_LIGHT_WALL_RIGHT
            };
            
            RGBColor colors[] = { 0, 0, 0, 0, 0 };
            SetLEDColors(leds, colors, 5);
        }
        catch(...) {}
    }
    
    if(dev_handle != nullptr)
    {
        if(interface_claimed)
        {
            libusb_release_interface(dev_handle, 0);
            interface_claimed = false;
        }
        
        libusb_close(dev_handle);
        dev_handle = nullptr;
    }
    
    if(usb_context != nullptr)
    {
        libusb_exit(usb_context);
        usb_context = nullptr;
    }
}

std::string AMBXController::GetDeviceLocation()
{
    return location;
}

std::string AMBXController::GetSerialString()
{
    return serial;
}

bool AMBXController::IsInitialized()
{
    return initialized;
}

void AMBXController::SendPacket(unsigned char* packet, unsigned int size)
{
    if(!initialized || dev_handle == nullptr || !interface_claimed)
    {
        return;
    }
    
    int actual_length = 0;
    libusb_interrupt_transfer(dev_handle, AMBX_ENDPOINT_OUT, packet, size, &actual_length, 100);
}

void AMBXController::SetLEDColor(unsigned int led, RGBColor color)
{
    if(!initialized)
    {
        return;
    }
    
    unsigned char color_buf[6];
    
    color_buf[0] = AMBX_PACKET_HEADER;
    color_buf[1] = led;
    color_buf[2] = AMBX_SET_COLOR;
    color_buf[3] = RGBGetRValue(color);
    color_buf[4] = RGBGetGValue(color);
    color_buf[5] = RGBGetBValue(color);

    SendPacket(color_buf, 6);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

void AMBXController::SetLEDColors(unsigned int* leds, RGBColor* colors, unsigned int count)
{
    for(unsigned int i = 0; i < count; i++)
    {
        SetLEDColor(leds[i], colors[i]);
    }
}
