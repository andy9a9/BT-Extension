# BT-Extension
Bluetooth remote controll for 230AC extension.

default name: "Bt-ext"

# Communication settings:
baud:   9600
data:   8b
stop:   1
parity: none

# String (12B):
frame include: [header][cmd][data][crc]
- frame [1B]: 0xA2

- cmd [1B]:
 - name: 0x90
 - search: 0x91
 - toggle: 0x92
 - time: 0x93

- data [8B] - MSB first:
 - name: [8B]
 - search: 0x01[1B] 0x00[7B]
 - toggle: ON: 0x0F[1B] 0x00[7B] / OFF: 0xF0[1B] 0x00[7B]
 - time:  hours[1B] minutes[1B] seconds[1B] 0x00[5B]

- crc [2B] - LSB first:  
  - counting from commad (cmd include)

OK: device sends ~header as confirmation = ~0xA2 = 0x5D  
ERROR: 0xF1 [1B]

turn ON the device:  
 - TX: {0xA2} {0x92} {0x0F 0x00 0x00 0x00 0x00 0x00 0x00 0x00} {0x00 0xA1}  
 - RX: ~0xA2 - OK

turn OFF the device:  
 - TX: {0xA2} {0x92} {0xF0 0x00 0x00 0x00 0x00 0x00 0x00 0x00} {0x01 0x82}  
 - RX: ~0xA2 - OK
