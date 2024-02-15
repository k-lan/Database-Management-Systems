#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>

// My added libraries 
#include <fstream>
#include <cmath> 
#include <cstring> 
using namespace std;

const int PAGE_SIZE = 4096;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    Record(){}

    void print() const {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    // Convert record to string representation for writing to file
    std::string toString() const {
        return std::to_string(id) + "|" + name + "|" + bio + "|" + std::to_string(manager_id) + "|";
    }

    // Convert a string record back to a record
    static Record stringToRecord(const std::string& str) {
        Record record;
        std::istringstream ss(str);
        std::string token;

        // Parsing using '|' delimiter
        std::getline(ss, token, '|');
        record.id = stoi(token);

        std::getline(ss, record.name, '|');
        std::getline(ss, record.bio, '|');

        std::getline(ss, token, '|');
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

    Page(int overflowPage) : data(PAGE_SIZE, '\0'), freeSpace(PAGE_SIZE - sizeof(int) * 3), overflow(overflowPage) {}
    // Open a page (still have to create)
    //Page(char )

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

    // returns a vector of records to be mofied
    // vector<Record> getRecords() {
    //     vector<Record> records;
    //     for (int i = 0; i < getRecordSize(); ++i) {
    //         string retrievedString(data.begin() + slotDirectory[i].first, data.begin() + slotDirectory[i].first + slotDirectory[i].second);
    //         records.push_back(Record::stringToRecord(retrievedString));
    //     }
    // }
    
    // Write record to the page and update slot directory
    // TODO: PROBS SAVE FILE LOCATION IN A NEW PARAMETER, PASS BY REF
    bool writeRecord(const Record& record) {
        // record.print();
        std::string recordString = record.toString();
        int recordLength = recordString.length();
        // sizeof slot directory, pointer to overflow page, free space, and num records
        size_t dirByteSize = slotDirectory.size() * sizeof(std::pair<int, int>);

        if (recordLength + sizeof(int) * 2 > freeSpace - dirByteSize) // Assure that new record and two values for directory fit
            return false;

        int start = PAGE_SIZE - freeSpace - sizeof(int) * 3; // make sure to ignore the numrecords, overflow, freespace at end
        for (char c : recordString)
            data[start++] = c;

        slotDirectory.emplace_back(start - recordLength, recordLength);
        cout << "offset is : \t" << start - recordLength << "\t recordLength is: " << recordLength << "\n";

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

    void writeToFile(const std::string& fileName, int pageNumber) {
        writeSlotDirectory(); // Ensure the slot directory is written to data before updating page
        
        // Open the file in read-write mode. If the file doesn't exist, it won't create a new one.
        fstream file(fileName, ios::binary | ios::in | ios::out);
        
        if (!file.is_open()) {
            cerr << "Failed to open file for writing." << endl;
            return;
        }

        // Calculate the offset based on the page number
        // Note: pageNumber starts from 1, so to get to page 5, the offset is 4096 * (5 - 1)
        int offset = PAGE_SIZE * pageNumber;
        file.seekp(offset, ios::beg); // Seek to the correct position for writing

        // Write the page data to the specific location
        if (!file.write(data.data(), PAGE_SIZE)) {
            cerr << "Failed to write page to file." << endl;
        }
        file.close();
    }

};

class LinearHashIndex {

private:
    const int BLOCK_SIZE = 4096;
    
    vector<bitset<8>> index; // hash index, size 8 sice we are assuming we wont have more than 256 regular buckets

    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeOFBlock; // Next place to write a new OF page
    string fName;      // Name of index file
    Page page;
    int storageUsed; // 

    // TEMP FOR ADD/REMOVE FUNCTIONALITY FOR PAGES
    int page_num = 0;
    // Insert new record into index
    void insertRecord(Record record) {

        // Steps to inserting record
        // Increase overall used space one record at a time, uesd to see if we increase i or not
        storageUsed += record.toString().length() + sizeof(int) * 2;
        checkIncreaseStorage(); // check if we need to increase space, if we do, this will create space for our new record/reoganize pages

        // Get the hash index for the record
        bitset<8> hash = hashFunction(record.id);

        // get the index of the page that it will be stored in. May be put in bit flipped page
        int page_idx = searchLastIBits(hash);
        // cout << page_idx << endl;
        // insert record at that page
        if (page_idx != -1) {
            // Read in page that record should be inserted into
            page = readPage(page_idx);
            // write the record to the page
            while (!page.writeRecord(record)) {
                // couldn't write page because no space, must write in overflow
                // if overflow page exists, open it and insert record here, if space
                if (page.getOverflow() != -1) {
                    page_idx = page.getOverflow() / PAGE_SIZE;
                    page = readPage(page_idx);
                } else {
                    // overflow did not exist, create an overflow page and write the record, potential infinite loop if record is too big for one page.
                    page = Page(nextFreeOFBlock);
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
        cout << "load is: " << load << endl;
        if ( load >= 0.7 ) {
            // we're beyond capacity, must increase storage
            increaseStorage();
        }
    }

    void increaseStorage() {
        // increase n
        n++;

        // vector to hold records that will be moved in data
        vector<Record> toReWrite; 

        // extend our hash index
        bitset<8> newIdx = bitset<8>(n - 1);
        index.push_back(newIdx);

        // move the overflow page from this position, if it exists
        moveOverflow();

        page = Page(); // create blank page and write it to file at new page
        page.writeToFile(fName, n - 1);

        // need to extend i too? if i^2 < n, we have to increase i as well.
        if (n > pow(i, 2)) {
            i++;
            // reformat all previous pages, read from pages 0 - (n-1) and re-insert its records
            for (int j = 0; j < n - 1; j++) {
                // Read page j, rewrite records to newly indexed position
                page = readPage(j); // two pages in memory at this point in time

                // TODO for each page
                // DONT FORGET OVERFLOW PAGES
                // for each j page and it's overflow pages (if exists)
                do {
                    // get record
                    // if last i bits match newIdx, set offset in slot dir to -1 (delete from the page), add record to toRewrite vector
                    // write the cleaned up page back to memory / delete the page
                    // insert records from toRewrite
                    for (int j = 0; j < page.getRecordSize(); j++) {
                        // Get records from page
                        string retrievedString(page.data.begin() + page.slotDirectory[j].first, page.data.begin() + page.slotDirectory[j].first + page.slotDirectory[j].second);
                        Record record = Record::stringToRecord(retrievedString);
                        record.print(); // test that records are correctly reading
                        // Mask to extract the last i bits
                        bitset<8> mask((1 << i) - 1);

                        // Extract the last i bits from the searchBits
                        bitset<8> lastIBitsOfSearch = hashFunction(record.id) & mask;

                        // Do we have a record that we have to move?
                        if (lastIBitsOfSearch == newIdx) {
                            // we need to move this record
                            // set record at j's offset to -1
                            page.slotDirectory[j].first = -1;
                            storageUsed -= record.toString().length() + sizeof(int) * 2;

                            // add the record to reWrite record list
                            toReWrite.push_back(record);
                        }
                    }

                } while (page.getOverflow() != -1);


                // ALSO need to fix the page if we only change n (but in this case only read one page)
            }

            // Write the moved records into new location
            for (auto& record : toReWrite) {
                insertRecord(record);
            }
        }
    }

    // Moves the overflow at the extended position to the end of the datafile.
    void moveOverflow() {
        // Location of current overflow page
        int overflowIdx = n * PAGE_SIZE;
        // check if a page exists here
        // if overflow Idx is not same as nextfreeofblock, we know that OF pages have been written
        if (overflowIdx != nextFreeOFBlock) {
            // scan thru pages to find 
            int j = 0;
            page = readPage(j); 
            // Find the page pointing to overflow page
            while (page.getOverflow() != overflowIdx && j < n) {
                page = readPage(j++);
                cout << "searching for OF page, printing in case of infinite loop..." << endl; 
            }
            // We have now found this page, update overflow idx
            int oldOfPage = page.getOverflow();
            page.setOverflow(nextFreeOFBlock);

            // write the overflow page to new location
            page = readPage(oldOfPage / PAGE_SIZE);
            page.writeToFile(fName, nextFreeOFBlock);
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

        // If here, no exact match found. Now, flip the MSB of the last i bits and search again.
        int msbPosition = i - 1;  // index for the MSB in the last i bits
        // Flip the MSB of the last i bits
        cout << "Original version of last i: " << i << " bits is: " << lastIBitsOfSearch << endl;
        bitset<8> flippedLastIBitsOfSearch = lastIBitsOfSearch ^ (bitset<8>(1) << msbPosition);
        cout << "Flipped version of last i: " << i << " bits is: "<< flippedLastIBitsOfSearch << endl;

        // Repeat the search with flipped MSB
        for (int j = 0; j < index.size(); j++) {
            bitset<8> lastIBitsOfItem = index[j] & mask;
            if (lastIBitsOfItem == flippedLastIBitsOfSearch) {
                return j; // Match found with flipped MSB
            }
        }

        return -1; // No match found even after flipping MSB
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

        // print out binary numbers for testing
        for (const auto& bin : index) {
            std::cout << bin << std::endl;
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
            insertRecord(Record(fields));
            // records.push_back(Record(fields)); // TEMP use insertrecords once implemented
        }

        // TODO: Write hash index to the end of the data file
        // writeIndexToFile();
        

        file.close();
    }

    // Read page #pageNumber and return as a Page variable.
    // Used to reaccess pages.
    Page readPage(int pageNumber) {
        cout << "\tIM READING INTO THE SPECIFIED PAGE, THE PAGE NUM IS:  " << pageNumber << "\n";

        // read a page from memory, clean it up if deleted entries
        ifstream file(fName, ios::binary);
        if (!file.is_open()) {
            cerr << "Failed to open file for reading." << endl;
            return 0;
        }

        // Calculate the offset to the desired page
        file.seekg(pageNumber * PAGE_SIZE, ios::beg);

        // Read the page data
        vector<char> buffer(PAGE_SIZE);
        if (!file.read(buffer.data(), PAGE_SIZE)) {
            cerr << "Failed to read page from file." << endl;
            file.close();
            return 0;
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

        Page openedPage(overflowPage); // save into new page with overflow stored here

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
                    return 0;
                }
                // record.print();
            }
        }

        return openedPage;
    }

    // Given an ID, find the relevant record and print it
    // Record findRecordById(int id) {
    //     return;
    // }
};
