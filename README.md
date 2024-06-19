# Database-Management-Systems
A variety of database management systems implemented from scratch for CS 440, Database Management Systems. View each management system's README for a more in-depth explanation.

## Overview

### 1. Storage Buffer Manager
**Description**: A system for managing records stored in pages within a data file. It reads employee records from a CSV file, stores them in pages, and allows querying records by their ID. Useful for accessing information that does not fit inside of main memory.

**Key Features**:
- Implements a `Record` class for storing individual records.
- Uses a `Page` class to manage storage and slot directories.
- Provides a `StorageBufferManager` class for handling file operations and querying.
- Saved to a datafile that can be stored on external (cheap) memory.

### 2. Linear Hash Index
**Description**: A linear hash index system for managing records, ensuring efficient storage and retrieval. It dynamically grows as the number of records increases.  This is an extension of the storage buffer manager and is useful for accessing information that does not fit inside of main memory.

**Key Features**:
- Implements a hash-based indexing system using `Page` and `Record` classes.
- Dynamically adjusts the number of buckets (`n`) and bits (`i`) used for indexing.
- Uses temporary files for overflow handling and reorganization.
- Saved to a datafile that can be stored on external (cheap) memory.

### 3. External Memory Sorting
**Description**: An external memory sorting algorithm for sorting large datasets that do not fit into main memory. It sorts records from a CSV file and outputs the sorted records into another CSV file.

**Key Features**:
- Implements a two-pass sorting algorithm.
- Uses temporary files to store sorted runs.
- Merges sorted runs in the second pass to produce the final sorted output.

### 4. Optimized Merge Sort Join
**Description**: An optimized merge sort join algorithm for performing a join between two datasets (`Emp.csv` and `Dept.csv`). The join is based on the employee ID (`eid`) and manager ID (`managerid`).

**Key Features**:
- Implements external memory sorting for both datasets.
- Uses a merge join to combine matching records from the two datasets.
- Produces an output file with the joined records.

## Program Names

1. **Storage_Buffer_Manager**
2. **Linear_Hash_Index**
3. **External_Memory_Sorting**
4. **Optimized_Merge_Sort_Join**

Each program is designed to demonstrate different aspects of database management, from storage and indexing to sorting and joining large datasets. Refer to each program's README for detailed explanations and usage instructions.