/////////////////////////////////////////
//UARDECS Sample Program "LCD_AQM1602"
//I2C connection test with AQM1602 Ver1.1
//By H.kurosaki 2018/05/30
//////////////////////////////////////////

//秋月電子の小型液晶AQM1602シリーズの表示テストです。
//受信したCCMの値を液晶に表示し、タイムアウトするとDisconnectと表示します。
//ピッチ変換基板の利用を推奨します。

//結線方法 以下の４ピンを使用します
//Arduino側-AQM1602側
//[SCL]-[SCL]
//[SDA]-[SDA]
//[5V]-[+V]
//[GND]-[GND]

//注意点
//AQM1602のI2Cバスは10kΩ以上の抵抗でプルアップする必要があります
//Arduinoの標準プルアップ抵抗値は30kΩなので問題ないですが、
//プルアップ抵抗値が小さすぎる場合、正常に動作しないことがあります

#include <SPI.h>
#include <Ethernet2.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Uardecs.h>
#include <Wire.h>
//スケッチ→ライブラリをインクルード→ライブラリを管理から
//AQM1602を検索して出てくるFabo213 LCD...のライブラリをインストールすること
#include "FaBoLCDmini_AQM0802A.h"

FaBoLCDmini_AQM0802A lcd;

const byte U_InitPin = 3;//このピンは変更可能です
const byte U_InitPin_Sense=HIGH;
const char U_name[] PROGMEM= "UARDECS Node v.1.0";
const char U_vender[] PROGMEM= "XXXXXX Co.";
const char U_uecsid[] PROGMEM= "000000000000";
const char U_footnote[] PROGMEM= "Test node";
char U_nodename[20] = "Sample";
UECSOriginalAttribute U_orgAttribute;
const int U_HtmlLine = 0;
struct UECSUserHtml U_html[U_HtmlLine]={};

//////////////////////////////////
// UserCCM setting
// CCM用の素材
//////////////////////////////////

enum {
CCMID_cnd,
CCMID_Rcv,
CCMID_dummy, //CCMID_dummyは必ず最後に置くこと
};


const int U_MAX_CCM = CCMID_dummy;//CCMの総数を2に
UECSCCM U_ccmList[U_MAX_CCM];
//CCM定義用の素材1、被らないように適当な変数名で3つ宣言
const char ccmInfoCnd[] PROGMEM= "NodeCondition";//CCMの説明(Webでのみ表示)
const char ccmTypeCnd[] PROGMEM= "cnd.xXX";//CCMのType文字列
const char ccmUnitCnd[] PROGMEM= "";//CCMの単位(この場合単位無し)
//CCM定義用の素材2、被らないように適当な変数名で3つ宣言
const char ccmInfoRcv[] PROGMEM= "Receive CCM";//CCMの説明(Webでのみ表示)
const char ccmTypeRcv[] PROGMEM= "Receive.xXX";//CCMのType文字列
const char ccmUnitRcv[] PROGMEM= "";//CCMの単位(この場合単位無し)



void UserInit(){
//MAC address is printed on sticker of Ethernet Shield.
//You must assign unique MAC address to each nodes.
//MACアドレス設定、必ずEthernet Shieldに書かれた値を入力して下さい。
//全てのノードに異なるMACアドレスを設定する必要があります。
U_orgAttribute.mac[0] = 0x12;
U_orgAttribute.mac[1] = 0x34;
U_orgAttribute.mac[2] = 0x56;
U_orgAttribute.mac[3] = 0x78;
U_orgAttribute.mac[4] = 0x9A;
U_orgAttribute.mac[5] = 0xBC;



//UECSsetCCM(送受信の区分,通し番号,CCM説明,Type,単位,priority,少数桁数,送信頻度設定[A_1S_0で1秒間隔])
UECSsetCCM(true,CCMID_cnd,ccmInfoCnd,ccmTypeCnd,ccmUnitCnd,29,0,A_1S_0);//送信CCM
UECSsetCCM(false,CCMID_Rcv,ccmInfoRcv,ccmTypeRcv,ccmUnitRcv,29,0,S_1S_0);//受信CCM
}
void OnWebFormRecieved(){}
void UserEverySecond()
  {
  lcd.clear();

  lcd.print("Receive Value:");
  lcd.command(0x40+0x80);//改行
  if(U_ccmList[CCMID_Rcv].validity)
    {
      lcd.print(U_ccmList[CCMID_Rcv].value);
    }
  else
    {
      lcd.print("Disconnect");
     }
  }
void UserEveryMinute(){}
void UserEveryLoop(){}
void loop()
{
UECSloop();
}
void setup()
{
lcd.begin();
lcd.init();
//for I2C voltage 5V
lcd.command(0x38);delay(1);
lcd.command(0x39);delay(1);
lcd.command(0x14);delay(1);
lcd.command(0x73);delay(1);
lcd.command(0x51);delay(2);
lcd.command(0x6c);delay(300);
lcd.command(0x38);delay(1);
lcd.command(0x01);delay(2);
lcd.command(0x0c);

/*
//for I2C voltage 3.3V
    lcd.command(0x38); delay(1);
    lcd.command(0x39); delay(1);
    lcd.command(0x14); delay(1);
    lcd.command(0x73); delay(1);
    lcd.command(0x56); delay(2);
    lcd.command(0x6C); delay(300);
    lcd.command(0x38); delay(1);
    lcd.command(0x01); delay(2);
    lcd.command(0x0C); delay(2);
*/
UECSsetup();
}

