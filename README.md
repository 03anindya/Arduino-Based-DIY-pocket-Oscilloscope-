# Arduino-Based-DIY-pocket-Oscilloscope-
Contains details about the project along with the program files for reference


# 📟 Arduino-Based Oscilloscope

This repository contains the design, components, circuit diagram, and working details for an **Arduino-based oscilloscope**.  
The project uses **Arduino UNO** with an **OLED display** to measure and display real-time waveform data, frequency, duty cycle, time period, and peak voltage.

---

## 📖 Introduction

An **oscilloscope** is an essential tool for electronics engineers and hobbyists, used to visualize how electrical signals change over time.  
Traditional oscilloscopes come in two major types:
- **CRO** (Cathode Ray Oscilloscope) – Displays waveform only.
- **DSO** (Digital Storage Oscilloscope) – Displays and stores waveform data with advanced measurement features.

This project implements a **low-cost, portable alternative** using Arduino UNO.

---

## ⚙️ Features

- Displays waveform, frequency, duty cycle, time period, and peak voltage (as average voltage).
- Operates at **5 Vp-p** input (suitable for most small electronics applications).
- Portable, lightweight, and customizable.

---

## 🛠 Components Used

| Component            | Specification           |
|----------------------|-------------------------|
| Arduino UNO          | Main controller         |
| Zener Diode          | IN4148                  |
| Resistors            | 10 kΩ, 100 kΩ, 10 Ω     |
| Potentiometer        | 100 kΩ                  |
| Capacitor            | 0.1 μF Ceramic          |
| OLED Display         | Output display          |
| Push Button          | For controls            |
| Multimeter           | For calibration         |

---

## 🔌 Working Principle

- The oscilloscope works at **5 Vp-p** max input voltage.
- A **potentiometer** is included for calibration to ensure accurate readings for varying signals.
- The Arduino reads analog voltage data, processes it, and sends the waveform and calculated parameters to the OLED display.
- Can measure:
  - **Waveform**
  - **Frequency**
  - **Duty Cycle**
  - **Time Period**
  - **Peak Voltage**

---

## 📊 Advantages over CRO/DSO

- ✅ **Low cost** compared to commercial oscilloscopes
- ✅ **Portable & lightweight**
- ✅ **Customizable** according to user needs
- ✅ Good **IoT/electronics learning project**

---

## ⚠️ Limitations

- ❌ Max input voltage limited to **5 Vp-p**
- ❌ No waveform storage (Arduino UNO has only 32 KB memory)
- ❌ Requires manual calibration before use

---

## 📐 Circuit Diagram

The circuit was designed and tested in **Proteus 8 Professional**.  
(Include `circuit_diagram.png` or `.pdf` in the repo for reference)

---

## 🚀 Future Improvements

- Use a **higher-spec microcontroller** to handle higher input voltages.
- Add **data storage** capability for waveform history.
- Improve sampling rate and resolution for higher-frequency applications.

---

## 📚 References

- [Instructables – Arduino Oscilloscope Projects](https://www.instructables.com/)
- ChatGPT – for code modification and general guidance
- Proteus 8 Professional – for circuit simulation and testing

---

## 📜 License

This project is licensed under the **Apache License 2.0** — see the `LICENSE` file for details.

---

## 👤 Author

**Anindya Ghosh**  
Academy of Technology, Dept. of Electronics and Communication
Anindya Ghosh
