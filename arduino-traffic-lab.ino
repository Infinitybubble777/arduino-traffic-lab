// Model pedestrian crossing for an Arduino Uno and five LEDs.

constexpr uint8_t CAR_RED = 4;
constexpr uint8_t CAR_YELLOW = 5;
constexpr uint8_t CAR_GREEN = 6;
constexpr uint8_t WALK_RED = 7;
constexpr uint8_t WALK_GREEN = 8;
constexpr uint8_t BUTTON = 2;

constexpr unsigned long DEBOUNCE_MS = 35;
constexpr unsigned long MIN_GREEN_MS = 5000;
constexpr unsigned long YELLOW_MS = 2000;
constexpr unsigned long CLEARANCE_MS = 1000;
constexpr unsigned long WALK_MS = 5000;

enum class Phase : uint8_t {
  CarGreen,
  CarYellow,
  BeforeWalk,
  Walk,
  AfterWalk,
};

Phase phase = Phase::CarGreen;
unsigned long phaseStartedAt = 0;
unsigned long buttonChangedAt = 0;
bool lastReading = HIGH;
bool stableReading = HIGH;
bool walkRequested = false;

void enterPhase(Phase next, unsigned long now) {
  phase = next;
  phaseStartedAt = now;

  // Turn permissive signals off before enabling any for the next phase.
  digitalWrite(CAR_GREEN, LOW);
  digitalWrite(CAR_YELLOW, LOW);
  digitalWrite(WALK_GREEN, LOW);
  digitalWrite(CAR_RED, next == Phase::CarGreen || next == Phase::CarYellow ? LOW : HIGH);
  digitalWrite(WALK_RED, next == Phase::Walk ? LOW : HIGH);
  digitalWrite(CAR_GREEN, next == Phase::CarGreen ? HIGH : LOW);
  digitalWrite(CAR_YELLOW, next == Phase::CarYellow ? HIGH : LOW);
  digitalWrite(WALK_GREEN, next == Phase::Walk ? HIGH : LOW);
}

void readButton(unsigned long now) {
  const bool reading = digitalRead(BUTTON);
  if (reading != lastReading) {
    buttonChangedAt = now;
    lastReading = reading;
  }
  if (reading != stableReading && now - buttonChangedAt >= DEBOUNCE_MS) {
    stableReading = reading;
    // A held button triggers one request. The current cycle serves any
    // additional presses until the cars receive green again.
    if (stableReading == LOW && phase == Phase::CarGreen) {
      walkRequested = true;
    }
  }
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(CAR_RED, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(WALK_RED, OUTPUT);
  pinMode(WALK_GREEN, OUTPUT);
  enterPhase(Phase::CarGreen, millis());
}

void loop() {
  const unsigned long now = millis();
  readButton(now);
  const unsigned long elapsed = now - phaseStartedAt;

  switch (phase) {
    case Phase::CarGreen:
      if (walkRequested && elapsed >= MIN_GREEN_MS) {
        walkRequested = false;
        enterPhase(Phase::CarYellow, now);
      }
      break;
    case Phase::CarYellow:
      if (elapsed >= YELLOW_MS) {
        enterPhase(Phase::BeforeWalk, now);
      }
      break;
    case Phase::BeforeWalk:
      if (elapsed >= CLEARANCE_MS) {
        enterPhase(Phase::Walk, now);
      }
      break;
    case Phase::Walk:
      if (elapsed >= WALK_MS) {
        enterPhase(Phase::AfterWalk, now);
      }
      break;
    case Phase::AfterWalk:
      if (elapsed >= CLEARANCE_MS) {
        enterPhase(Phase::CarGreen, now);
      }
      break;
  }
}
