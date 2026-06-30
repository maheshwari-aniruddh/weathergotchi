# WeatherGotchi — Wiring

Board: **Seeed Studio XIAO ESP32-C6**
Display: **Waveshare 1.5" RGB OLED (SSD1351, 128×128, SPI)**
Battery: **KP-603048 3.7V 2000 mAh LiPo (JST-PH 2.0, 2-pin)**

Pins match `WeatherGotchi/config.h`. The XIAO only breaks out GPIO
2,3,4,5,6,7,8,9,10,20,21 — these all use the silkscreen pad labels.

## OLED (SPI) → XIAO

| OLED pad     | Wire   | XIAO pad | GPIO | config.h    |
|--------------|--------|----------|------|-------------|
| VCC          | red    | **3V3**  | —    | (power)     |
| GND          | black  | **GND**  | —    | (power)     |
| DIN (MOSI)   | blue   | **D10**  | 10   | `OLED_MOSI` |
| CLK (SCK)    | yellow | **D8**   | 8    | `OLED_SCK`  |
| CS           | orange | **D7**   | 20   | `OLED_CS`   |
| DC           | green  | **D6**   | 21   | `OLED_DC`   |
| RST          | white  | **D0**   | 2    | `OLED_RST`  |

> The SSD1351 module runs at 3.3 V logic — drive it from **3V3**, not 5V.

## Button (optional) → XIAO

| Button | XIAO pad | GPIO | config.h     |
|--------|----------|------|--------------|
| leg A  | **D1**   | 3    | `BUTTON_PIN` |
| leg B  | **GND**  | —    | —            |

Active-low with the internal pull-up. Short press = next screen, long press
(~1 s) = force a weather refresh.

## Battery → XIAO (underside BAT pads)

| LiPo wire | XIAO pad |
|-----------|----------|
| red  (+)  | **BAT+** |
| black (−) | **BAT−** |

The XIAO charges the LiPo over its USB-C port automatically. **Check polarity
with a multimeter before plugging the JST in** — connector wiring is not
guaranteed, and reversed polarity can destroy the board.

> Battery % is *estimated from runtime*, not measured — there's no voltage
> divider wired. See the battery notes in `config.h`.

## Charge Detect (optional, currently OFF)

| Signal | XIAO pad | GPIO | config.h |
|--------|----------|------|----------|
| USB presence (optional) | **A2** | 4 | `CHARGE_DETECT_PIN` |

If you wire a USB-presence detector (e.g., a resistor divider on the USB 5V line), connect it to A2. Set `USE_CHARGE_DETECT 1` to enable. Currently disabled (`USE_CHARGE_DETECT 0`).

## Diagram

```
                Waveshare 1.5" OLED (SSD1351)
              +-----------------------------------+
              | VCC GND DIN CLK CS  DC  RST       |
              +--+---+---+---+---+---+---+---------+
                 |   |   |   |   |   |   |
   3V3 ----------+   |   |   |   |   |   |
   GND --------------+   |   |   |   |   |
   D10 (GPIO10) MOSI-----+   |   |   |   |
   D8  (GPIO8)  SCK ---------+   |   |   |
   D7  (GPIO20) CS --------------+   |   |
   D6  (GPIO21) DC ------------------+   |
   D0  (GPIO2)  RST ---------------------+

          Seeed XIAO ESP32-C6
        +-----------------------+
   USB-C|()                     |
        |  D0  D1  D2 ... 3V3   |
        |  D6  D7  D8  D9  D10  |
        +----+--------------+---+
             |              |
   Button:   D1 ---[ btn ]--- GND
                              (GPIO3, internal pull-up)

        Underside:  BAT+ o---(red)---  LiPo +
                    BAT- o---(black)-- LiPo -
```
