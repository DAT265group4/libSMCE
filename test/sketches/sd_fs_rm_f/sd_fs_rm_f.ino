#include <SD.h>

void setup() {
    SD.remove("bar/baz");
}

void loop() {
    delay(1);
}