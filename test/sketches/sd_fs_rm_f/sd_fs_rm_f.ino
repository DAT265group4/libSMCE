#include <SD.h>

void setup() {
    if(SD.exists("bar/file_to_del"))
        SD.remove("bar/file_to_del");
    SD.remove("bar");
}

void loop() {
    delay(1);
}