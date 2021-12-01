#include <SD.h>

void setup() {
    SD.begin();
    SD.mkdir("/foo");
    SD.mkdir("bar");
    File f = SD.open("bar/baz", FILE_WRITE);
    f.print("quxx");
    f.close();
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);

    SD.remove("bar/baz");
}

void loop() {
    delay(1);
}