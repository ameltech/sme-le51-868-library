# SmartEverything Telit LE51-868  Sigfox complete module
---
This is a library for the Telit LE51-868 component.
A Short Range to Long Range module designed to cover the 863-870 MHz band
working with the Telit Proprietary protocol and acting as a SIGFOX gateway.

[Telit LE51-868 Home Page]
(http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/)

This component use an internal USART to communicate, no extra pin are required.

It was principally designed to work with the SmartEverything board, but could
be easily adapt and use on every Arduino and Arduino Certified boards.

Written by Mik <smkk@axelelettronica.it>.  

## Repository Contents
---
* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager.

## Releases
---
#### v2.1.0 Third Release 18-Jul-2016
* Enachement:<br>
    Unlocked the library from the SME specific UART. The User can change the serial at the begin() method.

#### v2.0.0 Second Release 18-Dec-2015
* Fixed Issue:<br>
    The problem that a message with a 0x00(zero hex) is sent trunked. 
* New Features:<BR> 
    Fw Upgrade from a Sketch. 
    Change internal UART baudRate. 
    Added power Save functionality (need v1.1.0 of SmartEverything core). 
    Added downLink functionality, the SmartEverything is now able to read the SigFox callback<br>

#### v1.0.0 First Release



## Documentation
---
* **[Installing an Arduino Library Guide](http://www.arduino.cc/en/Guide/Libraries#toc3)** - How to install a SmartEverything library on the Arduino IDE using the Library Manager


## License Information
---

Copyright (c) Amel Technology. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA




# About SIGFOX
---
## In a few words
---
**SIGFOX is an operated telecommunication network, dedicated to the Internet of Things.**

It is an operated network, meaning you do not have to handle any installation or maintenance operations.
SIGFOX is seamless and out-of-the box, allowing you to forget about communication and keep focused on the core of your project.

It is a LPWA (Low-Power Wide-Area) network, currently deployed in Western Europe & San Francisco.

The SIGFOX allows a bidirectional communication, both from & to the device.
The communication is always initiated by the device.

The SIGFOX protocol is designed for small messages sent every now and then. It is not appropriate for high-bandwith usages (multimedia, permanent broadcast).

Its focus on energy efficiency allows you to build connected devices able to last years on a standard battery.

## Technology
---
The SIGFOX network operates on sub-GHz frequencies, on ISM bands : 868MHz in Europe/ETSI & 915MHz in the US/FCC.

SIGFOX uses an Ultra-Narrow Band (UNB) modulation, which is key to our ability to provide a scalable & high-capacity network.

With a 162dB path loss SIGFOX enable long range communications, with much longer reach than GSM.

There is no negociation between the device and a receiving station. The device simply emits in the available frequency band (+/- its own frequency shift).
The signal is detected by the closer base stations, decoded & forwarded to the network backend.
Deduplication & other protocol operations are handed by the network itself.
Message is then forwarded to your own application, and accessible using SIGFOX's API. ()

## SIGFOX Specifics
---
**upLink**
- Up to 12 bytes per message
- Up to 140 messages per day

**downLink**
- Up to 8 bytes per message
- Up to 4 messages per day

**Message size**
You can send up to 12 bytes (96 bits) per message. 
The protocol metadata already include a timestamp & the device unique id.

**Number of messages**
The maximum number of messages that can be sent each day is 140. This limitation is in part here to comply with regulations.
The european regulation on the 868MHz band enforce a transmission duty cycle of 1%. 
A unique device is not allowed to emit more than 1% of the time each hour. 
As emission of a message can take up to ~6 seconds, this allows up to 6 messages per hour

## SIGFOX for Makers
---
[SIGFOX Makers Home Page] (http://makers.sigfox.com/)

