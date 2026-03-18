# bq34z100-g1 + SN74HC164 Battery Gauge Setup

Hardware: 1S Li-ion (3800mAh), MCP73833 Charger (500/1000mA), 7-LED Display via Shift Register, Raspberry Pi 3 A+ (via 5V Boost).

## 1. Initial Data Flash Configuration
Before starting, set these key parameters in **bqStudio**:

### 7-LED Display (SN74HC164)
*   **LED_Comm Configuration**: `0x63` (7 LEDs, Port Expander Mode, Button Trigger).
*   **Number of LEDs**: `7` (Verify in Configuration subclass).
*   **LED Hold Time**: `4` seconds.

### Gauge Thresholds
*   **Design Capacity**: `3800` mAh
*   **Design Energy**: `14060` mWh (3.7V * 3800)
*   **Taper Current**: `85` mA (Safe for 500mA/1000mA charging modes).
*   **Quit Current**: `20` mA (Must be lower than Taper).
*   **Dsg Relax Time**: `60` s (Speeds up OCV detection).
*   **Terminate Voltage**: `3200` mV (Set ~100mV above SBC/Boost cutoff).

## 2. Chemistry ID (GPCCHEM)
Do not use generic IDs. Follow this log sequence for the TI matching tool:
1.  Charge to full (Wait for **FC** bit).
2.  Rest **2 hours** (Wait for **OCVTAKEN=1**).
3.  Discharge at **C/10** (approx. 380mA - 500mA) until **Terminate Voltage**.
4.  Rest **5 hours** (Wait for **OCVTAKEN=1**).
5.  Zip the log and upload to [TI GPC Tool](https://www.ti.com). Program the resulting ID (e.g., `0x3230`) into the gauge.

## 3. Calibration
*   **Voltage**: Calibrate against a high-quality multimeter (Crucial for Qmax accuracy).
*   **Current**: Calibrate with a known stable load (e.g., 500mA).
*   **Temperature**: Set **TEMPS bit = 1** (External) for NTC thermistor.

## 4. The Learning Cycle (04 -> 05 -> 06)
1.  **IT_ENABLE**: Send command `0x0021`. **Update Status** should be `04`.
2.  **Empty Rest**: Wait at 0mA until **OCVTAKEN=1**.
3.  **Charge to Full**: Charge until **FC** bit is Red.
4.  **Full Rest**: Unplug charger. Wait **2 hours** until **OCVTAKEN=1**.
5.  **Discharge**: Run SBC load (500-600mA) until it shuts down.
6.  **Final Rest**: Wait **5 hours**.
    *   **Update Status 05**: Qmax updated.
    *   **Update Status 06**: Ra Table (Resistance) updated.

## 5. Troubleshooting
*   **OCVTAKEN won't set?** Check that **Current** is exactly `0mA`. If it flickers, re-calibrate CC Offset.
*   **Qmax too high?** Usually caused by starting a cycle on a non-rested battery or poor voltage calibration. Manually reset Qmax to 3800 and Update Status to 04 if it drifts.
*   **LEDs don't light?** Check wiring to SDA/SCL and ensure **VEN** pin is pulsed to Ground.
