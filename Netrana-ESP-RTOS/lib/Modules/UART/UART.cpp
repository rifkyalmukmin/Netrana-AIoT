#include "UART.h"

const uint32_t SERIAL_READ_TIMEOUT_MS = 150;

void initUART() {
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial2.setTimeout(SERIAL_READ_TIMEOUT_MS);
}

bool splitCSV(const String &line, String out[], int maxTokens, char sep) {
  int count = 0, start = 0;
  while (count < maxTokens) {
    int idx = line.indexOf(sep, start);
    if (idx < 0) { out[count++] = trimCopy(line.substring(start)); break; }
    out[count++] = trimCopy(line.substring(start, idx));
    start = idx + 1;
  }
  return count > 0;
}

bool toFloatSafe(const String &s, float &val) {
  if (s.length() == 0) return false;
  val = s.toFloat();
  if (val == 0.0f && s != "0" && s != "0.0" && s != "0.00") {
    String t = s; t.replace("cm", ""); t.trim();
    if (t.length() == 0) return false;
    val = t.toFloat();
    if (val == 0.0f && t != "0" && t != "0.0" && t != "0.00") return false;
  }
  return true;
}

bool parseBracketBTN(const String &line, String &name, float &front, float &bottom) {
  if (!line.startsWith("[BTN]")) return false;
  String s = line; s.replace("-=", "="); s.replace("[BTN]", ""); s.trim();

  int p1 = s.indexOf("name="); if (p1 < 0) return false;
  int p1e = s.indexOf(' ', p1);
  String namePart = p1e > 0 ? s.substring(p1 + 5, p1e) : s.substring(p1 + 5); namePart.trim();

  int p2 = s.indexOf("front=");
  int p2e = s.indexOf(' ', p2);
  String frontPart = (p2 >= 0) ? (p2e > 0 ? s.substring(p2 + 6, p2e) : s.substring(p2 + 6)) : "";

  int p3 = s.indexOf("bottom=");
  int p3e = s.indexOf(' ', p3);
  String bottomPart = (p3 >= 0) ? (p3e > 0 ? s.substring(p3 + 7, p3e) : s.substring(p3 + 7)) : "";

  name = namePart;
  front = frontPart.toFloat();
  bottom = bottomPart.toFloat();
  return name.length() > 0;
}

void readLineFromUART(String &line) {
    line = Serial2.readStringUntil('\n');
    line.trim();
}