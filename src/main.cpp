// #include <Arduino.h>
// #include <WebServer.h>
// #include <WiFi.h>
// #include <L298N.h>
// #include <ESP32Servo.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// float left_distance;
// float front_distance;
// float right_distance;
// class HCSR04
// {
// private:
//     int trigPin, echoPin;

// public:
//     HCSR04(int trig, int echo)
//     {
//         trigPin = trig;
//         echoPin = echo;
//         pinMode(trigPin, OUTPUT);
//         pinMode(echoPin, INPUT);
//     }
//     float raw_getDistance()
//     {
//         digitalWrite(trigPin, LOW);
//         delayMicroseconds(5);
//         digitalWrite(trigPin, HIGH);
//         delayMicroseconds(15);
//         digitalWrite(trigPin, LOW);

//         long duration = pulseIn(echoPin, HIGH); 
//         // Serial.print("Duration: "); 
//         //Serial.println(duration);

//         if (duration == 0)
//             return 999.0;
//         float dis=duration * 0.034 / 2;

//         return (dis<=100?dis:100);
//     }

//     float getFilteredDistance() {
//         float readings[3];
//         for (int i = 0; i < 3; i++) {
//             readings[i] = raw_getDistance();
//             delay(10);
//         }
//         std::sort(readings, readings + 3); 
//         return readings[1];  
//     }
//     float getDistance() {
//         return getFilteredDistance();
//     }   
//    };

// class CAR
// {
// private:
//     L298N left, right;
//     HCSR04 scanner;
//     Servo servo;
//     const int threshold = 20;
//     int front, back;
//     const int motorspeed = 150;

// public:
//     void test()
//     {
//         servo.write(0);
//         Serial.printf("Testing HCSR04 distance: %f",scanner.getDistance());
//         for(int i=0;i<4;i++){
//             digitalWrite(back,HIGH);
//             digitalWrite(front,HIGH);
//             delay(300);
//             digitalWrite(back,LOW);
//             digitalWrite(front,LOW);
//         }
//         servo.write(180);
//     }

//     CAR(int ena, int in1, int in2, int enb, int in3, int in4, int trig, int echo, int servoPin, int front_led, int back_led)
//         : left(ena, in1, in2), right(enb, in3, in4), scanner(trig, echo)
//     {
//         front = front_led;
//         back = back_led;
//         servo.attach(servoPin);
//     }
//     void setup(){
//         servo.write(90);
//         pinMode(front, OUTPUT);
//         pinMode(back, OUTPUT);
//     }

//     void forward(int speed)
//     {
//         digitalWrite(back, LOW);
//         left.setSpeed(speed);
//         right.setSpeed(speed);
//         left.forward();
//         right.forward();
//         digitalWrite(front, HIGH);
//     }

//     void backward(int speed)
//     {
//         digitalWrite(front, LOW);
//         digitalWrite(back, HIGH);
//         left.setSpeed(speed);
//         right.setSpeed(speed);
//         left.backward();
//         right.backward();
//     }

//     void stop()
//     {
//         digitalWrite(back, LOW);
//         digitalWrite(front, LOW);
//         left.setSpeed(0); 
//         right.setSpeed(0);
//         left.stop();
//         right.stop();
//     }

//     void turnLeft(int speed)
//     {
//         left.stop();
//         right.setSpeed(speed);
//         right.forward();
//     }

//     void turnRight(int speed)
//     {
//         right.stop();
//         left.setSpeed(speed);
//         left.forward();
//     }

//     void spinAround(int speed)
//     {
//         left.setSpeed(speed);
//         right.setSpeed(speed);
//         left.forward();
//         right.backward();
//         digitalWrite(front, HIGH);
//         digitalWrite(back, HIGH);

//         delay(1500);
//         stop();
//     }

//     void adasDrive()
//     {
//         front_distance = scanner.getDistance();
//         Serial.printf("Front Distance: %f\n", front_distance);
//         if (front_distance > threshold)
//         {
//             forward(motorspeed);
//         }
//         else
//         {
//             stop();
//             delay(500);
//             servo.write(0);
//             delay(100);
//             left_distance = scanner.getDistance();
//             Serial.printf("Left Distance: %f\n", left_distance);
//             servo.write(180);
//             delay(100);
//             right_distance = scanner.getDistance();
//             Serial.printf("Right Distance: %f\n", right_distance);
//             if (left_distance > threshold)
//             {
//                 turnLeft(motorspeed);
//                 delay(200);
//                 //forward(motorspeed);
//             }
//             else if (right_distance > threshold)
//             {
//                 turnRight(motorspeed);
//                 delay(200);
//                 //forward(motorspeed);
//             }
//             else
//             {
//                 spinAround(motorspeed);
//                 delay(100);
//                 //forward(motorspeed);
//             }
//             servo.write(90);
//         }
//     }

//     void command(String command, int speed)
//     {

//         if (command == "F")
//         {
//             forward(speed);
//         }
//         else if (command == "L")
//         {
//             turnLeft(speed);
//         }
//         else if (command == "R")
//         {
//             turnRight(speed);
//         }
//         else if (command == "Rev")
//         {
//             backward(speed);
//         }
//         else if (command == "S")
//         {
//             stop();
//         }
//         else if (command == "Spin")
//         {
//             spinAround(speed);
//         }
//         if(command!="S"){
//             Serial.printf("Executed: %s at speed %d\n", command.c_str(), speed);
//         }
//         else{
//             Serial.println("Stopping car");
//         }
//     }
// };

// #define ena 25      // Motor A PWM (PWM Capable)
// #define in1 26      // Motor A IN1  
// #define in2 27      // Motor A IN2  
// #define enb 32      // Motor B PWM (PWM Capable)  
// #define in3 33      // Motor B IN3  
// #define in4 4       // Motor B IN4  

// #define trig 5      // Ultrasonic Sensor Trigger  
// #define echo 18     // Ultrasonic Sensor Echo (No Serial Conflict)  

// #define servoPin 23 // Servo Motor (PWM Capable)  
// #define indicator 2 //built in led
// #define front_led 21 // Front LED   
// #define back_led 15 //back led


// CAR mycar(ena,in1,in2,enb,in3,in4,trig,echo,servoPin,front_led,back_led);

// bool mode = true;
// bool ign = false;
// WebServer server(80);
// void handleMode()
// {
//     if (server.hasArg("mode"))
//     {
//         if (server.arg("mode") == "manual")
//         {
//             mode = false;
//             server.send(200, "text/plain", "MANUAL");
//             Serial.println("Mode=manual");
//             mycar.stop();
//         }
//         else if (server.arg("mode") == "auto")
//         {
//             mode = true;
//             server.send(200, "text/plain", "AUTO");
//         }
//     }
//     else
//     {
//         server.send(400, "text/plain", "Invalid request");
//     }
// }

// void handleCommand()
// {
//     if (ign && mode == false)
//     {
//         if (server.hasArg("cmd"))
//         {
//             String command = server.arg("cmd");
//             mycar.command(command, 200);
//             server.send(200, "text/plain", "Command executed");
//         }
//         else
//         {
//             server.send(400, "text/plain", "Invalid request");
//         }
//     }
//     else if (mode == true)
//     {
//         server.send(300, "text/plain", "using internal AI@1.6.0.7");
//     }
//     else
//     {
//         server.send(300, "text/plain", "ign off");
//     }
// }
// void handletest()
// {
//     server.send(200,"text/html","<h1>testing</h1>");
//     mycar.test();
// }
// void handleign()
// {
//     if (server.hasArg("ign"))
//     {
//         String data = server.arg("ign");
//         if (data == "true")
//         {
//             ign = true;
//             server.send(200, "text/plain", "ign on");
//         }
//         else if (data == "false")
//         {
//             ign = false;
//             digitalWrite(2, LOW);
//             server.send(200, "text/plain", "ign off");
//         }
//     }
// }
// void handleroot()
// {
//     server.send(200, "text/html", "<html><head><script>window.location.href = 'https://adas-car.web.app/';</script></head></html>");
// }
// void handlestatus(int what)
// {
//     String status;
//     if(what==1){
//         status="IGN: " + String(ign ? "ON" : "OFF");
//     }
//     else if(what==2){
//         status = "Mode: " + String(mode ? "AUTO" : "MANUAL");
//     }
//     server.send(200, "text/plain", status);
// }

// void setup() {
//     WiFi.softAP("ADAS CAR", "rohanbatra");
//     delay(500);
//     Serial.begin(115200);
//     pinMode(indicator, OUTPUT);
//     Serial.println("WiFi Started at " + WiFi.softAPIP().toString());

//     Serial.println("Setting up server handlers");
//     server.on("/mode", HTTP_POST, handleMode);
//     server.on("/command", HTTP_POST, handleCommand);
//     server.on("/", HTTP_GET, handleroot);
//     server.on("/ign", HTTP_POST, handleign);
//     server.on("/test", HTTP_GET, handletest);
//     server.on("/status/mode", HTTP_GET, []() { handlestatus(2); });
//     server.on("/status/ign", HTTP_GET, []() { handlestatus(1); });

//     Serial.println("All Handlers Initialized");

//     // Start the server
//     server.begin();
//     Serial.println("Server started");

//     Serial.println("Entering test mode");
//     mycar.setup();
//     mycar.test();
//     Serial.println("\nTest Complete");
// }

// void loop() {
//     server.handleClient();

//     if (ign) {
//         digitalWrite(indicator, HIGH);
        
//         if(mode){
//             mycar.adasDrive();
//         }
        
//     } 
//     else {
//         digitalWrite(indicator, LOW);
//         mycar.stop();
//     }
// }



// // void loop() {
// //     server.handleClient();
// //     if (ign) {
// //         digitalWrite(indicator, HIGH);
        
       
// //         if (mode && millis() - lastADASUpdate >= ADAS_INTERVAL) {
// //             lastADASUpdate = millis();
// //             mycar.adasDrive();
// //         }
// //     } 
// //     else {
// //         digitalWrite(indicator, LOW);
// //         mycar.stop();
// //     }
// // }


#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <L298N.h>
#include <ESP32Servo.h>

float left_distance;
float front_distance;
float right_distance;
class HCSR04
{
private:
    int trigPin, echoPin;

public:
    HCSR04(int trig, int echo)
    {
        trigPin = trig;
        echoPin = echo;
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }
    float raw_getDistance()
    {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(15);
        digitalWrite(trigPin, LOW);

        long duration = pulseIn(echoPin, HIGH);
        // Serial.print("Duration: ");
        // Serial.println(duration);

        if (duration == 0)
            return 999.0;
        float dis = duration * 0.034 / 2;

        return (dis <= 100 ? dis : 100);
    }

    float getFilteredDistance()
    {
        float readings[3];
        for (int i = 0; i < 3; i++)
        {
            readings[i] = raw_getDistance();
            delay(10);
        }
        std::sort(readings, readings + 3);
        return readings[1];
    }
    float getDistance()
    {
        return getFilteredDistance();
    }
};

class CAR
{
private:
    L298N left, right;
    HCSR04 scanner;
    Servo servo;
    const int threshold = 30;
    int front, back;
    const int motorspeed = 200; // 0 - 255

public:
    void test()
    {
        servo.write(0);
        Serial.printf("Testing HCSR04 distance: %f", scanner.getDistance());
        for (int i = 0; i < 4; i++)
        {
            digitalWrite(back, HIGH);
            digitalWrite(front, HIGH);
            delay(300);
            digitalWrite(back, LOW);
            digitalWrite(front, LOW);
        }
        servo.write(180);
        delay(50);
        servo.write(90);
    }

    CAR(int ena, int in1, int in2, int enb, int in3, int in4, int trig, int echo, int servoPin, int front_led, int back_led)
        : left(ena, in1, in2), right(enb, in3, in4), scanner(trig, echo)
    {
        front = front_led;
        back = back_led;
        servo.attach(servoPin);
    }
    void setup()
    {
        servo.write(90);
        pinMode(front, OUTPUT);
        pinMode(back, OUTPUT);
    }

    void forward(int speed)
    {
        digitalWrite(back, LOW);
        left.setSpeed(speed);
        right.setSpeed(speed);
        left.forward();
        right.forward();
        digitalWrite(front, HIGH);
    }

    void backward(int speed)
    {
        digitalWrite(front, LOW);
        digitalWrite(back, HIGH);
        left.setSpeed(speed);
        right.setSpeed(speed);
        left.backward();
        right.backward();
    }

    void stop()
    {
        digitalWrite(back, LOW);
        digitalWrite(front, LOW);
        left.setSpeed(0);
        right.setSpeed(0);
        left.stop();
        right.stop();
    }

    void turnLeft(int speed)
    {
        left.stop();
        right.setSpeed(speed);
        right.forward();
    }

    void turnRight(int speed)
    {
        right.stop();
        left.setSpeed(speed);
        left.forward();
    }

    void spinAround(int speed)
    {
        left.setSpeed(speed);
        right.setSpeed(speed);
        left.forward();
        right.backward();
        digitalWrite(front, HIGH);
        digitalWrite(back, HIGH);

        delay(1500);
        stop();
    }

    void adasDrive_org()
    {
        front_distance = scanner.getDistance();
        Serial.printf("Front Distance: %f\n", front_distance);
        if (front_distance > threshold)
        {
            forward(motorspeed);
        }
        else
        {
            stop();
            // delay(500);
            servo.write(0);
            delay(200);
            left_distance = scanner.getDistance();
            Serial.printf("Left Distance: %f\n", left_distance);
            servo.write(180);
            delay(100);
            right_distance = scanner.getDistance();
            Serial.printf("Right Distance: %f\n", right_distance);
            if (left_distance > threshold)
            {
                turnLeft(motorspeed);
                delay(200);
                // forward(motorspeed);
            }
            else if (right_distance > threshold)
            {
                turnRight(motorspeed);
                delay(200);
                // forward(motorspeed);
            }
            else
            {
                spinAround(motorspeed);
                delay(100);
                // forward(motorspeed);
            }
        }
        servo.write(90);
        delay(100);
    }
    void adasDrive()
{
    front_distance = scanner.getDistance();
    Serial.printf("Front Distance: %.2f cm\n", front_distance);

    if (front_distance > threshold)
    {
        forward(motorspeed);
    }
    else
    {
        stop();
        delay(100);
        Serial.println("Obstacle ahead! Scanning...");

        // Look left
        servo.write(30); // slightly left instead of full 0
        delay(300); // allow servo to settle
        left_distance = scanner.getDistance();
        Serial.printf("Left Distance: %.2f cm\n", left_distance);

        // Look right
        servo.write(150); // slightly right instead of full 180
        delay(300);
        right_distance = scanner.getDistance();
        Serial.printf("Right Distance: %.2f cm\n", right_distance);

        // Reset servo to front
        servo.write(90);
        delay(200);

        if (left_distance > threshold && left_distance > right_distance)
        {
            Serial.println("Turning left");
            turnLeft(motorspeed);
            delay(300);
        }
        else if (right_distance > threshold)
        {
            Serial.println("Turning right");
            turnRight(motorspeed);
            delay(300);
        }
        else
        {
            Serial.println("Spinning to find open path...");
            // spinAround(motorspeed);
            backward(motorspeed);
            delay(400); // spin a bit longer to clear area
        }
    }
}

    void command(String command, int speed)
    {

        if (command == "F")
        {
            forward(speed);
        }
        else if (command == "L")
        {
            //turnLeft(speed);
            turnRight(speed);
        }
        else if (command == "R")
        {
            //turnRight(speed);
            turnLeft(speed);
        }
        else if (command == "Rev")
        {
            backward(speed);
        }
        else if (command == "S")
        {
            stop();
        }
        else if (command == "Spin")
        {
            spinAround(speed);
        }
        if (command != "S")
        {
            Serial.printf("Executed: %s at speed %d\n", command.c_str(), speed);
        }
        else
        {
            Serial.println("Stopping car");
        }
    }
};

#define ena 25 // Motor A EN (PWM Capable)
#define in1 26 // Motor A IN1
#define in2 27 // Motor A IN2
#define enb 32 // Motor B EN (PWM Capable)
#define in3 33 // Motor B IN3
#define in4 4  // Motor B IN4

#define trig 5  // Ultrasonic Sensor Trigger
#define echo 18 // Ultrasonic Sensor Echo (No Serial Conflict)

#define servoPin 23  // Servo Motor (PWM Capable)
#define indicator 2  // built in led
#define front_led 21 // Front LED
#define back_led 15  // back led

CAR mycar(ena, in1, in2, enb, in3, in4, trig, echo, servoPin, front_led, back_led);

bool mode = true; // by default adas mode
bool ign = false; // by default off
AsyncWebServer server(80);
void handleMode(AsyncWebServerRequest *req)
{
    if (req->hasArg("mode"))
    {
        if (req->arg("mode") == "manual")
        {
            mode = false;
            req->send(200, "text/plain", "MANUAL");
            Serial.println("Mode=manual");
            mycar.stop();
        }
        else if (req->arg("mode") == "auto")
        {
            mode = true;
            req->send(200, "text/plain", "AUTO");
        }
    }
    else
    {
        req->send(400, "text/plain", "Invalid request");
    }
}

void handleCommand(AsyncWebServerRequest *req)
{
    if (ign)
    {
        if (mode == false)
        {
            if (req->hasArg("cmd"))
            {
                String command = req->arg("cmd");
                mycar.command(command, 200);
                req->send(200, "text/plain", "Command executed");
            }
            else
            {
                req->send(400, "text/plain", "Invalid request");
            }
        }
        else
        {
            req->send(300, "text/plain", "using internal AI@1.6.0.7");
        }
    }
    else
    {
        req->send(300, "text/plain", "ign off");
    }
}
void handletest(AsyncWebServerRequest *req)
{
    req->send(200, "text/html", "<h1>testing</h1>");
    mycar.test();
}
void handleign(AsyncWebServerRequest *req)
{
    if (req->hasArg("ign"))
    {
        String data = req->arg("ign");
        if (data == "true")
        {
            ign = true;
            req->send(200, "text/plain", "ign on");
        }
        else if (data == "false")
        {
            ign = false;
            digitalWrite(2, LOW);
            req->send(200, "text/plain", "ign off");
        }
    }
}
void handleroot(AsyncWebServerRequest *req)
{
    req->send(200, "text/html", "<html><head><script>window.location.href = 'https://adas-car.web.app/';</script></head></html>");
}

void handlestatus_ign(AsyncWebServerRequest *req)
{
    String status;
    status = "IGN: " + String(ign ? "ON" : "OFF");
    req->send(200, "text/plain", status);
}
void handlestatus_mode(AsyncWebServerRequest *req)
{
    String status;
    status = "Mode: " + String(mode ? "AUTO" : "MANUAL");
    req->send(200, "text/plain", status);
}
void setup()
{
    WiFi.softAP("ADAS CAR", "rohanbatra");
    delay(500);
    Serial.begin(115200);
    pinMode(indicator, OUTPUT);
    Serial.println("WiFi Started at " + WiFi.softAPIP().toString());

    Serial.println("Setting up server handlers");
    server.on("/mode", HTTP_POST, handleMode);
    server.on("/command", HTTP_POST, handleCommand);
    server.on("/", HTTP_GET, handleroot);
    server.on("/ign", HTTP_POST, handleign);
    server.on("/test", HTTP_GET, handletest);
    server.on("/status/mode", HTTP_GET, handlestatus_mode);
    server.on("/status/ign", HTTP_GET, handlestatus_ign);

    Serial.println("All Handlers Initialized");

    // Start the server
    server.begin();
    Serial.println("Server started");

    Serial.println("Entering test mode");
    mycar.setup();
    mycar.test();
    Serial.println("\nTest Complete");
}

void loop()
{
    if (ign)
    {
        digitalWrite(indicator, HIGH);

        if (mode)
        {
            mycar.adasDrive();
            delay(200);
        }
    }
    else
    {
        digitalWrite(indicator, LOW);
        mycar.stop();
    }
}
