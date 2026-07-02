# UARDECS
UARDECS is a library for implementing a protocol based on "UECS", a communication standard for Japanese horticulture. Compatible with Arduino UNO or Arduino Mega (dedicated to AVR based Arduino). To run this library you need a **W5500**-based Ethernet shield, such as the Arduino Ethernet Shield 2. The library depends on the **Ethernet2** library. When you install UARDECS through the Arduino Library Manager (Arduino IDE 1.8.10 or newer), the Library Manager will offer to install Ethernet2 automatically. If you install the library manually, please install the Ethernet2 library as well.

> **Note:** As of v2.0.3, support for the older W5100-based shields (and the standard `Ethernet.h` library) has been discontinued. Please use a W5500-based shield with the Ethernet2 library.

UARDECSは日本の施設園芸用の通信規格である"UECS"準拠のプロトコルをArduinoに実装するためのライブラリです。 Arduino UNOまたはArduino Megaに対応しています（AVR系Arduino専用）。 このライブラリを実行するには、Arduino Ethernet Shield 2 などの **W5500** チップを搭載したEthernetシールドが必要です。 本ライブラリは **Ethernet2** ライブラリに依存しています。 Arduino Library Manager（Arduino IDE 1.8.10以降）経由でUARDECSをインストールすると、Ethernet2も併せてインストールするよう自動的に案内されます。 手動でインストールする場合は、Ethernet2ライブラリも一緒にインストールして下さい。

> **注意:** v2.0.3より、旧来のW5100系シールド（および標準の `Ethernet.h` ライブラリ）のサポートを打ち切りました。W5500系のシールドとEthernet2ライブラリを使用して下さい。

詳細なマニュアル(日本語版)は以下で提供されます。
https://github.com/H-Kurosaki/UARDECS_Documents
