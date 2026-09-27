import sqlite3 #standard lirary
from pathlib import Path #file handling


DATABASE_PATH = Path(__file__).parent / "voters.db" #creates file in same directory


def get_connection():
    return sqlite3.connect(DATABASE_PATH) #creates database


def initialize_database():
    connection = get_connection()
    cursor = connection.cursor()  #creates database cursor

    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS voters (
            voter_id TEXT PRIMARY KEY,
            has_voted INTEGER NOT NULL DEFAULT 0
        )
        """
    ) #creates table

    connection.commit()
    connection.close()


def insert_sample_voters():
    connection = get_connection()
    cursor = connection.cursor()

    sample_voters = [
        ("1001", 0),
        ("1002", 0),
        ("1003", 0),
        ("1004", 0),
        ("1005", 0),
        ("1006", 0),
        ("1007", 0),
        ("1008", 0),
        ("1009", 0),
        ("1010", 0),
    ]

    cursor.executemany(
        """
        INSERT OR IGNORE INTO voters
        (voter_id, has_voted)
        VALUES (?, ?)
        """,
        sample_voters
    ) # ?-placeholder

    connection.commit()
    connection.close()


def check_voter(voter_id):
    connection = get_connection()
    cursor = connection.cursor()

    cursor.execute(
        """
        SELECT has_voted
        FROM voters
        WHERE voter_id = ?
        """,
        (voter_id,)
    )

    voter = cursor.fetchone()

    connection.close()

    if voter is None:
        return "VOTER_NOT_FOUND"

    if voter[0] == 1:
        return "VOTER_ALREADY_VOTED"

    return "VOTER_ALLOWED"


def mark_voter_voted(voter_id):
    connection = get_connection()
    cursor = connection.cursor()

    cursor.execute(
        """
        UPDATE voters
        SET has_voted = 1
        WHERE voter_id = ?
        AND has_voted = 0
        """,
        (voter_id,)
    )

    rows_updated = cursor.rowcount #gives no. of rows affected

    connection.commit()
    connection.close()

    if rows_updated == 1:
        return "VOTER_MARKED"

    return "VOTER_MARK_FAILED"


def reset_all_voters():
    connection = get_connection()
    cursor = connection.cursor()

    cursor.execute(
        """
        UPDATE voters
        SET has_voted = 0
        """
    )

    connection.commit()
    connection.close()


def display_all_voters():
    connection = get_connection()
    cursor = connection.cursor()

    cursor.execute(
        """
        SELECT voter_id, has_voted
        FROM voters
        ORDER BY voter_id
        """
    )

    voters = cursor.fetchall()

    connection.close()

    for voter_id, has_voted in voters:
        print(f"{voter_id} -> {has_voted}")


if __name__ == "__main__":
    initialize_database()
    insert_sample_voters()

    print("Database initialized successfully.")
    display_all_voters()