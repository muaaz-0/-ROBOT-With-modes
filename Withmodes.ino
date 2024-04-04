#include <WiFi.h>
WiFiClient client;
WiFiServer server(80);

#define trigPin 13 // D13 for the ultrasonic sensor's trigger pin
#define echoPin 12 // D12 for the ultrasonic sensor's echo pin

//Distance= (Time x Speed of Sound in Air (343 m/s))/2
unsigned int Speed=255;
/* WIFI settings */
const char* ssid = "Mr";
const char* password = "8765432100";

/* data received from application */
String  data =""; 

volatile bool flag = 1;
long duration, distance;
const float safeDistance = 40.0;


/* define L298N or L293D motor control pins */
int leftMotorForward = 2;     /* GPIO2(D4) -> IN3   */
int rightMotorForward = 5;   /* GPIO15(D8) -> IN1  */
int leftMotorBackward = 4;    /* GPIO0(D3) -> IN4   */
int rightMotorBackward = 18;  /* GPIO13(D7) -> IN2  */


/* define L298N or L293D enable pins */
int rightMotorENB = 15; /* GPIO14(D5) -> Motor-A Enable */
int leftMotorENB = 19;  /* GPIO12(D6) -> Motor-B Enable */

void setup()
{
  /* initialize motor control pins as output */
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT); 
  pinMode(leftMotorBackward, OUTPUT);  
  pinMode(rightMotorBackward, OUTPUT);

  /* initialize motor enable pins as output */
  pinMode(leftMotorENB, OUTPUT); 
  pinMode(rightMotorENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

   // Connect to WiFi network
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");

  
}

void loop()
{
/*digitalWrite(trigPin,LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);

 delayMicroseconds(10);

  duration = pulseIn(echoPin, HIGH); // receive reflected waves

 distance = (duration * 0.034) / 2;
 Serial.print("Distance: "); // Print the distance in centimeters
    Serial.println(distance);
*/


  //*******
    /* If the server available, run the "checkClient" function */ 
       server.available();
      client = server.available();
    if (!client) return; 
    data = checkClient ();
      if (data == "forward" || data == "back" || data == "left" || data =="right" || data == "stop") {
      flag = 1;
      manual();
    }
      else if (data=="max"  || data == "min" || data == "med" ){
        changespeed();
      }

      else if (data=="automatic")
      {
        flag=1;
        automatic();
      }
    
   
} 
// for manual control
 void manual(void)
 {
  /******** Run function according to incoming data from application *********/



    /* If the incoming data is "forward", run the "MotorForward" function */
    if (data == "forward" ) MotorForward();
    /* If the incoming data is "backward", run the "MotorBackward" function */
    else if (data == "back") MotorBackward();
    /* If the incoming data is "left", run the "TurnLeft" function */
    else if (data == "left") TurnLeft();
    /* If the incoming data is "right", run the "TurnRight" function */
    else if (data == "right") TurnRight();

    
    
    /* If the incoming data is "stop", run the "MotorStop" function */
    else if (data == "stop") MotorStop();

    else if (distance<18) MotorStop();
 }

 void changespeed(void){
          if (data=="min") Speed=80;
    else if (data=="med") Speed=150;
    else if (data=="max") Speed=250;
 }

 void automatic()
 {
  findDistance();

  while (true){
    if (distance>safeDistance)
    {
      Serial.print("Forward");
      MotorForward();
      findDistance();
     }
     else if (distance<safeDistance){
      MotorStop();
      delay(300);
      
     }
  }
  
  
 }


void findDistance() { 
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    duration = pulseIn(echo, HIGH);
    distance = (duration / 2) * 0.0343;
}
 
/*************** FORWARD *******************/
void MotorForward(void)   
{
  analogWrite(rightMotorENB,Speed);
  analogWrite(leftMotorENB,Speed);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
}

/*************** BACKWARD *******************/
void MotorBackward(void)   
{
 analogWrite(rightMotorENB,Speed);
  analogWrite(leftMotorENB,Speed);
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
}

/*************** TURN LEFT *******************/
void TurnLeft(void)   
{
  analogWrite(rightMotorENB,Speed);
  analogWrite(leftMotorENB,Speed);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);

  
}

/*************** TURN RIGHT *******************/
void TurnRight(void)   
{
  analogWrite(rightMotorENB,Speed);
  analogWrite(leftMotorENB,Speed);
   digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,LOW);
  
 
}

/*************** STOP *******************/
void MotorStop(void)   
{
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
}

/************ RECEIVE DATA FROM the APP **************/
String checkClient (void)
{
  while(!client.available()) delay(1); 
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}
