## Overview

This project implements a simple storage buffer manager in C++ to manage records stored in pages within a data file. The program reads employee records from a CSV file, stores them in pages, and allows querying records by their ID.

## Files

- **main.cpp**: The file containing the main function.
- **classes.h**: The main implementation file containing the Record, Page, and StorageBufferManager classes.

## Classes

### Record
This class represents an individual record with four fields: `id`, `name`, `bio`, and `manager_id`. 

**Methods:**
- `Record(vector<std::string> fields)`: Constructor that initializes a Record object from a vector of string fields.
- `Record()`: Default constructor.
- `void print() const`: Prints the record's fields.
- `std::string toString() const`: Converts the record to a string format suitable for storage.
- `static Record stringToRecord(const std::string& str)`: Converts a string back into a Record object.

### Page
This class represents a page in the data file with a fixed size (`PAGE_SIZE`). It manages the storage of records and the slot directory.

**Methods:**
- `Page()`: Constructor that initializes an empty page.
- `int getRecordSize()`: Returns the number of records in the page.
- `bool writeRecord(const Record& record)`: Writes a record to the page and updates the slot directory.
- `void writeSlotDirectory()`: Writes the slot directory to the page.
- `void writeToFile(const std::string& fileName)`: Writes the page to a file.

### StorageBufferManager
This class manages the overall storage of records, including reading from a CSV file and writing pages to a data file.

**Methods:**
- `StorageBufferManager(std::string NewFileName)`: Constructor that initializes the manager and creates a new data file.
- `void createFromFile(std::string csvFName)`: Reads records from a CSV file and writes them to pages.
- `void findRecordById(int id)`: Searches for a record by its ID and prints it.

## How It Works

1. **Initialization**:
   - A `StorageBufferManager` object is created, which initializes a new data file (`EmployeeRelation.data`).

2. **Reading CSV File**:
   - The `createFromFile` method reads records from `Employee.csv` and writes them to pages. If a page becomes full, it writes the page to the data file and starts a new page.

3. **Writing Pages**:
   - Each `Page` object stores records and a slot directory indicating the starting position and length of each record. Before writing to the file, the slot directory is written to the end of the page.

4. **Querying Records**:
   - The `findRecordById` method reads pages from the data file, extracts records, and prints the record if the ID matches the queried ID.

5. **Main Function**:
   - The `main` function creates the `StorageBufferManager`, reads records from the CSV file, and enters a loop allowing the user to query records by ID.

## Usage

1. **Compile the Program**:
   ```bash
   g++ -std=c++11 main.cpp -o main.out
   ```

2. **Run the Program**:
   ```bash
   ./main
   ```

3. **Enter a User ID**:
   - The program will prompt for a user ID to search for. Enter `0` to quit the program.

## Example

To create the employee relation file from `Employee.csv` and query records by ID:
```bash
./main
Press 0 to quit or enter the user ID to search in the data file: 1
    ID: 1
    NAME: John Doe
    BIO: Senior Developer
    MANAGER_ID: 5
Press 0 to quit or enter the user ID to search in the data file: 0
```

## Notes

- The `PAGE_SIZE` is set to 4096 bytes.
- The program assumes the CSV file (`Employee.csv`) is properly formatted.
- Error handling is included for file operations.