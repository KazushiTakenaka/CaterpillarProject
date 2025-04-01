#include "ESPNowManager.h"

/**
 * @brief ESPNowManagerクラスのコンストラクタです。
 * isPairedフラグをfalseで初期化します。
 */
ESPNowManager::ESPNowManager() : isPaired(false) {}

/**
 * @brief ESP-NOWを初期化します。
 * @return bool 初期化が成功した場合はtrue、失敗した場合はfalseを返します。
 */
bool ESPNowManager::init() {
    // ESP-NOWを初期化します
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed");
        return false; // 初期化失敗
    } else {
        Serial.println("ESP-NOW initialization successful");
        delay(500); // 初期化後に少し待機します (安定化のため)
        return true; // 初期化成功
    }
}

/**
 * @brief 指定されたMACアドレスのデバイスとペアリングします。
 * @param mac_addr [in] ペアリングする相手のMACアドレス (6バイト配列)。
 * @param channel [in] 使用するWi-Fiチャンネル (デフォルト: 0)。autoChannelがtrueの場合は無視されます。
 * @param autoChannel [in] trueの場合、現在のWi-Fiチャンネルを自動で使用します (デフォルト: true)。
 * @return bool ペアリングが成功した場合はtrue、失敗した場合はfalseを返します。
 * @note ペアリング失敗時は最大15回リトライします (約7.5秒)。
 */
bool ESPNowManager::pairDevice(const uint8_t *mac_addr, int channel, bool autoChannel) {
    esp_now_peer_info_t peerInfo = {}; // ピア情報を格納する構造体です
    memcpy(peerInfo.peer_addr, mac_addr, 6); // MACアドレスをコピーします
    // チャンネルを設定します (自動設定または手動設定)
    peerInfo.channel = autoChannel ? WiFi.channel() : channel;
    peerInfo.encrypt = false; // 暗号化は使用しません

    // ペアリングを試みます (最大15回リトライ)
    int retryCount = 0; // 再試行回数をカウントします
    esp_err_t addStatus;
    while ((addStatus = esp_now_add_peer(&peerInfo)) != ESP_OK && retryCount < 15) {
        Serial.printf("Pairing failed (Error: %d). Retrying...\n", addStatus);
        delay(500); // 再試行前に500ms待機します
        retryCount++;
    }

    // ペアリング結果を確認します
    if (addStatus != ESP_OK) {
        Serial.println("Pairing failed after multiple retries.");
        Serial.println("Could not connect.");
        isPaired = false; // ペアリング失敗フラグを設定します
    } else {
        Serial.println("Pairing successful");
        isPaired = true; // ペアリング成功フラグを設定します
    }

    return isPaired; // ペアリング結果を返します
}
