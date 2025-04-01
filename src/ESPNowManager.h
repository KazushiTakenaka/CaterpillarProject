#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

/**
 * @brief ESP-NOW通信の初期化とペアリングを管理するクラスです。
 */
class ESPNowManager {
public:
    /**
     * @brief ESPNowManagerクラスのコンストラクタです。
     * isPairedフラグをfalseで初期化します。
     */
    ESPNowManager();

    /**
     * @brief ESP-NOWを初期化します。
     * @return bool 初期化が成功した場合はtrue、失敗した場合はfalseを返します。
     */
    bool init();

    /**
     * @brief 指定されたMACアドレスのデバイスとペアリングします。
     * @param mac_addr [in] ペアリングする相手のMACアドレス (6バイト配列)。
     * @param channel [in] 使用するWi-Fiチャンネル (デフォルト: 0)。autoChannelがtrueの場合は無視されます。
     * @param autoChannel [in] trueの場合、現在のWi-Fiチャンネルを自動で使用します (デフォルト: true)。
     * @return bool ペアリングが成功した場合はtrue、失敗した場合はfalseを返します。
     * @note ペアリング失敗時は最大15回リトライします。
     */
    bool pairDevice(const uint8_t *mac_addr, int channel = 0, bool autoChannel = true);

    /**
     * @brief ペアリングが成功しているかどうかを示すフラグです。
     * pairDevice()の成功/失敗に応じて更新されます。
     */
    bool isPaired;

private:
    /**
     * @brief 送信側ESP32-AのMACアドレスを保存します (現在は未使用)。
     * @note この変数は現在の実装では使われていません。
     */
    uint8_t sender_mac[6];
};

#endif // ESPNOWMANAGER_H
