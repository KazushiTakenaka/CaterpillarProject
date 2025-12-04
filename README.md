# CaterpillarProject

ESP32を搭載したキャタピラー型ロボットの制御プロジェクトです。
ESP-NOWプロトコルを使用して、リモートコントローラーからの指示に基づき、ロボットの走行、ブザー、LEDを制御します。

## 主な機能

- **ESP-NOWによる無線操縦**: 低遅延のESP-NOWプロトコルを利用して、安定したリモート操作を実現します。
- **左右独立キャタピラ制御**: 2つのモーターを個別にPWM制御し、前進、後進、旋回など、自由な走行が可能です。
- **ブザー通知**: 特定の操作時にブザーを鳴らします。
- **ステータスLED**:
    - **青色LED**: 通信状態を示します。（ペアリング成功で点灯、通信ロスで明滅）
    - **白色LED**: バッテリーの低電圧を警告します。（電圧低下で点灯）

## ハードウェア構成

本プロジェクト（送信側）のESP32には、以下のコンポーネントが接続されていることを想定しています。

- ESP32
- 専用基板
- モータードライバ
- DCモーター x2
- ブザー
- LED x2 (白色、青色、その他)
- バッテリー (および電圧測定用の分圧回路)

## ピン配置

主要なピンの割り当ては以下の通りです。詳細は `PIN_DEFINITIONS.md` および `src/PinConfig.h` を参照してください。

| ピン番号 | 定数名      | 役割                   |
| :------- | :---------- | :--------------------- |
| GPIO 25  | `IN1`       | モーター1 制御         |
| GPIO 26  | `IN2`       | モーター1 制御         |
| GPIO 16  | `IN3`       | モーター2 制御         |
| GPIO 15  | `IN4`       | モーター2 制御         |
| GPIO 27  | `BUZZER`    | ブザー 制御            |
| GPIO 35  | `BATTERY`   | バッテリー電圧測定     |
| GPIO 17  | `WHITE_LED` | 白色LED 制御           |
| GPIO 18  | `BLUE_LED`  | 青色LED 制御           |

## ソフトウェア構造

- `main.cpp`: メインの処理ループ。ESP-NOWで受信したデータに基づき、`Caterpillar`クラスの各機能を呼び出します。
- `Caterpillar.h/.cpp`: モーター、ブザー、LEDの物理的な制御（PWM出力など）を行うクラスです。
- `ESPNowManager.h/.cpp`: ESP-NOWの初期化とペアリング処理を管理するクラスです。
- `DataStructures.h`: ESP-NOWで送受信するデータパケットの構造を定義します。
- `PinConfig.h`: プロジェクトで使用するGPIOピンとLEDCチャンネルを定義します。
- `Secret.h`: 通信相手（受信側）のMACアドレスを定義するためのファイルです。（**手動で作成・設定が必要**）
- `platformio.ini`: PlatformIOのプロジェクト設定ファイルです。

## セットアップとビルド

### 1. 開発環境
本プロジェクトは [PlatformIO](https://platformio.org/) を使用して開発されています。Visual Studio CodeにPlatformIO IDE拡張機能をインストールしてください。

### 2. MACアドレスの設定
本機が通信する相手（受信側/リモコン）のMACアドレスを設定する必要があります。

1. `src` ディレクトリに `Secret.h` という名前の新しいファイルを作成します。
2. 以下の内容を `Secret.h` にコピーし、`XX:XX:XX:XX:XX:XX` の部分を実際の受信側のMACアドレスに書き換えてください。

```cpp
#ifndef SECRET_H
#define SECRET_H

// 受信側ESP32のMACアドレスに書き換えてください
#define MAC_ADDRESS "XX:XX:XX:XX:XX:XX"

// --- 以下は自動変換用のコードなので変更しないでください ---
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static uint8_t MAC_ADDRESS_BYTE[6];

static void mac_str_to_uint8(const char* mac_str, uint8_t* mac_byte) {
    sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac_byte[0], &mac_byte[1], &mac_byte[2],
           &mac_byte[3], &mac_byte[4], &mac_byte[5]);
}

// MACアドレス文字列をバイト配列に変換する処理を一度だけ行う
class MacAddressInitializer {
public:
    MacAddressInitializer() {
        mac_str_to_uint8(MAC_ADDRESS, MAC_ADDRESS_BYTE);
    }
};
static MacAddressInitializer macInitializer;

#endif // SECRET_H
```

### 3. ビルドとアップロード
PlatformIOのインターフェースから `Build` と `Upload` を実行してください。

## 操作方法

本機は受信側（リモコン）から送信される以下のデータに基づいて動作します。

- **スライダー1 (`slideVal1`)**: モーター1（例: 右キャタピラ）の速度と方向を制御します。
- **スライダー2 (`slideVal2`)**: モーター2（例: 左キャタピラ）の速度と方向を制御します。
- **スイッチ1 (`sw1`)**: ブザーを鳴らします。

同時に、本機は自身のバッテリー電圧を受信側に送信します。
