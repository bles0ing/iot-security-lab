# CAN Bus Security Experiment

## 1. Hardware Setup
- **MCU**: STM32F103C8T6 (x2)
- **CAN Transceiver**: TJA1050 Module (x2)
  - **Note**: Built-in 120Ω termination resistor.
- **Indicators**: 1x Blue LED (Normal), 2x Red LEDs (Alarm)
- **Display**: 0.96" OLED (I2C)
- **Buzzer**: Not used

## 2. CAN Payload Definition
c
// Low Speed (10 km/h)
uint8_t speed_l[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A};

// High Speed (1000 km/h - Abnormal)
uint8_t speed_h[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE8};

// Junk Data (Flooding Test)
uint8_t rubbish[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


## 3. Experiment Results

### Normal State
- **LED**: Blue ON
- **OLED**: Displays raw 8-byte HEX data.

### Spoofing Attack
- **Logic**: Sending high-speed data (`speed_h`).
- **LED**: Both Red LEDs ON.
- **Detection**: IDS checks data range (Speed > 120 km/h).

### DoS Flooding
- **Logic**: Sending `rubbish` data at **> 60 FPS**.
- **LED**: Both Red LEDs Blinking.
- **Detection**: IDS frequency analysis triggers alarm.

## 4. IDS Rules
- **Data Validation**: Checks payload for physical limits.
- **Frequency Check**: > 60 FPS threshold.

## 5. Replay Attack
The replay code is **commented out** in the final version because the current whitelist-based IDS cannot detect attacks with valid data and normal frequency.