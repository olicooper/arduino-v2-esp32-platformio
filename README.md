### Current status

Simple project isn't building...

```log
Linking .pio\build\lolin32_lite\firmware.elf
c:/users/<username>/.platformio/packages/toolchain-xtensa32/bin/../lib/gcc/xtensa-esp32-elf/8.4.0/../../../../xtensa-esp32-elf/bin/ld.exe: final link failed: bad value
collect2.exe: error: ld returned 1 exit status
*** [.pio\build\lolin32_lite\firmware.elf] Error 1
```

When `Serial` is used I get:
```log
Compiling .pio\build\lolin32_lite\src\main.c.o
src/main.c: In function 'setup':
src/main.c:9:5: error: 'Serial' undeclared (first use in this function); did you mean 'erfcl'?
     Serial.begin(115200);
     ^~~~~~
     erfcl
src/main.c:9:5: note: each undeclared identifier is reported only once for each function it appears in
src/main.c: In function 'loop':
src/main.c:14:5: error: 'Serial' undeclared (first use in this function); did you mean 'erfcl'?
     Serial.println("Hello");
     ^~~~~~
     erfcl
```

### Additional info

1. `C:\Users\<username>\.platformio\packages` includes:
    * framework-arduinoespressif32 (v2.0.0)
    * framework-espidf (v4.3.0)
    * tool-cmake (v3.16.4)
    * tool-idf (v1.0.1)
    * toolchain-esp32ulp (v1.22851.191205)
    * toolchain-xtensa-esp32 (v8.4.0+2021r1)
    * tool-esptoolpy (v1.30100.210531)
