#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// =====================================================
// PARALLEL LCD CONFIGURATION
// RS, E, D4, D5, D6, D7
// =====================================================

LiquidCrystal lcd(
    12, 11, 5, 4, 3, 2
);

// =====================================================
// 4x4 KEYPAD CONFIGURATION
// =====================================================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {
    22, 24, 26, 28
};

byte colPins[COLS] = {
    30, 32, 34, 36
};

Keypad keypad = Keypad(
    makeKeymap(keys),
    rowPins,
    colPins,
    ROWS,
    COLS
);

// =====================================================
// VOTING BOX PUSHBUTTON CONFIGURATION
// =====================================================

const int CANDIDATE_1_BUTTON = 38;
const int CANDIDATE_2_BUTTON = 40;
const int CANDIDATE_3_BUTTON = 42;
const int NOTA_BUTTON = 44;

// =====================================================
// EEPROM CONFIGURATION
// =====================================================

// Option 0 = Candidate 1
// Option 1 = Candidate 2
// Option 2 = Candidate 3
// Option 3 = NOTA

const int NUMBER_OF_OPTIONS = 4;

const int EEPROM_MAGIC_ADDRESS = 0;
const int EEPROM_VOTES_START_ADDRESS = 4;

const byte EEPROM_MAGIC_VALUE = 0xAB;

// =====================================================
// MODE VARIABLES
// =====================================================

bool votingMode = false;
bool adminMode = false;

String inputBuffer = "";

// =====================================================
// EEPROM FUNCTIONS
// =====================================================

int getVoteAddress(int optionIndex) {
    return EEPROM_VOTES_START_ADDRESS +
           optionIndex * sizeof(long);
}

void initializeEEPROMIfRequired() {
    byte magicValue;

    EEPROM.get(
        EEPROM_MAGIC_ADDRESS,
        magicValue
    );

    if (magicValue != EEPROM_MAGIC_VALUE) {
        Serial.println("EEPROM_NOT_INITIALIZED");

        for (
            int i = 0;
            i < NUMBER_OF_OPTIONS;
            i++
        ) {
            long initialVotes = 0;

            EEPROM.put(
                getVoteAddress(i),
                initialVotes
            );
        }

        EEPROM.put(
            EEPROM_MAGIC_ADDRESS,
            EEPROM_MAGIC_VALUE
        );

        Serial.println("EEPROM_INITIALIZED");
    }
    else {
        Serial.println("EEPROM_READY");
    }
}

long getVoteCount(int optionIndex) {
    long voteCount = 0;

    EEPROM.get(
        getVoteAddress(optionIndex),
        voteCount
    );

    return voteCount;
}

void incrementVoteCount(int optionIndex) {
    long currentCount =
        getVoteCount(optionIndex);

    currentCount++;

    EEPROM.put(
        getVoteAddress(optionIndex),
        currentCount
    );
}

// Optional testing function
void resetEEPROMVotes() {
    for (
        int i = 0;
        i < NUMBER_OF_OPTIONS;
        i++
    ) {
        long zero = 0;

        EEPROM.put(
            getVoteAddress(i),
            zero
        );
    }

    Serial.println("EEPROM_VOTES_RESET");
}

// =====================================================
// LCD FUNCTIONS
// =====================================================

void showMessage(
    String line1,
    String line2 = "",
    int delayTime = 1000
) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);

    delay(delayTime);
}

void displayMainMenu() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("A: Voting");

    lcd.setCursor(0, 1);
    lcd.print("B: Admin");
}

// =====================================================
// SERIAL COMMUNICATION
// =====================================================

void sendToPython(String message) {
    Serial.println(message);
}

String waitForPythonResponse(
    unsigned long timeoutMs = 10000
) {
    unsigned long startTime = millis();

    while (
        millis() - startTime < timeoutMs
    ) {
        if (Serial.available()) {
            String response =
                Serial.readStringUntil('\n');

            response.trim();

            return response;
        }
    }

    return "BRIDGE_TIMEOUT";
}

// =====================================================
// KEYPAD NUMERIC INPUT
// =====================================================

String readNumericInput(
    String line1,
    String line2
) {
    inputBuffer = "";

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);

    while (true) {
        char key = keypad.getKey();

        if (key == NO_KEY) {
            continue;
        }

        // Clear input
        if (key == '*') {
            inputBuffer = "";

            lcd.setCursor(0, 1);
            lcd.print("                ");

            lcd.setCursor(0, 1);

            continue;
        }

        // Submit input
        if (key == '#') {
            if (inputBuffer.length() > 0) {
                return inputBuffer;
            }

            continue;
        }

        // Accept only numbers
        if (
            key >= '0' &&
            key <= '9'
        ) {
            if (inputBuffer.length() < 15) {
                inputBuffer += key;

                lcd.setCursor(0, 1);
                lcd.print("                ");

                lcd.setCursor(0, 1);

                for (
                    unsigned int i = 0;
                    i < inputBuffer.length();
                    i++
                ) {
                    lcd.print("*");
                }
            }
        }
    }
}

// =====================================================
// PYTHON DATABASE COMMUNICATION
// =====================================================

bool verifyVoter(String voterId) {
    sendToPython(
        "CHECK_VOTER:" + voterId
    );

    String response =
        waitForPythonResponse();

    if (response == "VOTER_ALLOWED") {
        return true;
    }

    if (
        response ==
        "VOTER_ALREADY_VOTED"
    ) {
        showMessage(
            "Already Voted",
            "Access Denied",
            2000
        );

        return false;
    }

    if (
        response ==
        "VOTER_NOT_FOUND"
    ) {
        showMessage(
            "Invalid Voter",
            "Not Registered",
            2000
        );

        return false;
    }

    showMessage(
        "Verification Error",
        response,
        2000
    );

    return false;
}

bool markVoterAsVoted(String voterId) {
    sendToPython(
        "MARK_VOTED:" + voterId
    );

    String response =
        waitForPythonResponse();

    if (response == "VOTER_MARKED") {
        return true;
    }

    showMessage(
        "Database Error",
        response,
        2000
    );

    return false;
}

// =====================================================
// VOTING BOX BUTTON FUNCTIONS
// =====================================================

int readVotingBoxButton() {
    if (
        digitalRead(CANDIDATE_1_BUTTON)
        == LOW
    ) {
        return 0;
    }

    if (
        digitalRead(CANDIDATE_2_BUTTON)
        == LOW
    ) {
        return 1;
    }

    if (
        digitalRead(CANDIDATE_3_BUTTON)
        == LOW
    ) {
        return 2;
    }

    if (
        digitalRead(NOTA_BUTTON)
        == LOW
    ) {
        return 3;
    }

    return -1;
}

int waitForVoteButton() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Press Vote Btn");

    lcd.setCursor(0, 1);
    lcd.print("C1 C2 C3 NOTA");

    while (true) {
        int selectedOption =
            readVotingBoxButton();

        if (selectedOption != -1) {
            delay(100);

            // Wait for button release
            while (
                readVotingBoxButton()
                != -1
            ) {
                delay(10);
            }

            return selectedOption;
        }
    }
}

// =====================================================
// DISPLAY SELECTED OPTION
// =====================================================

void displaySelectedOption(
    int selectedOption
) {
    if (selectedOption == 0) {
        showMessage(
            "Candidate 1",
            "Vote Recorded",
            2000
        );
    }
    else if (selectedOption == 1) {
        showMessage(
            "Candidate 2",
            "Vote Recorded",
            2000
        );
    }
    else if (selectedOption == 2) {
        showMessage(
            "Candidate 3",
            "Vote Recorded",
            2000
        );
    }
    else if (selectedOption == 3) {
        showMessage(
            "NOTA",
            "Vote Recorded",
            2000
        );
    }
}

// =====================================================
// VOTING MODE
// =====================================================

void startVotingMode() {
    votingMode = true;

    showMessage(
        "Voting Mode",
        "Started",
        1000
    );

    while (votingMode) {
        String voterId = readNumericInput(
            "Enter Voter ID",
            "*Clr #Enter"
        );

        // 0000# exits voting mode
        if (voterId == "0000") {
            votingMode = false;

            showMessage(
                "Voting Mode",
                "Exited",
                1000
            );

            break;
        }

        bool voterAllowed =
            verifyVoter(voterId);

        if (!voterAllowed) {
            continue;
        }

        // Actual vote is made using physical voting-box button
        int selectedOption =
            waitForVoteButton();

        // Store candidate/NOTA vote in EEPROM
        incrementVoteCount(
            selectedOption
        );

        // Store voter status in SQLite
        bool databaseUpdated =
            markVoterAsVoted(voterId);

        if (databaseUpdated) {
            displaySelectedOption(
                selectedOption
            );
        }
        else {
            showMessage(
                "Vote Recorded",
                "DB Sync Failed",
                2000
            );
        }
    }
}

// =====================================================
// ADMIN RESULTS DISPLAY
// =====================================================

void displayResults() {
    long candidate1 =
        getVoteCount(0);

    long candidate2 =
        getVoteCount(1);

    long candidate3 =
        getVoteCount(2);

    long nota =
        getVoteCount(3);

    // Screen 1: Candidate 1 and Candidate 2
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("C1:");
    lcd.print(candidate1);

    lcd.setCursor(9, 0);
    lcd.print("C2:");
    lcd.print(candidate2);

    // Screen 1 also shows Candidate 3
    lcd.setCursor(0, 1);
    lcd.print("C3:");
    lcd.print(candidate3);

    delay(2500);

    // Screen 2: NOTA
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("NOTA Votes:");

    lcd.setCursor(0, 1);
    lcd.print(nota);

    delay(2500);
}

// =====================================================
// ADMIN MODE
// =====================================================

void startAdminMode() {
    adminMode = true;

    showMessage(
        "Admin Mode",
        "Results",
        1000
    );

    while (adminMode) {
        // Display all results including NOTA
        displayResults();

        String command = readNumericInput(
            "Admin Command",
            "0000# Exit"
        );

        if (command == "0000") {
            adminMode = false;

            showMessage(
                "Admin Mode",
                "Exited",
                1000
            );

            break;
        }

        showMessage(
            "Invalid Command",
            "",
            1000
        );
    }
}

// =====================================================
// SETUP
// =====================================================

void setup() {
    // Start Serial
    Serial.begin(9600);

    // Initialize parallel LCD
    lcd.begin(16, 2);

    // Configure voting-box buttons
    pinMode(
        CANDIDATE_1_BUTTON,
        INPUT_PULLUP
    );

    pinMode(
        CANDIDATE_2_BUTTON,
        INPUT_PULLUP
    );

    pinMode(
        CANDIDATE_3_BUTTON,
        INPUT_PULLUP
    );

    pinMode(
        NOTA_BUTTON,
        INPUT_PULLUP
    );

    // Initialize EEPROM
    initializeEEPROMIfRequired();

    // Startup display
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Voting Machine");

    lcd.setCursor(0, 1);
    lcd.print("Ready");

    delay(1500);

    // Main menu
    displayMainMenu();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
    char key = keypad.getKey();

    if (key == NO_KEY) {
        return;
    }

    // A = Voting Mode
    if (key == 'A') {
        startVotingMode();

        displayMainMenu();
    }

    // B = Admin Mode
    else if (key == 'B') {
        startAdminMode();

        displayMainMenu();
    }
}