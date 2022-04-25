#include "LiquidCrystal595.h"
#include <inttypes.h>
#include "Arduino.h"

/*
 * @brief Constructor untuk inisialisasi object
 * @param argumen pertama yaitu letak data_pin HC595, kedua letak clock_pin HC595, ketiga letak latch_pin HC595
 */
LiquidCrystal595::LiquidCrystal595(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin ) {
  _clock_pin = clock_pin;
  _data_pin = data_pin;
  _latch_pin = latch_pin;
}

/*
 * @brief fungsi untuk memulai ( inisialisasi awal pada LCD )
 * @param jumlah kolom pada LCD yang digunakan, jumlah baris pada LCD yang digunakan
 */
void LiquidCrystal595::begin(uint8_t cols, uint8_t lines) {
  _cols  = cols;
  _lines = lines;

  _setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

  pinMode(_clock_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
  pinMode(_latch_pin, OUTPUT);

  //inisialisasi awal untuk masuk mode 4bit, sesuai pada diagram alur pada lembar data
  _write4bits(0x03);
  delayMicroseconds(4500); // wait min 4.1ms
  // second try
  _write4bits(0x03);
  delayMicroseconds(4500); // wait min 4.1ms
  // third go!
  _write4bits(0x03);
  delayMicroseconds(150);
  // finally, set to 4-bit interface
  _write4bits(0x02);

  // selesai , atur jumlah line , dan jumalh nilai dots.
  _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
  command(LCD_FUNCTIONSET | _displayfunction);

  // nyalakan display - cursor off - blink off 
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // hapus 
  clear();

  // atur arah 
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

/*
 * @brief fungsi untuk mengirimkan perintah ke LCD
 * @param nilai_perintah
 */
void LiquidCrystal595::command(uint8_t value) {
  _send(value, LOW);
}

/*
 * @brief fungsi untuk mengirimkan data ke LCD
 * @param nilai_data
 */
void LiquidCrystal595::write(uint8_t value) {
  _send(value, HIGH);
}

/*
 * @brief fungsi untuk mencetak kata pada LCD
 * @param const char* string 
 */
void LiquidCrystal595::print(const char* str) {
  uint8_t i = 0;
  while (str[i] != 0) {
    write(str[i]);
    ++i;
  }
}

/*
 * @brief fungsi untuk menyalakan backlight pada LCD
 * @param none
 */
void LiquidCrystal595::backlight() {
  _data_buffer |=  (1 << 7);
  _shiftUpdate();
}

/*
 * @brief fungsi untuk mematikan backlight pada LCD
 * @param none 
 */
void LiquidCrystal595::noBacklight() {
  _data_buffer &=  ~(1 << 7);
  _shiftUpdate();
}

/*
 * @brief fungsi untuk mengambil nilai kondisi backlight
 * @param none 
 * @retval boolean (1 = backlight_on ) - (0 = backlight_off)
 */
bool LiquidCrystal595::getBacklight() {
  return (_data_buffer & 0x80) ? 1 : 0;
}

/*
 * @brief fungsi untuk set alamat awal kolom pada tiap baris LCD
 * @param row0,row1,row2,row3
 */
void LiquidCrystal595::_setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/*
 * @brief fungsi untuk mengupdate status output pin 595 seuai nilai data_buffer saat ini 
 * @param none
 * @retval none
 */
void LiquidCrystal595::_shiftUpdate() {
  digitalWrite(_latch_pin, LOW);
  shiftOut(_data_pin, _clock_pin, MSBFIRST, _data_buffer);
  digitalWrite(_latch_pin, HIGH);
}

/*
 * @brief fungsi untuk mengirimkan sinyal PULSA pada pin ENABLE LCD
 * @param none
 * @retval none
 */
void LiquidCrystal595::_pulseEnable() {
  _data_buffer &= ~(1 << 2);
  _shiftUpdate();
  delayMicroseconds(1);
  _data_buffer |= (1 << 2);
  _shiftUpdate();
  delayMicroseconds(1);
  _data_buffer &= ~(1 << 2);
  _shiftUpdate();
  delayMicroseconds(100);
}

/*
 * @brief fungsi untuk mengirimkan data 4 bit ke LCD
 * @param nilai data
 */
void LiquidCrystal595::_write4bits(uint8_t value) {
  _data_buffer = (_data_buffer & 0x87) | ((value << 3) & 0x78);
  _shiftUpdate();

  _pulseEnable();
}

/*
 * @brief fungsi untuk mengirimkan data ( 8 Bit ) ke LCD
 * @param nilai data , mode ( HIGH = MODE_DATA - LOW = MODE_PERINTAH)
 */
void LiquidCrystal595::_send(uint8_t value, uint8_t mode) {
  if (mode) {
    _data_buffer |=  (1 << 0);
  } else {
    _data_buffer &= ~(1 << 0);
  }
  _shiftUpdate();

  _data_buffer &= ~(1 << 1);
  _shiftUpdate();

  _write4bits(value >> 4);
  _write4bits(value);
}

/********** perintah , untuk pengguna */

/*
 * @brief fungsi untuk menghapus tampilan dan mengembalikan posisi cursor pada posisi 0
 */
void LiquidCrystal595::clear() {
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

/*
 * @brief fungsi untuk mengembalikan posisi cursor pada posisi 0
 */
void LiquidCrystal595::home() {
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

/*
 * @brief fungsi untuk memindahkan posisi cursor
 * @param kolom, baris
 */
void LiquidCrystal595::setCursor(uint8_t col, uint8_t row) {
  if (row > _lines)
  {
    row = _lines;
  }
  command(LCD_SETDDRAMADDR | (_row_offsets[row] + col));
}

// Turn the display on/off (quickly)
void LiquidCrystal595::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal595::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// fungsi untuk kontrol cursor - noBlink() atau blink()
void LiquidCrystal595::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal595::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal595::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal595::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal595::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal595::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal595::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

//fungsi untuk menyimpan custom karakter pada GCRAM
// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal595::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    write(charmap[i]);
  }
}
