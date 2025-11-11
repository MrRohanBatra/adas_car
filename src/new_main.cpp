#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <stdlib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <algorithm>
#include <U8g2lib.h>
#include "pins.h"
#include <ACS712.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
ACS712 cur(curSensorPin, 3.3, 4095, 66);
float front_distance, left_distance, right_distance;
float battery = 0.0;
void showStatus(const String &mode, int speed)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf);

    String batteryStr = String(battery, 1) + "V";
    int bw = u8g2.getStrWidth(batteryStr.c_str());
    u8g2.drawStr(128 - bw - 2, 10, batteryStr.c_str());

    u8g2.drawStr(2, 14, ("MODE: " + mode).c_str());

    u8g2.drawStr(2, 30, ("SPD: " + String(speed) + "%").c_str());

    u8g2.sendBuffer();
}

void showAlert(const String &mainMessage, const String &subHeading = "")
{
    u8g2.clearBuffer();

    String batteryStr = String(battery, 1) + "V";
    u8g2.setFont(u8g2_font_6x12_tf);
    int bw = u8g2.getStrWidth(batteryStr.c_str());
    u8g2.drawStr(128 - bw - 2, 10, batteryStr.c_str());

    u8g2.setFont(u8g2_font_logisoso16_tf);
    int w = u8g2.getStrWidth(mainMessage.c_str());
    int x = (128 - w) / 2;
    int y = (subHeading.isEmpty()) ? 38 : 30;
    u8g2.drawStr(x, y, mainMessage.c_str());

    if (subHeading.length() > 0)
    {
        u8g2.setFont(u8g2_font_6x12_tf);
        int w2 = u8g2.getStrWidth(subHeading.c_str());
        int x2 = (128 - w2) / 2;
        u8g2.drawStr(x2, y + 18, subHeading.c_str());
    }

    u8g2.sendBuffer();
}

// class MotorDriver
// {
// private:
//     int ena, in1, in2, enb, in3, in4, chA, chB, speed;

// public:
//     MotorDriver(int ena, int in1, int in2, int enb, int in3, int in4, int speed, int chA = 3, int chB = 4)
//     {
//         this->ena = ena;
//         this->in1 = in1;
//         this->in2 = in2;
//         this->enb = enb;
//         this->in3 = in3;
//         this->in4 = in4;
//         this->chA = chA;
//         this->chB = chB;
//         this->speed = speed;
//     }

//     void begin()
//     {
//         pinMode(in1, OUTPUT);
//         pinMode(in2, OUTPUT);
//         pinMode(in3, OUTPUT);
//         pinMode(in4, OUTPUT);

//         ledcAttachPin(ena, chA);
//         ledcAttachPin(enb, chB);
//         ledcSetup(chA, 300, 8);
//         ledcSetup(chB, 300, 8);
//         stop();
//     }

//     void setSpeed(int s)
//     {
//         speed = constrain(s, 0, 255);
//         ledcWrite(chA, speed);
//         ledcWrite(chB, speed);
//     }

//     void forward()
//     {
//         digitalWrite(in1, HIGH);
//         digitalWrite(in2, LOW);
//         digitalWrite(in3, HIGH);
//         digitalWrite(in4, LOW);
//         ledcWrite(chA, speed);
//         ledcWrite(chB, speed);
//     }

//     void backward()
//     {
//         digitalWrite(in1, LOW);
//         digitalWrite(in2, HIGH);
//         digitalWrite(in3, LOW);
//         digitalWrite(in4, HIGH);
//         ledcWrite(chA, speed);
//         ledcWrite(chB, speed);
//     }

//     void turnLeft()
//     {
//         digitalWrite(in1, HIGH);
//         digitalWrite(in2, LOW);
//         digitalWrite(in3, LOW);
//         digitalWrite(in4, HIGH);
//         ledcWrite(chA, speed / 2);
//         ledcWrite(chB, speed);
//     }

//     void turnRight()
//     {
//         digitalWrite(in1, LOW);
//         digitalWrite(in2, HIGH);
//         digitalWrite(in3, HIGH);
//         digitalWrite(in4, LOW);
//         ledcWrite(chA, speed);
//         ledcWrite(chB, speed / 2);
//     }

//     void stop()
//     {
//         digitalWrite(in1, LOW);
//         digitalWrite(in2, LOW);
//         digitalWrite(in3, LOW);
//         digitalWrite(in4, LOW);
//         ledcWrite(chA, 0);
//         ledcWrite(chB, 0);
//     }
// };
class MotorDriver
{
private:
    int ena, in1, in2; // Right side
    int enb, in3, in4; // Left side
    int chA, chB, speed;

public:
    MotorDriver(int ena, int in1, int in2, int enb, int in3, int in4, int speed, int chA = 3, int chB = 4)
    {
        this->ena = ena;
        this->in1 = in1;
        this->in2 = in2;
        this->enb = enb;
        this->in3 = in3;
        this->in4 = in4;
        this->chA = chA;
        this->chB = chB;
        this->speed = speed;
    }

    void begin()
    {
        pinMode(in1, OUTPUT);
        pinMode(in2, OUTPUT);
        pinMode(in3, OUTPUT);
        pinMode(in4, OUTPUT);

        ledcSetup(chA, 300, 8);
        ledcSetup(chB, 300, 8);
        ledcAttachPin(ena, chA);
        ledcAttachPin(enb, chB);

        stop();
    }

    void setSpeed(int s)
    {
        speed = constrain(s, 0, 255);
    }

    void forward()
    {
        // Both sides forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        ledcWrite(chA, speed);
        ledcWrite(chB, speed);
    }

    void backward()
    {
        // Both sides backward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        ledcWrite(chA, speed);
        ledcWrite(chB, speed);
    }

    void left()
    {
        // Right side forward, left side backward
        digitalWrite(in1, HIGH); // Right forward
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH); // Left backward
        digitalWrite(in4, LOW);
        ledcWrite(chA, speed);
        ledcWrite(chB, speed);
    }

    void right()
    {
        // Right side backward, left side forward
        digitalWrite(in1, LOW); // Right backward
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW); // Left forward
        digitalWrite(in4, HIGH);
        ledcWrite(chA, speed);
        ledcWrite(chB, speed);
    }

    void stop()
    {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        ledcWrite(chA, 0);
        ledcWrite(chB, 0);
    }
};

void showDistance(float distance)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.drawStr(10, 30, "DISTANCE:");

    char buf[16];
    sprintf(buf, "%.1f cm", distance);
    u8g2.setFont(u8g2_font_logisoso22_tf);
    u8g2.drawStr((128 - u8g2.getStrWidth(buf)) / 2, 60, buf);

    u8g2.sendBuffer();
}
class HCSR04
{
private:
    int trigPin;
    int echoPin;
    int calibration = 0;

public:
    HCSR04(int trig, int echo)
    {
        trigPin = trig;
        echoPin = echo;
    }

    void begin()
    {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        digitalWrite(trigPin, LOW);
        delay(50);
    }

    float getDistanceRaw()
    {
        // Send trigger pulse (10µs)
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Measure echo pulse
        long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
        if (duration == 0)
            return -1; // No object detected

        // Calculate distance (cm)
        float distance = (duration * 0.0343) / 2.0;
        float corrected = distance - calibration;
        return corrected <= 0 ? 0 : corrected;
    }
    float getDistance(){
        int count = 5;
        int sum = 0;
        for (int i = 0; i < count;i++){
            sum += this->getDistanceRaw();
            delay(1);
        }
        return sum / count;
    }
};

class CAR
{
private:
    MotorDriver motor;
    HCSR04 scanner;
    Servo servo;
    const int threshold = 20;
    int front, back;
    int motorspeed = 200;
    int irsensor;

public:
    HCSR04 getscanner()
    {
        return scanner;
    }
    int getmotorspeed()
    {
        return this->motorspeed;
    }
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
    CAR(int ena, int in1, int in2, int enb, int in3, int in4,
        int trig, int echo, int servoPin, int front_led, int back_led, int irsensor1)
        : motor(ena, in1, in2, enb, in3, in4, motorspeed), scanner(trig, echo)
    {
        front = front_led;
        back = back_led;
        servo.attach(servoPin);
        this->irsensor = irsensor1;
    }
    void setup()
    {
        pinMode(front, OUTPUT);
        pinMode(back, OUTPUT);
        pinMode(irsensor, INPUT);
        scanner.begin();
        motor.begin();
        motor.setSpeed(motorspeed);
        servo.write(90);
        delay(300);
    }
    void forward(int speed)
    {
        digitalWrite(back, LOW);
        digitalWrite(front, HIGH);
        motor.setSpeed(speed);
        motor.forward();
    }

    void backward(int speed)
    {
        digitalWrite(front, LOW);
        digitalWrite(back, HIGH);
        motor.setSpeed(speed);
        motor.backward();
    }

    void stop()
    {
        digitalWrite(front, LOW);
        digitalWrite(back, LOW);
        motor.setSpeed(0);
        motor.stop();
    }

    void turnLeft(int speed)
    {
        digitalWrite(front, HIGH);
        digitalWrite(back, LOW);
        motor.setSpeed(speed);
        motor.left();
    }

    void turnRight(int speed)
    {
        digitalWrite(front, HIGH);
        digitalWrite(back, LOW);
        motor.setSpeed(speed);
        motor.right();
    }

    void spinAround(int speed)
    {
        digitalWrite(front, HIGH);
        digitalWrite(back, HIGH);
        motor.setSpeed(speed);
        motor.left();
        delay(1500);
        stop();
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

            int turnSpeed = 180;
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

        // front_distance = scanner.getDistance();
        // int temp = constrain(int(front_distance), 10, 200);
        // motorspeed = map(temp, 10, 200, 100, 200);
        front_distance = scanner.getDistance();

        int safeDistance = threshold + 20; // start accelerating after this

        if (front_distance <= threshold)
        {
            // Too close — stop
            motorspeed = 0;
        }
        else if (front_distance <= safeDistance)
        {
            // Between threshold and safe distance → slow zone
            motorspeed = map(front_distance, threshold, safeDistance, 100, 150);
        }
        else if (front_distance <= 200)
        {
            // Beyond safe zone → cruise zone
            motorspeed = map(front_distance, safeDistance, 200, 150, 200);
        }
        else
        {
            // Cap at max speed
            motorspeed = 200;
        }

        motorspeed = constrain(motorspeed, 0, 200);
        Serial.printf("Front Distance: %.2f cm | Adaptive Speed: %d\n", front_distance, motorspeed);

        // Serial.printf("Front Distance: %.2f cm | Speed: %d\n", front_distance, motorspeed);

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

            servo.write(150);
            delay(300);
            right_distance = scanner.getDistance();

            servo.write(90);
            delay(200);

            Serial.printf("Left: %.2f cm | Right: %.2f cm\n", left_distance, right_distance);
            // if (left_distance == -1 || front_distance == -1 || right_distance == -1)
            // {
            //     while (true)
            //     {
            //         showAlert("Error Hcsr04");
            //     }
            // }
            if (left_distance > threshold && left_distance > right_distance)
            {
                int turnDur = map(left_distance, threshold, 200, 300, 700);
                Serial.printf("Turning LEFT for %d ms\n", turnDur);
                backward(175);
                delay(150);
                turnLeft(180);
                delay(turnDur);
            }
            else if (right_distance > threshold)
            {
                int turnDur = map(right_distance, threshold, 200, 300, 700);
                Serial.printf("Turning RIGHT for %d ms\n", turnDur);
                backward(175);
                delay(150);
                turnRight(180);
                delay(turnDur);
            }
            else
            {
                Serial.println("No clear side. Reversing and random escape...");

                int reverseTime = 700;
                int reverseSpeed = 200;
                backward(reverseSpeed);
                delay(reverseTime);
                stop();
                delay(100);

                int turnDir = random(0, 2);
                int turnTime = 500;

                Serial.printf("Random escape: Turning %s for %d ms\n", turnDir == 0 ? "LEFT" : "RIGHT", turnTime);
                if (turnDir == 0)
                {
                    backward(175);
                    delay(150);
                    turnLeft(180);
                }
                else
                {
                    backward(175);
                    delay(150);
                    turnRight(180);
                }
                delay(turnTime);
                stop();
                delay(100);
            }
        }
    }
    void servotest()
    {
        servo.write(0);
        for (int k = 0; k <= 2; k++)
        {
            for (int i = 0; i <= 180; i += 10)
            {
                servo.write(i);
                delay(50);
            }
            for (int i = 180; i >= 0; i -= 10)
            {
                servo.write(i);
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
            spinAround(255);
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
                Serial.println("testing servo");
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
    void hcsr04test()
    {
        float dis = scanner.getDistance();
        showDistance(dis);
    }
};

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
            showAlert("MANUAL");
            // showStatus("MANUAL", mycar.getmotorspeed());
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
            showAlert("ADAS MODE");
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
                mycar.command(command, 255);
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
float getBatteryVoltage()
{
    float R1 = 4000, R2 = 2000;
    const int sampleCount = 100;
    long sumV = 0;
    long sumC = 0;

    for (int i = 0; i < sampleCount; i++)
    {
        sumV += analogRead(vin);
        sumC += cur.mA_DC();
        delay(3);
    }

    float avgRawV = sumV / float(sampleCount);
    float avgCur = sumC / float(sampleCount);
    float avgCurA = avgCur / 1000.0;
    float voltageOnPin = (avgRawV * 3.3) / 4095.0;
    float inputVoltage = voltageOnPin * ((R1 + R2) / R2);
    float totalVol = inputVoltage + fabs(avgCurA) * 0.4;
    return totalVol;
}
void showBootScreen()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso16_tr);
    const char *title = "ADAS CAR";
    u8g2.drawStr((128 - u8g2.getStrWidth(title)) / 2, 28, title);

    u8g2.setFont(u8g2_font_6x12_tf);
    const char *subtitle = "By Rohan Batra";
    u8g2.drawStr((128 - u8g2.getStrWidth(subtitle)) / 2, 50, subtitle);

    u8g2.sendBuffer();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Calibrating ACS712");
    cur.autoMidPointDC();
    Serial.print("Midpoint: ");
    Serial.println(cur.getMidPoint());
    Wire.begin(13, 14);
    u8g2.begin();
    showBootScreen();
    WiFi.softAP("ADAS CAR", "rohanbatra");
    mycar.setup();
    battery = getBatteryVoltage();
    pinMode(indicator, OUTPUT);
    Serial.println("WiFi Started at " + WiFi.softAPIP().toString());
    Serial.println("Setting up server handlers");
    server.on("/mode", HTTP_POST, handleMode);
    server.on("/command", HTTP_POST, handleCommand);
    server.on("/", HTTP_GET, handleroot);
    server.on("/ign", HTTP_POST, handleign);
    server.on("/status_mode", HTTP_GET, handlestatus_mode);
    server.on("/status_ign", HTTP_GET, handlestatus_ign);

    Serial.println("All Handlers Initialized");
    server.begin();
    Serial.println("Server started");
    Serial.println("testing servo");
    mycar.servotest();
    delay(2000);
}
int count = 0;
void showDistances(float left, float front, float right)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf); // small clean font

    // --- Title ---
    const char *title = "DISTANCE SCAN";
    int titleW = u8g2.getStrWidth(title);
    u8g2.drawStr((128 - titleW) / 2, 10, title);

    // --- Front ---
    String fStr = "FRONT: " + String(front, 1) + " cm";
    u8g2.drawStr(10, 28, fStr.c_str());

    // --- Left ---
    String lStr = "LEFT:  " + String(left, 1) + " cm";
    u8g2.drawStr(10, 44, lStr.c_str());

    // --- Right ---
    String rStr = "RIGHT: " + String(right, 1) + " cm";
    u8g2.drawStr(10, 60, rStr.c_str());

    // --- Optional battery indicator (top-right) ---
    String batteryStr = String(battery, 1) + "V";
    int bw = u8g2.getStrWidth(batteryStr.c_str());
    u8g2.drawStr(128 - bw - 2, 10, batteryStr.c_str());

    u8g2.sendBuffer();
}

void loop()
{

    if (ign)
    {
        digitalWrite(indicator, HIGH);

        if (mode)
        {
            // if (count == 0)
            // {

            //     // showStatus("ADAS", mycar.getmotorspeed());
            //     count++;
            // }
            // showAlert("ADAS");
            mycar.adasDrive();
            showDistances(left_distance, front_distance, right_distance);
            battery = getBatteryVoltage();
        }
        else
        {
            showAlert("MANUAL");
        }
    }
    else
    {
        digitalWrite(indicator, LOW);
        mycar.stop();
        showAlert("IGN OFF");
    }
    // mycar.hcsr04test();
    mycar.handleSerialCommands();
    battery = getBatteryVoltage();
    front_distance = mycar.getdis(90);
}
