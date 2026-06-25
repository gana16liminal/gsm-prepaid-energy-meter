

# GSM Based Prepaid Energy Meter Using Arduino

## Overview
A smart prepaid energy meter built on Arduino that measures real-time
power consumption, sends balance alerts via GSM SMS, and automatically
disconnects load when credit runs low — enabling remote recharge and
billing without needing physical site access.

## What it does
- Reads energy meter pulses via an optocoupler (IC 4N35) and converts
  them into consumed units and remaining balance
- Displays live units/balance on a 16x2 LCD
- Sends a **Low Balance SMS alert** when balance drops below ₹15
- Automatically **disconnects the load via relay** when balance drops
  below ₹5, and sends a "Light Cut" SMS alert
- Supports **remote recharge via SMS** — the user sends a message with
  the recharge amount, the system decodes it, updates the balance, and
  sends a confirmation SMS
- Includes anti-theft provisions (micro limit switch) for tamper alerts

## Hardware Components
- Arduino UNO
- SIM800/900 GSM Module
- Single Channel Relay Module
- Micro Limit Switch (anti-theft / tamper detection)
- 5V Buzzer
- Optocoupler IC 4N35 (pulse sensing from energy meter)
- 10K Resistor
- Zero PCB (prototyping board)
- 16x2 LCD Display

## Wiring Summary
- LCD data pins (RS, EN, D4–D7) → Arduino digital pins 7, 6, 5, 4, 3, 2
- GSM module Tx/Rx → Arduino Rx/Tx (UART)
- Relay control → Arduino pin 12 (via ULN2003 driver)
- Pulse sensing (optocoupler output) → Arduino pin 8

## Algorithm
1. Initialize LCD, GSM module, and display "System Ready"
2. Continuously read energy meter pulses and update consumed units
3. Calculate remaining balance: `balance = remaining_units * rate_per_unit`
4. If balance > ₹15 → keep relay ON (power connected)
5. If balance < ₹15 → send "Low Balance" SMS warning
6. If balance < ₹5 → disconnect relay (cut power) and send "Light Cut" SMS
7. On receiving a recharge SMS → decode amount, update balance in EEPROM,
   reconnect power, and send confirmation SMS

### Pulse-to-Unit Calculation (from project report)
- Energy meter pulse rate used: 3200 imp/kWh
- For a 100W load: ~5.33 pulses/minute
- Power factor per pulse: ~0.3125 W
- Example: a 100W bulb running 24 hours consumes ~2.4 units
  → at ₹5/unit, that's ₹12/day

## Tech Stack
- Arduino IDE (C/C++)
- EEPROM (for persistent balance storage across power cycles)
- GSM AT command set (SIM800/900)

## Results
- Successfully measured real-time voltage, current, and power via serial
  monitor and LCD
- SMS alerts (low balance, light cut, recharge confirmation) delivered
  reliably during testing
- Relay-based load disconnect verified at the ₹5 threshold

## What I learned
Building the relay-disconnect logic taught me to think carefully about
failure states — what should happen at exactly zero/low credit, and how
to make sure the system fails safely (clear warnings before cutoff)
rather than cutting power abruptly without notice. Working with GSM AT
commands also gave me hands-on experience with serial communication
and real hardware timing, which behaves very differently from a clean
simulation.

## Demo

![LCD Display Readings](<img width="754" height="499" alt="Lcd displaying sms" src="https://github.com/user-attachments/assets/fc343d43-18bd-43e2-9dd0-f4dfb31f4a4c" />

![Hardware Setup](<img width="1121" height="437" alt="hardware setup" src="https://github.com/user-attachments/assets/1c5bdbb0-02a1-4843-8453-94e041a01ee1" />

![Recharge SMS Alert](<img width="1083" height="443" alt="recharge sms alert" src="https://github.com/user-attachments/assets/9c1002dc-4875-41e5-b8cd-639cfec9feb1" />

![Circuit diagram].(/<img width="1204" height="636" alt="circuit diagram" src="https://github.com/user-attachments/assets/9252e72f-3f0e-4392-b2d7-23884254a23c" />

