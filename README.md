# Waste_Sorter
A smart waste sorter system powered by an ESP32 Dev Board and ESP32-CAM, featuring servo-controlled bins and AI-based object detection for classifying plastic, paper, and organic waste.

---

## Project Overview

This smart bin automatically sorts waste based on what object is detected by the camera.  
A trained Edge Impulse model runs on the ESP32-CAM and sends the classification result via UART to the ESP32 board, which controls servo motors to open the correct bin lid.

 **Object detection with ML (Edge Impulse + ESP32-CAM)**  
 **Three servo-controlled waste compartments**  
 **OLED display + LEDs for visual feedback**  
 **Laser-cut enclosure (DXF + ZIP provided)**  
 **Fully open-source hardware + code**

---

##  Repository Contents

| File / Folder | Description |
|---------------|-------------|
| `ESP32code.ino` | Main controller code for servo control, OLED, LEDs |
| `camCode.ino` | ESP32-CAM code for image capture + ML inference |
| `Bill Of Material.pdf` | List of electrical components used |
| `bin structure.zip` | Laser-cut design files for the physical bin |
| `README.md` | Project documentation (this file) |

---

##  Hardware Used

| Component | Purpose |
|-----------|---------|
| ESP32 Dev Board | Main logic, UART communication, servo + OLED control |
| ESP32-CAM | Runs Edge Impulse ML model + sends classification |
| 3 × Servo Motors (SG90 / MG996R) | Opens bin lids based on detected material |
| 3 × LEDs | Indicates which bin is activated |
| OLED Display (SSD1306, I2C) | Shows status + classification result |
| Power Supply (5V) | Powers servos + boards |
| Jumper wires, resistors, switches | Basic wiring and control |
| Laser-cut enclosure (3mm material) | Physical housing for bin system |

---

## Pin Configuration

### **ESP32 Board**
| Function | Pin |
|----------|-----|
| Servo 1 (Organic) | GPIO 12 |
| Servo 2 (Paper)   | GPIO 14 |
| Servo 3 (Plastic) | GPIO 27 |
| LED 1 (Organic)   | GPIO 4 |
| LED 2 (Paper)     | GPIO 16 |
| LED 3 (Plastic)   | GPIO 33 |
| OLED SDA          | GPIO 21 |
| OLED SCL          | GPIO 22 |
| UART RX (from CAM)| GPIO 17 |
| UART TX (to CAM)  | GPIO 16 |

### **ESP32-CAM**
Runs Edge Impulse model and sends text result via UART.

---

## Machine Learning (Edge Impulse)

The model is trained using **image classification** with 3 waste categories:

- **Organic**
- **Paper**
- **Plastic**

Training is done via Edge Impulse Studio, then exported as an Arduino library for the ESP32-CAM.

 Early training accuracy: **0% (poor) — no classification**  
 After more data + retraining: **50% accuracy and improving**

---
## Product images
<img width="549" height="646" alt="Screenshot 2025-11-06 144130" src="https://github.com/user-attachments/assets/acc9feb8-e75e-4f20-938e-6d5bd387410e" />

