#ifndef SERVER_SETTINGS_H
#define SERVER_SETTINGS_H

#include <iostream>
#include <string>
#include <fstream>

class ServerSettings {
public:
    ServerSettings();

    int KeepAliveTime();
    int MaxFileServeSize();

private:
    void CreateDefaultConfigFile();
    void SetDefaultValues();
    bool LoadConfigSettings(std::ifstream& config_file);

    bool ParseKeepAliveTimerAndSet(std::string& buffer);
    bool ParseMaxFileServeSizeAndSet(std::string& buffer);

    int ParseTimeUnitsAndConvertToMilliseconds(std::string& units, int unknown_unit_time);
    int ParseBytesUnitesAndConvertToBytes(std::string& units, int unknown_unit_bytes);


    int m_max_file_serve_size;
    int m_keep_alive_time;
};

#endif  //!SERVER_SETTINGS_H