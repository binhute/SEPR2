#include "json_storage.h"

JsonStorage::JsonStorage(const String &path, bool debugEnable) {
    this->debugEnable = debugEnable;
    this->path = path;
    LittleFS.begin(true);
    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        deserializeJson(doc, file);
        file.close();
        return;
    }
    return;
}

bool JsonStorage::field(const String &key, int value) {
    doc[key] = value;
    return true;
}

bool JsonStorage::field(const String &key, const String &value) {
    doc[key] = value;
    return true;
}

bool JsonStorage::field(const String &key, float value) {
    doc[key] = value;
    return true;
}

bool JsonStorage::field(const String &key, bool value) {
    doc[key] = value;
    return true;
}

JsonObject JsonStorage::getNode(const String &key) {
    if (!doc[key].isNull()) {
        return doc[key].as<JsonObject>();
    }
    JsonDocument emptyDoc;
    return emptyDoc.as<JsonObject>();
}

String JsonStorage::getNodeToString(const String &key) {
    if (!doc[key].isNull()) {
        String out;
        serializeJsonPretty(doc[key], out);
        return out;
    }
    return "";
}

String JsonStorage::list() {
    String out;
    serializeJsonPretty(doc, out);
    return out;
}

void JsonStorage::clear() {
    doc.clear();
    LittleFS.remove(path);
}

bool JsonStorage::save() {
    File file = LittleFS.open(path, "w");
    serializeJson(doc, file);
    return true;
}