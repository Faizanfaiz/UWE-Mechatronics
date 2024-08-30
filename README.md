
# Single-Leg Robot Dog Project

This repository contains the setup, code, and documentation for the development of a single-leg robot dog. The project is designed to provide a platform for students to experiment with robotic leg control, dynamics, and various motor control algorithms using a Raspberry Pi 5 and RMD X motors.

## Table of Contents
- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Setup](#software-setup)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Project Overview
The objective of this project is to develop a single robotic leg that can be used as a testbed for students learning about robotic control systems, motor dynamics, and gait patterns. The robotic leg is powered by RMD X motors and controlled via a Raspberry Pi 5, interfaced through a Waveshare 2-Channel CAN HAT. This platform allows for practical experimentation in a controlled environment.

## Hardware Requirements
- Raspberry Pi 5 (64-bit)
- Waveshare 2-Channel CAN HAT
- RMD X Motors (specific model used: [specify your model])
- Power Supply for Raspberry Pi and Motors
- MicroSD Card (with Raspberry Pi OS 64-bit installed)
- Mechanical components for the robotic leg (joints, frame, etc.)
- CAN Bus cables and connectors

## Software Setup
### 1. Install Raspberry Pi OS (64-bit)
Ensure your Raspberry Pi is running the 64-bit version of Raspberry Pi OS.

### 2. Setup Waveshare 2-CH CAN HAT
Follow the instructions provided by Waveshare to set up the 2-Channel CAN HAT. The detailed guide can be found here: [Waveshare 2-CH CAN HAT Setup Guide](https://www.waveshare.com/wiki/2-CH_CAN_HAT).

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

## Usage
### Controlling the Robotic Leg
The repository includes Python scripts to control the RMD X motors, allowing you to experiment with different control strategies for the robotic leg.

Example command to initialize motor communication:
\`\`\`bash
python3 initialize_motor.py --can-interface can0
\`\`\`

### Running Test Gait Patterns
You can experiment with different gait patterns using the provided scripts. Ensure that the leg is securely mounted and powered on before running the scripts.

Example to run a basic gait pattern:
\`\`\`bash
python3 run_gait_pattern.py --pattern "basic_trot"
\`\`\`

### Monitoring Motor Feedback
Monitor the feedback from the RMD X motors, such as position, speed, and torque:
\`\`\`bash
python3 monitor_feedback.py --can-interface can0
\`\`\`

## Project Structure
\`\`\`
├── README.md
├── LICENSE.md
├── setup/
│   ├── setup_can_hat.md
│   └── motor_setup.md
├── scripts/
│   ├── initialize_motor.py
│   ├── run_gait_pattern.py
│   └── monitor_feedback.py
├── docs/
│   ├── hardware_setup.md
│   └── software_instructions.md
└── tests/
    └── motor_tests.py
\`\`\`

## Contributing
Contributions are welcome. Please fork the repository, create a new branch, and submit a pull request with your improvements or new features.

## License
This project is licensed under a proprietary license for exclusive use by [University Name]. Unauthorized use outside the university is prohibited.
