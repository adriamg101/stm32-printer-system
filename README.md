# Validation System for an Inkjet Printer — STM32F429
 
Bare-metal C firmware for the **STM32F429I-DISCOVERY** board, developed as a validation rig for the print carriage of an inkjet printer (fictional company "R-Tech" project). The system captures, processes and visualizes in real time the positioning signals and analog sensor data of a moving print head.
 
The project is structured into three incremental modules that form a single firmware:
 
1. **Position encoder management and timing**
2. **Analog data acquisition via DMA**
3. **Graphical visualization on an LCD-TFT display**
---
 
## 1. System context
 
In an inkjet printer, the print head moves along a carriage driven by a motor and a belt. A **quadrature position encoder** (channels A/B) and an **index sensor (IDX)** provide the carriage's position, speed and direction. At the same time, two **analog distance sensors** (mounted at the front and rear of the carriage) measure the gap between the print head nozzles and the paper (*Pen-to-Paper Spacing*), a parameter critical to print quality.
 
The firmware emulates the control and validation subsystem of this mechanism without requiring the real printer hardware (the encoder channels themselves are simulated via GPIOs and timers).
 
---
 
## 2. Module 1 — Position encoder, GPIOs, timers and interrupts
 
**Goal:** detect carriage movement and generate synchronization signals with precise, fully hardware-driven timing.
 
- **Encoder simulation:** generation of two quadrature signals (A/B, 90° phase shift) using timers, with frequency configurable according to the simulated carriage speed (10–20 cm/s), with no CPU intervention.
- **Interrupt-driven handling** of rising/falling edges on A, B and IDX, including detection of the first edge received after index activation (the A/B arrival order is not deterministic).
- **`PULSE` signal generation** (50 µs pulse) synchronized with every encoder edge, using the timer's hardware output (no software toggling) to minimize CPU load.
- **Instantaneous speed calculation** by measuring the interval between edges with a free-running timer counter, meeting a maximum error of ±0.5%.
- **Direction-error detection** (`ERR`) if the movement direction reverses during the printing phase.
- **End-of-print-zone signal (`EOP`)** after covering the defined 60 cm.
- **Performance instrumentation:** measurement of ISR execution time and latency using auxiliary GPIOs and an oscilloscope, plus calculation of the CPU bandwidth consumed by encoder interrupt handling at maximum speed.
**Concepts applied:** general-purpose and advanced STM32F4 timers, input capture, hardware signal generation, external interrupts (EXTI), producer/consumer synchronization between ISR and main loop, timing analysis in real-time embedded systems.
 
---
 
## 3. Module 2 — Analog acquisition (ADC) via DMA
 
**Goal:** efficiently acquire synchronized readings from two distance sensors without blocking the CPU.
 
- Configuration of **2 ADC channels at 12-bit resolution**, hardware-triggered on every relevant encoder edge (200 µm intervals), ensuring a sampling rate tied to actual physical movement rather than a free-running timer.
- **DMA transfer of conversion results** into two memory buffers (3000 samples per sensor, covering 60 cm of travel), offloading per-sample writes from the CPU.
- Implementation (and evaluation) of **double-buffered DMA** to minimize the latency between the end of acquisition and the start of data processing.
- **CPU–DMA synchronization via interrupts** on transfer completion.
- **Software post-processing** of the 3000 samples per channel:
  - Voltage-to-distance (mm) conversion based on the sensor's calibration curve.
  - Generation of a 300-value table using a downsampling scheme with moving-window averaging over 5 samples.
  - Calculation of mean, maximum and minimum distance per sensor.
**Concepts applied:** multi-channel triggered ADC, DMA controller (streams/channels), memory-to-peripheral transfers with double buffering, data-pipeline design for real-time acquisition, CPU load reduction through autonomous peripherals.
 
---
 
## 4. Module 3 — Graphical visualization (LCD-TFT)
 
**Goal:** render the two processed waveforms on screen, oscilloscope-style, using a custom low-level graphics library.
 
- **Custom-built graphics library**, with no dependency on vendor-provided drawing functions, implemented purely through direct access to the frame buffer:
  - `SetPixel` — writes an individual pixel in **ARGB4444** format.
  - `DibuixaLiniaHoritzontal` / `DibuixaLiniaVertical` — line drawing (implemented exclusively on top of `SetPixel`).
  - `EsborraDades` / `EsborraPantalla` — selective clearing of the data area vs. full-screen clearing.
- Manual management of the **frame buffer in external SDRAM**, including pixel-address calculation and the choice of memory access width (8/16/32-bit) depending on whether the whole pixel or only one component (e.g. the alpha channel) is being modified — optimizing the number of accesses over the 16-bit external bus.
- Configuration of the **LTDC (LCD-TFT Controller)** and its layers, including frame buffer address assignment in SDRAM and an alpha-blending strategy between layers to avoid redrawing static elements (window frames, color legends) on every update.
- Initialization of **external SDRAM** via the **FMC (Flexible Memory Controller)** and of the **ILI9341** LCD controller over SPI.
- Rendering of two waveforms (300 points each, 300×101 px window) updated every 60 cm of carriage travel, synchronized with modules 1 and 2.
**Concepts applied:** LCD-TFT frame buffer controllers, external memory management (SDRAM/FMC), pixel formats with alpha channel, hardware layering and blending, memory access optimization in byte-addressable architectures.
 
---
 
## 5. Platform and tools
 
| | |
|---|---|
| **MCU** | STM32F429ZI (ARM Cortex-M4F) |
| **Board** | STM32F429I-DISCOVERY |
| **Language** | C (bare-metal, no RTOS) |
| **IDE** | STM32CubeIDE |
| **Peripherals used** | GPIO, EXTI, TIM (capture/PWM/free-running), ADC, DMA, LTDC, FMC/SDRAM, SPI |
| **Validation tools** | Oscilloscope, debugger (real-time variable/memory inspection) |
 
---
 
## 6. Design highlights
 
- **All time-critical work is offloaded to hardware** (encoder generation, pulse generation, ADC triggering), keeping the CPU free for processing and decision-making — a common pattern in real-time embedded systems with tight latency constraints.
- **Layered, reusable architecture**: each stage of the project builds on the previous one, sharing the GPIO/timer/interrupt infrastructure.
- **Graphics library built from scratch**, with no reliance on third-party drawing routines, focused on efficient external-memory access.
- **Systematic instrumentation and validation** of each block (ISR timing, latency, DMA data consistency) as an integral part of development rather than an afterthought.
 
