#include <SD.h>

void setup() {
    SD.begin();
    SD.mkdir("/foo");
    SD.mkdir("bar");
    File f = SD.open("bar/baz", FILE_WRITE);
    f.print("quxx");
    f.close();

    SD.mkdir("dir_to_del");
    File f2 = SD.open("bar/file_to_del", FILE_WRITE);
    f2.close();

    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
}

void loop() {
    delay(1);
}