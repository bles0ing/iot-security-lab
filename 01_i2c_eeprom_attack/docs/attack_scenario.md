# 攻击场景说明

## 场景设定

某工控设备的运行模式存储在外部 AT24C02 EEPROM 中。设备上电后从 EEPROM 读取配置字符串，根据内容决定运行模式。设备代码逻辑为"上电只读，空芯片才写默认值"，因此一旦 EEPROM 内容被篡改，设备将长期信任被污染的数据。

## 攻击流程

### Phase 1： reconnaissance（侦察）

攻击者物理接触设备，发现 AT24C02 芯片为 DIP8 直插封装，可通过插槽直接拔出。使用逻辑分析仪夹在 I2C 总线上，捕获到设备上电时的通信数据：

```
SCL: ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
SDA: [START] [0xA0] [0x00] [0xA1] [S] [E] [C] [U] [R] [E] [_] [2] [0] [2] [6] [STOP]
```

确认数据以明文 "SECURE_2026" 传输，无加密无校验。

### Phase 2：Extraction（提取）

断电后拔出 AT24C02，插入 CH341A 编程器，使用 NeoProgrammer 读取完整 256 字节内容：

```
偏移量   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
0x0000: 53 45 43 55 52 45 5F 32 30 32 36 FF FF FF FF FF
```

ASCII: `SECURE_2026`

### Phase 3：Tampering（篡改）

在 NeoProgrammer 编辑模式下，将 `SECURE` 的十六进制值：

```
53 45 43 55 52 45
```

改为 `HACKED`：

```
48 41 43 4B 45 44
```

保存为 `hacked.bin`，通过 CH341A 写回 AT24C02。

### Phase 4：Persistence（持久化验证）

将篡改后的 AT24C02 插回设备，上电观察：

1. OLED 显示 `"HACKED_2026"` ✅
2. 按复位键 → 仍显示 `"HACKED_2026"` ✅
3. 断电重上电 → 仍显示 `"HACKED_2026"` ✅
4. 逻辑分析仪抓包确认总线传输 `"HACKED_2026"` ✅

攻击持久化成功。

## 攻击效果分析

| 维度 | 正常状态 | 被攻击状态 |
|---|---|---|
| EEPROM 内容 | SECURE_2026 | HACKED_2026 |
| OLED 显示 | SECURE_2026 | HACKED_2026 |
| 复位后 | SECURE_2026 | HACKED_2026 |
| 断电重上电 | SECURE_2026 | HACKED_2026 |
| I2C 总线数据 | SECURE_2026 | HACKED_2026 |

## 攻击前提条件

1. 攻击者能物理接触设备（拔出 EEPROM）
2. EEPROM 中存储关键配置且无完整性校验
3. 设备代码信任外部存储数据，不做验证
4. EEPROM 为通用型号（AT24C02），可用廉价编程器读写

## 现实映射

| 实验要素 | 现实对应 |
|---|---|
| AT24C02 存储运行模式 | 工业控制器存储设备配置/校准参数 |
| CH341A 物理篡改 | 现场维护人员或被入侵者替换芯片 |
| I2C 明文传输 | 设备内部总线通常无加密 |
| 上电只读不写 | 许多设备只在首次运行时写入默认值 |
| 篡改持久化 | 设备生命周期内一直使用被污染配置 |

## 防御方案

### 方案 A：完整性校验

```c
// 伪代码
uint8_t data[16];
read_eeprom(data);
uint8_t checksum = calculate_crc8(data, 15);
if (checksum != data[15]) {
    // 校验失败，触发告警，使用硬编码默认值
    trigger_alarm();
    load_default_config();
}
```

### 方案 B：加密存储

```c
// 伪代码
uint8_t encrypted[16];
read_eeprom(encrypted);
uint8_t decrypted[16];
aes_decrypt(encrypted, key, decrypted);
if (verify_magic(decrypted)) {
    use_config(decrypted);
} else {
    trigger_alarm();
}
```

### 方案 C：内部存储

将关键配置移至 STM32 内部 Flash 或 OTP（One-Time Programmable）区域，外部不可物理访问。

## 总结

本实验完整复现了从侦察、提取、篡改到持久化的 I2C EEPROM 攻击链。核心结论：**当关键配置存储在外部可物理访问的存储器中，且设备无完整性校验机制时，攻击者可以通过物理手段持久地控制设备行为。**
