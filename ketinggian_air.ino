#include <SoftwareSerial.h>
#define ultrasonic_trigger_pin 8
#define ultrasonic_echo_pin 9
#define NOTIF_TINGGI "1"
#define NOTIF_NORMAL "2"
#define NOTIF_RENDAH "3"
#define NOTIF_OFF "4"
#define buzz_pin 22

const byte rxPin = 6;
const byte txPin = 7;

//----Var untuk ultarsonic
float lokasi_sensor = 35;
float batas_atas = lokasi_sensor - 20;
float batas_bawah = 0;


String status_ketinggian;
String recent_notif = "3";
String recent_status = "Rendah";
int recent_ketinggian = 0; //limit bawah untuk kondisi normal

boolean initial =  true;


SoftwareSerial ESP8266 (rxPin, txPin);

int counter_tinggi = 0;
int counter_normal = 0;
int counter_rendah = 0;

String notif = "";

void setup() {
  pinMode(ultrasonic_trigger_pin, OUTPUT);
  pinMode(ultrasonic_echo_pin, INPUT);
  pinMode(buzz_pin,OUTPUT);
  Serial.begin(115200);   
  ESP8266.begin(115200);
}

void loop(){
  Serial.println("============================================================================================");
  String data = bacaKetinggian();
  if(initial){
    initial = false;
  }else{
    String server = "http://192.168.43.3/automation_system_adi/index.php/api/post_data/" + data;  
    ESP8266.print(server);
  }
  delay(1000);
  Serial.print("Counter rendah : ");
  Serial.println(counter_rendah);
  Serial.print("Counter normal : ");
  Serial.println(counter_normal);
  Serial.print("Counter Tinggi : ");
  Serial.println(counter_tinggi);
  Serial.println("============================================================================================");
}

String bacaKetinggian(){
  Serial.println("Menjalankan Fungsi bacaKetinggian()");
  long duration, distance ;
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
  ketinggian_air = (lokasi_sensor - distance) - 1;
  Serial.print("Ketinggian air saat ini :  ");
  Serial.println(ketinggian_air);
  delay(3000);  
  
  
  
  String retVal;
  if(ketinggian_air < 0 || ketinggian_air > 20){
    Serial.print("Pembacaan sensor Minus : ");
    retVal = (String)recent_ketinggian +"/"+ recent_status + "/" + recent_notif + "/" ;  
  }else
  if(ketinggian_air >= 0 && ketinggian_air <= 20){
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
    if(counter_tinggi >= 10){
        notif = NOTIF_TINGGI;
        digitalWrite(buzz_pin,HIGH);
    }else
    if(counter_normal >= 10){
        notif = NOTIF_NORMAL;
        digitalWrite(buzz_pin,LOW);
    }else
    if(counter_rendah >= 10){
        notif = NOTIF_RENDAH;
        digitalWrite(buzz_pin,LOW);
    }else
    if(counter_rendah < 10 || counter_normal < 10 || counter_tinggi < 10){
        notif = recent_notif;
        digitalWrite(buzz_pin,LOW);
    }
    Serial.print("Pembacaan sensor tidak Minus : ");
    retVal = (String)ketinggian_air +"/"+ status_ketinggian + "/" + notif + "/" ;  
    recent_ketinggian = ketinggian_air;
    recent_notif = notif;
    recent_status = status_ketinggian;
  }
  Serial.println(retVal);
  return retVal;
}
