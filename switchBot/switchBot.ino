#include <SoftwareSerial.h>
#include <Servo.h>

Servo myservo; // 建立Servo物件，控制伺服馬達
SoftwareSerial BT(A2, A3); // A2->藍芽TX,A3->藍芽RX
char val;  // 儲存接收資料的變數
String cmd;
int state = 0;

void setup() {
  Serial.begin(9600);   // 與電腦序列埠連線
//  Serial1.begin(9600);
  //Serial.println("BT is ready!");
  myservo.attach(3, 500, 2400); // 修正脈衝寬度範圍
  myservo.write(90); // 一開始先置中90度
  delay(3000);
 
  // 設定HC-05藍牙模組，AT命令模式的連線速率。
  BT.begin(9600);
}
 
void loop() {
  // 若收到「序列埠監控視窗」的資料，則送到藍牙模組
  if (Serial.available()) {
    val = Serial.read();
//    Serial1.print(val);
  }
 
  // 若收到藍牙模組的資料，則送到「序列埠監控視窗」
  if (BT.available()) {
    delay(50);
    cmd = BT.readString();
    cmd.trim();
//    Serial.println(cmd);
    if(cmd=="on"){
      myservo.writeMicroseconds(1000);
      state = 1;
    }
    if(cmd=="off"){
      myservo.writeMicroseconds(1900);
      state = 0;
    }
    if(cmd == "trig"){
      if(state == 0){
        myservo.writeMicroseconds(1000);
        state = 1;
      }
      else {
        myservo.writeMicroseconds(1900);
        state = 0;
      }
    }
    BT.print("OK");
//    delay(300);
  }
}
