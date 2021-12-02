#include <SD.h>

void setup() {
    SD.rmdir("dir_to_del");
    SD.rmdir("bar/baz");
}

void loop() {
    delay(1);
}