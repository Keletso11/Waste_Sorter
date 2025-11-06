#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>

// ======================= OLED SETUP =======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ======================= SERVO SETUP =======================
#define SERVO1_PIN 12  // Organic
#define SERVO2_PIN 14  // Plastic
#define SERVO3_PIN 27  // Paper

Servo servo1;
Servo servo2;
Servo servo3;

// ======================= SERVO ANGLES =======================
const int SERVO_OPEN_ANGLE = 0;    // Lid fully open
const int SERVO_CLOSED_ANGLE = 180; // Lid fully closed

// ======================= LED SETUP =======================
#define LED1_PIN 4
#define LED2_PIN 16
#define LED3_PIN 33

// ======================= VARIABLES =======================
bool binIsOpen = false;
String currentOpenBin = "";
unsigned long binOpenStartTime = 0;
const unsigned long BIN_OPEN_DURATION = 5000; // 5 seconds
const unsigned long DEBOUNCE_DELAY = 1000;    // 1 second between commands
unsigned long lastCommandTime = 0;

float CONFIDENCE_THRESHOLD = 0.7; // Minimum confidence to accept command

bool demoMode = true; // Enable demo mode
unsigned long demoStartTime = 0;
int demoStep = 0;     // 0 = organic, 1 = plastic, 2 = paper

// ======================= ESP-NOW STRUCT =======================
typedef struct struct_message {
    char command[20];
    char label[20];
    float confidence;
} struct_message;

struct_message myData;

// ======================= FUNCTION DECLARATIONS =======================
void openBin(const String &type);
void closeAllBins();
void updateDisplay(const String &line1, const String &line2 = "", const String &line3 = "");
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len);
void runDemo();

// ======================= SETUP =======================
void setup() {
    Serial.begin(115200);

    // Initialize LEDs
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED not found!");
        while (1);
    }
    updateDisplay("Smart Bin", "Initializing...", "Please wait");

    // Initialize servos
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo3.attach(SERVO3_PIN);
    closeAllBins();
    delay(1000);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        updateDisplay("ESP-NOW Failed", "Restart system");
        while (1);
    }

    // Register new ESP-NOW callback
    esp_now_register_recv_cb(OnDataRecv);

    updateDisplay("Smart Bin", "System Ready", "Waiting...");
    Serial.println("System initialized successfully");
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    demoStartTime = millis(); // Start demo mode timer
}

// ======================= LOOP =======================
void loop() {
    // Auto-close bin after 5 seconds
    if (binIsOpen && (millis() - binOpenStartTime >= BIN_OPEN_DURATION)) {
        closeAllBins();
        binIsOpen = false;
        currentOpenBin = "";
        updateDisplay("Bin Closed", "Ready for next item...");
    }

    // Run demo mode if enabled
    if (demoMode) {
        runDemo();
    }

    delay(20); // small delay for loop
}

// ======================= DEMO MODE =======================
void runDemo() {
    // Run each step every 7 seconds (5s open + 2s pause)
    if (millis() - demoStartTime >= 7000) {
        demoStartTime = millis();

        if (binIsOpen) {
            closeAllBins();
            binIsOpen = false;
            currentOpenBin = "";
            updateDisplay("Bin Closed", "Ready for next...");
        }

        switch (demoStep) {
            case 0:
                openBin("ORGANIC");
                break;
            case 1:
                openBin("PLASTIC");
                break;
            case 2:
                openBin("PAPER");
                break;
        }

        demoStep++;
        if (demoStep > 2) demoStep = 0; // Loop back
    }
}

// ======================= ESP-NOW CALLBACK =======================
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
    if (len != sizeof(struct_message)) return;

    memcpy(&myData, incomingData, sizeof(struct_message));

    // Debounce
    if (millis() - lastCommandTime < DEBOUNCE_DELAY) return;
    lastCommandTime = millis();

    // Confidence check
    if (myData.confidence < CONFIDENCE_THRESHOLD) {
        Serial.println("Low confidence, ignoring command");
        return;
    }

    String label = String(myData.label);
    label.trim();
    label.toUpperCase();

    if (binIsOpen) {
        updateDisplay("Bin Busy", currentOpenBin + " open", "Wait...");
        return;
    }

    if (label.indexOf("ORGANIC") >= 0) {
        openBin("ORGANIC");
    } else if (label.indexOf("PLASTIC") >= 0) {
        openBin("PLASTIC");
    } else if (label.indexOf("PAPER") >= 0) {
        openBin("PAPER");
    } else {
        updateDisplay("Unknown Material", label, "Try again");
    }
}

// ======================= BIN CONTROL =======================
void openBin(const String &type) {
    Serial.printf("Opening %s bin\n", type.c_str());
    binIsOpen = true;
    currentOpenBin = type;
    binOpenStartTime = millis();

    updateDisplay("Opening:", type + " Bin", "Please wait...");

    closeAllBins();
    delay(200); // small delay before opening

    if (type == "ORGANIC") {
        servo1.write(SERVO_OPEN_ANGLE);
        digitalWrite(LED1_PIN, HIGH);
    } else if (type == "PLASTIC") {
        servo2.write(SERVO_OPEN_ANGLE);
        digitalWrite(LED2_PIN, HIGH);
    } else if (type == "PAPER") {
        servo3.write(SERVO_OPEN_ANGLE);
        digitalWrite(LED3_PIN, HIGH);
    }
}

// ======================= CLOSE ALL BINS =======================
void closeAllBins() {
    servo1.write(SERVO_CLOSED_ANGLE);
    servo2.write(SERVO_CLOSED_ANGLE);
    servo3.write(SERVO_CLOSED_ANGLE);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);

    Serial.println("All bins closed");
}

// ======================= OLED DISPLAY =======================
void updateDisplay(const String &line1, const String &line2, const String &line3) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(line1);
    display.setCursor(0, 20);
    display.println(line2);
    display.setCursor(0, 40);
    display.println(line3);
    display.display();
}