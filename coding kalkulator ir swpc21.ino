#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h> 
#include <math.h>

// Inisialisasi
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definisi
const int IR_RECEIVER_PIN = 7;
const int BUZZER_PIN = 11;

// Variabel logika
double input_sekarang = 0;
double nilai_simpan = 0;
char operator_aktif = ' ';
bool barusan_hitung = false;
bool decimal_mode = false;
double decimal_multiplier = 0.1;

// FUNGSI BUZZER
void buatNadaManual(unsigned int frekuensi, unsigned long durasi_ms) {
  // jeda waktu gelombang berdasarkan frekuensi ms
  long jeda_mikro = 1000000L / frekuensi / 2;
  long jumlah_siklus = (durasi_ms * 1000L) / (jeda_mikro * 2);
  
  for (long i = 0; i < jumlah_siklus; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(jeda_mikro);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(jeda_mikro);
  }
}

// Implementasi sound
void bipPendek() {
  buatNadaManual(2000, 60);
}

void biBiP() {
  buatNadaManual(1500, 70);
  delay(80);                 
  buatNadaManual(2000, 110); 
}

// FUNGSI kalkulasi berantai
void eksekusiOperasiSederhana() {
  if (operator_aktif == '+') nilai_simpan = nilai_simpan + input_sekarang;
  else if (operator_aktif == '-') nilai_simpan = nilai_simpan - input_sekarang;
  else if (operator_aktif == '*') nilai_simpan = nilai_simpan * input_sekarang;
  else if (operator_aktif == '/') {
    if (input_sekarang != 0) nilai_simpan = nilai_simpan / input_sekarang;
    else nilai_simpan = 0;
  }
  else if (operator_aktif == '%') nilai_simpan = fmod(nilai_simpan, input_sekarang);
  else if (operator_aktif == '^') nilai_simpan = pow(nilai_simpan, input_sekarang);
}

void perbaruiLayar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  // 1. Menampilkan baris riwayat operasi di atas
  if (operator_aktif != ' ') {
    // Cek apakah nilai_simpan itu angka bulat
    if (nilai_simpan == floor(nilai_simpan)) {
      lcd.print((long)nilai_simpan); // Tampilkan tanpa koma jika bulat
    } else {
      lcd.print(nilai_simpan, 4);    // Tampilkan 4 desimal jika pecahan
    }
    lcd.print(" ");
    lcd.print(operator_aktif);
  } else {
    lcd.print("wasap yo :v");
  }
  
  // 2. Menampilkan angka input/hasil utama di baris bawah
  lcd.setCursor(0, 1);
  if (input_sekarang == floor(input_sekarang)) {
    lcd.print((long)input_sekarang); // Sederhana: T Tampilkan angka bulat langsung (tanpa .0000)
  } else {
    lcd.print(input_sekarang, 4);    // Otomatis tampilkan desimal jika berupa pecahan
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // buzzer mati di awal


  lcd.init();
  lcd.backlight();
  
  // Memulai sensor IR dengan feedback dinonaktifkan biar hemat resource
  IrReceiver.begin(IR_RECEIVER_PIN, DISABLE_LED_FEEDBACK);
  
  lcd.setCursor(0, 0);
  lcd.print("math logic");
  lcd.setCursor(0, 1);
  lcd.print("boot...");
    buatNadaManual(1500, 100); // Nada 1 
  delay(50);
  buatNadaManual(1000, 100); // Nada 2 
  delay(50);
  buatNadaManual(2000, 150); // Nada 3 

  delay(3000);
  perbaruiLayar();
}

void tambahAngka(int angka) {
  if (barusan_hitung) {
    input_sekarang = 0;
    barusan_hitung = false;
  }
  
  if (!decimal_mode) {
    input_sekarang = (input_sekarang * 10) + angka;
  } else {
    input_sekarang = input_sekarang + (angka * decimal_multiplier);
    decimal_multiplier *= 0.1;
  }
}

double keRadian(double derajat) {
  return derajat * M_PI / 180.0;
}

void loop() {
  // Cek data IR input
  if (IrReceiver.decode()) {
    
    
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      
      uint32_t code = IrReceiver.decodedIRData.decodedRawData;
      bool tombolValid = true;
      
      switch(code) {
        // GANTI KODE SESUAI KODE HEX REMOTE KAMU!
        case 0xFF007F80: tambahAngka(0); bipPendek(); break; 
        case 0xFE017F80: tambahAngka(1); bipPendek(); break; 
        case 0xFD027F80: tambahAngka(2); bipPendek(); break; 
        case 0xFC037F80: tambahAngka(3); bipPendek(); break; 
        case 0xFB047F80: tambahAngka(4); bipPendek(); break; 
        case 0xFA057F80: tambahAngka(5); bipPendek(); break; 
        case 0xF9067F80: tambahAngka(6); bipPendek(); break; 
        case 0xF8077F80: tambahAngka(7); bipPendek(); break; 
        case 0xF7087F80: tambahAngka(8); bipPendek(); break; 
        case 0xF6097F80: tambahAngka(9); bipPendek(); break; 

        // main OPERATOR MATH
        case 0xAF507F80: 
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '+'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;
          
        case 0xAE517F80: 
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '-'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;
          
        case 0xED127F80: 
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '*'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;
          
        case 0xE7187F80: 
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '/'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;
          
        case 0xEA157F80: // % (persen)
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '%'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;
          
        case 0xE6197F80: // ^ (Pangkat)
          if (operator_aktif == ' ') nilai_simpan = input_sekarang;
          else eksekusiOperasiSederhana();
          operator_aktif = '^'; input_sekarang = 0; decimal_mode = false; bipPendek(); break;

        // --- SAKLAR SISTEM dan PECAHAN ---
        case 0xB44B7F80: // CE
          input_sekarang = 0; nilai_simpan = 0; operator_aktif = ' '; decimal_mode = false; barusan_hitung = false; bipPendek(); break;
        case 0xA05F7F80: // +/-
          input_sekarang = -input_sekarang; bipPendek(); break;
        case 0xBE417F80: // Tombol Titik / Desimal
          if (!decimal_mode) { decimal_mode = true; decimal_multiplier = 0.1; } bipPendek(); break;

        // --- RUMUS ILMIAH/SCIENTIFIC INSTAN ---
        case 0xB54A7F80: input_sekarang = sqrt(input_sekarang); barusan_hitung = true; bipPendek(); break; // sqrt
        case 0xF40B7F80: input_sekarang = abs(input_sekarang); barusan_hitung = true; bipPendek(); break;  // abs
        case 0xE31C7F80: if(input_sekarang != 0) input_sekarang = 1.0 / input_sekarang; barusan_hitung = true; bipPendek(); break; // 1/x
        
        // Fungsi Trigonometri
        case 0xEE117F80: input_sekarang = sin(keRadian(input_sekarang)); barusan_hitung = true; bipPendek(); break; // sin
        case 0xAB547F80: input_sekarang = cos(keRadian(input_sekarang)); barusan_hitung = true; bipPendek(); break; // cos
        case 0xEF107F80: input_sekarang = tan(keRadian(input_sekarang)); barusan_hitung = true; bipPendek(); break; // tan
        
        // Kebalikan Trigonometri
        case 0xF50A7F80: input_sekarang = asin(input_sekarang) * 180.0 / M_PI; barusan_hitung = true; bipPendek(); break; // asin
        case 0xBA457F80: input_sekarang = acos(input_sekarang) * 180.0 / M_PI; barusan_hitung = true; bipPendek(); break; // acos
        case 0xBD427F80: input_sekarang = atan(input_sekarang) * 180.0 / M_PI; barusan_hitung = true; bipPendek(); break; // atan
        
        case 0xBB447F80: input_sekarang = M_E; barusan_hitung = true; bipPendek(); break; // Nilai Logaritma Alami e
        case 0xBC437F80: input_sekarang = exp(input_sekarang); barusan_hitung = true; bipPendek(); break; // e^x
        case 0xA8577F80: if(input_sekarang > 0) input_sekarang = log(input_sekarang); barusan_hitung = true; bipPendek(); break; // ln
        case 0xB04F7F80: if(input_sekarang > 0) input_sekarang = log10(input_sekarang); barusan_hitung = true; bipPendek(); break; // log

        
        case 0xEB147F80: // =
          if (operator_aktif != ' ') {
            eksekusiOperasiSederhana();
            input_sekarang = nilai_simpan; 
            nilai_simpan = 0;
            operator_aktif = ' ';
            barusan_hitung = true;
            decimal_mode = false;
            biBiP(); 
          }
          break;

        default:
          tombolValid = false;
          break;
      }
      
      if(tombolValid) {
        perbaruiLayar();
      }
    }
    
    // clean buffer penerima agar IR menangkap sinyal lagi
    IrReceiver.resume(); 
  }
}
