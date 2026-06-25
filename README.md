# gsm-prepaid-energy-meter
A smart prepaid energy meter built on Arduino that measures real-time  power consumption, sends balance alerts via GSM SMS, and automatically  disconnects load when credit reaches zero - enabling remote recharge  and billing without needing physical site access.

## What it does
- Measures real-time power/energy consumption
- Sends SMS alerts for balance updates via GSM module
- Implements relay-based automatic load disconnection on zero credit
- Enables remote recharge, removing the need for manual meter visits

## Tech Stack
- Microcontroller: Arduino
- Communication: GSM module (SMS-based)
- Hardware: Relay-based load control, energy measurement sensors

## Key Features
- Real-time energy monitoring
- Automated prepaid billing logic
- Remote, contactless recharge and disconnection system

## What I learned
Building the relay-disconnect logic taught me to think about failure 
states carefully - what happens at exactly zero credit, and how to 
make sure the system fails safely rather than just cutting power 
abruptly.

## Demo
