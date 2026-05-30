# Spyder TKL Solder Firmware with VIA

## Flashing Keyboard

 1. Open the *plywrks_ply8x_solder_via.bin* file with [QMK Toolbox](https://qmk.fm/toolbox).
 2. Enter bootloader mode on the keyboard via 3 ways:
	 -    **Bootmagic reset**: Hold down the Esc key and plug in the keyboard.
	 -    **Physical reset button**: Set the slide switch to  `BOOTLOADER`, then briefly press the button on the back of the PCB.
	 -    **Keycode in layout**: Press the key mapped to  `QK_BOOT`  if it is available.
3. Once in bootloader mode, click the "Flash" button in QMK toolbox.

## Upload Design File to VIA
1. Go to [VIA](https://www.usevia.app/) and enable the  **Design** tab under settings.
2. Upload the *via_definition.json* file in the Design tab.
3. Pair keyboard to VIA.

Indicator customisation for the Caps Locks and Scroll Lock should appear under the "INDICATORS" tab.
