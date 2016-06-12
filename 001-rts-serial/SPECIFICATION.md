Pot client - RTS SPI Protocol
=============================

copied from [handwritten draft](./draft.jpg).

1. Schematic
------------

![](./schematic.png)

2. Timing
---------

\* SPI has no flow control from slave.

![](./timing.png)

3. Payload
----------

**3.1. Request**

fixed width 32bit

![](./frame-request.png)

* **V**: version. must be 0.
* **W**: write to device. (0: read, 1: write)
* **R**: reserved. must be 0.
* **RID**: request id. arbitrary.
* **DID**: device id. described in Section 4.
* **data**: read operation parameter, or data to write to device.

**3.2. Response**

fixed width 32bit

![](./frame-response.png)

* **V**: version. must be 0.
* **OK**: result ok from RTS. (0: ok, 1: bad)
* **R**: reserved. must be 0.
* **RID**: request id. same value as the corresponding request it follows.
* **DID**: device id. described in Section 4.
* **data**:
  * if OK is ok, the read data (req W=r) or the written data (req W=w).
  * if OK is bad, ERRNO, described in Section 5.

4. Device
---------

TBD.

5. Error
--------

TBD.
