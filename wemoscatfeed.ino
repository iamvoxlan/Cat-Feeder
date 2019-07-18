#include <Servo.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <time.h>


#define FIREBASE_HOST "catfeeder-665fb.firebaseio.com"
#define FIREBASE_AUTH "DXhERx9kT6MK9cTXwnhOd5MgXQGbF5qGwmCi1XKp"
#define WIFI_SSID "cowards"
#define WIFI_PASSWORD "eyesonme"

Servo myservo;
const int trigPin = D6;
const int echoPin = D5;
const int servoPin = D7;
long distance;
long duration;
String statusNow;
boolean statusMorning = false;
boolean statusNoon = false;
boolean statusNight = false;
boolean notEat = true;
boolean isButtonPressed = false;
int minute, hour, second;
int day, prevday;


int timezone = 8 * 3600;
int dst = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);
  myservo.write(0);
  delay(1000);
  myservo.detach();
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK"); 
}

void rotateCan() {
  myservo.attach(servoPin);
  myservo.write(180);
  delay(1500);
  myservo.write(0);
  delay(1500);
  myservo.detach();
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
}

void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  hour = p_tm->tm_hour;
  minute = p_tm->tm_min;
  second = p_tm->tm_sec;
  prevday=day;
  day = p_tm->tm_mday;
  statusMorning= Firebase.getBool("/Status/statusMorning");
  statusNoon = Firebase.getBool("/Status/statusNoon");
  statusNight = Firebase.getBool("/Status/statusNight");
  if (day!=prevday){
    String statusLogs = ""+day;
    if (statusMorning){
      statusLogs = statusLogs+" pagi,";
    }
    if (statusNoon){
      statusLogs = statusLogs+" siang, ";
    }
    if (statusNight){
      statusLogs = statusLogs+" malam.";
    }
    String name = Firebase.pushString("logs", statusLogs);
    statusNow= "null";
    statusMorning = false;
    statusNight = false;
    statusNoon = false;
    Firebase.setBool("/Status/statusMorning", false);
    Firebase.setBool("/Status/statusNoon", false);
    Firebase.setBool("/Status/statusNight", false);
  }
  
  if (hour<12 && hour>=7){
    statusNow = "morning";
  }
  else if (hour<18 && hour>=13){
    statusNow = "noon";
  }
  else if (hour<23 && hour>=19){
    statusNow = "night";
  }
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
    distance = duration / 58.2;

  Serial.println(statusNow+", time in hour:"+hour+" distance: "+distance+"cm");
  if (WiFi.status() == WL_CONNECTED){
    isButtonPressed = Firebase.getBool("buttonPress");
  }
  
  if (isButtonPressed){
     Firebase.setBool("buttonPress", false);
     isButtonPressed = false;
     Serial.println("Feeed it!");
     rotateCan();
     notEat = false;
  }

  if (statusNow.equals("morning")){
    if (statusMorning==false && distance <=30){
      Firebase.setBool("/Status/statusMorning", true);
      rotateCan();
      statusMorning = true;
    }
  }
  else if (statusNow.equals("noon")){
    if (statusNoon == false && distance<=30){
      Firebase.setBool("/Status/statusNoon", true);
      rotateCan();
      statusNoon= true;
    }
  }
  else if (statusNow.equals("night")){
    if (statusNight == false && distance<=30){
      Firebase.setBool("/Status/statusNight", true);
      rotateCan();
      statusNight= true;
    }
  }

  delay(500);
  
}
