# STM32 LoRa & GPS Telemetry Board (GoraMi)

This project is an STM32-based hardware design that combines long-range RF communication (LoRa), precise location tracking (GPS), and motion/orientation sensing (IMU) on a single compact PCB.

It is designed to provide a robust hardware infrastructure for vehicle tracking, rocket/UAV telemetry systems, weather stations, and remote IoT applications.

## PCB Preview

| Top (component side) | Bottom (solder side) |
|:---:|:---:|
| ![PCB front render](docs/frontofpcb.png) | ![PCB back render](docs/backofpcb.png) |

## Project Features and Hardware Components

At the heart of the board lies a powerful ARM Cortex-M4 processor, surrounded by industry-standard peripheral modules:

* **Main Processor (MCU):** STM32F411CEU6 (100 MHz, Cortex-M4)
* **Long-Range Communication:** Ebyte E22-400M30S (LoRa, up to 30 dBm / 1 W)
* **Positioning:** u-blox NEO-6M GPS module
* **Motion Sensor (IMU):** MPU-6050 (3-axis accelerometer + gyroscope, I2C)
* **Power Management:** AP2112K-3.3 LDO (5 V USB to regulated 3.3 V)
* **Connections:**
  * USB for power
  * Independent SMA antenna connectors (LoRa and GPS)
  * ST-Link programming and debug header

## Development Progress

| Phase | Status | Notes |
|-------|--------|-------|
| Schematic design | Done | Hierarchical sheets: power, MCU, LoRa, sensors/GPS |
| Custom symbols & footprints | Done | `CompLib/` project-specific library |
| PCB layout (placement & routing) | Done | 2-layer board, RF and power paths routed |
| 3D models & board review | Done | STEP models linked for major ICs and modules |
| Manufacturing outputs | Ready | BOM, pick-and-place, Gerber archive in `production/` |
| Embedded firmware (C) | In progress | GPS, IMU, LoRa drivers; periodic telemetry TX |

A high-resolution schematic PDF is available at [`docs/schematics.pdf`](docs/schematics.pdf).

## Repository Layout

```
├── GoraMi.kicad_pro          # KiCad project entry point
├── GoraMi.kicad_sch          # Root schematic
├── GoraMi.kicad_pcb          # Board layout
├── mcu_core.kicad_sch        # STM32, clock, debug
├── power_management.kicad_sch
├── lora_communication.kicad_sch
├── sensors_and_gps.kicad_sch
├── CompLib/                  # Symbols, footprints, 3D models
├── docs/                     # Schematic PDF, PCB renders
├── production/               # BOM, PnP CSV, Gerber ZIP
├── software/goramİ/          # STM32CubeIDE firmware project
├── fp-lib-table
└── sym-lib-table
```

## Firmware

Firmware lives in **`software/goramİ/`** and is built with **[STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)** (CubeMX project: `goramİ.ioc`).

### Architecture

| Layer | Path | Role |
|-------|------|------|
| Application | `Core/Src/app/` | Telemetry packing, main loop |
| Drivers | `Core/Src/drivers/` | GPS (NMEA), MPU-6050, E22 LoRa (SPI) |
| Board | `Core/Src/board/` | Pin defaults, shared config |

### MCU peripherals

| Peripheral | Pins | Device |
|------------|------|--------|
| USART1 @ 9600 | PA9 TX, PA10 RX | NEO-6M GPS |
| I2C1 | PB6 SCL, PB7 SDA | MPU-6050 |
| SPI2 | PB13-15 | E22-400M30S |
| GPIO | PB12 NSS, PA8 NRST, PA12 BUSY, PA15 DIO1 | LoRa control |

### Build and flash

1. Install STM32CubeIDE and the **STM32Cube FW_F4** pack (v1.28.x matches this project).
2. Clone the repo and import **File -> Import -> Existing Projects into Workspace** -> select `software/goramİ`.
3. **Project -> Build All** (verify `0 errors`).
4. Connect **ST-Link (SWD)** and the board power supply.
5. **Run -> Debug** to flash and run on hardware.

> Firmware targets the physical board. CubeIDE **Run/Debug** requires ST-Link; build-only checks compile without hardware.

### Telemetry output

Every **1 s**, the firmware sends a CSV line over LoRa (values in **milli-units**):

```
T,<lat>,<lon>,<alt>,<fix>,<sats>,<ax>,<ay>,<az>
```

Example: `41123456` = 41.123456 degrees. No GPS fix: `T,NA,NA,NA,0,...`

Default LoRa settings (configurable in `Core/Inc/board/board_config.h`): channel **23** (~433 MHz band), transparent TX via E22 SPI protocol.

## Hardware - KiCad

The PCB was developed with **[KiCad 10.0.1](https://www.kicad.org/)**.

1. Install [KiCad 10.0.1](https://www.kicad.org/download/).
2. Clone the repository:
   ```bash
   git clone https://github.com/usarrahim/stm32-lora-gps-tracker.git
   cd stm32-lora-gps-tracker
   ```
3. Open `GoraMi.kicad_pro` in KiCad.

## Manufacturing

| File | Description |
|------|-------------|
| `production/GoraMi.csv` | Bill of materials |
| `production/GoraMi-all-pos.csv` | Pick-and-place (centroid) |
| `production/GoraMi-production.zip` | Gerber, drill, and job files for fab |

## Roadmap

- [x] Design the electrical schematic
- [x] Import custom symbols and footprints
- [x] Optimize PCB component placement
- [x] Route signal and power traces
- [x] Generate manufacturing files (Gerber / BOM / Pick & Place)
- [x] Initial firmware: GPS NMEA, MPU-6050, E22 LoRa telemetry

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.