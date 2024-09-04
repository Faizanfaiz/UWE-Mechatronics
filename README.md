
# Single-Leg Robot Dog Project

This repository contains the setup, code, and documentation for the development of a single-leg robot dog. The project is designed to provide a platform for students to experiment with robotic leg control, dynamics, and various motor control algorithms using a Raspberry Pi 5 and RMD X motors.

## Table of Contents
- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Setup](#software-setup)
- [License](#license)

## Project Overview
The objective of this project is to develop a single robotic leg that can be used as a testbed for students learning about robotic control systems, motor dynamics, and gait patterns. The robotic leg is powered by RMD X motors and controlled via a Raspberry Pi 5, interfaced through a Waveshare 2-Channel CAN HAT. This platform allows for practical experimentation in a controlled environment.

## Hardware Requirements
- Raspberry Pi 5
- Waveshare 2-Channel CAN HAT
- RMD-X8 Motors
- Power Supply for Raspberry Pi and Motors
- MicroSD Card (with Raspberry Pi OS 64-bit installed)
- Mechanical components for the robotic leg (joints, frame, etc.)
- CAN Bus cables and connectors

## Software Setup
### 1. Install Raspberry Pi OS (64-bit)
Ensure your Raspberry Pi is running the 64-bit version of Raspberry Pi OS.

### 2. Setup Waveshare 2-CH CAN HAT/ 2-CH CAN FD HAT
Before Following any of the instructions below, make sure you check your expnasion Board and Follow the relevant Instructions:

Follow the instructions provided by Waveshare to set up the 2-Channel CAN HAT. The detailed guide can be found here: [Waveshare 2-CH CAN HAT Setup Guide](https://www.waveshare.com/wiki/2-CH_CAN_HAT).

Follow the instructions provided by Waveshare to set up the 2-Channel CAN FD HAT. The detailed guide can be found here: [Waveshare 2-CH CAN FD HAT Setup Guide](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT).

### 3. Install Required Dependencies
Install the necessary software packages and dependencies for motor control and CAN communication:
\`\`\`bash
sudo apt-get update
sudo apt-get install can-utils python-can
\`\`\`

### 4. Configure the CAN Interface
Configure the CAN interface to communicate with the RMD X motors. Follow the Waveshare guide for initial setup, and then set the bitrate and interface settings as needed:
\`\`\`bash
sudo ip link set can0 up type can bitrate 1000000
\`\`\`

## License
This project is licensed under a proprietary license for exclusive use by [University Name]. Unauthorized use outside the university is prohibited.
