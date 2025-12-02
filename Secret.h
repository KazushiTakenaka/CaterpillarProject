#ifndef SECRET_H
#define SECRET_H

#include <Arduino.h>

/**
 * @brief MACアドレスなどの秘密情報を定義するヘッダーファイルです。
 * このファイルで定義された変数は、Secret.cppで実体が定義されます。
 * このファイルはダミーファイルです。使用するにはファイルをsrcに移動してください。
 */

/**
 * @brief 通信相手のMACアドレスを文字列形式で定義します
 * @note この変数はSecret.cppで定義されています。
 */
extern const char* MAC_ADDRESS_STR;

/**
 * @brief 通信相手のMACアドレスをバイト配列形式で定義します (ESP-NOW用)。
 * @note この変数はSecret.cppで定義されています。
 */
extern const uint8_t MAC_ADDRESS_BYTE[6];

#endif // SECRET_H
