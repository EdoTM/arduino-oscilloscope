# Oscilloscope

## Features

- **Real-time plotting.** Thanks to the use of the `PyQtGraph` python library, which allows for fast plotting, the
  voltage on the selected analog pin is plotted in real-time.

- **Conversion of any of the analog channels.** Change analog channel by sending `a<pin>` to the serial, where `<pin>`
  is
  the number of the pin to be used. For example, to use the `A3` pin, send `a3` (default pin: `A0`).

- **Configurable sampling rate.** Change sampling rate by sending `f<rate>` to the serial, where `<rate>` is the number
  of milliseconds per sample. For example, to use a sampling rate of 20 ms per sample, send `f20` (default rate: 15 ms).

- **Voltage switcher.** Pressing a button will switch the voltage on the `D13` pin on and off. This can be used to test
  components both with and without power.

- **Square wave generator.** A square wave is generated on the `D8` pin. Pressing a button will change its period
  cycling
  between 500ms, 1s, 2s and 3s. This can be used to test components that require a square wave input. The current period
  is indicated by two LEDs.

- **Create a file with the measurements.** Using the *dumper* utility, it is possible to create a file with the
  measurements. The file will contain voltage measurements of each sample. The name of the file will be the timestamp
  (also a `latest.txt` symlink is created), and can be visualized using any plotting utility e.g. `gnuplot`. The file
  will be created when the program is closed.

## How to use

1. Connect the Arduino to the computer with the USB cable.
2. Setup hardware (see below).
3. Upload `oscilloscope.hex` to the Arduino.
4. Setup software (see below).
5. Run the `plot.py` file with python to start plotting, or use the *dumper* utility to create a file with the
   measurements.

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

### Plotting

Python version 3.10 or higher is required[^1]. The required libraries can be installed with the following command:

```shell
pip install -r requirements.txt
```

Then, open the `plot.py` file with python. The program will read the serial port and plot the voltage on the `A0` pin in
real-time.

[^1]: Probably lower versions will work too, but they have not been tested.

### Dumper

The *dumper* utility can be used to create a file with the voltage measurements, as described in the *Features* section.
The utility can be compiled with make command:

```shell
make
```

Then, run the `dumper` executable. The program will read the serial port and create a file with the voltage measurements
of each sample. The name of the file will be the timestamp (also a `latest.txt` symlink is created), and can be
visualized using any plotting utility e.g. `gnuplot`. The file will be created when the program is closed.