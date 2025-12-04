#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

// --- ESP-NOW データ構造体定義 ---

// 受信するデータの構造体です
struct ReceivedDataPacket {
  int slideVal1; int slideVal2;       // スライダー1, 2の値 (0-255)
  int sld_sw1_1; int sld_sw1_2;     // スライドスイッチ1の状態
  int sld_sw2_1; int sld_sw2_2;     // スライドスイッチ2の状態
  int sld_sw3_1; int sld_sw3_2;     // スライドスイッチ3の状態
  int sld_sw4_1; int sld_sw4_2;     // スライドスイッチ4の状態
  int sw1; int sw2; int sw3; int sw4; // ボタンスイッチ1-4の状態
  int sw5; int sw6; int sw7; int sw8; // ボタンスイッチ5-8の状態
};

// 送信するデータの構造体です (必要に応じて変更してください)
struct SaneDataPacket {
  int val1; int val2; int val3; int val4; int val5; // 送信データ例
};

#endif // DATA_STRUCTURES_H
