#include <Arduino_JSON.h>
#include <JSON.h>
#include <JSONVar.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include "ESPino32CAM.h"
#include "ESPino32CAM_QRCode.h"

ESPino32CAM cam;
ESPino32QRCode qr;
WiFiClient client;

char *ssid = "Henshin";
const char *password = "kr453145";

String serverName = "http://localhost:8000/api/v1/absen";

String capture_qr_scan()
{
  camera_fb_t *fb = cam.capture();
  dl_matrix3du_t *image_rgb;

  String qrData = "";

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

        Serial.println("Camera capture success");

        cam.printDebug("");
        cam.printfDebug("Version: %d", res.version);
        cam.printfDebug("ECC level: %c", res.eccLevel);
        cam.printfDebug("Mask: %d", res.mask);
        cam.printDebug("Data type: " + qr.dataType(res.dataType));
        cam.printfDebug("Length: %d", res.length);
        cam.printDebug("Payload: " + res.payload);
        qrData = res.payload;
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

    http.addHeader("Content-Type", "application/json");

    JSONVar myObject = JSON.parse(qrData);
    int httpResponseCode = http.POST(qrData);

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
    send_qr_data_to_server(qrData);
  }

  delay(3000);
}
