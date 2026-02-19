# 🏓 Distributed Pong Embedded Control System

A distributed real-time electromechanical ping pong game developed for  
**TTK4155 – Embedded & Industrial Computer Systems Design (NTNU)**.

---

## 📌 System Overview

The project implements a **two-node distributed embedded system** communicating over a CAN bus.

### 🔹 Node 1 – ATmega162
Handles user interface and high-level game logic.

- External SRAM interface  
- Address decoding (NAND logic)  
- ADC joystick input  
- OLED display (SPI)  
- CAN controller (MCP2515)  
- User I/O interface  

---

### 🔹 Node 2 – ATSAM3X8E (Arduino Due)
Handles real-time control and electromechanical actuation.

- Servo control (PWM)  
- DC motor control  
- Quadrature encoder feedback  
- IR beam detection  
- Solenoid activation (via relay + transistor driver)  

---

## 🔄 Communication Architecture

The two nodes communicate via:

- **CAN bus (interrupt-driven)**
- Real-time frame transmission
- Distributed control structure

This forms a deterministic embedded control system.

---

## ⚙️ Key Technical Concepts Implemented

- External memory interfacing (XMEM)
- Address decoding using NAND logic
- SPI communication
- CAN protocol (interrupt-driven)
- ADC sampling and calibration
- PWM generation (servo + motor)
- PI motor control
- Real-time interrupt handling
- Electromechanical actuation

---

## 🎯 Final Result

A fully functional **computer-controlled distributed ping pong game**, integrating:

- Embedded hardware design  
- Real-time communication  
- Low-level driver development  
- Control systems  
- Mechanical actuation  

---

## 🛠 Technologies Used

- C (bare-metal)
- AVR (ATmega162)
- ARM Cortex-M3 (ATSAM3X8E)
- MCP2515 CAN Controller
- PWM & Timer modules
- SPI, ADC, External SRAM

---
