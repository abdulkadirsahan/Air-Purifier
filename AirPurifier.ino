#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin tanımlamaları
#define BUZZER_PIN 8       // Buzzer'ın bağlı olduğu pin
#define DHTPIN 2           // DHT sensörünün veri pini
#define FAN_PIN 9          // Transistörün base ucunun bağlı olduğu pin
#define MQ135_PIN A0       // MQ-135 sensörünün bağlı olduğu pin

// DHT sensörünün tipi
#define DHTTYPE DHT11      // 

DHT dht(DHTPIN, DHTTYPE);  // DHT sensör nesnesi oluşturma

// OLED ekran ayarları
#define SCREEN_WIDTH 128   // OLED ekran genişliği, piksel cinsinden
#define SCREEN_HEIGHT 64   // OLED ekran yüksekliği, piksel cinsinden
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // OLED ekran nesnesi oluştur

void setup() {
  Serial.begin(9600);          
  dht.begin();                 // DHT sensörünü başlat
  pinMode(FAN_PIN, OUTPUT);    // Fan pinini çıkış olarak ayarla 
  analogWrite(FAN_PIN, 0);     // Fanı başlangıçta kapalı başlat

  pinMode(BUZZER_PIN, OUTPUT); // Buzzer pinini çıkış olarak ayarla
  digitalWrite(BUZZER_PIN, LOW); // Buzzer başlangıçta kapalı
  
  // OLED ekranı başlat
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C adresini kontrol et, 0x3C genelde kullanılır
  display.clearDisplay(); // Ekranı temizle
  display.setTextSize(1); // Metin boyutunu ayarla
  display.setTextColor(WHITE); // Metin rengini ayarla
}

void loop() {
  // Hava kalitesi ölçümü
  int mq135Value = analogRead(MQ135_PIN); // MQ-135 sensör değerini oku

  // DHT sensöründen sıcaklık ve nem değerlerini oku
  float h = dht.readHumidity();         // Nem değerini oku
  float t = dht.readTemperature();      // Sıcaklık değerini oku
  
  // Okunan değerleri seri monitöre yazdır
  Serial.print("CO2 Degeri: "); 
  Serial.print(mq135Value);
  Serial.print(" | Sıcaklık: ");
  Serial.print(t);
  Serial.print(" °C | Nem: ");
  Serial.print(h);
  Serial.println(" %");

  // OLED ekranında verileri göster
  display.clearDisplay(); // Ekranı temizle
  display.setCursor(0, 0); // Başlangıç konumunu ayarla
  display.print("CO2 Degeri: ");
  display.print(mq135Value);
  display.println("   "); // Satır boşluğu bırak
  display.print("Sicaklik: ");
  display.print(t);
  display.println(" C");
  display.print("Nem: ");
  display.print(h);
  display.println(" %");
  
  // Ekranı güncelle
  display.display(); 

  // Hava kalitesi ve sıcaklık/nem durumuna göre fanı kontrol et
  if (mq135Value > 400 || t > 50 || h > 100) { // Örneğin,MQ-135 değeri 700'ü, sıcaklık 20°C'yi veya nem 100%'yi aştığında
    analogWrite(FAN_PIN, 255); // Fanı maksimum hızda çalıştır
    Serial.println("Fan Açık");
  } else {
    analogWrite(FAN_PIN, 0); // Fanı kapat
    Serial.println("Fan Kapalı");
  }

  // CO2 seviyesi 700'ü geçerse buzzer'ı aktif et
  if (mq135Value > 700) {
    digitalWrite(BUZZER_PIN, HIGH); // Buzzer'ı aç
    Serial.println("Buzzer Çalıyor! CO2 Seviyesi Yüksek");
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Buzzer'ı kapat
  }

  delay(2000); // 2 saniye bekle
}

