#include "Control.h"

void parserControlInput(String &line, String &kind, String &name, float &distanceFront, float &distanceBottom) {
    kind = ""; name = ""; distanceFront = -1.0f; distanceBottom = -1.0f;
    if (line.length() == 0) return;

    // Parser format bracket: "[BTN] name=RIGHT front-=392.61 bottom-72.39"
    if (line.startsWith("[BTN]")) {
      if (parseBracketBTN(line, name, distanceFront, distanceBottom)) {
        kind = "BTN";
        Serial.print("[BTN-BRACKET] name="); Serial.print(name);
        Serial.print(" front="); Serial.print(distanceFront, 2);
        Serial.print(" bottom="); Serial.println(distanceBottom, 2);
      }
      return;
    }

    // Parser CSV: "BTN, NAME, front, bottom" atau "NONE, front, bottom"
    String tok[4];
    if (!splitCSV(line, tok, 4, ',')) return;

    kind = tok[0]; kind.trim();
    if (kind.equalsIgnoreCase("BTN")) {
      name = tok[1]; name.trim();
      toFloatSafe(tok[2], distanceFront);
      toFloatSafe(tok[3], distanceBottom);
    } else if (kind.equalsIgnoreCase("NONE")) {
      toFloatSafe(tok[1], distanceFront);
      toFloatSafe(tok[2], distanceBottom);
    } else {
      kind = "";
    }
}