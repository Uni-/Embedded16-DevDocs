Pot client - RTS SPI Protocol
=============================

iPot DevDoc 001

This document describes serial communication between the pot client and its realtime subsystem.

1. Introduction
---------------

**1.1. Background**

While the pot side of iPot system typically consists a Raspberry Pi 3B with Linux, it cannot guarantee to be reliable for collecting real world sensor data (Linux provides RT_PREEMPT). Furthermore, many sort of sensors, watering and lighting features came out that they have level/shifting issues.

The client *realtime subsystem*, or client RTS for short, is added to solve these issues. Client RTS manages the physical subsystem in real time, running a designated AVR series microprocessor on a specially designed circuit board.

**1.2. SPI**

![](./schematic.png)

Communication between the pot client and RTS is suggested to be run over a duplex serial communication channel called *serial peripheral interface bus* or SPI.

SPI is not a well-designed reliable communication channel, but rather a face-to-face sequential bit-shifting registers with clock edge and enable signal. Therefore it has no slave acknowledgement, no flow control from slave, no parity check.

For details, see [Wikipedia article on SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus).

2. Timing
---------

![](./timing.png)

WIP

3. Payload
----------

**3.1. Request**

fixed width 32bit

![](./frame-request.png)

* **V**: version, must be 0.
* **W**: write to device. (0: read, 1: write)
* **R**: reserved, must be 0.
* **P**: parity, odd; the number of set bit over whole payload should be odd by manipulating this parity bit.
* **RID**: request id, arbitrary value the requesting side assigns.
* **DID**: device id, described in Section 4.
* **data**: parameter to the requesting operation;
  * if W is read, device reading parameter, depending on the device.
  * if W is write, data to write to the device.

**3.2. Response**

fixed width 32bit

![](./frame-response.png)

* **V**: version, must be 0.
* **OK**: result ok from RTS. (0: ok, 1: bad)
* **R**: reserved, must be 0.
* **P**: parity, odd; the number of set bit over whole payload should be odd by manipulating this parity bit.
* **RID**: request id, same value as the corresponding request it follows.
* **DID**: device id, described in Section 4.
* **data**: result of the requested operation it follows;
  * if OK is ok, the read data (req W=r) or the written data (req W=w).
  * if OK is bad, ERRNO, described in Section 5.

4. Device
---------

Every request is an operation, read or write, over a _device_. In other words, each device is an abstraction that represents an accessible field bound to some physical/logical attributes of RTS. Some devices are both readable and writable. Some others are not; they can be readable-only, writable-only, or totally unavailable in some context.

Each device is assigned one byte device ID, used in request and response as DID.

* 2'h0x: information over RTS itself and supporting components
* 2'h1x: sensing on air condition
* 2'h2x: sensing on soil condition
* 2'h3x: sensing on light condition
* 2'h4x: sensing on water condition
* 2'h5x ~ 2'h7x: reserved for further sensing
* 2'h8x: manipulation over RTS itself and supporting components
* 2'h9x: motivating on air condition
* 2'hax: motivating on soil condition
* 2'hbx: motivating on light condition
* 2'hcx ~ 2'hfx: reserved for further motivating

### 2'h00: system alive

This device is readable and writable.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: OK=ok/data=zero if alive and working normal. Some abnormal cases cause no response.

**Write**
* Any writing attempt will restart RTS immediately with no response, if possible. RTS will try to boot normally, but the result is not certain.

### 2'h10: air sensors availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of sensing devices of this series, 1 for available and 0 for not. Device 2'h10 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'h1f assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0007.

### 2'h11: air humidity

This device is readable only.

**Read**
* Request. No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response. Data part of the response represents the relative humidity of air multiplied by 1000; i.e. \[0, 1000\] in integer, dividing it by 10 gets the relative humidity in percentage.

### 2'h12: air temperature

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Data part represents the temperature of air in degree Celsius multiplied by 10; e.g. 273 means 27.3 degree Celsius, so dividing it by 10 gets the relative humidity in percentage.

### 2'h20: soil sensors availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of sensing devices of this series, 1 for available and 0 for not. Device 2'h20 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'h2f assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0003.

### 2'h21: soil humidity

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Data part represents the relative humidity of soil multiplied by 1000; i.e. \[0, 1000\] in integer, dividing it by 10 gets the relative humidity in percentage.

### 2'h30: light sensors availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of sensing devices of this series, 1 for available and 0 for not. Device 2'h30 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'h3f assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0003.

### 2'h31: light intensity

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Data part represents illuminance in Lux, varying over \[0, 40000\] in integer.

### 2'h40: water sensors availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of sensing devices of this series, 1 for available and 0 for not. Device 2'h40 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'h4f assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0007.

### 2'h41: water tank level

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Data part represents the water level; 0(4'h0000) if tank is empty, -1(4'hffff) if tank has water.

### 2'h42: saucer water level

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Data part represents the water level; 0(4'h0000) if saucer is empty, -1(4'hffff) if saucer has water.

### 2'h80: system tick

This device is readable and writable.

### 2'h90: air motivators availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of motivating devices of this series, 1 for available and 0 for not. Device 2'h90 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'h9f assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0003.

### 2'h91: water spray motor

This device is writable only.

**Write**
* Request: Data part represents the duration motor should run to pump up water and spray it over the pot. It is expected to drain water in speed of 0.5ml/s. The value should be in range of \[0, 32767\]; in other words, down to 4'h0000 (inclusive) and up to 4'h7fff (inclusive).
* Response: If the motor is runnable, RTS will respond with OK=ok. Data part should be ignored while it is expected to be zero.

### 2'ha0: soil motivators availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of motivating devices of this series, 1 for available and 0 for not. Device 2'hb0 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'hbf assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0003.

### 2'ha1: water pump motor

This device is writable only.

**Write**
* Request: Data part represents the duration motor should run to pump up water and pour it onto the pot. It is expected to drain water in speed of 0.5ml/s. The value should be in range of \[0, 32767\]; in other words, down to 4'h0000 (inclusive) and up to 4'h7fff (inclusive).
* Response: If the motor is runnable, RTS will respond with OK=ok. Data part should be ignored while it is expected to be zero.

### 2'hb0: other motivators availability

This device is readable only.

**Read**
* Request: No parameter. Data part is slightly ignored, but recommended to be zero-filled.
* Response: Always OK=ok. Data part is an array of bitflags. Each bit represents availability of motivating devices of this series, 1 for available and 0 for not. Device 2'hb0 is assigned to bit 0 (LSB), and continued along devices, reaching device 2'hbf assigned to bit 15 (MSB). When in normal case, data is expected to have value 4'h0007.

### 2'hb1: primary lamp

This device is writable only.

**Write**
* Request: Data part should be 0 or 1 that represents lamp off and on respectively.
* Response: If the lamp is lightable, RTS will respond with OK=ok. Data part should be ignored while it is expected to be zero.

### 2'hb2: secondary lamp

This device is writable only.

**Write**
* Request: Data part should be 0 or 1 that represents lamp off and on respectively.
* Response: If the lamp is lightable, RTS will respond with OK=ok. Data part should be ignored while it is expected to be zero.

5. Error
--------

TBD.

4'h0000: E_STATUS_NONCE.
4'h0000: E_STATUS_WAIT.
4'h0000: E_ERROR_UNKNOWN.
4'h0000: E_ERROR_INITIALIZATION_FAILURE.
4'h0000: E_ERROR_UNSUPPORTED_OPERATION.
