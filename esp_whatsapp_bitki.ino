#include <WiFi.h>    
#include <HTTPClient.h>
#include <UrlEncode.h>

const char* ssid = "Superbox_Wifi_7830";
const char* password = "52J836mG3A";
String MobileNumber = "905312887601";
String APIKey = "1810192";
#include <dht11.h> // dht11 kütüphanesini kodlarımıza dahil ediyoruz.
int DhtPin=25; // DhtPin olarak Dijital 2'yi belirliyoruz.
dht11 dht_sensor; 
int sensorPin=33;
int sensorValue = 0; // Sensör verisi burada saklanacak
int tnem2;
int yuzde_toprak_nem;
int chk;
int su_motor=14; // Role modulunun sinyal pininin bulundugu yeri kart degişkenine tanımladık.  
unsigned long startTime; // Başlangıç zamanı saklamak için değişken
int dhtpow=26;
unsigned long oncekiMillis = 0;
const unsigned long interval = 1000; // 1 saniye
int sayac = 0;
int mesajsayac = 0;

unsigned long baslangicZamani; 
int tnpin=32;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Your Local IP address is: ");
  Serial.println(WiFi.localIP());
  baslangicZamani = millis();  // Başlangıç zamanını kaydetme
pinMode(su_motor, OUTPUT); // Role kartının bulundugu pini açılmasını sağladık. 
digitalWrite(su_motor, HIGH);
pinMode(tnpin, OUTPUT);
digitalWrite(tnpin, LOW);
pinMode(dhtpow, OUTPUT);
digitalWrite(dhtpow, LOW);
  delay(1000);
    sendMessage("OTOMATİK BİTKİ YETİŞTİRME");
;}

void arttirSayac() {
  unsigned long suankiMillis = millis();

  if (suankiMillis - oncekiMillis >= interval) {
    sayac++;
    mesajsayac++;

    oncekiMillis = suankiMillis;
  }
    if (sayac==5400){
      digitalWrite(tnpin, HIGH);
      digitalWrite(dhtpow, HIGH);
      delay(600000);
      digitalWrite(tnpin, LOW);
      digitalWrite(dhtpow, LOW);
      sayac=0;
      }
}

void loop() {
       int tnpdurum = digitalRead(tnpin);
   chk = dht_sensor.read(DhtPin);
   unsigned long gecenSure = millis() - baslangicZamani; // Geçen süreyi hesapla
  int toprak_nem = analogRead(sensorPin);
tnem2 = map(toprak_nem,0,4095,0,100);
yuzde_toprak_nem =(100-tnem2);
          unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    unsigned long days = elapsedTime / (24UL * 60UL * 60UL * 1000UL);
    unsigned long remainingMillis = elapsedTime % (24UL * 60UL * 60UL * 1000UL);

    unsigned long hours = remainingMillis / (60UL * 60UL * 1000UL);
    remainingMillis %= (60UL * 60UL * 1000UL);

    unsigned long minutes = remainingMillis / (60UL * 1000UL);
    remainingMillis %= (60UL * 1000UL);

    unsigned long seconds = remainingMillis / 1000UL;
  delay(1000);
 
  if (yuzde_toprak_nem<50){
  if (tnpdurum == LOW || tnpdurum==0){
digitalWrite(su_motor, LOW);
delay(1200);
digitalWrite(su_motor, HIGH);
delay(15000);
}
}
if (yuzde_toprak_nem>50){
digitalWrite(su_motor, HIGH);
}
arttirSayac();
Serial.println(sayac);
Serial.println(mesajsayac);
Serial.println(toprak_nem);
Serial.println(yuzde_toprak_nem);
Serial.println(tnpdurum);
if (mesajsayac==5){
sendMessage("OTOMATİK BİTKİ YETİŞTİRME");
delay(500);
sendMessage("Toprak Nemi: %"+String(yuzde_toprak_nem));
delay(500);
sendMessage("Esp Yüzey Isısı/Nemi:  "+ String(dht_sensor.temperature) +"C / "+"%"+String(dht_sensor.humidity));
delay(500);
sendMessage(String(days)+" Gün "+String(hours)+" Saat "+String(minutes)+" Dakika "+String(seconds)+" Saniyedir Açık");
mesajsayac=0;
}
  }

void sendMessage(String message){
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + MobileNumber + "&apikey=" + APIKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
