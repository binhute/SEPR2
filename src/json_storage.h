#ifndef _JSON_STORAGE_H_
#define _JSON_STORAGE_H_

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>
using namespace std;

// enum Ftype {
//     INT, BOOL, FLOAT, STRING
// };

class JsonStorage {
private:
    bool debugEnable;
    String path;
    JsonDocument doc;
public:
    JsonStorage(const String &path, bool debugEnable = true);
    bool field(const String &key, int value);
    bool field(const String &key, const String &value);
    bool field(const String &key, float value);
    bool field(const String &key, bool value);
    bool nestField(const String &mainKey, const String &obj);
    JsonObject getNode(const String &key);
    String getNodeToString(const String &key);
    String list();
    void clear();
    bool save();
};

#endif