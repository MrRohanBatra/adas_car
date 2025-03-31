#include <Arduino.h>
#include <ESP32Servo.h>
// #include <BluetoothSerial.h>
#include <WiFi.h>
#include <WebServer.h>

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

//     float getDistance()
//     {
//         digitalWrite(trigPin, LOW);
//         delayMicroseconds(2);
//         digitalWrite(trigPin, HIGH);
//         delayMicroseconds(10);
//         digitalWrite(trigPin, LOW);

//         long int duration = pulseIn(echoPin, HIGH);
//         if (duration == 0)
//             return 999.0;
//         return (duration * 0.034 / 2);
//     }
// };
class HCSR04 {
    private:
        int trigPin, echoPin;
    
    public:
        HCSR04(int trig, int echo) {
            trigPin = trig;
            echoPin = echo;
            pinMode(trigPin, OUTPUT);
            pinMode(echoPin, INPUT);
        }
    
        float getDistance() {
            digitalWrite(trigPin, LOW);
            delayMicroseconds(5);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(15);
            digitalWrite(trigPin, LOW);
    
            long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout
            Serial.print("Duration: "); Serial.println(duration);
    
            if (duration == 0) return 999.0;  // No response
            return (duration * 0.034 / 2);
        }
    };
    
class L298N
{
private:
    int ena, in1, in2, pwmChannel;
    int pwmFreq = 500;
    int pwmResolution = 8;

public:
    L298N(int enaPin, int in1Pin, int in2Pin, int channel)
    {
        ena = enaPin;
        in1 = in1Pin;
        in2 = in2Pin;
        pwmChannel = channel;

        pinMode(in1, OUTPUT);
        pinMode(in2, OUTPUT);
        ledcSetup(pwmChannel, pwmFreq, pwmResolution);
        ledcAttachPin(ena, pwmChannel);
    }

    void forward(int speed)
    {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, constrain(speed, 0, 255));
    }

    void backward(int speed)
    {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        ledcWrite(pwmChannel, constrain(speed, 0, 255));
    }

    void stop()
    {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, 0);
    }
};

class CAR
{
private:
    L298N left, right;
    HCSR04 scanner;
    Servo servo;
    const int threshold = 20;
    int front, back;

public:
    void test()
    {
        // Move from 0 to 180
        for (int i = 0; i <= 180; i += 10)
        {
            servo.write(i);
            delay(500);
        }

        // Move back from 180 to 0
        for (int i = 180; i >= 0; i -= 10)
        {
            servo.write(i);
            delay(500);
        }
    }

    CAR(int ena, int in1, int in2, int enb, int in3, int in4, int trig, int echo, int servoPin, int front_led, int back_led)
        : left(ena, in1, in2, 0), right(enb, in3, in4, 1), scanner(trig, echo)
    {
        front = front_led;
        back = back_led;
        servo.attach(servoPin);
        servo.write(90);
        pinMode(front, OUTPUT);
        pinMode(back, OUTPUT);
    }

    void forward(int speed)
    {
        digitalWrite(back, LOW);
        left.forward(speed);
        right.forward(speed);
        digitalWrite(front, HIGH);
    }

    void backward(int speed)
    {
        digitalWrite(front, LOW);
        digitalWrite(back, HIGH);

        left.backward(speed);
        right.backward(speed);
    }

    void stop()
    {
        digitalWrite(back, LOW);
        digitalWrite(front, LOW);

        left.stop();
        right.stop();
    }

    void turnLeft(int speed)
    {
        left.stop();
        right.forward(speed);
    }

    void turnRight(int speed)
    {
        right.stop();
        left.forward(speed);
    }

    void spinAround(int speed)
    {
        left.forward(speed);
        right.backward(speed);
        digitalWrite(front, HIGH);
        digitalWrite(back, HIGH);

        delay(1500);
        stop();
    }

    void adasDrive(int speed)
    {
        servo.write(90);
        float frontDistance = scanner.getDistance();
        Serial.println(frontDistance);
        if (frontDistance > threshold)
        {
            forward(speed);
        }
        else
        {
            stop();
            delay(500);
            servo.write(0);
            delay(500);
            float leftDistance = scanner.getDistance();
            servo.write(180);
            delay(500);
            float rightDistance = scanner.getDistance();
            servo.write(90);

            if (leftDistance > threshold)
            {
                turnLeft(speed);
                delay(1000);
            }
            else if (rightDistance > threshold)
            {
                turnRight(speed);
                delay(1000);
            }
            else
            {
                spinAround(speed);
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
            turnLeft(speed);
        }
        else if (command == "R")
        {
            turnRight(speed);
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
        Serial.print("executed command: ");
        Serial.println(command);
    }
};

// #define ena 13      // Motor A PWM
// #define in1 14      // Motor A IN1
// #define in2 15      // Motor A IN2
// #define enb 16      // Motor B PWM
// #define in3 17      // Motor B IN3
// #define in4 18      // Motor B IN4
// #define trig 19     // Ultrasonic Sensor Trigger
// #define echo 32     // Ultrasonic Sensor Echo
// #define servoPin 22 // Servo Motor
// #define front_led 3 // Front LED (Changed from 10 to 2)
// #define back_led 4  // Back LED (Changed from 11 to 4)
#define ena 25      // Motor A PWM (PWM Capable)
#define in1 26      // Motor A IN1  
#define in2 27      // Motor A IN2  
#define enb 32      // Motor B PWM (PWM Capable)  
#define in3 33      // Motor B IN3  
#define in4 4       // Motor B IN4  

#define trig 5      // Ultrasonic Sensor Trigger  
#define echo 18     // Ultrasonic Sensor Echo (No Serial Conflict)  

#define servoPin 23 // Servo Motor (PWM Capable)  

#define front_led 2 // Front LED (Built-in LED)  
#define back_led 15 // Back LED (Safe for Output)  

WebServer server(80);
CAR myCar(ena, in1, in2, enb, in3, in4, trig, echo, servoPin, front_led, back_led);
bool mode = true;
bool ign = false;
void handleMode()
{
    if (server.hasArg("mode"))
    {
        if (server.arg("mode") == "manual")
        {
            mode = false;
            server.send(200, "text/plain", "MANUAL");
            Serial.println("Mode=manual");
        }
        else if (server.arg("mode") == "auto")
        {
            mode = true;
            server.send(200, "text/plain", "AUTO");
        }
    }
    else
    {
        server.send(400, "text/plain", "Invalid request");
    }
}

void handleCommand()
{
    if (ign && mode == false)
    {
        if (server.hasArg("cmd"))
        {
            String command = server.arg("cmd");
            myCar.command(command, 200);
            server.send(200, "text/plain", "Command executed");
        }
        else
        {
            server.send(400, "text/plain", "Invalid request");
        }
    }
    else if (mode == true)
    {
        server.send(300, "text/plain", "using internal AI@1.6.0.7");
    }
    else
    {
        server.send(300, "text/plain", "ign off");
    }
}
void handletest()
{
    server.send(200,"text/html","<h1>testing</h1>");
    myCar.test();
}
void handleign()
{
    if (server.hasArg("ign"))
    {
        String data = server.arg("ign");
        if (data == "true")
        {
            ign = true;
            server.send(200, "text/plain", "ign on");
        }
        else if (data == "false")
        {
            ign = false;
            digitalWrite(2, LOW);
            server.send(200, "text/plain", "ign off");
        }
    }
}
void handleroot()
{
    server.send(200, "text/html", "<html><head><script>window.location.href = 'https://adas-car.web.app/';</script></head></html>");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("WiFi Starting ....");
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nWiFi not found. Starting Soft AP mode.");
        WiFi.softAP("ADAS CAR", "rohanbatra");
        Serial.print("Soft AP IP Address: ");
        Serial.println(WiFi.softAPIP());
    }

    server.on("/mode", HTTP_POST, handleMode);
    server.on("/command", HTTP_POST, handleCommand);
    server.on("/", HTTP_GET, handleroot);
    server.on("/ign", HTTP_POST, handleign);
    server.on("/test", HTTP_GET, handletest);

    server.begin();
    Serial.println("Setup complete");
    Serial.println("Current mode: " + String(WiFi.getMode() == WIFI_AP ? "AP" : "STA"));
    Serial.println(WiFi.localIP());
    pinMode(2, OUTPUT);
}

void loop()
{
    server.handleClient();
    if (ign)
    {
        if (mode)
        {
            digitalWrite(2, HIGH);
            myCar.adasDrive(255);
        }
        else
        {   myCar.stop();
            digitalWrite(2, LOW);
        }
    }
    delay(10);
}