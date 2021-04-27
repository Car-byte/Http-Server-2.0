#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include "server_settings.h"



 ServerSettings::ServerSettings() {

    std::ifstream config_file;
    config_file.open("httpserver.conf");
    if (config_file.fail()) {
        CreateDefaultConfigFile();
        SetDefaultValues();
    }
    else {
        bool error_loading = LoadConfigSettings(config_file);

        //if the settings are malformed or something else
        if (error_loading) {
            std::cout << "Error loading config data, config file could be malformed or corrupt. Loading default settings.\n";
            SetDefaultValues();
        }
    }
}



void ServerSettings::CreateDefaultConfigFile() {
    std::ofstream new_config_file;
    new_config_file.open("httpserver.conf");
    if (new_config_file.fail()) {
        std::cout << "Error creating an http config file\n";
        exit(EXIT_FAILURE);
    }

    new_config_file << "Keep Alive Timer: 1s\n";
    new_config_file << "Max File Serve Size: 20MB\n";
}



void ServerSettings::SetDefaultValues() {
    m_keep_alive_time = 1 * 1000;
    m_max_file_serve_size = 20 * 1000000;
}



bool ServerSettings::LoadConfigSettings(std::ifstream& config_file) {

    std::string buffer;
    getline(config_file, buffer);
    if (config_file.eof()) {
        return false;
    }

    bool success = ParseKeepAliveTimerAndSet(buffer);

    if (!success) {
        return false;
    }

    getline(config_file, buffer);
    if (config_file.eof()) {
        return false;
    }

    success = ParseMaxFileServeSizeAndSet(buffer);

    return success;
}



bool ServerSettings::ParseKeepAliveTimerAndSet(std::string& buffer) {

    int index = buffer.find("Keep Alive Timer: ");    
    if (index == std::string::npos) {
        return false;
    }

    index += 18;
    std::string time_alive;
    std::string time_units;
    bool on_units = false;
    for (int i = index; i < buffer.size() && buffer[i] != ' '; ++i) {

        if (!isdigit(buffer[i])) {
            on_units = true;
        }

        if (on_units) {
            time_units += buffer[i];
        }
        else {
            time_alive += buffer[i];
        }
    }

    if (time_alive.empty() || time_units.empty()) {
        return false;
    }

    int non_unit_time;

    try {
        non_unit_time = std::stoi(time_alive);
        m_keep_alive_time = ParseTimeUnitsAndConvertToMilliseconds(time_units, non_unit_time);
    }
    catch (std::exception ex) {
        return false;
    }
}



bool ServerSettings::ParseMaxFileServeSizeAndSet(std::string& buffer) {

    int index = buffer.find("Max File Serve Size: ");
    if (index == std::string::npos) {
        return false;
    }

    //size of the search term
    index += 21;

    std::string units;
    std::string size_without_units;
    bool on_units = false;
    for (int i = index; i < buffer.size() && buffer[i] != ' '; ++i) {

        if (!isdigit(buffer[i])) {
            on_units = true;
        }

        if (on_units) {
            units += buffer[i];
        }
        else {
            size_without_units += buffer[i];
        }
    }

    if (size_without_units.empty() || units.empty()) {
        return false;
    }

    try {
        int size_no_units = std::stoi(size_without_units);
        m_max_file_serve_size = ParseBytesUnitesAndConvertToBytes(units, size_no_units);
    }
    catch (std::exception ex) {
        return false;
    }
}



int ServerSettings::ParseBytesUnitesAndConvertToBytes(std::string& units, int unknown_unit_bytes) {

    for (int i = 0; i < units.size(); ++i) {
        units[i] = std::tolower(units[i]);
    }

    if (units == "bytes" || units == "byte") {
        return unknown_unit_bytes;
    }
    else if (units == "kilobytes" || units == "kilobyte" || units == "kb") {
        return unknown_unit_bytes * 1000;
    }
    else if (units == "megabytes" || units == "megabyte" || units == "mb") {
        return unknown_unit_bytes * 1000000;
    }
    else if (units == "gigabytes" || units == "gigabyte" || units == "gb") {
        return unknown_unit_bytes * 1000000000;
    }

    throw std::exception("No valid units for size of header file");
}




int ServerSettings::ParseTimeUnitsAndConvertToMilliseconds(std::string& units, int unknown_unit_time) {

    for (int i = 0; i < units.size(); ++i) {
        units[i] = std::tolower(units[i]);
    }

    if (units == "s" || units == "second" || units == "seconds") {
        return unknown_unit_time * 1000;
    }
    else if (units == "ms" || units == "milliseconds") {
        return unknown_unit_time;
    }

    throw std::exception("No valid units of time");
}

