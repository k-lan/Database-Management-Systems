#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>

// My added libraries 
#include <fstream>
#include <cmath> // for pow() funct
#include <cstring> 
using namespace std;

const int PAGE_SIZE = 4096;

class Record {
public:
    int id, manager_id;
    string bio, name;

    Record(vector<string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    Record(){}

    void print() const {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        // Check if bio length is greater than 50 characters (dont crowd terminal)
        if (bio.length() > 50) {
            cout << "\tBIO: " << bio.substr(0, 50) << "...\n"; // Print only the first 50 characters followed by an ellipsis
        } else {
            cout << "\tBIO: " << bio << "\n"; // Print the entire bio if it's 50 characters or less
        }
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    // Convert record to string representation for writing to file
    string toString() const {
        return to_string(id) + "|" + name + "|" + bio + "|" + to_string(manager_id) + "|";
    }

    // Convert a string record back to a record
    static Record stringToRecord(const string& str) {
        Record record;
        istringstream ss(str);
        string token;

        // Parsing using '|' delimiter
        getline(ss, token, '|');
        record.id = stoi(token);

        getline(ss, record.name, '|');
        getline(ss, record.bio, '|');

        getline(ss, token, '|');
        record.manager_id = stoi(token);

        return record;
    }
};

class Page {
private:
    
    int freeSpace;
    // Page layout:
    // [data ... data ... start_slot_dir: [(offset, size),...,(offset, size)], num_records, free_space, overflow_ptr]
    // End of file is overflow_ptr, num_records, then slot directory
public:
    vector<char> data; // The actual data of the page
    vector<pair<int, int>> slotDirectory; // (Starting position, length) of each record
    int overflow; // pointer to overflow page, 4 bytes

    // initialize a new page, freespace is space minus overflow page pointer,slot dir num_slots and freeSpace
    Page() : data(PAGE_SIZE, '\0'), freeSpace(PAGE_SIZE - sizeof(int) * 3), overflow(-1) {}

    // initialize a new page with an overflow pointer.
    Page(int overflowPage) : data(PAGE_SIZE, '\0'), freeSpace(PAGE_SIZE - sizeof(int) * 3), overflow(overflowPage) {}

    // deconstructor
    ~Page() {}

    // gettor for recordsize
    int getRecordSize() {
        return slotDirectory.size();
    }

    // getter for overflow page offset
    int getOverflow() {
        return overflow;
    }

    // setter for overflow location
    void setOverflow(int of) {
        overflow = of; 
    }
    
    // Write record to the page and update slot directory
    bool writeRecord(const Record& record) {
        // record.print();
        string recordString = record.toString();
        int recordLength = recordString.length();
        // sizeof slot directory, pointer to overflow page, free space, and num records
        size_t dirByteSize = slotDirectory.size() * sizeof(pair<int, int>);

        if (recordLength + sizeof(int) * 2 > freeSpace - dirByteSize) // Assure that new record and two values for directory fit
            return false;

        int start = PAGE_SIZE - freeSpace - sizeof(int) * 3; // make sure to ignore the numrecords, overflow, freespace at end
        for (char c : recordString)
            data[start++] = c;

        slotDirectory.emplace_back(start - recordLength, recordLength);
        // cout << "offset is : \t" << start - recordLength << "\t recordLength is: " << recordLength << "\n";

        freeSpace -= recordLength; // Subtract record
        return true;
    }

    void writeSlotDirectory() {
        // writes the slot directory to the datafile. 
        int numSlots = slotDirectory.size();
        int directoryStart = PAGE_SIZE - sizeof(int); // Start at the end of the page

        // Write metadata to end of page
        memcpy(&data[directoryStart], &overflow, sizeof(int));
        directoryStart -= sizeof(int);
        memcpy(&data[directoryStart], &freeSpace, sizeof(int));
        directoryStart -= sizeof(int);
        memcpy(&data[directoryStart], &numSlots, sizeof(int));
        
        // Write the slot directory entries
        for (auto& entry : slotDirectory) {
            int start = entry.first;
            int length = entry.second;
            // cout << "first entry is : " << start << "\t second is: " << length << "\n";
            directoryStart -= sizeof(int); // Move backwards
            memcpy(&data[directoryStart], &length, sizeof(int));
            directoryStart -= sizeof(int); // Move backwards
            memcpy(&data[directoryStart], &start, sizeof(int));
        }
    }

    void writeToFile(const string& fileName, int pageNumber) {
        writeSlotDirectory(); // Ensure the slot directory is written to data before updating page
        
        // Open the file in read-write mode. If the file doesn't exist, it won't create a new one.
        fstream file(fileName, ios::binary | ios::in | ios::out);
        
        if (!file.is_open()) {
            cerr << "Failed to open file for writing." << endl;
            return;
        }

        // Calculate the offset based on the page number
        // Note: pageNumber starts from 0
        int offset = PAGE_SIZE * (pageNumber);
        file.seekp(offset, ios::beg); // Seek to the correct position for writing

        // Write the page data to the specific location
        if (!file.write(data.data(), PAGE_SIZE)) {
            cerr << "Failed to write page to file." << endl;
        }
        file.close();
    }

    void appendToFile(const string& fileName) {
        writeSlotDirectory(); // Ensure the slot directory is written to data before updating page

        // Open the file in append mode. This will add data to the end of the file.
        fstream file(fileName, ios::binary | ios::out | ios::app);

        if (!file.is_open()) {
            cerr << "Failed to open file for appending." << endl;
            return;
        }

        // Write the page data to the end of the file
        if (!file.write(data.data(), PAGE_SIZE)) {
            cerr << "Failed to append page to file." << endl;
        }
        file.close();
    }

};

class LinearHashIndex {

private:
    const int BLOCK_SIZE = 4096;
    
    vector<bitset<8>> index; // hash index, size 8 sice we are assuming we wont have more than 256 regular buckets

    vector<int> ids; // Store vector of all ID's to test at the end. FOR TESTING.

    // vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeOFBlock; // Next place to write a new OF page
    string fName;      // Name of index file
    Page page;
    int storageUsed;
    int recordsFound; // For teting, should find 50 records

    // TEMP FOR ADD/REMOVE FUNCTIONALITY FOR PAGES
    int page_num = 0;

    // Gets the size of the data file, used for testing.
    void getFileSize() {
    // Open the file in binary mode
        ifstream file(fName, ios::binary | ios::ate);

        if (file.is_open()) {
            streampos size = file.tellg();
            cout << "Size of the file: " << size << " bytes." << endl;
            file.close();
        } else {
            cout << "UNABLE TO OPEN IN FILESIZE" << endl;
        }
    }


    // Insert new record into index
    void insertRecord(Record record) {
        // cout << "Inserting record with name: " << record.name <<endl;
        // Increase overall used space one record at a time, uesd to see if we increase i or not
        storageUsed += record.toString().length() + sizeof(int) * 2;
        checkIncreaseStorage(); // check if we need to increase space, if we do, this will create space for our new record/reoganize pages

        // Get the hash index for the record
        bitset<8> hash = hashFunction(record.id);

        // get the index of the page that it will be stored in. May be put in bit flipped page
        int page_idx = searchLastIBits(hash);

        // insert record at that page
        if (page_idx != -1) {
            // Read in page that record should be inserted into
            page = readPage(page_idx);
            // write the record to the page
            while (!page.writeRecord(record)) {
                // couldn't write page because no space, must write in overflow
                // if overflow page exists, open it and insert record here, if space
                if (page.getOverflow() > 0) { // != -1
                    page_idx = page.getOverflow() / PAGE_SIZE;
                    page = readPage(page_idx);
                } else {
                    // overflow did not exist, create an overflow page and write the record, potential infinite loop if record is too big for one page.
                    page.setOverflow(nextFreeOFBlock);
                    page.writeToFile(fName, page_idx);

                    // save the overflow of that page, write it back
                    page = Page(); // Create new page that will be stored at the nextFreeOFBlock
                    page_idx = nextFreeOFBlock / PAGE_SIZE; // write the overflow page
                    nextFreeOFBlock += PAGE_SIZE;
                }
            }
        } else {
            cerr << "No index exists for this record, this shouldn't happen so something is wrong..." << endl;
        }

        //  Final step: write the page at the index which it was written
        page.writeToFile(fName, page_idx);
    }

    void checkIncreaseStorage() {
        // Check overall space, if it is too large, we need to increase buckets n
        float load = (float) storageUsed / (float) (PAGE_SIZE * n);
        // cout << "load is: " << load << endl;
        if ( load >= 0.7 ) {
            // we're beyond capacity, must increase storage
            increaseStorage();
        }
    }

    void increaseStorage() {

        bitset<8> newIdx = bitset<8>(n); // new idx, ie: 100
        index.push_back(newIdx);

        // vector to hold records that will be moved in data
        vector<Record> toReWrite;

        // move the overflow page from this position, if it exists
        moveOverflow();

        n++; // MOVED THIS FROM ABOVE moveOverflow()
        page = Page(); // create blank page and write it to file at new page
        page.writeToFile(fName, n - 1);

        // need to extend i too? if 2^i < n, we have to increase i as well.
        if (n > pow(2, i)) {
            i++;
        }
        // reformat all previous pages, read from pages 0 - (n-1) and re-insert its records
        for (int j = 0; j < (n - 1); j++) {
            // Read page j, rewrite records to newly indexed position
            cleanRecords(toReWrite, j);
        }

        // Write the moved records into new location
        for (auto& record : toReWrite) {
            // cout << "Rewriting record with name: " << record.name << endl;
            insertRecord(record);
        }
    }

    // cleans records on specified page, setting their location value to -1, puts into moveRecords to then be reinserted at new location
    void cleanRecords(vector<Record> &moveRecords, int page_idx) {

        // Read in page to read for records
        page = readPage(page_idx);
        // flag to keep track of overflow
        // if overflow exists, loop over records
        int hasOverflow = 1;
        bitset<8> newIdx = bitset<8>(n - 1);

        // for each j page and it's overflow pages (if exists)
        while (hasOverflow) {
            // get record
            // if last i bits match newIdx, set offset in slot dir to -1 (delete from the page), add record to toRewrite vector
            // write the cleaned up page back to memory / delete the page
            // insert records from toRewrite
            for (int k = 0; k < page.getRecordSize(); k++) {
                // Get records from page
                string retrievedString(page.data.begin() + page.slotDirectory[k].first, page.data.begin() + page.slotDirectory[k].first + page.slotDirectory[k].second);
                Record record = Record::stringToRecord(retrievedString);
                // Mask to extract the last i bits
                bitset<8> mask((1 << i) - 1);

                // Extract the last i bits from the searchBits
                bitset<8> lastIBitsOfSearch = hashFunction(record.id) & mask;
                
                // Do we have a record that we have to move?
                if (lastIBitsOfSearch == newIdx) {
                    // we need to move this record
                    // set record at k's offset to -1
                    page.slotDirectory[k].first = -1;
                    storageUsed -= record.toString().length() + sizeof(int) * 2;

                    // add the record to reWrite record list
                    moveRecords.push_back(record);
                }
            }
            // All records scanned in this page, write the page back to memory at original location
            page.writeToFile(fName, page_idx);

            // Read in the overflow page if it exists
            if (page.getOverflow() > 0) { // != -1
                page_idx = (page.getOverflow() / PAGE_SIZE);
                page = readPage(page_idx);
                hasOverflow = 1;
            } else {
                hasOverflow = 0;
            }
        // Repeat above actions on the overflow page, or break loop
        }
    }

    // Moves the overflow at the extended position to the end of the datafile.
    void moveOverflow() {
        // Location of current new page / where the overflow is stored if it exists
        int overflowIdx = (n) * PAGE_SIZE;
        // check if a page exists here
        // if overflow Idx is not same as nextfreeofblock, we know that OF pages have been written
        if (overflowIdx != nextFreeOFBlock) {
            int matchingOFPage;
            for (int j = 0; j < n; j++) {
                page = readPage(j);
                if (page.getOverflow() == overflowIdx) {
                    matchingOFPage = j;
                    break;
                }
            }
            // We have now found this page, update overflow idx
            int oldOfPage = page.getOverflow();

            page.setOverflow(nextFreeOFBlock);
            page.writeToFile(fName, matchingOFPage);

            // write the overflow page to new location
            page = readPage(oldOfPage / PAGE_SIZE);
            page.appendToFile(fName); // append overflow to the end of the file
        }

        // index nextFreeOFBlock
        nextFreeOFBlock += PAGE_SIZE;
    }

    // search the last i bits
    // return index to page containing those bits
    int searchLastIBits(const bitset<8>& searchBits) {
        // Mask to extract the last i bits
        bitset<8> mask((1 << i) - 1);

        // Extract the last i bits from the searchBits
        bitset<8> lastIBitsOfSearch = searchBits & mask;

        // Iterate over the vector and compare for an exact match
        for (int j = 0; j < index.size(); j++) {
            bitset<8> lastIBitsOfItem = index[j] & mask;
            if (lastIBitsOfItem == lastIBitsOfSearch) {
                return j; // Exact match found
            }
        }

        int msbPosition; // index for the MSB in the last i bits
        if (pow(2, i) == n) {
            // The next n increase will increase i, so check one bit over
            msbPosition = i;
        }  else {
            msbPosition = i - 1;
        }
        // Flip the MSB of the last i bits
        bitset<8> flippedLastIBitsOfSearch = lastIBitsOfSearch ^ (bitset<8>(1) << msbPosition);

        // Repeat the search with flipped MSB
        for (int j = 0; j < index.size(); j++) {
            bitset<8> lastIBitsOfItem = index[j] & mask;
            if (lastIBitsOfItem == flippedLastIBitsOfSearch) {
                return j; // Match found with flipped MSB
            }
        }
        cerr << "Coudln't find page at search bits. This shouldn't happen, terminating." << endl;
        exit(1); 
        // return -1; // No match found even after flipping MSB (Should not happen)
    }

    bitset<8> hashFunction(int id) {
        return bitset<8>(id % 216); // Assuming you want to use mod 216 for some hashing logic
    }

    // Write the hash index to the file to be re-retrieved later (if needed), written to very end of data file
    void writeIndexToFile() {

        return; //TODO
    }

public:
    LinearHashIndex(string indexFileName) {
        n = 4; // Start with 4 buckets in index
        i = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        nextFreeOFBlock = n * PAGE_SIZE; // next free overflow block will go after potential max buckets
        fName = indexFileName + ".data";

        // Create your EmployeeIndex file and write out the initial 4 buckets
        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately

        // Create file
        ofstream outputFile(fName);
        if (!outputFile.is_open()) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        outputFile.close();

        // write out initial 4 buckets [00, 01, 10, 11]
        // write a blank page for each bucket
        for (int j = 0; j < n; j++) {
            index.push_back(bitset<8>(j));
            page = Page();
            page.writeToFile(fName, j); // write that page to file at page location j
        }
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        ifstream file(csvFName);
        string line;
        // vector<Record> records; // TEMPORARY keep records in a vector for testing

        // check if the file opened
        if (!file.is_open()) {
            cerr << "Cannot open file\n";
            return;
        }

        // read each line from the file
        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields; // record fields

            // split the line into fields using comma delim
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // create record object from the fields and store in vector
            ids.push_back(Record(fields).id);
            insertRecord(Record(fields));
        }

        // Write the index to the end of the file.
        writeIndexToFile();

        // // _______________________________
        //  //  print out binary numbers for testing
        // for (const auto& bin : index) {
        //     cout << bin << endl;
        // }

        // // Search all ids for testing
        // for (int j = 0; j < ids.size(); j++) {
        //     cout << endl;
        //     findRecordById(ids[j]);
        //     cout << endl;
        // }
        // cout << "Total records found is: " << recordsFound << endl;
        // // _______________________________

        file.close();
        
    }

    // Read page #pageNumber and return as a Page variable.
    // Used to reaccess pages.
    Page readPage(int pageNumber) {
        // read a page from memory, clean it up if deleted entries
        ifstream file(fName, ios::binary);
        if (!file.is_open()) {
            cerr << "Failed to open file for reading." << endl;
            exit(1);
        }

        // Calculate the offset to the desired page
        file.seekg(pageNumber * PAGE_SIZE, ios::beg);

        // Read the page data
        vector<char> buffer(PAGE_SIZE);
        if (!file.read(buffer.data(), PAGE_SIZE)) {
            cerr << "Failed to read page from file." << endl;
            cout << "THIS IS THE ERROR IM RUNNING INTO?" << endl;
            // file.close();
            exit(1);
        }
        file.close(); // Close the file after reading
        // [data ... data ... start_slot_dir: [(offset, size),...,(offset, size)], num_records, free_space, overflow_ptr]
        // Extract the slot directory and records
        // Page page;
        int numSlots;
        int freeSpace;
        int overflowPage; 
        // copy numSlots
        memcpy(&numSlots, &buffer[PAGE_SIZE - sizeof(int) * 3], sizeof(int));
        // copy freeSpace
        memcpy(&freeSpace, &buffer[PAGE_SIZE - sizeof(int) * 2], sizeof(int));
        // copy ptr to overflow
        memcpy(&overflowPage, &buffer[PAGE_SIZE - sizeof(int)], sizeof(int));

        // cout << "OverflowPage is: " << overflowPage << " for page number: " << pageNumber << endl;
        Page openedPage(overflowPage); // save into new page with overflow stored here, two pages in main memory now

        int directoryStart = PAGE_SIZE - sizeof(int) * 3;

        for (int i = 0; i < numSlots; ++i) {
            int start, length;
            directoryStart -= sizeof(int);
            memcpy(&length, &buffer[directoryStart], sizeof(int));
            directoryStart -= sizeof(int);
            memcpy(&start, &buffer[directoryStart], sizeof(int));

            // Dont add a record if it's been deleted, deleted records marked as -1
            if (start != -1) {
                // Extract the record string
                string recordString(buffer.begin() + start, buffer.begin() + start + length);
                Record record = Record::stringToRecord(recordString);
                // Write the record back to the page
                if (!openedPage.writeRecord(record)) {
                    cerr << "Failed to write record to page." << endl;
                    exit(1);
                }
            }
        }

        return openedPage;
    }

    // Given an ID, find the relevant record and print it
    void findRecordById(int id) {
        int hasOverflow; // Tag to check if there is an overflow page to check
        // Get the hash index for that id
        int pg = searchLastIBits(hashFunction(id));
        // read the page in at that location in memory
        page = readPage(pg);

        // Search the page and the overflow pages
        do {
            hasOverflow = 0; // clear flag if not first pass
            // get record
            // if last i bits match newIdx, set offset in slot dir to -1 (delete from the page), add record to toRewrite vector
            // write the cleaned up page back to memory / delete the page
            // insert records from toRewrite
            for (int k = 0; k < page.getRecordSize(); k++) {
                // Get records from page
                string retrievedString(page.data.begin() + page.slotDirectory[k].first, page.data.begin() + page.slotDirectory[k].first + page.slotDirectory[k].second);
                Record record = Record::stringToRecord(retrievedString);

                // do we have a match?
                if (id == record.id) {
                    cout << "Record with id: " << id << " has been found in page: " << pg << " or in it's overflow." << endl;
                    recordsFound++; // for testing all records
                    record.print(); // Print the match to show that we've retrieved it
                }
            }

            // Read in the overflow page if it exists
            if (page.getOverflow() > 0) { // ORIGINAL DELETED WAS != -1
                page = readPage(page.getOverflow() / PAGE_SIZE);
                hasOverflow = 1;
            }
        // Repeat above actions on the overflow page, or break loop
        } while (hasOverflow == 1);
    }
};
