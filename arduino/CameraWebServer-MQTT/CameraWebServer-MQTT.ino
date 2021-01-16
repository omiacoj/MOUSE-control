#include <WiFi.h>

#include <MQTT.h>

#include "base64.h"



TaskHandle_t Task1;
TaskHandle_t Task2;


String beacon_id = "espcameramouse";

WiFiClient net;
// 16 kb buffer
MQTTClient client(16348);


String imgDataB64;
bool publish_frame = false;
long last_frame_time = 0;
float fps = 1.0;

String last_command;

///#include <analogWrite.h>

#include "esp_camera.h"

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"


const char* ssid = "Accessphone";
const char* password = "Accessphone";


#define DRIVE_V 14
#define DRIVE_B 12
#define DIRECTION_L 13
#define DIRECTION_R 15
#define lpwm 2
#define rpwm 16

void motor(int drive, int direction)
{

  // pwm = 980 Hz = 980 signals / sec
  // => 1 signal = (1/980)s = 1000/980 ms = 1000000/980 s
  float signal_length = 1000000.0 / 980.0;
  float pwmSpeed = 127.0;
  float high_length = (signal_length / 255.0) * pwmSpeed;
  /*
    // principle
    digitalWrite(lpwm, HIGH);
    delayMicroseconds(high_length);
    digitalWrite(lpwm, HIGH);
    delayMicroseconds(signal_length - high_length);
  */
  // Serial.println("D: " + String(drive));
  // Serial.println("R: " + String(direction));



  if (drive == 0 && direction == 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, LOW);
  }

  if (drive < 0 && direction == 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, HIGH);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, LOW);

    for (int l = 0; l < 10; l++) {
      digitalWrite(lpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }

  }

  if (drive > 0 && direction == 0) {
    digitalWrite(DRIVE_V, HIGH);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, LOW);

    for (int l = 0; l < 10; l++) {
      digitalWrite(lpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }

  }




  if (drive == 0 && direction < 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, HIGH);


    for (int l = 0; l < 10; l++) {

      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }
  }


  if (drive < 0 && direction < 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, HIGH);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, HIGH);

    for (int l = 0; l < 10; l++) {
      digitalWrite(lpwm, HIGH);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);

    }

  }

  if (drive > 0 && direction < 0) {
    digitalWrite(DRIVE_V, HIGH);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, LOW);
    digitalWrite(DIRECTION_R, HIGH);

    for (int l = 0; l < 10; l++) {
      digitalWrite(lpwm, HIGH);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }

  }




  if (drive == 0 && direction > 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, HIGH);
    digitalWrite(DIRECTION_R, LOW);
    for (int l = 0; l < 10; l++) {
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }
  }

  if (drive < 0 && direction > 0) {
    digitalWrite(DRIVE_V, LOW);
    digitalWrite(DRIVE_B, HIGH);

    digitalWrite(DIRECTION_L, HIGH);
    digitalWrite(DIRECTION_R, LOW);

    for (int l = 0; l < 10; l++) {
      digitalWrite(lpwm, HIGH);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);

    }

  }

  if (drive > 0 && direction > 0) {
    digitalWrite(DRIVE_V, HIGH);
    digitalWrite(DRIVE_B, LOW);

    digitalWrite(DIRECTION_L, HIGH);
    digitalWrite(DIRECTION_R, LOW);

    for (int l = 0; l < 10; l++) {

      digitalWrite(lpwm, HIGH);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, LOW);
      digitalWrite(rpwm, LOW);
      delayMicroseconds(signal_length - high_length);
    }





  }


  /*

    if (drive == 0)
    {
      digitalWrite(DRIVE_V, LOW);
      digitalWrite(DRIVE_B, LOW);
    }
    else if (drive < 0)
    {
      digitalWrite(DRIVE_V, LOW);
      digitalWrite(DRIVE_B, HIGH);
      //analogWrite(lpwm, 127);

      digitalWrite(lpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, HIGH);
      delayMicroseconds(signal_length - high_length);
    }
    else
    {
      digitalWrite(DRIVE_V, HIGH);
      digitalWrite(DRIVE_B, LOW);
      //analogWrite(lpwm, 127);

      digitalWrite(lpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(lpwm, HIGH);
      delayMicroseconds(signal_length - high_length);
    }

    if (direction == 0)
    {
      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, LOW);
    }
    else if (direction < 0)
    {
      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, HIGH);
      //analogWrite(rpwm, 127);

      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(signal_length - high_length);
    }
    else
    {
      digitalWrite(DIRECTION_L, HIGH);
      digitalWrite(DIRECTION_R, LOW);
      //analogWrite(rpwm, 127);

      digitalWrite(rpwm, HIGH);
      delayMicroseconds(high_length);
      digitalWrite(rpwm, HIGH);
      delayMicroseconds(signal_length - high_length);
    }*/
}

void webSocketEvent(String &topic, String &payload) {


  Serial.println("webSocketEvent: " + String(payload));

  last_command = payload;
  last_command.trim();
}

void connect_mqtt() {


  Serial.print("\nconnecting mqtt...");

  //String mqtt_id = "esp32-" + beacon_id;
  delay(1000);
  while (!client.connect(String(beacon_id).c_str())) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected to mqtt!");


  client.subscribe("/espcamera/mousemovement");
  Serial.println("\nsubscribed to topic!");

}

void setup_camera_wifi() {

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 40;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }



  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");


  client.begin("13.81.105.139", net);

  client.onMessage(webSocketEvent);

  connect_mqtt();


}


void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();


  setup_tasks();


}






void setup_tasks() {
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */
  delay(500);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */
  delay(500);


}

//Task1code: diy analog signal; motor driver
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  Serial.print("Init motor");
  pinMode(DRIVE_V, OUTPUT);
  pinMode(DRIVE_B, OUTPUT);
  pinMode(DIRECTION_L, OUTPUT);
  pinMode(DIRECTION_R, OUTPUT);
  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1

  //motor(0, 0);

  for (;;) {
    // motor timing loop

    int speed = 127;

    if (last_command == "down")
    {
      //int drive = -1 * speed;
      Serial.println("down motor");
      motor(-1 * speed, -1 * speed);
    }
    else if (last_command == "up")
    {
      //int drive = speed;
      Serial.println("up motor");
      motor(speed, speed);
    }
    else if (last_command == "stop")
    {
      //int drive = 0;
      Serial.println("stop motor");
      motor(0, 0);
    }
    else if (last_command == "off")
    {
      //int drive = 0;
      Serial.println("off motor");
      last_command = "";
      digitalWrite(DRIVE_V, LOW);
      digitalWrite(DRIVE_B, LOW);

      digitalWrite(DIRECTION_L, LOW);
      digitalWrite(DIRECTION_R, LOW);
    }
    //last_command = "";
    //vTaskDelay(1);
  }
}

//Task2code: wifi camera mqtt
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  setup_camera_wifi();

  for (;;) {
    loop_camera();
  }
}

void loop() {
  vTaskDelay(1);
}


void loop_camera()
{
  publish_frame = false;
  if (millis() - last_frame_time > (1000.0 / fps)) {
    last_frame_time = millis();



    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
      Serial.println("Camera capture failed");
      esp_camera_fb_return(fb);
      return;
    }

    if (fb->format != PIXFORMAT_JPEG)
    {
      Serial.println("Non-JPEG data not implemented");
      return;
    }

    // client.sendBinary((const char *)fb->buf, fb->len);



    imgDataB64 = base64::encode(fb->buf, fb->len);
    publish_frame = true;
    Serial.print("Frame size: ");
    Serial.println(fb->len);




    esp_camera_fb_return(fb);
  }
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability
  if (!client.connected()) {
    Serial.println("Reconnecting mqtt");
    connect_mqtt();
  }
  if (publish_frame) {
    client.publish("/espcamera/stream", imgDataB64);
    imgDataB64 = "";
  }
}
