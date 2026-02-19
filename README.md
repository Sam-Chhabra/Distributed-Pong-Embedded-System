# Distributed-Pong-Embedded-System

Distributed Embedded Ping Pong Control System

This project implements a distributed embedded electromechanical ping pong game developed as part of TTK4155 – Industrial and Embedded Computer Systems Design at NTNU.

The system consists of two interconnected nodes:

• Node 1 (ATmega162)
	•	External SRAM and address decoding
	•	ADC joystick input
	•	OLED display via SPI
	•	CAN controller (MCP2515)
	•	User I/O interface

• Node 2 (ATSAM3X8E – Arduino Due)
	•	Servo control via PWM
	•	DC motor control
	•	Quadrature encoder feedback
	•	IR beam detection
	•	Solenoid activation

The nodes communicate over a CAN bus, forming a distributed real-time control system.

Key concepts implemented:
	•	External memory interfacing
	•	Address decoding with NAND logic
	•	SPI communication
	•	CAN protocol (interrupt-driven)
	•	ADC sampling and calibration
	•	PWM generation
	•	PI motor control
	•	Real-time interrupt handling
	•	Electromechanical actuation

The final system enables a fully functional computer-controlled ping pong game.
