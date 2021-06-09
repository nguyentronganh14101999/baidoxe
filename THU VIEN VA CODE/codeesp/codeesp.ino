#ifdef ESP32
#include <WiFi.h>
#define DEN 23
#define QUAT 22
#define NUTDEN 21
#define NUTQUAT 19
#else
#include <ESP8266WiFi.h>
#define DEN D0
#define QUAT D1
#define NUTDEN D2
#define NUTQUAT D3
#endif

#include <SocketIoClient.h>

extern bool connect;

#include <ArduinoJson.h>// 6.10.0


const char* ssid     = "Manchester UTD";
const char* password = "brunofernandes94";

// Server Ip
const char* server = "192.168.1.28";
// Server port
int port = 3000;

SocketIoClient socket;


String DataMqttJson = "";

int bienTB1 = 0;
int bienTB2 = 0;

long last1 = 0;

long gioduoiden = 0;
long phutduoiden = 0;
long giotrenden = 0;
long phuttrenden = 0;

long gioduoiquat = 0;
long phutduoiquat = 0;
long giotrenquat = 0;
long phuttrenquat = 0;
long giothuc = 0;
long phutthuc = 0;
int biengui = 0;

int HenGioDen = 0;
int HenGioQuat = 0;
int TB1 = 0;
int TB2 = 0;
long nhietdo = 0;
long doam = 0;
long C1 = 500;
long C2 = 1000;
long C3  = 4000;
String Data = "";
long last = 0;


void setupNetwork()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wifi connected!");
}

void handleMessage(const char* message , size_t length)
{
  last = millis();
  Serial.print("Data nhận được: ");
  Serial.println(message);
  Data = message;


  ParseJson(String(Data));
  

  Data = "";
  last = millis();
}
void setup()
{
  Serial.begin(9600);
  pinMode(DEN, OUTPUT);
  pinMode(QUAT, OUTPUT);
  
  digitalWrite(DEN, LOW);
  digitalWrite(QUAT, LOW);

  pinMode(NUTDEN, INPUT_PULLUP);
  pinMode(NUTQUAT, INPUT_PULLUP);

  
  setupNetwork();
  
  // kết nối server nodejs
  socket.begin(server, port);
  // lắng nghe sự kiện server gửi
  socket.on("ESP", handleMessage);
  last = millis();
  Serial.println("ESP Start");
}

void loop()
{
  DuytriServer();

  SendDataNodeJS();
  NutNhan();
  
}

void DuytriServer()
{
  socket.loop();
}


//socket.emit("JSON", JsonData.c_str());

void ParseJson(String Data)
{
  // đưa dữ liệu json vào thư viện json để kiểm tra đúng hay sai , đúng thì tách dữ liệu => điều khiển

  const size_t capacity = JSON_OBJECT_SIZE(2) + 256;
  DynamicJsonDocument JSON1(capacity);
  DeserializationError error1 = deserializeJson(JSON1, Data);
  if (error1)
  {
    Serial.println("Data JSON Error!!!");
    return;
  }
  else
  {
    Serial.println();
    Serial.println("Data JSON ESP: ");
    serializeJsonPretty(JSON1, Serial);

    if (JSON1["TB1"] == "0")
    {
      // OFF TB1

      digitalWrite(DEN, LOW);
      TB1 = 0;
      Serial.println("Den OFF!!!");

    }
    else if (JSON1["TB1"] == "1")
    {
      // ON TB 1
      digitalWrite(DEN, HIGH);
      TB1 = 1;
      Serial.println("Den ON!!!");
    }
    else if (JSON1["TB2"] == "0")
    {
      // OFF TB2
      digitalWrite(QUAT, LOW);
      TB2 = 0;
      Serial.println("QUAT OFF!!!");

    }
    else if (JSON1["TB2"] == "1")
    {
      // ON TB 2

      digitalWrite(QUAT, HIGH);
      TB2 = 1;
      Serial.println("QUAT ON!!!");

    }
    else if (JSON1.containsKey("C1") == 1)
    {
      String DataC1 = JSON1["C1"];
      C1 = DataC1.toInt();
      Serial.print("C1: ");
      Serial.println(C1);
    }
    else  if (JSON1.containsKey("C2") == 1)
    {
      String DataC2 = JSON1["C2"];
      C2 = DataC2.toInt();
      Serial.print("C2: ");
      Serial.println(C2);

    }
  }
}

void Datajson(String DataND, String DataDA, String DataTB1, String DataTB2, String DataC1 , String DataC2 , String DataC3 , String DataH1, String DataH2)
{
  DataMqttJson  = "{\"ND\":\"" + String(DataND) + "\"," +
                  "\"DA\":\"" + String(DataDA) + "\"," +
                  "\"TB1\":\"" + String(DataTB1) + "\"," +
                  "\"TB2\":\"" + String(DataTB2) + "\"," +
                  "\"C1\":\"" + String(DataC1) + "\"," +
                  "\"C2\":\"" + String(DataC2) + "\"," +
                  "\"C3\":\"" + String(DataC3) + "\"," +
                  "\"H1\":\"" + String(DataH1) + "\"," +
                  "\"H2\":\"" + String(DataH2) + "\"}";
  Serial.println();
  Serial.print("DataMqttJson: ");
  Serial.println(DataMqttJson);

  socket.emit("ESPJSON", DataMqttJson.c_str());

}

void SendDataNodeJS()
{
  if (millis() - last >= 1000)
  {
    if (connect)
    {
      Chuongtrinhcambien();
      Datajson(String(nhietdo), String(doam), String(TB1), String(TB2), String(C1) , String(C2) , String(C3) , String(HenGioDen), String(HenGioQuat));
    }
    last = millis();
  }
}


void NutNhan()
{


  if (digitalRead(NUTDEN) == LOW)
  {
    Serial.println("nút nhấn đèn đã được nhấn!!!");
    delay(300);

    while (1)
    {
      DuytriServer();
      if (digitalRead(NUTDEN) == HIGH)
      {
        // làm việc
        if (HenGioDen == 0)
        {
          DK_DEN1();
        }

        delay(300);
        // thoát vòng lặp vô hạn
        break;
      }
    }
  }



  if (digitalRead(NUTQUAT) == LOW)
  {
    Serial.println("nút nhấn QUẠT đã được nhấn!!!");
    delay(300);

    while (1)
    {
      DuytriServer();
      if (digitalRead(NUTQUAT) == HIGH)
      {
        // làm việc
        if (HenGioQuat == 0)
        {
          DK_QUAT1();
        }
        delay(300);

        break;
      }
    }
  }

}

void DK_DEN()
{
  Serial.println("Onclick đèn!!!");
  last = millis();

    DK_DEN1();

  last = millis();

}

void DK_QUAT()
{
  Serial.println("Onclick Quạt!!!");
  last = millis();
  
    DK_QUAT1();


  last = millis();

}

void DK_DEN1()
{

  if (TB1 == 0)
  {
    digitalWrite(DEN, HIGH);
    TB1 = 1;
    Serial.println("Đèn ON!!!");
  }
  else if (TB1 == 1)
  {
    digitalWrite(DEN, LOW);
    TB1 = 0;
    Serial.println("Đèn OFF!!!");
  }
  // TB1 gửi lên web để hiển thị đèn or tb ON hoặc OFF ( 0 1)


}

void DK_QUAT1()
{

  if (TB2 == 0)
  {
    digitalWrite(QUAT, HIGH);
    TB2 = 1;
    Serial.println("quạt ON!!!");
  }
  else if (TB2 == 1)
  {
    digitalWrite(QUAT, LOW);
    TB2 = 0;
    Serial.println("quạt OFF!!!");
  }

}

void Chuongtrinhcambien()
{

  nhietdo = nhietdo + 1;
  doam = doam + 2;

}
