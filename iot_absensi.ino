#include <WiFi.h>
#include <HTTPClient.h>

#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT11

#include "ESPino32CAM.h"
#include "ESPino32CAM_QRCode.h"

ESPino32CAM cam;
ESPino32QRCode qr;
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

const char *ssid = ".fadilp";
const char *password = "//akuFadil22;";

String serverName = "http://192.168.43.252:8000/test";

String capture_qr_scan()
{
  camera_fb_t *fb = cam.capture();
  dl_matrix3du_t *image_rgb;

  String qrData = "no_qr_data";

  if (!fb)
  {
    Serial.println("Camera capture failed");
    return qrData;
  }
  else
  {
    if (cam.jpg2rgb(fb, &image_rgb))
    {
      cam.clearMemory(fb);
      cam.printDebug("\r\nQR Read:");
      qrResoult res = qr.recognition(image_rgb);
      if (res.status)
      {

        Serial.println("Camera capture david");

        cam.printDebug("");
        cam.printfDebug("Version: %d", res.version);
        cam.printfDebug("ECC level: %c", res.eccLevel);
        cam.printfDebug("Mask: %d", res.mask);
        cam.printDebug("Data type: " + qr.dataType(res.dataType));
        cam.printfDebug("Length: %d", res.length);
        cam.printDebug("Payload: " + res.payload);
        qrData = res.payload;

        // Wait a few seconds between measurements.
        // delay(500);
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        // float h = dht.readHumidity();
        // // Read temperature as Celsius (the default)
        // float t = dht.readTemperature();
        // // Read temperature as Fahrenheit (isFahrenheit = true)
        // float f = dht.readTemperature(true);
        // // Check if any reads failed and exit early (to try again).
        // if (isnan(h) || isnan(t) || isnan(f))
        // {
        //   Serial.println(F("Failed to read from DHT sensor!"));
        //   return;
        // }
        // // Compute heat index in Fahrenheit (the default)
        // float hif = dht.computeHeatIndex(f, h);
        // // Compute heat index in Celsius (isFahreheit = false)
        // float hic = dht.computeHeatIndex(t, h, false);

        // Serial.print(F("Humidity: "));
        // Serial.print(h);
        // Serial.print(F("%  Temperature: "));
        // Serial.print(t);
        // Serial.print(F("°C "));
        // Serial.print(f);
        // Serial.print(F("°F  Heat index: "));
        // Serial.print(hic);
        // Serial.print(F("°C "));
        // Serial.print(hif);
        // Serial.println(F("°F"));
      }
      else
      {
        cam.printDebug("Gagal mengscan QR Code");
      }
    }
    cam.clearMemory(image_rgb);
    return qrData;
  }
}

void send_qr_data_to_server(String qrData)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Terhubung WIFI ke ");
    Serial.print(WiFi.SSID());
    Serial.println();

    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(serverName.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}

void setup()
{
  Serial.begin(115200);

  dht.begin();

  if (cam.init() != ESP_OK)
  {
    Serial.println("Fail to init esp");
  }
  else
  {
    qr.init(&cam);
    sensor_t *s = cam.sensor();
    s->set_framesize(s, FRAMESIZE_VGA);
    s->set_whitebal(s, true);
  }

  // Inisialisasi WIFI
  WiFi.begin(ssid, password);
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Gagal Terkoneksi");
  }
  else
  {
    Serial.println("WiFi Berhasil Disambungkan");
  }
}

void loop()
{
  String qrData = capture_qr_scan();
  if (qrData.isEmpty())
  {
    Serial.println("Tidak ada data dari camera QR");
  }
  else
  {
    // send_qr_data_to_server(qrData);
  }

  delay(3000);
}
