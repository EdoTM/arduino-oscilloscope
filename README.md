# Oscilloscope

## Features

- **Real-time plotting.** Thanks to the use of the `PyQtGraph` python library, which allows for fast plotting, the
  voltage on `A0` pin is plotted in real-time.

- **Voltage switcher.** Pressing a button will switch the voltage on the `D13` pin on and off. This can be used to test
  components both with and without power.

- **Square wave generator.** A square wave is generated on the `D8` pin. Pressing a button will change its period
  cycling
  between 500ms, 1s, 2s and 3s. This can be used to test components that require a square wave input. The current period
  is indicated by two LEDs.

## How to use

1. Connect the Arduino to the computer with the USB cable.
2. Upload `oscilloscope.hex` to the Arduino.
3. Run the `plot.py` file with python.

The voltage on the `A0` pin will be plotted in real-time. The components can be powered either via the power pins (5V or
3.3V), the `D13` pin, that can be toggled between 0V and 5V, or the square wave on the `D8` pin.

# Setup

## Hardware

The required hardware is the following:

- 1x Arduino Uno
- 1x USB type A to type B cable (the one used to connect the Arduino to the computer)
- 2x Switches
- 2x 220Ω resistor
- 3x LEDs
- Jumper cables or wires
- Breadboard or similar

### Wiring

The diagram of the circuit is the following:

![Diagram](https://github.com/EdoTM/so-project-2023/assets/19765770/40b66547-bdda-4c7a-9945-6b7479624adf)

- Put a switch between `D12` pin and ground. This will be used activate and deactivate 5V power on the `D13` pin.
- Put a LED between `D11` and ground (with a 220Ω resistor in between). This is optional, as it will be used to indicate
  when the `D13` pin is active.
- Put a switch between `D10` pin and ground. This will be used to change the period of the square wave on pin `D8`.
- Put two LEDs, one between `D6` and ground and the other between `D7` and ground (with a 220Ω resistor in between), as
  shown in the diagram. These will be used to indicate the state of the square wave on pin `D8`.

## Software

Python version 3.10 or higher is required[^1]. The required libraries can be installed with the following command:

```shell
pip install -r requirements.txt
```

Then, open the `plot.py` file with python. The program will read the serial port and plot the voltage on the `A0` pin in
real-time.

[^1]: Probably lower versions will work too, but they have not been tested.