#include <SoftwareSerial.h>
#include <IRremote.h>

//SoftwareSerial BT(2, 3); // 接收腳, 傳送腳
char val;  // 儲存接收資料的變數
String cmd;
const int IR_rec_pin = 2;             // IR 接收器輸出腳位
int IRstate = LOW;                    // IR 接收器輸出腳位狀態
int IRstate_last = LOW;               // IR 接收器輸出腳位狀態(上一次)
unsigned long int time_last = 0;      // 上一次 IRstate 變化的時間

boolean isIdle = true;                // 是否在等待 IR 訊號模式Idle）

const long int durationMax = 5000;   // 一段時間沒變化就進入 Idle，單位 us
const long int durationMin = 300;     // 電壓狀態不變的最小持續時間，單位 us

unsigned int raw_on[300];
unsigned int raw_off[300];
int k = 0;

IRsend irsend;

void IR_rec_Check(int raw[300]);//接收紅外線資訊
void IR_send(int irSignal[300]);//發射紅外線資訊

void setup() {
  Serial.begin(115200);   // 與電腦序列埠連線 
  Serial3.begin(9600);// 設定HM-10藍牙模組，AT命令模式的連線速率。
  pinMode( IR_rec_pin, INPUT );        // 設定針腳 I/O 模式
  IRstate = digitalRead( IR_rec_pin ); // 取得腳位元狀態初始值
  IRstate_last = IRstate;

  Serial.println("Please set up first");
}



void loop() 
{
  // 若收到「序列埠監控視窗」的資料，
  if (Serial.available()) {
    cmd = Serial.readString();
    cmd.trim();
  }
 
  // 若收到藍牙模組的資料，
  if (Serial3.available()) 
  {
    cmd = Serial3.readString();
    cmd.trim();
    Serial.println(cmd);
    delay(300);
  }

  k = 0;
  unsigned long int time_now_set;
  unsigned long int dT_set = 0;

  if(cmd=="set_on") {
    Serial.println("");
    Serial.println("waiting");
    for(int i=0 ; i<300 ; i++){
      raw_on[i] = 0;
    }
    time_now_set = millis();
    while(dT_set<5000){
      dT_set = millis()-time_now_set;
      IR_rec_Check(raw_on);
      delayMicroseconds( 5 );
    }
    Serial.println("stop waiting");
    for(int i=0 ; i<300 ; i++){
      Serial.print(raw_on[i]);
      Serial.print(",");
    }
    cmd = "";
  }
  else if(cmd=="set_off") {
    Serial.println("waiting");
    for(int i=0 ; i<300 ; i++){
      raw_off[i] = 0;
    }
    time_now_set = millis();
    while(dT_set<5000){
      dT_set = millis()-time_now_set;
      IR_rec_Check(raw_off);
      delayMicroseconds( 5 );
    }
    Serial.println("stop waiting");
    cmd = "";
  }
  else if(cmd=="on") {
    IR_send(raw_on);
    for(int i=0 ; i<300 ; i++){
      Serial.print(raw_on[i]);
      Serial.print(",");
    }
    Serial.println(",");
    cmd = "";
  }
  else if(cmd=="off") {
    IR_send(raw_off);
    cmd = "";
  }

}

void IR_rec_Check(int raw[300]){
  
  IRstate = digitalRead( IR_rec_pin );// 讀取腳位狀態

  if( IRstate != IRstate_last ){      //

    long int timeNow = micros( );      // 記錄目前時間
    long int dT = timeNow - time_last; // 與上一次腳位變化的時間差

      if( (dT >= durationMax) && !isIdle ){     // 時間間隔大於設定的時間，且原本的狀態為接收中狀態
        isIdle = true; //進入等待狀態
        Serial.println( "Idling...\n" );
      }
      else if( (dT < durationMax) && (dT > durationMin) ){
        isIdle = false; //進入接收中狀態
          if(IRstate == HIGH){ 
            Serial.print(dT);
            raw[k] = abs(dT);
            k++;
          }
          else {
            Serial.print( 0-dT );
            raw[k] = abs(0-dT);
            k++;
          }
        Serial.print(" ");
      }
 // 記錄此次時間
  time_last = timeNow;
  }
 // 記錄此次狀態
 IRstate_last = IRstate;
}//end IR_rec_Check

void IR_send(unsigned int irSignal[300]){
  Serial.println("sending");
  int khz = 38;
  irsend.sendRaw(irSignal, 300, khz);
  delay(1000);
}
