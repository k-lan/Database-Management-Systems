## Overview

This project implements a join algorithm in C++ to perform an optimized merge sort join between two datasets, `Emp.csv` and `Dept.csv`. The join is performed based on the employee ID (`eid`) in `Emp.csv` and the manager ID (`managerid`) in `Dept.csv`. The implementation uses external memory sorting to handle large datasets that do not fit into main memory, followed by a merge join to combine matching records.

## Files

- **main.cpp**: The main implementation file containing the join algorithm.
- **record_class.h**: Header file containing the `Records` class, which is used to store and manipulate employee and department records.

## Classes

### Records
This class represents records from the `Emp.csv` and `Dept.csv` files with appropriate fields for each type of record.

**Fields:**
- `EmpRecord`: Structure containing fields `eid`, `ename`, `age`, and `salary`.
- `DeptRecord`: Structure containing fields `did`, `dname`, `budget`, and `managerid`.
- `no_values`: Indicator to check if there are more tuples.
- `number_of_emp_records`: Tracks the number of employee records in the buffer.
- `number_of_dept_records`: Tracks the number of department records in the buffer.

**Methods:**
- `void reset()`: Resets the record to default values.
- `void printEmpRecord()`: Prints the employee record.
- `void printDeptRecord()`: Prints the department record.
- `void joinRecords(Records &dept)`: Joins a department record with the employee record.

### Functions

- `Records Grab_Emp_Record(fstream &empin)`: Reads a single record from the employee CSV file.
- `Records Grab_Dept_Record(fstream &deptin)`: Reads a single record from the department CSV file.
- `bool compareEmpByEID(const Records& record1, const Records& record2)`: Compares two employee records by `eid`.
- `bool compareDeptByEID(const Records& record1, const Records& record2)`: Compares two department records by `managerid`.

## How It Works

### Main Steps

1. **Initialization**:
   - Opens the input files `Emp.csv` and `Dept.csv` for reading.
   - Creates `Join.csv` for outputting the joined results.
   - Initializes buffers and temporary file management structures.

2. **Pass 1 - Creating Sorted Runs**:
   - Reads records into the buffer from `Emp.csv` and sorts them when the buffer is full.
   - Writes sorted records to temporary files.
   - Repeats the process for `Dept.csv`.

3. **Pass 2 - Merging and Joining Sorted Runs**:
   - Reads the first record from each temporary file into the buffer.
   - Finds the smallest `eid` and `managerid` in the buffer and performs the join if they match.
   - Writes the joined records to the output file.
   - Continues until all records are processed.

4. **Cleanup**:
   - Closes and deletes all temporary files.

### Functions in Main Steps

- **sortBuffer(int size_of_sort, bool isEmp)**: Sorts the buffer using `compareEmpByEID` or `compareDeptByEID`.
- **writeBufferToTempFile(int size_of_buffer, int &file_index, bool isEmp)**: Writes the sorted buffer to a temporary file.
- **mergeJoinRuns(fstream& outputFile)**: Merges and joins the sorted runs from temporary files and writes them to the output file.
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

To perform the join between `Emp.csv` and `Dept.csv` and output the joined records to `Join.csv`:
```bash
./main.out
```

## Notes

- The `buffer_size` is set to 22, indicating the number of records that can be held in the main memory at one time.
- The program assumes the `Emp.csv` and `Dept.csv` files are properly formatted with appropriate fields.
- Temporary files are created during the sorting process and are deleted after the final merge join.
- Error handling is included for file operations to ensure smooth execution.