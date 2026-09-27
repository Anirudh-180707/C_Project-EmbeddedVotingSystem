import serial
import time

from database import (
    initialize_database,
    insert_sample_voters,
    check_voter,
    mark_voter_voted,
)


SERIAL_URL = "rfc2217://localhost:4000"
BAUD_RATE = 9600


def connect_to_wokwi():
    while True:
        try:
            print("Connecting to Wokwi...")

            connection = serial.serial_for_url(
                SERIAL_URL,
                baudrate=BAUD_RATE,
                timeout=1
            )

            print("Connected to Wokwi virtual serial port.")
            return connection

        except Exception as error:
            print(f"Connection failed: {error}")
            print("Make sure Wokwi simulation is running.")
            time.sleep(3)


def process_message(ser, message):
    message = message.strip()

    if not message:
        return

    print(f"Arduino → Python: {message}")

    # ---------------------------------------------
    # VOTER VERIFICATION
    # ---------------------------------------------

    if message.startswith("CHECK_VOTER:"):
        voter_id = message.split(":", 1)[1]

        result = check_voter(voter_id) #Database fn

        print(
            f"Checking voter ID {voter_id}: "
            f"{result}"
        )

        ser.write((result + "\n").encode()) #sends as byte

        print(f"Python → Arduino: {result}")

    # ---------------------------------------------
    # MARK VOTER AS VOTED
    # ---------------------------------------------

    elif message.startswith("MARK_VOTED:"):
        voter_id = message.split(":", 1)[1]

        result = mark_voter_voted(voter_id)

        print(
            f"Updating voter ID {voter_id}: "
            f"{result}"
        )

        ser.write((result + "\n").encode())

        print(f"Python → Arduino: {result}")

    # ---------------------------------------------
    # EEPROM STATUS MESSAGES
    # ---------------------------------------------

    elif message == "EEPROM_INITIALIZED":
        print("Arduino EEPROM was initialized.")

    elif message == "EEPROM_NOT_INITIALIZED":
        print("Arduino EEPROM was not initialized.")

    else:
        print(f"Unrecognized Arduino message: {message}")


def main():
    print("Starting voting system Python bridge...")

    initialize_database() #database fn
    insert_sample_voters()

    print("SQLite database ready.")

    ser = connect_to_wokwi() #ln 16

    print("Bridge is running.")
    print("Waiting for Arduino messages...")

    try:
        while True:
            if ser.in_waiting:  #check if message is available in buffer
                message = ser.readline().decode(
                    errors="ignore"
                )

                process_message(ser, message) #ln 36

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\nStopping bridge...")

    finally:
        ser.close()
        print("Serial connection closed.")


if __name__ == "__main__":
    main()