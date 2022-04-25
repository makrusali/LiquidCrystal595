# Library LiquidCrystal595

Basis kode, ide, dan Sebagian kode dari [https://github.com/arduino-libraries/LiquidCrystal](https://github.com/arduino-libraries/LiquidCrystal)

## Library ini digunakan untuk rangkaian sebagai berikut

![Gambar Rangkaian](/docs/gambar_rangkaian.png)

## Contoh Kode

```cpp
    #include <LiquidCrystal595.h>
    
    /*
     * LiquidCrystal595 lcd(data_pin,clock_pin,latch_pin);
     */
    LiquidCrystal595 lcd(2,3,4);
    
    void setup(){
      //konfigurasi lcd yang digunkan kolom=16 - baris 2
      lcd.begin(16,2);
      //nyalakan backlight
      lcd.backlight();
      
      //arahkan cursor pada posisi kolom=3 - baris=0
      lcd.setCursor(3,0);
      //cetak "Halo Dunia!"
      lcd.print("Halo Dunia!");
      
      //arahkan cursor pada posisi kolom=0 - baris=1
      lcd.setCursor(4,1);
      //cetak "INDONESIA"
      lcd.print("INDONESIA");
    }
    
    void loop() {
    }
```
