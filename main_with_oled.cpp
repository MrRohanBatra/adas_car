#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <L298N.h>
#include <ESP32Servo.h>
#include <stdlib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);


void showAlert(String mess) {
    display.clearDisplay();

    
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE);

    String message = mess;
    int16_t x1, y1;
    uint16_t w, h;

    
    display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);

    int x = (display.width() - w) / 2;
    int y = (display.height() - h) / 2;

    display.setCursor(x, y);
    display.println(message);
    display.display();
}


float front_distance, left_distance, right_distance;
void showSensorStatus_org(const String& heading, int distanceCM, bool irBlocked) {
    display.clearDisplay();
  
    // Heading centered
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    int16_t x = (display.width() - heading.length() * 12) / 2;  // 12 pixels per char in size 2
    display.setCursor(x, 0);
    display.print(heading);
  
    // Distance
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("Distance: ");
    display.print(distanceCM);
    display.println(" cm");
  
    // IR Sensor
    display.setCursor(0, 30);
    display.print("IR: ");
    display.println(irBlocked ? "Blocked" : "Clear");
  
    display.display();
  }
  void showSensorStatus(const String& heading, int distanceCM, bool irBlocked) {
    display.clearDisplay();
  
    // Heading centered at top
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    int16_t headingX = (display.width() - heading.length() * 12) / 2;  // Approx 12 pixels per char at size 2
    display.setCursor(headingX, 1);
    display.print(heading);
  
    display.setTextSize(2);  // Keep large size for central text
    String centerText;
  
    if (irBlocked) {
      centerText = "IR Blocked";
    } else {
      centerText = String(distanceCM) + " cm";
    }
  
    // Center the centerText on the display, below the heading
    int16_t textX = (display.width() - centerText.length() * 12) / 2;
    int16_t textY = 26; // Leave enough space below heading
    display.setCursor(textX, textY);
    display.print(centerText);
  
    display.display();
  }
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

        if (duration == 0)
            return 999.0;
        float dis = duration * 0.034 / 2;
        dis -= 5;
        return (dis <= 200 ? dis : 200);
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
        return (readings[1]);
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
    int motorspeed = 180;
    int irsensor;

public:
    float getdis(int angle)
    {
        servo.write(angle);
        float data = scanner.getDistance();
        delay(10);
        servo.write(91);
        return data;
    }
    void setSpeed(int speed)
    {
        this->motorspeed = speed;
    }
    void test()
    {
        servotest();
        delay(100);
    }

    CAR(int ena, int in1, int in2, int enb, int in3, int in4, int trig, int echo, int servoPin, int front_led, int back_led, int irsensor1)
        : left(ena, in1, in2), right(enb, in3, in4), scanner(trig, echo)
    {
        front = front_led;
        back = back_led;
        servo.attach(servoPin);
        this->irsensor = irsensor1;
    }
    void setup()
    {
        servo.write(90);
        pinMode(front, OUTPUT);
        pinMode(back, OUTPUT);
        pinMode(irsensor, INPUT);
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
            }
            else if (right_distance > threshold)
            {
                turnRight(motorspeed);
                delay(200);
            }
            else
            {
                spinAround(motorspeed);
                delay(100);
            }
        }
        servo.write(90);
        delay(100);
    }
    void adasDrive_gynamic()
    {
        if (digitalRead(irsensor) == LOW)
        {
            stop();
            Serial.println("IR: Dead zone obstacle detected!");

            backward(200);
            delay(700);
            stop();
            delay(100);

            int turnDir = random(0, 2);
            if (turnDir == 0)
            {
                Serial.println("Random escape: Turning LEFT");
                turnLeft(255);
            }
            else
            {
                Serial.println("Random escape: Turning RIGHT");
                turnRight(255);
            }

            delay(1000);
            stop();
            delay(100);

            if (digitalRead(irsensor) == LOW == LOW)
            {
                Serial.println("IR clear after random turn. Moving forward.");
                forward(motorspeed);
            }
            else
            {
                Serial.println("IR still blocked. Holding position.");
                stop();
            }

            return;
        }

        front_distance = scanner.getDistance();
        int temp = constrain(int(front_distance), 10, 200);
        motorspeed = map(temp, 10, 95, 100, 200);

        Serial.printf("Front Distance: %.2f cm | Speed: %d\n", front_distance, motorspeed);

        if (front_distance > threshold)
        {
            forward(motorspeed);
        }
        else
        {
            stop();
            delay(100);
            Serial.println("Obstacle ahead! Scanning...");

            servo.write(30);
            delay(300);
            left_distance = scanner.getDistance();
            Serial.printf("Left Distance: %.2f cm\n", left_distance);

            servo.write(150);
            delay(300);
            right_distance = scanner.getDistance();
            Serial.printf("Right Distance: %.2f cm\n", right_distance);

            servo.write(90);
            delay(200);

            if (left_distance > threshold && left_distance > right_distance)
            {
                Serial.println("Turning left");
                turnLeft(255);
                delay(900);
            }
            else if (right_distance > threshold)
            {
                Serial.println("Turning right");
                turnRight(255);
                delay(900);
            }
            else
            {
                Serial.println("Spinning to find open path...");

                backward(200);
                delay(1000);
                stop();
                delay(100);

                int turnDir = random(0, 2);
                if (turnDir == 0)
                {
                    Serial.println("Random escape: Turning LEFT");
                    turnLeft(255);
                }
                else
                {
                    Serial.println("Random escape: Turning RIGHT");
                    turnRight(255);
                }

                delay(1000);
                stop();
                delay(100);
            }
        }
    }
    void adasDrive()
    {

        if (digitalRead(irsensor) == LOW)
        {
            stop();
            Serial.println("IR: Dead zone obstacle detected!");

            int reverseSpeed = 180;
            int reverseTime = 600;

            Serial.printf("Backing up at %d speed for %d ms\n", reverseSpeed, reverseTime);
            backward(reverseSpeed);
            delay(reverseTime);
            stop();
            delay(100);

            int turnDir = random(0, 2);
            Serial.printf("IR escape: Turning %s\n", turnDir == 0 ? "LEFT" : "RIGHT");

            int turnSpeed = 220;
            int turnTime = 800;

            if (turnDir == 0)
                turnLeft(turnSpeed);
            else
                turnRight(turnSpeed);

            delay(turnTime);
            stop();
            delay(100);

            if (digitalRead(irsensor) == LOW)
            {
                Serial.println("IR clear after turn. Moving forward.");
                forward(motorspeed);
            }
            else
            {
                Serial.println("Still blocked. Holding.");
                stop();
            }

            return;
        }

        front_distance = scanner.getDistance();
        int temp = constrain(int(front_distance), 10, 200);
        showSensorStatus("ADAS MODE",temp,(digitalRead(irsensor)==LOW)?true:false);
        motorspeed = map(temp, 10, 200, 100, 200);

        Serial.printf("Front Distance: %.2f cm | Speed: %d\n", front_distance, motorspeed);

        if (front_distance > threshold)
        {
            forward(motorspeed);
        }
        else
        {
            stop();
            delay(100);
            Serial.println("Obstacle ahead! Scanning...");

            servo.write(31);
            delay(300);
            left_distance = scanner.getDistance();
            showSensorStatus("ADAS MODE",int(left_distance),(digitalRead(irsensor)==LOW)?true:false);
            servo.write(151);
            delay(300);
            right_distance = scanner.getDistance();
            showSensorStatus("ADAS MODE",int(right_distance),(digitalRead(irsensor)==LOW)?true:false);
            servo.write(91);
            delay(200);
            Serial.printf("Left: %.2f cm | Right: %.2f cm\n", left_distance, right_distance);

            if (left_distance > threshold && left_distance > right_distance)
            {
                int turnDur = map(left_distance, threshold, 200, 500, 1000);
                Serial.printf("Turning LEFT for %d ms\n", turnDur);
                showAlert("LEFT TURN");
                backward(175);
                delay(150);
                turnLeft(255);
                delay(turnDur);
            }
            else if (right_distance > threshold)
            {
                int turnDur = map(right_distance, threshold, 200, 500, 1000);
                Serial.printf("Turning RIGHT for %d ms\n", turnDur);
                showAlert("RIGHT TURN");
                backward(175);
                delay(150);
                turnRight(255);
                delay(turnDur);
            }
            else
            {
                Serial.println("No clear side. Reversing and random escape...");
                showAlert("REVERSE");
                int reverseTime = 1000;
                int reverseSpeed = 200;
                backward(reverseSpeed);
                delay(reverseTime);
                stop();
                delay(100);

                int turnDir = random(0, 2);
                int turnTime = 1000;

                Serial.printf("Random escape: Turning %s for %d ms\n", turnDir == 0 ? "LEFT" : "RIGHT", turnTime);
                if (turnDir == 0)
                {
                    showAlert("LEFT TURN");
                    backward(175);
                    delay(150);
                    turnLeft(255);
                }
                else
                {
                    showAlert("RIGHT TURN");
                    backward(175);
                    delay(150);
                    turnRight(255);
                }
                delay(turnTime);
                stop();
                delay(100);
            }
        }
    }
    
    void servotest()
    {
        int count = 0;
        const int totalSteps = 2 * (180 / 10 + 1) * 2; 
        servo.write(0);
    
        for (int k = 0; k <= 2; k++)
        {
            for (int i = 0; i <= 180; i += 10)
            {
                servo.write(i);
                showProgressBar("Testing Servo", (count * 100) / totalSteps);
                count++;
                delay(50);
            }
    
            for (int i = 180; i >= 0; i -= 10)
            {
                servo.write(i);
                showProgressBar("Testing Servo", (count * 100) / totalSteps);
                count++;
                delay(50);
            }
    
            servo.write(90);
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

            turnRight(speed);
        }
        else if (command == "R")
        {

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
    void handleSerialCommands()
    {
        static bool streaming = false;

        if (Serial.available())
        {
            String cmd = Serial.readStringUntil('\n');
            cmd.trim();

            if (cmd.equalsIgnoreCase("gsv"))
            {
                Serial.println("Started streaming sensor data...");
                streaming = true;
            }
            else if (cmd.equalsIgnoreCase("servo"))
            {
                servotest();
            }
            else if (cmd.equalsIgnoreCase("stop"))
            {
                Serial.println("Stopped streaming.");
                streaming = false;
            }
        }
        if (streaming)
        {
            float front = scanner.getDistance();
            bool irState = (digitalRead(irsensor) == LOW);

            Serial.println("=== Sensor Readings ===");
            Serial.printf("Ultrasonic Front Distance: %.2f cm\n", front);
            Serial.printf("IR Sensor: %s\n", irState ? "Obstacle Detected" : "Clear");
            Serial.printf("Servo: %d\n", servo.read());
            Serial.println("========================");
            delay(500);
        }
    }
};

const int ena = 25;
const int in1 = 26;
const int in2 = 27;
const int enb = 32;
const int in3 = 33;
const int in4 = 4;
const int trig = 5;
const int echo = 18;
const int servoPin = 23;
const int indicator = 2;
const int front_led = 21;
const int back_led = 15;
const int irSensorPin = 19;

CAR mycar(ena, in1, in2, enb, in3, in4, trig, echo, servoPin, front_led, back_led, irSensorPin);

bool mode = true;
bool ign = false;
AsyncWebServer server(80);

void handleMode(AsyncWebServerRequest *req)
{
    if (req->hasArg("mode"))
    {
        if (req->arg("mode") == "manual")
        {
            mode = false;
            req->send(200, "text/plain", "MANUAL");
            Serial.println("Mode = manual");
            for (int i = 0; i < 3; i++)
            {
                mycar.stop();
                delay(100);
            }
            mycar.stop();
            mycar.setSpeed(255);
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
void scanI2CDevices() {
    Serial.println("Scanning for I2C devices...");
  
    byte count = 0;
    for (byte address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0) {
        Serial.print("Found device at 0x");
        Serial.println(address, HEX);
        count++;
        delay(10);
      }
    }
  
    if (count == 0) {
      Serial.println("No I2C devices found.");
    } else {
      Serial.print("Total devices found: ");
      Serial.println(count);
    }
  }
void setup()
{
    Serial.begin(115200);
    Wire.begin(13,14);
    scanI2CDevices();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("OLED failed");
        while (true)
            ;
    }
    showAlert("ADAS CAR");
    delay(2000);
    display.clearDisplay();
    showProgressBar("Booting ADAS System", 0);
    for (int i = 1; i <= 100; i++)
    {
        showProgressBar("Booting ADAS System", i);
        delay(100);
    }
    showProgressBar("Initializing Wifi",0);
    WiFi.softAP("ADAS CAR", "rohanbatra");
    delay(500);
    for(int i=1;i<=100;i++){
        showProgressBar("Initializing Wifi",i);
        delay(60);
    }
    
    showProgressBar("Starting Webserver",0);
    pinMode(indicator, OUTPUT);
    Serial.println("WiFi Started at " + WiFi.softAPIP().toString());
    Serial.println("Setting up server handlers");
    server.on("/mode", HTTP_POST, handleMode);

    showProgressBar("Starting Webserver",20);
    server.on("/command", HTTP_POST, handleCommand);

    showProgressBar("Starting Webserver",50);
    server.on("/", HTTP_GET, handleroot);

    showProgressBar("Starting Webserver",80);
    server.on("/ign", HTTP_POST, handleign);

    Serial.println("All Handlers Initialized");
    delay(1000);
    showProgressBar("Starting Webserver",100);
    mycar.servotest();
    server.begin();
    Serial.println("Server started");
}
void loop()
{

    if (ign)
    {
        digitalWrite(indicator, HIGH);

        if (mode)
        {
            //showAlert("ADAS MODE");
            mycar.adasDrive();
            delay(200);
        }
        else{
            showAlert("MANUAL");
        }
    }
    else
    {
        digitalWrite(indicator, LOW);
        mycar.stop();
        showAlert("IGN OFF");
    }
    mycar.handleSerialCommands();
}
