# AMBX-OpenRGB-Controllers

This repository contains OpenRGB controllers for the Philips amBX Gaming lights system and the MadCatz Cyborg Gaming Light.

## Philips amBX Controller

### Overview
The Philips amBX Gaming lights system is an ambient lighting system that includes:
- Left and right satellite lights
- A wallwasher with three lighting zones (left, center, and right)

This controller allows OpenRGB to control these lights, providing a modern interface for hardware that might otherwise be unusable on modern systems.

### Features
- Full support for all five lighting zones of the amBX system
- Direct mode control with per-LED color settings
- Uses standard libusb drivers instead of proprietary Jungo drivers

## MadCatz Cyborg Gaming Light Controller

### Overview
The MadCatz Cyborg Gaming Light is a desk/monitor-mounted ambient light with RGB capability. This controller allows OpenRGB to directly control the color and brightness of this lighting device.

### Features
- Full RGB color control
- Brightness adjustment (0-100%)

## Installation

To use these controllers with OpenRGB:

1. Clone this repository or download the controller files
2. Place the AMBXController and/or MadCatzCyborgController folders in the `Controllers/` directory of your OpenRGB source code
3. Build OpenRGB according to the official instructions
4. Launch OpenRGB to detect and control your devices

### Driver Requirements

#### Windows
- Install the WinUSB driver for the devices using [Zadig](https://zadig.akeo.ie/)
- Note: Installing the WinUSB driver will make the original manufacturer software non-functional

#### Linux
- Ensure you have proper udev rules set up for USB access
- You may need to run OpenRGB with elevated privileges or add udev rules

## Troubleshooting

If OpenRGB fails to detect your devices:
- Ensure the device is properly connected via USB
- Check that the correct drivers are installed
- Verify no other software is currently using the device
- Make sure all lights are properly connected to their respective controllers
- Check the OpenRGB log for specific error messages

## Technical Details

### Philips amBX Controller
- VID/PID: 0x0471/0x083F
- Uses interrupt transfers for communication
- Protocol documented in header files

### MadCatz Cyborg Controller
- VID/PID: 0x06A3/0x0DC5
- Uses HID feature reports for communication
- Supports positioning and brightness control

## License

This project is licensed under GPL-2.0 as part of the OpenRGB project.

## Support

These controllers were developed with AI assistance, which involves subscription and API costs. If you find them useful and would like to support me in making things like this in the future, you're welcome to contribute via:

<a href="https://www.buymeacoffee.com/Wolfieee"><img src="https://img.buymeacoffee.com/button-api/?text=Buy me a pizza&emoji=🍕&slug=Wolfieee&button_colour=40DCA5&font_colour=ffffff&font_family=Poppins&outline_colour=000000&coffee_colour=FFDD00" /></a>

Your support helps maintain the AI tools that make this development possible, but my code will always remain free and open source regardless.

## Acknowledgments

- Thanks to the OpenRGB team for creating and maintaining such a valuable project
- Thanks to the community for testing and providing feedback
- Special thanks to the reverse engineering community for documenting these devices' protocols
