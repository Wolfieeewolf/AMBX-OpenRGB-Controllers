/*---------------------------------------------------------*\
| AMBXControllerDetect.cpp                                  |
|                                                           |
|   Detector for Philips amBX Gaming lights                 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "Detector.h"
#include "LogManager.h"
#include "AMBXController.h"
#include "RGBController.h"
#include "RGBController_AMBX.h"
#include "ResourceManager.h"

#ifdef _WIN32
#include "dependencies/libusb-1.0.27/include/libusb.h"
#else
#include <libusb.h>
#endif

/******************************************************************************************\
*                                                                                          *
*   DetectAMBXControllers                                                                  *
*                                                                                          *
*       Detect Philips amBX Gaming devices                                                 *
*                                                                                          *
\******************************************************************************************/

void DetectAMBXControllers()
{
    libusb_context* ctx = NULL;

    if(libusb_init(&ctx) < 0)
    {
        return;
    }

    libusb_device** devs;
    libusb_device* dev;
    libusb_device_handle* dev_handle;
    ssize_t num_devs;

    num_devs = libusb_get_device_list(ctx, &devs);

    if(num_devs <= 0)
    {
        return;
    }

    for(ssize_t i = 0; i < num_devs; i++)
    {
        dev = devs[i];

        libusb_device_descriptor desc;
        if(libusb_get_device_descriptor(dev, &desc) != 0)
        {
            continue;
        }

        if(desc.idVendor == AMBX_VID && desc.idProduct == AMBX_PID)
        {
            uint8_t bus = libusb_get_bus_number(dev);
            uint8_t address = libusb_get_device_address(dev);

            char device_path[64];
            snprintf(device_path, 64, "%d-%d", bus, address);

            int ret = libusb_open(dev, &dev_handle);

            if(ret < 0)
            {
                continue;
            }

            libusb_close(dev_handle);

            AMBXController* controller = new AMBXController(device_path);

            if(controller->IsInitialized())
            {
                RGBController_AMBX* rgb_controller = new RGBController_AMBX(controller);
                ResourceManager::get()->RegisterRGBController(rgb_controller);
            }
            else
            {
                delete controller;
            }
        }
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
}

REGISTER_DETECTOR("Philips amBX", DetectAMBXControllers);
