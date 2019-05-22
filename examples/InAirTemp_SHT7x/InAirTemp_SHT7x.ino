/////////////////////////////////////////
//UARDECS Sample Program for SHT7x/SHT1x
//Ver1.2
//By H.kurosaki 2015/12/10
//////////////////////////////////////////
//[概要]
//温湿度センサSHT75/SHT71/SHT15/SHT11から温湿度を読み出してCCMに出力します
//さらに、飽差を計算して出力します
//CCMは毎秒出力されますが値の更新は5秒に1回です
//[使い方]
//想定する動作環境はAruduinoUNO + Ethernet Shield2を標準としています
//MEGAでも動きますが、ピン番号の違いに注意して下さい
//SHT71とSHT75の違いは精度のみです
//SHT1xにはコンデンサを追加することでSHT7xと同じように使用できます
//○注意
//センサが接続されていないと異常な値が出力されます
//○ArduinoIDE
//Ver1.7.2以降を使用します
//○使用するライブラリ1
//Arduino用SHTシリーズ用ライブラリ
//http://playground.arduino.cc/Code/Sensirion
//librariesの下にSensirionというフォルダを作って上記Sensirion.hとSensirion.cppを置いて下さい
//上記サイトで配布されているものは旧IDE向けでVer1.x.x以降ではそのままコンパイルできません
//Sensirion.cppの先頭部分に以下の様な記述があります
/*
[書き換え前]
extern "C" {
  // AVR LibC Includes
  #include <stddef.h>
  #include <stdint.h>
  #include <math.h>

  // Wiring Core Includes
  #include "WConstants.h"
}

#include "Sensirion.h"
[ここまで]
*/
//これを以下のように書き換えます
/*
[書き換え後]
extern "C" {
  // AVR LibC Includes
  #include <stddef.h>
  #include <stdint.h>
  #include <math.h>
}
#include "Sensirion.h"
// Wiring Core Includes
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
[ここまで]
*/
//[参考]
//書き換え済みのライブラリが以下のページで配布されています(2015.11現在)
//http://arms22.blog91.fc2.com/?tag=SHT11
//
//○使用するライブラリ2
//UARDECS
//http://uecs.org/arduino/uardecs.html
//説明書に従ってインストールしてください
//
//○SHT75/SHT71の接続
//SHT7xにはコネクタ 廣杉計器 FRR-71041-04が適合します
//SHT7xピン配置
//チップ側を上にして
//┌――┐
//｜■■｜
//｜■■｜
//｜■■｜
//└┐┌┘
//　｜｜
//┌┘└┐
//｜｜｜｜
//｜｜｜｜
//｜｜｜｜
//①②③④
//
//センサ→Arduino
//①:クロック→D6へ
//②:電源→5Vへ
//③:GNDへ
//④:データ→D5へ
//
//①,④をArduinoの他のピンに接続する場合、DIO_SHT75DATA,DIO_SHT75CLKの値を書き換えて下さい
//
//○その他
//D3ピンがIPアドレスのリセット用に使用されています(変更可)
//

#include <SPI.h>
#include <Ethernet.h> //Ver1.7.2以降でW5100搭載機種
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Sensirion.h>
#include <Uardecs.h>


////////////////////////////SHT7x
//SHT7xのシリアルデータピンの番号を指定して下さい
#define DIO_SHT75DATA 5
#define DIO_SHT75CLK  6

Sensirion sht = Sensirion(DIO_SHT75DATA, DIO_SHT75CLK);
/////////////////////////////////////
//IP reset jupmer pin setting
/////////////////////////////////////
//Pin ID. This pin is pull-upped automatically.
const byte U_InitPin = 3;
const byte U_InitPin_Sense=HIGH;  
////////////////////////////////////
//Node basic infomation
///////////////////////////////////
const char U_name[] PROGMEM= "UARDECS Node v.1.0";
const char U_vender[] PROGMEM= "XXXXXX Co.";
const char U_uecsid[] PROGMEM= "000000000000";
const char U_footnote[] PROGMEM= "Test node";
char U_nodename[20] = "Sample";
UECSOriginalAttribute U_orgAttribute;
//////////////////////////////////
// html page1 setting
//////////////////////////////////
const int U_HtmlLine = 0; //Total number of HTML table rows.
struct UECSUserHtml U_html[U_HtmlLine]={
};

//////////////////////////////////
// UserCCM setting
//////////////////////////////////
//define CCMID for identify
//CCMID_dummy must put on last
enum {
CCMID_InAirTemp,
CCMID_InAirHumid,
CCMID_InAirHumidDef,
CCMID_cnd,
CCMID_dummy,
};

const int U_MAX_CCM = CCMID_dummy;
UECSCCM U_ccmList[U_MAX_CCM];

const char ccmNameTemp[] PROGMEM= "Temperature";
const char ccmTypeTemp[] PROGMEM= "InAirTemp.mIC";
const char ccmUnitTemp[] PROGMEM= "C";

const char ccmNameHumid[] PROGMEM= "AirHumid";
const char ccmTypeHumid[] PROGMEM= "InAirHumid.mIC";
const char ccmUnitHumid[] PROGMEM= "%";

const char ccmNameHumidDef[] PROGMEM= "HumidDef";
const char ccmTypeHumidDef[] PROGMEM= "InAirHumidDef.mIC";
const char ccmUnitHumidDef[] PROGMEM= "g m3";

const char ccmNameCnd[] PROGMEM= "NodeCondition";
const char ccmTypeCnd[] PROGMEM= "cnd.mIC";
const char ccmUnitCnd[] PROGMEM= "";

void UserInit(){
//必ずシールドに貼ってあるアドレスに書き換えて使用して下さい
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x03;

//Set ccm list
UECSsetCCM(true, CCMID_InAirTemp, ccmNameTemp, ccmTypeTemp, ccmUnitTemp, 29, 1, A_1S_0);
UECSsetCCM(true, CCMID_InAirHumid, ccmNameHumid, ccmTypeHumid,ccmUnitHumid, 29, 1, A_1S_0);
UECSsetCCM(true, CCMID_InAirHumidDef, ccmNameHumidDef, ccmTypeHumidDef,ccmUnitHumidDef, 29, 2, A_1S_0);
UECSsetCCM(true,  CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29, 0, A_10S_0);
}
void OnWebFormRecieved(){
}
void UserEverySecond(){
  readSHT7x();
}
void UserEveryMinute(){
}
void UserEveryLoop(){
}
void loop(){
UECSloop();
}
void setup(){
UECSsetup();
  U_ccmList[CCMID_cnd].value=0;
}

void readSHT7x(){

    static char count=0;
    count++;
    count%=5;

      //SHT-7x読み出し(自己発熱の影響を抑えるために5秒間隔で計測する)
      if(sht.measRdy()&& count==0)
      {

      //仕様上U_ccmList[*].valueには整数値しか格納できません
      //Set ccm listで小数点に1を指定した場合、1の位が少数第1位として扱われます
      //temperature、humidityは浮動小数点型なので10倍して整数値に変換することで対応します

      uint16_t rawData;
      sht.measTemp(&rawData);                // sht.meas(TEMP, &rawData, BLOCK)
      float temperature=sht.calcTemp(rawData);
      U_ccmList[CCMID_InAirTemp].value =(long)(temperature*10);
      sht.measHumi(&rawData);                // sht.meas(HUMI, &rawData, BLOCK)
      float humidity=sht.calcHumi(rawData, temperature);
      U_ccmList[CCMID_InAirHumid].value =(long)(humidity*10);

      //飽差計算
      float humidpress=6.1078*pow(10,(7.5*temperature/(temperature+237.3)));
      float humidvol=217*humidpress/(temperature+273.15);
      //float humiddef=(100-humidity)*humidvol/100;//飽差
      U_ccmList[CCMID_InAirHumidDef].value=(100-humidity)*humidvol;//少数が下位2桁なのでそのまま出力
      }
}


