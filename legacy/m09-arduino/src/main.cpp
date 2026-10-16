#include <Arduino.h>
#include <bsp.h>
void setup() {
    Serial.begin(115200);
    bsp::begin();
    Serial.println(bsp::board_name());
}
void loop() { delay(1000); }
