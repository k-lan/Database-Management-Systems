## Overview

This project implements an external memory sorting algorithm in C++ to sort large datasets that do not fit into main memory. The program sorts records from `Emp.csv` and outputs the sorted records into `EmpSorted.csv`. The implementation uses a two-pass approach: in the first pass, it creates sorted runs, and in the second pass, it merges these runs.

## Files

- **main.cpp**: The main implementation file containing the external memory sorting logic.
- **record_class.h**: Header file containing the `Records` class, which is used to store and manipulate employee records.

## Classes

### Records
This class represents a record from the `Emp.csv` file with fields: `eid`, `ename`, `age`, and `salary`.

**Methods:**
- `void printEmpRecord() const`: Prints the `eid`, `ename`, and `no_values` of the record.
- `void reset()`: Resets the record to default values.

### Functions

- `bool compareByEID(const Records& record1, const Records& record2)`: Compares two records by `eid` for sorting.
- `Records Grab_Emp_Record(fstream &empin)`: Reads a single record from the CSV file into the `Records` structure.

## How It Works

### Main Steps

1. **Initialization**:
   - Opens the input file `Emp.csv` for reading and creates `EmpSorted.csv` for output.
   - Initializes buffers for sorting and temporary file management.

2. **Pass 1 - Creating Sorted Runs**:
   - Reads records into the buffer.
   - Sorts the buffer when it is full and writes sorted records to temporary files.
   - Continues reading until all records are processed.

3. **Pass 2 - Merging Sorted Runs**:
   - Reads the first record from each temporary file into the buffer.
   - Finds the smallest `eid` in the buffer, writes it to the output file, and replaces it with the next record from the same temporary file.
   - Continues until all records are merged and written to the output file.

4. **Cleanup**:
   - Closes and deletes all temporary files.

### Functions in Main Steps

- **sortBuffer(int size_of_sort)**: Sorts the buffer using `compareByEID`.
- **writeBufferToTempFile(int size_of_buffer, int &file_index)**: Writes the sorted buffer to a temporary file.
- **mergeRuns(fstream& outputFile)**: Merges the sorted runs from temporary files and writes them to the output file.
- **closeAndDeleteFiles()**: Deletes all temporary files after merging.
- **clearBuffer()**: Resets the buffer to hold new records.

## Usage

1. **Compile the Program**:
   ```bash
   g++ -std=c++11 main.cpp -o main.out
   ```

2. **Run the Program**:
   ```bash
   ./main.out
   ```

## Example

To sort the `Emp.csv` file and output the sorted records to `EmpSorted.csv`:
```bash
./main.out
```

## Notes

- The `buffer_size` is set to 22, indicating the number of records that can be held in the main memory at one time.
- The program assumes the `Emp.csv` file is properly formatted with records in the format: `eid,ename,age,salary`.
- Temporary files are created during the sorting process and are deleted after the final merge.
- Error handling is included for file operations to ensure smooth execution.