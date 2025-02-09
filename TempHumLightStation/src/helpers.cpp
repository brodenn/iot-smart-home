#include "../include/helpers.h"
#include "../include/automation.h"

void handleSetpoints(const String& data) {
    // Check if the data contains valid setpoints
    if (data.startsWith("temp=") && data.indexOf("&humidity=") != -1) {
        // Parse the incoming data (assuming it's in URL-encoded format)
        int tempIndex = data.indexOf("temp=");
        int humIndex = data.indexOf("&humidity=");

        if (tempIndex != -1 && humIndex != -1) {
            String tempStr = data.substring(tempIndex + 5, humIndex);
            String humStr = data.substring(humIndex + 10);

            // Ensure the parsed values are valid numbers
            if (tempStr.length() > 0 && humStr.length() > 0) {
                float temp = tempStr.toFloat();
                float hum = humStr.toFloat();

                // Convert to integer format used by Automation_SetSetpoints
                int16_t tempInt = static_cast<int16_t>(temp * 100);
                int16_t humInt = static_cast<int16_t>(hum * 100);

                // Update setpoints
                Automation_SetSetpoints(tempInt, humInt);
            }
        }
    }
}