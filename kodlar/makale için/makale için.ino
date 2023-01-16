//------------------------------------------------------------------------------------LCD ve gaz sensörü kütüphanelerimizi tanıtıyoruz------------------------------------------

#include <MQ2.h>                              //Gaz sensörü kütüphanesi.
#include <LiquidCrystal_I2C.h>                //LCD ekran kütüphanesi.
LiquidCrystal_I2C lcd(0x3F, 16, 2);           //LCD ekranın özelliklerini tanıtıyoruz.

//------------------------------------------------------------------------------------RFID kütüphanelerini tanıtıyoruz----------------------------------------------------------

#include <SPI.h>                              //İletiişim kütüphanesi.
#include <MFRC522.h>                          //RFID kütüphanesi.
#include <Servo.h>                            //Servo kütüphanesi.

//------------------------------------------------------------------------------------RFID ve servonun pininini tanıtıyourz-----------------------------------------------------

int RST_PIN = 9;                              //RFID nin reset pinini tanıtıyoruz.
int SS_PIN = 10;                              //RFID nin chip select pinini tanıtıyoruz.
int servoPin = 8;                             //Servo motor pinini tanıtıyouz.

//------------------------------------------------------------------------------------Buzzer pininini tanıtıyourz----------------------------------------------------------------

int buzzerPin = 4;                            //Buzzer pinini tanıtıyoruz.

//------------------------------------------------------------------------------------Esik değeri beliriyoruz---------------------------------------------------------------------

int esikDeger = 200;                          //Esik değeri belirliyoruz.
  
//------------------------------------------------------------------------------------Led pininini tanıtıyourz--------------------------------------------------------------------

int yled = 5;                                 //Yeşil led pini tanıtıyoruz.
int kled = 6;                                 //Kırmızı led pini tanıtıyoruz.

//------------------------------------------------------------------------------------RFID ID bilgisini giriyoruz-----------------------------------------------------------------

Servo motor;                                  //Servo motor için değişken oluşturuyoruz.
MFRC522 rfid(SS_PIN, RST_PIN);                //RFID modülünün ayarlarını yapıyoruz.
byte ID[4] = {101,217,0,109};                 //ID bilgisini giriyoruz.

//------------------------------------------------------------------------------------Gaz sensörünün pinlerini tanıtıyoruz--------------------------------------------------------

MQ2 mq2(Analog_Input);                        //Gaz sensörünü analog giriş olarak tanıtıyoruz.
int Analog_Input = A0;                        //Gaz sensörünü analog girişni A0 olarak tanıtıyoruz.
int smoke;                                    //Dumanı (bozulam oranını ölçüyoruz).


void setup(){

//------------------------------------------------------------------------------------Lcd ve gaz sensörünü başlatıyoruz------------------------------------------------------------

  lcd.init();                                 //Lcd ekranı başlatıyoruz.
  lcd.backlight();                            //Lcd ekranın arka ışıgını başlatıyoruz.
  mq2.begin();                                //Gaz sensörünü başlatıyoruz.

//------------------------------------------------------------------------------------RFID yi başlatıyoruz-------------------------------------------------------------------------

  motor.attach(servoPin);                     //Servo motor pinini motor değişkeni ile ilişkilendiyirouz.
  SPI.begin();                                //İletişim kütüphanesini başlatıyoruz.
  rfid.PCD_Init();                            //RFID modülünü başlatıyoruz.

//------------------------------------------------------------------------------------Buzzerın girişini belirtiyoruz----------------------------------------------------------------
 
  pinMode(buzzerPin,OUTPUT);                  //Buzzer pinini çıkış olarak ayarlıyoruz.


//------------------------------------------------------------------------------------Led girişleri belitiroyuz---------------------------------------------------------------------

  pinMode (yled,OUTPUT);                      //Yeşil ledi çıkış olarak ayarlıyoruz.
  pinMode (kled,OUTPUT);                      //Kırmızı ledi çıkış olarak ayarlıyoruz.
  
}

void loop(){

//------------------------------------------------------------------------------------Gaz sensörünü okuyoruz------------------------------------------------------------------------

  float* values= mq2.read(true);              //Gazdan gelen bilgileri okuyoruz.
  smoke = mq2.readSmoke();                    //Gazdan duman bilgsini okuyoruz.

//------------------------------------------------------------------------------------Buzzeri ve ledi yakıyoruz-------------------------------------------------------------------- 

   int bilgi = analogRead (A0);               //Bilgi için gaz sensörünü kullanıyoruz.
  if (bilgi>esikDeger) {                      //Gelen değerin esik değerden büyük olursa yapılıcakları;
    digitalWrite(buzzerPin,0);                //Buzzer pinini devre dışı bırakıyoruz.
    digitalWrite(kled,1);                     //Kırmızı ledi aktifleştiriyoruz.
    digitalWrite(yled,0);                     //Yeşil ledi devre dışı bırakıyoruz.
    delay(70);                                //70ms bekliyoruz.
    digitalWrite(buzzerPin,1);                //Buzzer pinini etkinleştiriyoruz.
    delay(70);                                //70ms bekliyoruz.
    digitalWrite(buzzerPin,0);                //Buzzer pinin etkinleştiriyoruz.
    delay(70);                                //70ms bekliyoruz.
    digitalWrite(kled,1);                     //Kırmızı led aktifleştiriyoruz.
    delay(70);                                //70ms bekliyoruz.
    digitalWrite(kled,0);                     //Kırmızı led aktifleştiriyoruz.
    delay(70);                                //70ms bekliyoruz.                              

  //son 8 satırın amacı kırmızı ledin ve buzzerin açılıp kapanmasını sağlamak.
  } 
  else{                                       //Yetkisiz girişte içerideki komutlar çalıştırılır.
    digitalWrite(buzzerPin,0);                //Buzzer pinin devre dışı bırakıyoruz.                
    digitalWrite(kled,0);                     //Kırmızı ledi devredışı bırakıyoruz.
    digitalWrite(yled,1);                     //Yeşil ledi başlatıyırouz.
  }
  
//------------------------------------------------------------------------------------Gaz değerlerini lcd eranda okuyoruz-----------------------------------------------------------  

  lcd.setCursor(1,0);                         //Lcd ekranın 2.satırın 1 sütünündan yazmaya başlıyoruz.
  lcd.print("Bozulma orani:");                //Lcd ekranın 1.satırın 1 sütününa bozulma oranı yazdırıyoruz.
  lcd.setCursor(7,1);                         //Lcd ekranın 2.satırın 7 sütünündan yazmaya başlıyoruz.
  lcd.print((smoke*100)/10000);               //Bozulma oranını 100/10000 oranla okuyoruz.
  lcd.print(" %");                            //Lcd ekranın 2.satırın 7 sütününa % değeri ekliyoruz.
  delay(100);                                 //100ms bekliyoruz.

//------------------------------------------------------------------------------------RFID nin ID karşılaştırılması------------------------------------------------------------------  

  if ( ! rfid.PICC_IsNewCardPresent())        //Yeni kartın okunmasını bekliyoruz.
    return;

  if ( ! rfid.PICC_ReadCardSerial())          //Kart okunmadığı zaman bekliyoruz.
    return;

  if (rfid.uid.uidByte[0] == ID[0] &&         //Okunan kart ID si ile ID değişkenini karşılaştırıyoruz.
    rfid.uid.uidByte[1] == ID[1] && 
    rfid.uid.uidByte[2] == ID[2] && 
    rfid.uid.uidByte[3] == ID[3] ) {

//------------------------------------------------------------------------------------Servoyu dödnrürüyoruz--------------------------------------------------------------------------      

        motor.write(180);                     //Servo motoru 180 dereceye getiriyoruz.
        delay(1000);                          //1sn bekliyoruz.
        motor.write(0);                       //Servo motoru 0 dereceye getiriyoruz.
        delay(100);                           //1msn bekliyoruz.

//------------------------------------------------------------------------------------Lcd ekrana başarılı giriş yazdırma-------------------------------------------------------------

        lcd.clear();                          //Lcd ekranı temizliyoruz.   
        lcd.setCursor(0,0);                   //Lcd ekranın 1.satır 1.sütünündan yazmaya başlıyoruz.
        lcd.print("  Motor kapagi");          //Lcd ekranın 1.satır 1.sütününa motor kapaağı yazıdırıotuez.     
        lcd.setCursor(6,1);                   //Lcd ekranın 2.satırın 6 sütünündan yazmaya başlıyoruz.
        lcd.print("acik");                    //Lcd ekranın 2.satırın 6 sütününa  açık yazıyoruz.
        delay(3000);                          //3000ms bekliyoruz.
    }
    else{                                     //Yetkisiz girişte içerideki komutlar çalıştırılır.

//------------------------------------------------------------------------------------Lcd ekrana başarısız giriş yazdırma-------------------------------------------------------------

      lcd.clear();                            //Lcd ekranın temizliyoruz.
      lcd.setCursor(0,0);                     //Lcd ekranın 1.satır 1.sütünündan yazmaya başlıyoruz.
      lcd.print(" Yetkisiz Giris");           //Lcd ekranın 1.satır 1.sütününa yetkisiz giriş yazdırıyoruz.
      delay(3000);                            //3000ms bekliyoruz.
   }
  rfid.PICC_HaltA();                          //RFID kartın yeni kart okuyana kadar durmasını sağlıyoruz.



}

//*************************************************************** M_YAHYA ********************************************************

