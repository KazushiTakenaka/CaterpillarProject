#include <Arduino.h>
#include "Secret.h"

/**
 * @brief MACアドレスなどの秘密情報を定義するソースファイルです。
 * Secret.hで宣言された変数の実体をここで定義します。
 * このファイルはダミーファイルです。使用するにはファイルをsrcに移動してください。
 */

// --- 通信相手のMACアドレス定義 ---
// ここに通信したい相手のESP32のMACアドレスを設定してください。
// 例:
// A0:A0:65:52:D3:94
/**
 * @brief 通信相手のMACアドレス (文字列形式) です。
 */
const char* MAC_ADDRESS_STR = "00:00:00:00:00:00";

/**
 * @brief 通信相手のMACアドレス (バイト配列形式) です。
 * ESP-NOWのペアリングや送信先指定に使用します。
 */
const uint8_t MAC_ADDRESS_BYTE[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};