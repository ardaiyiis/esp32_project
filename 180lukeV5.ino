//WiFi.mode(WIFI_OFF); WIFI KAPATMA ÖZELLİĞİ EKLENECEK.
//MANUAL WEB SERVERDAN SÜRME ÖZELLİĞİ EKLENECEK. PAN VE TİLT İÇİN.

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>       
#include <WebServer.h>  
#include "Gui.h"   // .h file that stores your html page code
#include <iostream>
#include <sstream>
#include "esp_heap_caps.h"

#define MAX_EASING_SERVOS 2
#define ENABLE_EASE_QUADRATIC
#define PI 3.1415

#include "ServoEasing.hpp"
#include "PinDefinitionsAndMore.h" //PAN SERVO PİNİ 13 (panServo) Olarak belirlenmiştir. TILT SERVO PİNİ 12 (tiltServo) Olarak belirlenmiştir. 

// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
//#define USE_INTRANET

int tiltLimitH = 85;
int tiltLimitL = 5;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[2048];

bool SomeOutput = false;

// just some buffer holder for char operations
char buf[32];

//#include <nvs_flash.h> //FLASH BELLEĞİ TEMİZLEMEK İÇİN.

#define LASER_PIN 15
#define PAN_PIN 13
#define TILT_PIN 12

Preferences laserObject;
Preferences motionObject;
Preferences panLimitMaxObject;
Preferences panLimitMinObject;
Preferences tiltLimitMaxObject;
Preferences tiltLimitMinObject;
Preferences pan0, pan1, pan2, pan3, tilt0, tilt1, tilt2, tilt3;

int panValue0,panValue1,panValue2,panValue3,tiltValue0,tiltValue1,tiltValue2,tiltValue3;

ServoEasing panServo; //pan
ServoEasing tiltServo; //tilt

//durum takibi için gerekli değişkenler
bool laserStatus;
bool motionStatus; 
bool zigzagStatus = true;        
bool buttonStatus; //WEB BUTONU!             
int lastButtonStatus = LOW;

//MANUAL MOVEMENT CONTROL PARAMETERS.
int manualPan = 85; 
int panSlider = 0;
int manualTilt = 85; 
int tiltSlider = 0; 

hw_timer_t * timer = NULL;
volatile uint8_t laserCycle = 0;


const char* password = "12345678";
IPAddress local_IP(192,168,1,0);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void setServos(int panDegree ,int tiltDegree, int servoSpeed, int easeType) {
  //Servoların Smooth dönmesi için PWM kontrol mantığını değiştir.

    panServo.setSpeed(servoSpeed);
    tiltServo.setSpeed(servoSpeed);

    if(easeType == 0) {
    panServo.setEasingType(EASE_LINEAR);
    //Serial.println(F("LINEAR S1"));
    tiltServo.setEasingType(EASE_LINEAR);
    //Serial.println(F("LINEAR S2"));
    }
    
    else if(easeType == 1) {
    panServo.setEasingType(EASE_QUADRATIC_IN_OUT);
    //Serial.println(F("QUADRATIC QUADRATIC IN OUT S1"));
    tiltServo.setEasingType(EASE_QUADRATIC_IN_OUT);
    //Serial.println(F("QUADRATIC QUADRATIC IN OUT S2"));
    }

    else if(easeType == 2) {    
    panServo.setEasingType(EASE_QUADRATIC_OUT);
    //Serial.println(F("QUADRATIC QUADRATIC OUT S1"));
    tiltServo.setEasingType(EASE_QUADRATIC_OUT);
    //Serial.println(F("QUADRATIC QUADRATIC OUT S2"));
    }
    
//TILT SINIRLAMASI  
    if(tiltDegree >= tiltLimitH){
      tiltDegree = tiltLimitH;
      }

     if(tiltDegree <= tiltLimitL){
      tiltDegree = tiltLimitL;
      }

//HAREKET BAŞLASIN
    if(motionStatus == true){
    panServo.setEaseTo(panDegree);
    tiltServo.setEaseTo(tiltDegree);
    setEaseToForAllServosSynchronizeAndStartInterrupt(servoSpeed);
    while (ServoEasing::areInterruptsActive()) {
      server.handleClient();
    }
    }

}

int zigzagMotion(float panPre, float panPost, float tiltPre, float tiltPost, int speedValue, int typeValue) {
    // Define the two points
    float x1 = panPre, y1 = tiltPre;
    float x2 = panPost, y2 = tiltPost;
      

    // Calculate the distance between the two points
    float d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

    // Calculate the angle between the two points
    float theta = atan2(y2 - y1, x2 - x1);

    // Define the spring amplitude and frequency
    int A = 12;
    int f = 1;

    // Calculate the wavelength and angular frequency of the spring
    int n = 8;
    float l = d / (n + 1);
    float w = 2 * PI * f;

     // Calculate the x and y coordinates of points along the spring
    int num_points = 100;
    float *x = (float*)malloc(num_points * sizeof(float));
    float *y = (float*)malloc(num_points * sizeof(float));
    float increment = (x2 - x1) / (num_points - 1);
    for (int i = 0; i < num_points; i++) {
        x[i] = x1 + i * increment;
        y[i] = y1 + A * sin(w * (x[i] - x1) / l) * sin(theta) + (y2 - y1 - A * sin(w * (d - l) / l)) * (x[i] - x1) / (x2 - x1);
    }

    // Calculate the derivatives
    float dx = x[1] - x[0];
    float dy, deriv[num_points - 1];
    for (int i = 0; i < num_points; i++) {
        if (i == 0 || i == num_points - 1) {
            dy = 0;
        } else {
            dy = (y[i + 1] - y[i - 1]) / (2 * dx);
        }
        if (i < num_points - 1) {
            deriv[i] = dy / dx;
        }
    }

    // Find the indices where the derivative changes sign
    int num_zero_deriv = 0;
    int *zero_deriv_indices_x = (int*)malloc(num_points * sizeof(int));
    int *zero_deriv_indices_y = (int*)malloc(num_points * sizeof(int));
    for (int i = 0; i < num_points - 2; i++) {
        if (deriv[i] != 0 && deriv[i + 1] != 0 && signbit(deriv[i]) != signbit(deriv[i + 1])) {
            zero_deriv_indices_x[num_zero_deriv] = i + 1;
            zero_deriv_indices_y[num_zero_deriv] = i + 1;
            num_zero_deriv++;
        }
    }

        for (int i = 0; i < num_zero_deriv; i++) {
        setServos(x[zero_deriv_indices_x[i]],y[zero_deriv_indices_y[i]],speedValue,typeValue);
//        Serial.println(x[zero_deriv_indices_x[i]],y[zero_deriv_indices_y[i]]);
    }
    free(x);
    free(y);
    free(zero_deriv_indices_x);
    free(zero_deriv_indices_y);
    return 0;
}


//RANDOM SEARCH PARAMETRELERİ.
int panLimitMax;
int panLimitMin;
int tiltLimitMax;
int tiltLimitMin;

int bolunmesayisiP;
int bolunmesayisiT;
int aralikdegeriP;
int aralikdegeriT;

int prePan = 90;
int preTilt = (tiltLimitH+tiltLimitL)/2;
int panValue;
int tiltValue;
int speedValue;
int typeValue;

void randomSearch() {
  bolunmesayisiP = random(4,8);
  bolunmesayisiT = random(4,8);
  aralikdegeriP = abs(panLimitMax-panLimitMin)/bolunmesayisiP;
  aralikdegeriT = abs(tiltLimitMax-tiltLimitMin)/bolunmesayisiT;

  for(int t=0; t<bolunmesayisiT; t++){
  for(int i=0; i<bolunmesayisiP; i++){
  panValue = random(min(panLimitMax,panLimitMin) + (aralikdegeriP*i) , min(panLimitMax,panLimitMin) + aralikdegeriP*(i+1));
  tiltValue = random(min(tiltLimitMax,tiltLimitMin) + (aralikdegeriT*t) , min(tiltLimitMax,tiltLimitMin) + aralikdegeriT*(t+1));
  
  speedValue = random(8,15); //15-30 default
  typeValue = random(0,2);
  
    if (zigzagStatus == false) {
      setServos(panValue,tiltValue,speedValue,typeValue);
    }
    
    else if (zigzagStatus == true) {
      Serial.println("RANDOM ICINDE ZIGZAGA GIRDI");  
      zigzagMotion(prePan,panValue,preTilt,tiltValue,speedValue,typeValue);
      Serial.println("RANDOM ICINDE ZIGZAGA CIKTI");

//
//        // Define the two points
//        double x1 = prePan, y1 = preTilt;
//        double x2 = panValue, y2 = tiltValue;
//          
//    
//        // Calculate the distance between the two points
//        double d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
//    
//        // Calculate the angle between the two points
//        double theta = atan2(y2 - y1, x2 - x1);
//    
//        // Define the spring amplitude and frequency
//        double A = 12;
//        double f = 1;
//    
//        // Calculate the wavelength and angular frequency of the spring
//        int n = 8;
//        double l = d / (n + 1);
//        double w = 2 * PI * f;
//    
//        // Calculate the x and y coordinates of points along the spring
//        int num_points = 200;
//        double x[num_points], y[num_points];
//        double increment = (x2 - x1) / (num_points - 1);
//        for (int i = 0; i < num_points; i++) {
//            x[i] = x1 + i * increment;
//            y[i] = y1 + A * sin(w * (x[i] - x1) / l) * sin(theta) + (y2 - y1 - A * sin(w * (d - l) / l)) * (x[i] - x1) / (x2 - x1);
//        }
//    
//        // Calculate the derivatives
//        double dx = x[1] - x[0];
//        double dy, deriv[num_points - 1];
//        for (int i = 0; i < num_points; i++) {
//            if (i == 0 || i == num_points - 1) {
//                dy = 0;
//            } else {
//                dy = (y[i + 1] - y[i - 1]) / (2 * dx);
//            }
//            if (i < num_points - 1) {
//                deriv[i] = dy / dx;
//            }
//        }
//    
//        // Find the indices where the derivative changes sign
//        int num_zero_deriv = 0;
//        int zero_deriv_indices_x[num_points], zero_deriv_indices_y[num_points];
//        for (int i = 0; i < num_points - 2; i++) {
//            if (deriv[i] != 0 && deriv[i + 1] != 0 && signbit(deriv[i]) != signbit(deriv[i + 1])) {
//                zero_deriv_indices_x[num_zero_deriv] = i + 1;
//                zero_deriv_indices_y[num_zero_deriv] = i + 1;
//                num_zero_deriv++;
//            }
//        }
//    
//            for (int i = 0; i < num_zero_deriv; i++) {
//            setServos(x[zero_deriv_indices_x[i]],y[zero_deriv_indices_y[i]],speedValue,typeValue);
//        }
//
//    
    }
    
  prePan = panValue;
  preTilt = tiltValue;
  
  }
  }
}


void servosAttach() {
          
  if (panServo.attach(SERVO1_PIN, 90) == INVALID_SERVO) {
        Serial.println(F("Error attaching PAN SERVO"));
        /*while (true) {
        Serial.println("PAN SERVOSU BAĞLANAMADI!");}*/
    }

  if (tiltServo.attach(SERVO2_PIN, (tiltLimitH+tiltLimitL)/2) == INVALID_SERVO) {
        Serial.println(F("Error attaching TILT SERVO"));
        /*while (true) {
        Serial.println("TILT SERVOSU BAĞLANAMADI");}*/
    }
    
  //dummyServo1.attach(DUMMY_SERVO1_PIN);
  //dummyServo2.attach(DUMMY_SERVO2_PIN); 
  
  delay(1000); 
}

void stationSetup(){
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void accessPointSetup(){
  
  Serial.print("Setting AP (Access Point)…");
  
  // AP Configuration
  WiFi.softAPConfig(local_IP, gateway, subnet);

  // Open to Wi-Fi network with SSID and password
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  //IPAddress IP = WiFi.softAPIP(); //RANDOM IP ADRESİ İÇİN.
  Serial.print("AP IP address: ");
  Serial.println(local_IP);
}

void IRAM_ATTR onTimer(){
  if(laserStatus == false){
  analogWrite(LASER_PIN, 0);
  }
  
  else{
  if(laserCycle<=3){
  analogWrite(LASER_PIN, 255);
  motionStatus=1;
  //WiFi.mode(WIFI_OFF); WIFI KAPATMA ÖZELLİĞİ EKLENECEK.
  }
  else if(laserCycle==4){
  analogWrite(LASER_PIN, 0);
  laserCycle=0;
  motionStatus=0;
  }
  laserCycle++;
  //Serial.println(laserCycle);
  }
}

void setup() {
  
  //nvs_flash_erase(); // Flash bellekteki bölümleri siler.
  //nvs_flash_init(); // Bölümleri yeniden tanımlar.
 
  Serial.begin(115200);
  
  //Nesne için "laserPin" ismi verildi. (15 karakteri geçemez!)
  laserObject.begin("laserPin", false); //false for both read and write.
  motionObject.begin("motionPin", false);
  
  panLimitMaxObject.begin("panLimitMax", false);
  panLimitMinObject.begin("panLimitMax", false);
  tiltLimitMaxObject.begin("panLimitMax", false);
  tiltLimitMinObject.begin("panLimitMax", false);
  
  panLimitMax = panLimitMaxObject.getInt("panLimitMax", false);
  panLimitMin = panLimitMinObject.getInt("panLimitMin", false);
  tiltLimitMax = panLimitMaxObject.getInt("tiltLimitMax", false);
  tiltLimitMin = panLimitMaxObject.getInt("tiltLimitMin", false);


  pan0.begin("pan0", false);
  pan1.begin("pan1", false);
  pan2.begin("pan2", false);
  pan3.begin("pan3", false);
  tilt0.begin("tilt0", false);
  tilt1.begin("tilt1", false);
  tilt2.begin("tilt2", false);
  tilt3.begin("tilt3", false);

  panValue0 = pan0.getInt("pan0", false);
  panValue1 = pan1.getInt("pan1", false);
  panValue2 = pan2.getInt("pan2", false);
  panValue3 = pan3.getInt("pan3", false);
  tiltValue0 = tilt0.getInt("tilt0", false);
  tiltValue1 = tilt1.getInt("tilt1", false);
  tiltValue2 = tilt2.getInt("tilt2", false);
  tiltValue3 = tilt3.getInt("tilt3", false);
    
  pinMode(LASER_PIN, OUTPUT);
  servosAttach();
  //setUpPinModes(); //GEREKSİZ.
    
  accessPointSetup();
  //stationSetup(); //accessPointSetup ile aynı anda kullanılamaz!

  //LAZERİN BAŞLANGIÇ DURUMU İÇİN.
  laserStatus = laserObject.getBool("state", false);
  
  if(laserStatus == true){
  analogWrite(LASER_PIN, 255); //BAŞLANGIÇTA AÇIK OLMASI İÇİN.
  }

  if(laserStatus == false){
  analogWrite(LASER_PIN, 0); //BAŞLANGIÇTA KAPALI OLMASI İÇİN.
  }

  //HAREKETİN BAŞLANGIÇ DURUMU İÇİN.
  motionStatus = motionObject.getBool("state", false);
  /*
  if(motionStatus == true){
  //BAŞLANGIÇTA AÇIK OLMASI İÇİN.
  }

  if(motionStatus == false){
  //BAŞLANGIÇTA KAPALI OLMASI İÇİN.
  }
  */
  
  //Serial.println("start timer ");
  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
  timerAlarmWrite(timer, 1000000000, true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmEnable(timer); // enable

  //disableCore0WDT();
  //maybe disable watch dog timer 1 if needed
  //disableCore1WDT();
  
  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  
  server.on("/UPDATE_SLIDER_0", UpdateSlider_0);
  server.on("/UPDATE_SLIDER_1", UpdateSlider_1);
  server.on("/BUTTON_0", ProcessButton_0);
  server.on("/BUTTON_1", ProcessButton_1);
  server.on("/UPDATE_FORMS", UpdateForms);
  
  server.begin();
}

void loop() {

  // vTaskDelay(portMAX_DELAY); //INTERRUPTTA BİR SIKINTI YAŞARSAN ÇAĞIR.

  if(motionStatus == true){
    randomSearch();
  }
  Serial.print("Free heap: ");
  Serial.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
  server.handleClient();
}

void UpdateSlider_0() {

  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("VALUE");

  // convert the string sent from the web page to an int
  manualPan = t_state.toInt();
  //Serial.print("UpdateSlider"): Serial.println(FanSpeed)

//SLIDER İÇİN GERİ BESLEME
  panSlider = map(manualPan, 0 , 180, 0, 180);
  strcpy(buf,"");
  sprintf(buf,"%d", panSlider);
  sprintf(buf,buf);

  server.send(200, "text/plain", buf); //Send web page

  //TILT SINIRLAMASI (110 derece) 
    if(manualTilt >= tiltLimitH){
      manualTilt = tiltLimitH;
      }

     if(manualTilt <= tiltLimitL){
      manualTilt = tiltLimitL;
      }

    if(motionStatus == false){
    panServo.setEaseTo(manualPan);
    tiltServo.setEaseTo(manualTilt);
    setEaseToForAllServosSynchronizeAndStartInterrupt(25);
    while (ServoEasing::areInterruptsActive()) {
      server.handleClient();
    }
    }
}

void UpdateSlider_1() {

  String t_state = server.arg("TILTER");

  tiltSlider = t_state.toInt();

  manualTilt = map(tiltSlider, 0, 110, tiltLimitL, tiltLimitH);
  strcpy(buf,"");
  sprintf(buf,"%d", tiltSlider);
  sprintf(buf,buf);
  
  server.send(200, "text/plain", buf); //Send web page

  //TILT SINIRLAMASI (110 derece) 
    if(manualTilt >= tiltLimitH){
      manualTilt = tiltLimitH;
      }

     if(manualTilt <= tiltLimitL){
      manualTilt = tiltLimitL;
      }

    if(motionStatus == false){
    panServo.setEaseTo(manualPan);
    tiltServo.setEaseTo(manualTilt);
    setEaseToForAllServosSynchronizeAndStartInterrupt(25);
    while (ServoEasing::areInterruptsActive()) {
      server.handleClient();
    }
    }
}

void ProcessButton_0() {

  laserStatus = !laserStatus;  

  if(laserStatus == true){
  analogWrite(LASER_PIN, 255); //AÇIK OLMASI İÇİN.
  }

  else if(laserStatus == false){
  analogWrite(LASER_PIN, 0); //KAPALI OLMASI İÇİN.
  }

  laserObject.putBool("state", laserStatus);

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  
    if (laserStatus) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
}

void ProcessButton_1() {

  motionStatus = !motionStatus;  
  motionObject.putBool("state", motionStatus);

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  
    if (motionStatus) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
}

void UpdateForms() {
  String form_details = server.arg("PANTILTIDENTITY"); //form_details = ilgili pan değeri, ilgili tilt değeri, text box sırası(0,1,2,3).
  //Serial.println(form_details);
 
int tempPan;
int tempTilt;
int identity;

int ind1;
int ind2;
int ind3;

  ind1 = form_details.indexOf(',');  //finds location of first ,
  tempPan = form_details.substring(0, ind1).toInt();   //captures first data String
  ind2 = form_details.indexOf(',', ind1+1 );   //finds location of second ,
  tempTilt = form_details.substring(ind1+1, ind2+1).toInt();   //captures second data String
  ind3 = form_details.indexOf(',', ind2+1 );
  identity = form_details.substring(ind2+1).toInt();

  if(identity == 0) {
    panValue0 = tempPan;
    tiltValue0 = tempTilt;
    pan0.putInt("pan0", panValue0);
    tilt0.putInt("tilt0", tiltValue0);
    tiltValue0 = map(tiltValue0, 0, 110, tiltLimitL, tiltLimitH);
  }

  else if(identity == 1) {
    panValue1 = tempPan;
    tiltValue1 = tempTilt;
    pan1.putInt("pan1", panValue1);
    tilt1.putInt("tilt1", tiltValue1);
    tiltValue1 = map(tiltValue1, 0, 110, tiltLimitL, tiltLimitH);
  }

  else if(identity == 2) {
    panValue2 = tempPan;
    tiltValue2 = tempTilt;
    pan2.putInt("pan2", panValue2);
    tilt2.putInt("tilt2", tiltValue2);
    tiltValue2 = map(tiltValue2, 0, 110, tiltLimitL, tiltLimitH);
  }

   else if(identity == 3) {
    panValue3 = tempPan;
    tiltValue3 = tempTilt;
    pan3.putInt("pan3", panValue3);
    tilt3.putInt("tilt3", tiltValue3);
    tiltValue3 = map(tiltValue3, 0, 110, tiltLimitL, tiltLimitH);
  }

  panLimitMax = max(max(panValue0, panValue1), max(panValue2,panValue3));
  panLimitMin = min(min(panValue0, panValue1), min(panValue2,panValue3));
  tiltLimitMax = max(max(tiltValue0, tiltValue1), max(tiltValue2,tiltValue3));
  tiltLimitMin = min(min(tiltValue0, tiltValue1), min(tiltValue2,tiltValue3));

  panLimitMaxObject.putInt("panLimitMax", panLimitMax);
  panLimitMinObject.putInt("panLimitMin", panLimitMin);
  tiltLimitMaxObject.putInt("tiltLimitMax", tiltLimitMax);
  tiltLimitMaxObject.putInt("tiltLimitMin", tiltLimitMin);

  server.send(200, "text/plain", ""); 
}

void SendWebsite() {
  //Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}

void SendXML() {
  // Serial.println("sending xml");
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");
  // show laser status
  if (laserStatus) {
    strcat(XML, "<LAZER>1</LAZER>\n");
  }
  else {
    strcat(XML, "<LAZER>0</LAZER>\n");
  }

  if (motionStatus) {
    strcat(XML, "<HAREKET>1</HAREKET>\n");
  }
  else {
    strcat(XML, "<HAREKET>0</HAREKET>\n");
  }

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  //Serial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);
}
  
