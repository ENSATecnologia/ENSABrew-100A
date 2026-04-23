## MegaCore setup

### 1) Install MegaCore using Arduino Boards Manager

This method requires **Arduino IDE 1.6.4 or newer**.

1. Open the Arduino IDE.
2. Go to **File > Preferences**.
3. Add the following URL in **Additional Boards Manager URLs**:

   ```text
   https://mcudude.github.io/MegaCore/package_MCUdude_MegaCore_index.json
   ```

4. Go to **Tools > Board > Boards Manager...**.
5. Wait for package indexes to finish downloading.
6. Find **MegaCore** and click **Install**.
7. Close the Boards Manager window after installation.

### 2) First-time board configuration

1. Wire your microcontroller according to the [pinout](#pinout).
2. In **Tools > Board**, select **MegaCore** and your target MCU.
3. Select your preferred clock frequency (**16 MHz** is common).
4. Select your programmer in **Tools > Programmer**.
5. Click **Burn Bootloader** to set fuses and bootloader.

### 3) Upload firmware

You can upload in two ways:

1. **Via USB-to-serial adapter**
   - Disconnect ISP programmer.
   - Connect USB-to-serial interface.
   - Select the correct serial port and click **Upload**.
   - If upload times out, check RX/TX wiring and auto-reset circuit.

2. **Via ISP programmer**
   - Keep programmer connected.
   - Hold `Shift` while clicking **Upload**.
   - This uploads directly with the programmer (bootloader is not used).

---

## Pinout

### ATmega2561 (64-pin)

There is no universal Arduino pinout standard for this chip family. MegaCore defines a practical mapping. The default LED pin is Arduino **D13 (PB5)**.

> On ENSAino-100A, this behavior maps to **LED LD5**, which blinks when reset/bootloader activity occurs.

<p align="center">
  <img src="https://i.imgur.com/sweRJs3.jpg" width="350" alt="ATmega2561 pinout from MegaCore" />
</p>

---

## Programmer notes

- Always ensure the selected MCU, clock, and programmer match your hardware setup.
- For time-critical applications, prefer a stable external oscillator.
- After changing fuse-related options, run **Burn Bootloader** again to apply settings.

---

## Credits

- [MCUdude/MegaCore](https://github.com/MCUdude/MegaCore)
