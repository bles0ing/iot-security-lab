# RS485 Dual-Board Instruction Scanner & Response Verification

> **Keywords**: STM32F103 · RS485 · UART Idle Interrupt · Half-Duplex Communication · Noise Immunity · Embedded Debugging

## Project Overview

This project implements a dual-board communication system over an RS485 bus:

- **Scanner (sender)**: Sends instruction bytes `0x01` through `0x08` one by one, waiting for the receiver to reply `ACK` or `NAK` to each instruction. Upon receiving a valid reply, it prints the instruction code and the response result on the OLED.
- **Receiver**: Upon receiving an instruction, it checks whether it is a **valid instruction code** — `0x03` and `0x06` are considered valid and trigger an `ACK` reply; the rest (`0x01`, `0x02`, `0x04`, `0x05`, `0x07`, `0x08`) are invalid and trigger a `NAK` reply. The OLED shows `NORMAL` or `ABNORMAL`, and an LED indicates the result: PA2 lights up for normal (valid instruction) and PA3 lights up for abnormal (invalid instruction).

The goal is to verify the reliability of RS485 half-duplex communication and to solve the data misalignment problem caused by power-on / reset noise.

## Hardware List

| Component | Quantity | Notes |
|-----------|:--------:|-------|
| STM32F103C8T6 dev board | 2 | One as Scanner, one as Receiver |
| MAX485 / SP3485 module | 2 | RS485 transceiver |
| 0.96" OLED (I2C) | 2 | Status display |
| Push button | 1 | Manually triggers sending on the Scanner |
| LED | 2 | PA2 (normal), PA3 (abnormal) |
| USB-to-Serial module | 1 | Connected to Scanner's USART1 (PA9/PA10) for debug prints |
| Jumper wires |若干 | A/B lines, power, ground |

## Pin Mapping

### UART3 (RS485 transceiver, same on Scanner and Receiver)

| Function | Pin | Description |
|----------|-----|-------------|
| DE (Driver Enable) | PB9 | High = transmit enabled |
| RE (Receiver Enable) | PB8 | Low = receive enabled (MAX485 typically uses opposite logic for RE vs DE; check your module) |
| DI (Data Input) | PB10 | Connected to USART3_TX |
| RO (Data Output) | PB11 | Connected to USART3_RX |

> USART3 TX/RX are remapped to PB10/PB11. DE and RE are controlled separately by GPIO: before sending, set DE high and RE low; before receiving, set DE low and RE high.

### Other Pins

| Function | Pin | Description |
|----------|-----|-------------|
| LED Normal (G_LED) | PA2 | Lights up when a valid instruction is received (ACK) |
| LED Abnormal (B_LED) | PA3 | Lights up when an invalid instruction is received (NAK) |
| Debug UART (USART1) | PA9 (TX), PA10 (RX) | Connected to PC via USB-to-Serial module for `printf` debug output |
| OLED (I2C) | PB6 (SCL), PB7 (SDA) | Displays scan status and results (adjust to your actual config) |

## Wiring Diagram

### RS485 Bus Between the Two STM32 Boards

```
Scanner (Board A)              Receiver (Board B)
  PB10 (DI/TX)  ------------- PA10 (RO/RX)
  PB11 (RO/RX)  ------------- PA9  (DI/TX)
  PB9  (DE)                  PB9  (DE)
  PB8  (RE)                  PB8  (RE)
  A (+)  -------------------- A (+)
  B (-)  -------------------- B (-)
  GND    -------------------- GND
```

> DE/RE are controlled independently by each MCU. The RS485 module's A/B lines are cross-connected (A to A, B to B). **Note**: DI/RO on one side connects to RO/DI on the other (TX to RX), consistent with a normal UART connection.

### Scanner to PC (Debug UART)

```
Scanner PA9  (USART1_TX) ---- USB-to-Serial module RXD
Scanner PA10 (USART1_RX) ---- USB-to-Serial module TXD
GND                         ---- GND
```

## Instruction Set & Response Rules

The Scanner sends the following 8 instruction bytes in sequence:

| Instruction | Meaning | Receiver Decision | Reply | LED |
|-------------|---------|-------------------|-------|-----|
| `0x01` | Instr 1 | Invalid | `NAK` | PA3 (abnormal) |
| `0x02` | Instr 2 | Invalid | `NAK` | PA3 (abnormal) |
| `0x03` | Instr 3 | **Valid** | `ACK` | PA2 (normal) |
| `0x04` | Instr 4 | Invalid | `NAK` | PA3 (abnormal) |
| `0x05` | Instr 5 | Invalid | `NAK` | PA3 (abnormal) |
| `0x06` | Instr 6 | **Valid** | `ACK` | PA2 (normal) |
| `0x07` | Instr 7 | Invalid | `NAK` | PA3 (abnormal) |
| `0x08` | Instr 8 | Invalid | `NAK` | PA3 (abnormal) |

**Valid instruction codes**: `0x03`, `0x06`
**Responses**: `ACK` (ASCII, 0x41 0x43 0x4B) or `NAK` (0x4E 0x41 0x4B)

## Communication Protocol

```
Scanner                         Receiver
  |                                |
  |---- [0x01 ~ 0x08] ----------->|
  |                                |-- Check if 0x03 / 0x06
  |<--- "ACK" (valid) / "NAK" (invalid)
  |                                |-- Light PA2 / PA3 LED
  |-- Receive reply --> print instruction & result
```

Scan flow: The Scanner starts at `0x01` and sends one instruction per button press, up to `0x08`. The Receiver decides instantly and replies to each instruction, while lighting the corresponding LED (PA2 for normal / PA3 for abnormal). After receiving the reply, the Scanner prints the instruction code and `ACK`/`NAK` via the OLED and the debug UART (USART1) to verify the communication and instruction validity.

## Debugging Log (Pitfalls Encountered)

### Pitfall 1: OLED refresh inside interrupt froze the screen
- **Symptom**: The OLED only refreshed once, then became unresponsive.
- **Cause**: `HAL_UARTEx_RxEventCallback` directly called the OLED drawing function, which blocked the interrupt.
- **Fix**: The callback now only sets a flag; all time-consuming work is moved to the main loop.

### Pitfall 2: Instruction code and response were always off by one
- **Symptom**: After sending `0x03`, the ACK info for the *next* instruction `0x04` was printed; after sending `0x06`, `0x07` was printed instead.
- **Cause**: Two separate counters were used — `count` (incremented on send) and `cnt` (incremented on receive). Their asynchronous behavior caused the index to lose sync, so the code always printed "the next, unsent byte."
- **Fix**: Use a single `count` counter; in the receive handler, print `arr[count-1]` (the instruction that was just sent) and stop maintaining a separate `cnt`.

### Pitfall 3: A board's hardware issue caused unstable communication
- **Symptom**: After swapping the roles of the two boards, one board could never receive properly (and its CAN loopback test also failed).
- **Cause**: That board had a hardware defect in its UART receive path (possibly a cold solder joint or damage from a previous CAN experiment).
- **Fix**: Assign that board permanently as the Scanner (sender, since its TX works), and the other as the Receiver; plan to replace the faulty board.

### Pitfall 4: Resetting the receiver falsely triggered the sender's receive handler (key turning point)
- **Symptom**: Pressing the Receiver's reset button made the Scanner's OLED suddenly show `01` and its response info — even though no button had been pressed.
- **Cause**: The reset created electrical noise on the RS485 bus, which the Scanner's UART mistook for a valid data frame, firing the idle interrupt and executing the receive-handling code, which incorrectly advanced the `count` state.
- **Fix**: Added `memcmp`-based filtering in the receive handler — only data whose first 3 bytes are `"ACK"` or `"NAK"` is processed; everything else (noise, partial frames) is discarded without changing any counter state. Also added bias resistors in hardware (A pulled up, B pulled down) to suppress noise.

## Runtime Behavior

- **Valid instruction**: Scanner sends `0x03` → Receiver judges it valid → lights PA2 LED → replies `ACK` → Scanner OLED shows `03 ACK`. Sending `0x06` behaves the same, showing `06 ACK`.
- **Invalid instruction**: Scanner sends `0x01`/`0x02`/`0x04`/`0x05`/`0x07`/`0x08` → Receiver judges it invalid → lights PA3 LED → replies `NAK` → Scanner shows e.g. `01 NAK`.
- **Scan flow**: From `0x01` to `0x08`, one instruction per button press; the Scanner prints valid instruction codes through the debug UART (USART1) for easy verification. After scanning finishes, the OLED does **not** show `DONE` (this project does not implement a scan-complete prompt; feel free to add it).
- **Noise immunity**: Resetting either board no longer falsely triggers the other board's receive handler, and the counter state stays intact, so the next button press continues from the correct instruction.

## Conclusion & Extensions

### Debugging Methodology Summary
1. **Do the minimum inside interrupts** (set a flag); handle business logic in the main loop.
2. **Keep index variables unified** to avoid asynchronous counters losing sync.
3. **Data validity filtering** (`memcmp` against `ACK`/`NAK`) is the simplest and most effective defense against power-on / reset noise.
4. **Hardware bias resistors** reduce noise triggering at the physical layer, complementing software filtering.

### Future Work
- Convert the manual button scan into an **automatic state-machine scan** for higher efficiency.
- Introduce a custom protocol with **frame header + checksum** for stronger robustness.
- Port to the **Modbus RTU** protocol for interoperability with industrial devices.
- Add a scan-complete (`DONE`) prompt and auto-loop functionality.

