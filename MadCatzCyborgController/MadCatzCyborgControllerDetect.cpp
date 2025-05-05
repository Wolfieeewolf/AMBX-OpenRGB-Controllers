/*---------------------------------------------------------*\
| MadCatzCyborgControllerDetect.cpp                         |
|                                                           |
|   Detector for MadCatz Cyborg Gaming Light                |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "Detector.h"
#include "MadCatzCyborgController.h"
#include "RGBController.h"
#include "RGBController_MadCatzCyborg.h"
#include <hidapi.h>

/*-----------------------------------------------------*\
| MadCatz Cyborg VID/PID                               |
\*-----------------------------------------------------*/
#define MADCATZ_VID        0x06A3
#define MADCATZ_CYBORG_PID 0x0DC5

/******************************************************************************************\
*                                                                                          *
*   DetectMadCatzCyborgControllers                                                         *
*                                                                                          *
*       Tests the USB address to find MadCatz Cyborg Gaming Light devices                  *
*                                                                                          *
\******************************************************************************************/

void DetectMadCatzCyborgControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);
    
    if(dev)
    {
        try
        {
            MadCatzCyborgController* controller = new MadCatzCyborgController(dev, info->path);
            controller->Initialize();
            
            RGBController_MadCatzCyborg* rgb_controller = new RGBController_MadCatzCyborg(controller);
            
            // Include device path in name to differentiate multiple devices
            rgb_controller->name = name + " at " + std::string(info->path);
            
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        }
        catch(...)
        {
            // If initialization fails, close the device
            hid_close(dev);
        }
    }
}

REGISTER_HID_DETECTOR("MadCatz Cyborg Gaming Light", DetectMadCatzCyborgControllers, MADCATZ_VID, MADCATZ_CYBORG_PID);
