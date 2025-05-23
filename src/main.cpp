#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char* ssid = "M5StickC-AP";
const char* password = "12345678";  // Must be at least 8 characters

// Create WebServer object on port 80
WebServer server(80);

// Function declarations
void handleRoot();
void handleHello();
void handleNotFound();
void flashScreenAndSayHello();
void restoreNormalDisplay();

// HTML content for the webpage
const char* htmlPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>M5StickC Plus</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 50px;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
            font-size: 2em;
        }
        .container {
            background-color: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            display: inline-block;
        }
        .hello-button {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 20px 2px;
            cursor: pointer;
            border-radius: 8px;
            transition: background-color 0.3s;
        }
        .hello-button:hover {
            background-color: #45a049;
        }
        .hello-button:active {
            background-color: #3e8e41;
        }
        #status {
            margin-top: 20px;
            font-weight: bold;
            color: #333;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Hello World!</h1>
        <p>Welcome to M5StickC Plus Web Server</p>
        <p>You are successfully connected to the device!</p>
        <button class="hello-button" onclick='sayHello()'>Say Hello to M5Stick!</button>
        <div id="status"></div>
    </div>
    
    <script>
        function sayHello() {
            var statusEl = document.getElementById('status');
            statusEl.innerHTML = 'Sending hello to M5Stick...';
            
            fetch('/hello')
                .then(function(response) {
                    return response.text();
                })
                .then(function(data) {
                    statusEl.innerHTML = data;
                    setTimeout(function() {
                        statusEl.innerHTML = '';
                    }, 3000);
                })
                .catch(function(error) {
                    statusEl.innerHTML = 'Error: ' + error;
                });
        }
    </script>
</body>
</html>
)";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleHello() {
  // Flash the screen and display "Hello"
  flashScreenAndSayHello();
  server.send(200, "text/plain", "Hello sent to M5StickC Plus!");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

void flashScreenAndSayHello() {
  // Store current display state
  M5.Lcd.fillScreen(BLACK);
  
  // Flash sequence with different colors
  for (int i = 0; i < 3; i++) {
    M5.Lcd.fillScreen(WHITE);
    delay(200);
    M5.Lcd.fillScreen(RED);
    delay(200);
    M5.Lcd.fillScreen(GREEN);
    delay(200);
    M5.Lcd.fillScreen(BLUE);
    delay(200);
  }
  
  // Display "HELLO!" message
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(20, 40);
  M5.Lcd.println("HELLO!");
  
  delay(2000);  // Show message for 2 seconds
  
  // Return to normal display
  restoreNormalDisplay();
}

void restoreNormalDisplay() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  
  // Redisplay connection info
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("WiFi AP Active");
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.print("SSID: ");
  M5.Lcd.println(ssid);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.print("Pass: ");
  M5.Lcd.println(password);
  M5.Lcd.setCursor(0, 55);
  M5.Lcd.print("IP: ");
  M5.Lcd.println(WiFi.softAPIP());
  M5.Lcd.setCursor(0, 70);
  M5.Lcd.println("Server: Ready");
}

void setup() {
  // Initialize M5StickC Plus
  M5.begin();
  
  // Initialize display
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  
  // Display startup message
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("Starting AP...");
  
  // Set up Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // Get AP IP address
  IPAddress IP = WiFi.softAPIP();
  
  // Display connection info on screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("WiFi AP Active");
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.print("SSID: ");
  M5.Lcd.println(ssid);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.print("Pass: ");
  M5.Lcd.println(password);
  M5.Lcd.setCursor(0, 55);
  M5.Lcd.print("IP: ");
  M5.Lcd.println(IP);
  M5.Lcd.setCursor(0, 70);
  M5.Lcd.println("Server: Ready");
  
  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/hello", handleHello);
  server.onNotFound(handleNotFound);
  
  // Start web server
  server.begin();
  
  Serial.begin(115200);
  Serial.println("M5StickC Plus Access Point Started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("IP Address: ");
  Serial.println(IP);
  Serial.println("Web server started on port 80");
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // Update M5StickC Plus (for button handling, etc.)
  M5.update();
  
  // Optional: Show connected clients count
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) {  // Update every 5 seconds
    int clients = WiFi.softAPgetStationNum();
    M5.Lcd.fillRect(0, 85, 240, 15, BLACK);  // Clear previous count
    M5.Lcd.setCursor(0, 85);
    M5.Lcd.print("Clients: ");
    M5.Lcd.println(clients);
    lastUpdate = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}