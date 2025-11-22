#pragma once

#include <string>
#include <ctime>
#include <cstdio>

static void writeNotification(const std::string& message) {
        const char* flagPath = "sdmc:/config/ultrahand/flags/NOTIFICATIONS.flag";

        // Check if flag file exists
        FILE* flagFile = fopen(flagPath, "r");
        if (!flagFile) {
            // Flag file does not exist, do nothing
            return;
        }
        fclose(flagFile);

        // Generate filename with timestamp
        std::string filename = "Horzon OC -" + std::to_string(std::time(nullptr)) + ".notify";
        std::string fullPath = "sdmc:/config/ultrahand/notifications/" + filename;

        // Write JSON manually
        FILE* file = fopen(fullPath.c_str(), "w");
        if (file) {
            fprintf(file, "{\n");
            fprintf(file, "  \"text\": \"%s\",\n", message.c_str());
            fprintf(file, "  \"fontSize\": 28\n");
            fprintf(file, "}\n");
            fclose(file);
        }
    }
