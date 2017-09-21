 //--deklarasi library
#include <SoftwareSerial.h>

//deklarasi konstanta
#define ultrasonic_trigger_pin 8 
#define ultrasonic_echo_pin 9
#define NOTIF_TINGGI "1"
#define NOTIF_NORMAL "2"
#define NOTIF_RENDAH "3"
const byte rxPin = 6;
const byte txPin = 7;

//--deklarasi global variable
float lokasi_sensor = 35;
float batas_atas = lokasi_sensor - 20;
float batas_bawah = 0;
String current_notif = "";
String recent_notif = "";
int recent_ketinggian = 0;
SoftwareSerial ESP8266 (rxPin, txPin);
int counter_tinggi = 0;
int counter_normal = 0;
int counter_rendah = 0;
String notif = "";

//--deklarasi procedur setup mikrokontroler meliputi pin ultrasonik dan serial
void setup() {
  pinMode(ultrasonic_trigger_pin, OUTPUT);
  pinMode(ultrasonic_echo_pin, INPUT);
  Serial.begin(115200);   
  ESP8266.begin(115200);
}

//--deklarasi procedur sebuah proses yang akan dilakukan secara kontinyu oleh mikrokontroler
void loop(){
  String data = bacaKetinggian();
  String server = "http://192.168.43.3/automation_system_adi/index.php/api/post_data/" + data;
  ESP8266.print(server);
  delay(1000);
  int ctr = 1;
  //do{
    Serial.println(ctr);
    if(ESP8266.available()){
      String response = ESP8266.readString();
      Serial.println(response);
    }
    ctr++;  
    //delay(1000);
  //}while(!ESP8266.available() || ctr >=15);
  delay(1000);
}

//--deklarasi function untuk membac ketinggian air dengan tipe data string, dimana string tersebut akan menjadi parameter url yang dikirim ke server
String bacaKetinggian(){
  Serial.println("Menjalankan Fungsi bacaKetinggian()");
  long duration, distance;
  int ketinggian_air;
  digitalWrite(ultrasonic_trigger_pin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(ultrasonic_trigger_pin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(ultrasonic_trigger_pin, LOW);
  duration = pulseIn(ultrasonic_echo_pin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print("Sensor diletakan di ketinggian : ");
  Serial.println(lokasi_sensor); 
  ketinggian_air = lokasi_sensor - distance;
  Serial.print("Ketinggian air saat ini :  ");
  Serial.println(ketinggian_air);
  String status_ketinggian = "";

//--awal proses fuzzy logic
  //untuk kondisi Tinggi
  if (ketinggian_air >= batas_atas){
    counter_tinggi ++;
    counter_normal = 0 ;
    counter_rendah = 0 ;
    status_ketinggian = "Tinggi";
    Serial.println("Ketinggian air sudah masuk batas atas, mohon untuk dibuka pintu Bendungan");
  }else
  //untuk kondisi normal
  if (ketinggian_air < batas_atas && ketinggian_air > (batas_atas - 10)){
    counter_tinggi = 0;
    counter_normal ++ ;
    counter_rendah = 0 ;
    status_ketinggian = "Normal";
    Serial.println("Ketinggian air kondisi normal");
  }else 
  //untuk kodisi rendah 
  if (ketinggian_air <= (batas_atas - 10)){
    counter_tinggi = 0;
    counter_normal = 0 ;
    counter_rendah ++ ;
    status_ketinggian = "Rendah";
    Serial.println("Ketinggian air kondisi surut, mohon untuk ditutup puntu Bendungan");
  }
  delay(3000);  
  
  if(recent_notif.equals("")){
    recent_notif = NOTIF_NORMAL;  
  }
  
  if(counter_tinggi >= 10){
      notif = NOTIF_TINGGI;
  }else
  if(counter_normal >= 10){
      notif = NOTIF_NORMAL;
  }else
  if(counter_rendah >= 10){
      notif = NOTIF_RENDAH;
  }else
  if(counter_rendah < 10 || counter_normal < 10 || counter_tinggi < 10){
      notif = recent_notif;
  }
  String retVal;
  if(ketinggian_air < 0 || ketinggian_air > 20){
    retVal = (String)recent_ketinggian +"/"+ status_ketinggian + "/" + notif + "/" ;  
  }else
  if(ketinggian_air >= 0 && ketinggian_air <= 20){
    retVal = (String)ketinggian_air +"/"+ status_ketinggian + "/" + notif + "/" ;  
    recent_ketinggian = ketinggian_air;
  }
  recent_notif = current_notif;
  return retVal; //nilai return dari function baca ketinggian //-- akhir proses fuzzy logic
}
 
