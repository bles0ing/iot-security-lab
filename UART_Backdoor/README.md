# UART Backdoor (Advanced)

## Purpose
Implement a stealthy UART backdoor on STM32 (HAL library) to understand the triggering mechanism, command control, and defense-evasion ideas of a firmware-level backdoor. This serves as an introductory practice for Industrial Control / IoT firmware security.

## Triggering (Two-Factor)
- Physical factor: press the on-board button `KEY1`
- Password factor: send the password `backroot` over USART1 followed by a newline
- Both conditions must be met to enter backdoor mode, preventing accidental triggering or brute-force activation
- A state machine (`bk_status`) switches between normal and backdoor modes; backdoor handling does NOT block the main loop

## Command Set
Once in backdoor mode, the UART responds to the following commands (silent in normal mode):

| Command | Function |
|---|---|
| `help` | List available commands |
| `led on` | Turn on the green LED (`G_LED_ON_ONLY`) |
| `unlock` | Exit backdoor mode and restore OLED to `System OK` |

## Lockout & Stealth Design
- Three consecutive wrong passwords → OLED shows `System Lock`; auto-unlocks after 10 seconds
- While active, the OLED still displays `System OK` (camouflage); a fast-blinking red LED acts as a covert indicator
- After `unlock`, state is cleared and the system returns to normal

## Problems Encountered & Solutions
Three typical problems drove the architecture from "working" to "state-machine driven":

1. **UART resource contention**: The backdoor logic and main loop shared USART1; while active, prints and command reception contended with each other, causing garbled output and truncated commands. Solution: clarify the producer–consumer boundary—the idle interrupt only fills `RxBuf`, `printf`/OLED use independent channels, and the main loop consumes commands uniformly.
2. **Blocking delays waiting for an incomplete buffer**: The initial version added `HAL_Delay` to "wait for a frame to finish", which slowed response and blocked the main loop's periodic tasks. Solution: switch to `HAL_UARTEx_ReceiveToIdle_IT` (idle-line interrupt reception); the callback fires on bus idle and data is consumed immediately—all blocking delays are eliminated.
3. **Main loop blocked after entering backdoor mode**: The initial version used a `while(1)` loop to wait for commands after activation, monopolizing the CPU and halting LED toggling, OLED refresh, and watchdog feeding. Solution: introduce a `bk_status` state machine scheduled by the main loop, processing only one command per call and returning promptly.

> These three fixes converge on one design principle: **the interrupt only fills the buffer, the idle interrupt removes blocking delays, and the state machine keeps the main loop responsive**—the key step from "working" to "architecturally sound".

## Verification (Hardware Verified)
- Platform: STM32F103C8T6 + CH340 (USB-TTL)
- Wiring: CH340 TX→PA10(RX), RX→PA9(TX), GND→GND; KEY1 on PC13
- Verified items: normal-mode silence, two-factor entry, `help`/`led on`/`unlock` commands, lockout after 3 failures, auto-unlock after 10 s — all passed

## Security Analysis
- Attack-chain view: physical access + password → trigger backdoor → info query / IO control → exit (`unlock` restores camouflage)
- Limitations: the password is hardcoded in firmware and can be extracted via dumping; future work includes `dump`/`write`/`gpio` commands, CRC self-check, and stronger multi-factor authentication

## Conclusion
This experiment advances from "51-MCU command control" to "STM32 state-machine backdoor", covering UART idle-line interrupt reception, GPIO/button interrupts, OLED display, and foundational firmware-security concepts. More importantly, by resolving UART contention, buffer-related delays, and main-loop blocking, it establishes an "interrupt-fills-buffer + state-machine dispatch" embedded-backdoor implementation pattern—laying the groundwork for firmware reverse engineering (Ghidra/IDA) and wireless security.

## Files
- Full report: `实验报告.docx`
