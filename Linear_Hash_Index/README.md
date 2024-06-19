## Overview

This project implements a linear hash index in C++ for managing records stored in a data file. The index dynamically grows as the number of records increases, ensuring efficient storage and retrieval. The program reads employee records from a CSV file, stores them in a linear hash index, and allows querying records by their ID.

## Files

- **main.cpp**: The main function.
- **classes.h**: The main implementation file containing the Record, Page, and LinearHashIndex classes.

## Classes

### Record
This class represents an individual record with four fields: `id`, `name`, `bio`, and `manager_id`.

**Methods:**
- `Record(vector<string> fields)`: Constructor that initializes a Record object from a vector of string fields.
- `Record()`: Default constructor.
- `void print() const`: Prints the record's fields.
- `string toString() const`: Converts the record to a string format suitable for storage.
- `static Record stringToRecord(const string& str)`: Converts a string back into a Record object.

### Page
This class represents a page in the data file with a fixed size (`PAGE_SIZE`). It manages the storage of records and the slot directory.

**Methods:**
- `Page()`: Constructor that initializes an empty page.
- `Page(int overflowPage)`: Constructor that initializes a page with an overflow pointer.
- `~Page()`: Destructor.
- `int getRecordSize()`: Returns the number of records in the page.
- `int getOverflow()`: Returns the overflow page offset.
- `void setOverflow(int of)`: Sets the overflow page offset.
- `bool writeRecord(const Record& record)`: Writes a record to the page and updates the slot directory.
- `void writeSlotDirectory()`: Writes the slot directory to the page.
- `void writeToFile(const string& fileName, int pageNumber)`: Writes the page to a specific location in the file.
- `void appendToFile(const string& fileName)`: Appends the page to the end of the file.

### LinearHashIndex
This class manages the linear hash index, including reading from a CSV file, writing records to pages, and handling index growth.

**Methods:**
- `LinearHashIndex(string indexFileName)`: Constructor that initializes the index and creates the initial data file.
- `void createFromFile(string csvFName)`: Reads records from a CSV file and adds them to the index.
- `Page readPage(int pageNumber)`: Reads a specific page from the data file.
- `void findRecordById(int id)`: Searches for a record by its ID and prints it.

**Private Methods:**
- `void insertRecord(Record record)`: Inserts a new record into the index.
- `void checkIncreaseStorage()`: Checks if storage needs to be increased and triggers the increase if necessary.
- `void increaseStorage()`: Increases the number of buckets in the index and redistributes records.
- `void cleanRecords(vector<Record> &moveRecords, int page_idx)`: Cleans records on a specified page and prepares them for re-insertion.
- `void moveOverflow()`: Moves overflow pages to the end of the data file.
- `int searchLastIBits(const bitset<8>& searchBits)`: Searches for the page containing the last `i` bits of the hash.
- `bitset<8> hashFunction(int id)`: Hash function for computing the hash index.
- `void writeIndexToFile()`: Writes the hash index to the end of the data file.

## How It Works

1. **Initialization**:
   - A `LinearHashIndex` object is created, which initializes the index and creates a new data file (`EmployeeIndex.data`) with initial buckets.

2. **Reading CSV File**:
   - The `createFromFile` method reads records from `Employee.csv` and adds them to the index. If a page becomes full, it handles overflow by creating overflow pages.

3. **Writing Pages**:
   - Each `Page` object stores records and a slot directory indicating the starting position and length of each record. Before writing to the file, the slot directory is written to the end of the page.

4. **Index Growth**:
   - The `checkIncreaseStorage` and `increaseStorage` methods ensure that the index grows dynamically as more records are added. Records are redistributed as the number of buckets increases.

5. **Querying Records**:
   - The `findRecordById` method searches for a record by its ID, reading the relevant page(s) from the data file, and prints the record if the ID matches.

6. **Main Function**:
   - The `main` function creates the `LinearHashIndex`, reads records from the CSV file, and enters a loop allowing the user to query records by ID.

## Usage

1. **Compile the Program**:
   ```bash
   g++ -std=c++11 main.cpp -o main.out
   ```

2. **Run the Program**:
   ```bash
   ./main.out
   ```

3. **Enter a User ID**:
   - The program will prompt for a user ID to search for. Enter `0` to quit the program.

## Example

To create the employee index from `Employee.csv` and query records by ID:
```bash
./main.out
Press 0 to quit or enter the user ID to search in the data file: 1
Record with id: 1 has been found in page: 0 or in its overflow.
    ID: 1
    NAME: John Doe
    BIO: Senior Developer...
    MANAGER_ID: 5
Press 0 to quit or enter the user ID to search in the data file: 0
```

## Notes

- The `PAGE_SIZE` is set to 4096 bytes.
- The program assumes the CSV file (`Employee.csv`) is properly formatted.
- Error handling is included for file operations.