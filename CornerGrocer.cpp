/*
 * ============================================================
 * File:        CornerGrocer.cpp
 * Author:      Eden Vertus
 * Course:      CS-210
 * Date:        2/19/2026
 * 
 * Description: Item-tracking program for the Corner Grocer.
 *              Reads CS210_Project_Three_Input_File.txt, counts
 *              how often each grocery item was purchased, backs
 *              the results up to frequency.dat automatically,
 *              then presents a 4-option menu.
 * ============================================================
 */

#include <iostream>   
#include <fstream>   
#include <map>        
#include <string>     
#include <limits>     
#include <algorithm>  
#include <cctype>     

using namespace std;


class GroceryTracker {
private:
    // ------------------------------------------------------------
    // itemFrequency: maps each grocery item name (string) to the
    // number of times it was purchased (int).  std::map keeps
    // entries sorted alphabetically, which makes printed output
    // easy to read.
    // ------------------------------------------------------------
    map<string, int> itemFrequency;

    // ------------------------------------------------------------
    // ToTitleCase: converts a word so the first letter is upper-
    // case and all remaining letters are lowercase.
    // Used so "apple", "Apple", and "APPLE" all map to "Apple",
    // preventing duplicate keys in the map.
    // ------------------------------------------------------------
    string ToTitleCase(const string& word) const {
        if (word.empty()) {
            return word;
        }
        string result = word;
        result[0] = static_cast<char>(toupper(static_cast<unsigned char>(result[0])));
        for (size_t i = 1; i < result.size(); ++i) {
            result[i] = static_cast<char>(tolower(static_cast<unsigned char>(result[i])));
        }
        return result;
    }

    // ------------------------------------------------------------
    // StripCarriageReturn: removes a trailing '\r' if present.
    // Needed because the input file uses Windows line endings
    // (CRLF), and a stray '\r' would corrupt the item name.
    // ------------------------------------------------------------
    string StripCarriageReturn(const string& word) const {
        if (!word.empty() && word.back() == '\r') {
            return word.substr(0, word.size() - 1);
        }
        return word;
    }

    // ------------------------------------------------------------
    // ToLower: converts the entire string to lowercase.
    // Used only for case-insensitive searching in GetFrequency,
    // not for storing items in the map.
    // ------------------------------------------------------------
    string ToLower(const string& word) const {
        string result = word;
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = static_cast<char>(tolower(static_cast<unsigned char>(result[i])));
        }
        return result;
    }

public:
    // ============================================================
    // LoadFromFile
    // Opens the given filename, reads one item per line, and
    // increments its count in the map.  Returns true on success.
    // ============================================================
    bool LoadFromFile(const string& filename) {
        ifstream inFile(filename);

        // Report failure and let main() decide what to do
        if (!inFile.is_open()) {
            cerr << "[ERROR] Cannot open input file: " << filename << "\n";
            return false;
        }

        string item;
        while (inFile >> item) {
            item = StripCarriageReturn(item);   // handle Windows CRLF
            item = ToTitleCase(item);           // normalise capitalisation
            itemFrequency[item]++;              // increment count (auto-initialises to 0)
        }

        inFile.close();
        return true;
    }

    // ============================================================
    // SaveBackup
    // Writes every item-frequency pair to frequency.dat.
    // Called once at start-up, with no user interaction needed.
    // Format:  ItemName Count  (one pair per line)
    // Returns true on success.
    // ============================================================
    bool SaveBackup(const string& filename) const {
        ofstream outFile(filename);

        if (!outFile.is_open()) {
            cerr << "[ERROR] Cannot create backup file: " << filename << "\n";
            return false;
        }

        // Iterate map (already sorted alphabetically) and write each entry
        for (const auto& entry : itemFrequency) {
            outFile << entry.first << " " << entry.second << "\n";
        }

        outFile.close();
        return true;
    }

    // ============================================================
    // GetFrequency  (supports Menu Option 1)
    // tries an exact Title Case match.
    // if no exact match, does a case-insensitive prefix
    // search so the user can type "apple" and still find "Apples".
    // matchedName is set to the actual key found in the map so
    // the correct full name is displayed to the user.
    // Returns 0 if nothing matches.
    // ============================================================
    int GetFrequency(const string& item, string& matchedName) const {
        string titleItem = ToTitleCase(item);

        // Step 1: exact match
        auto it = itemFrequency.find(titleItem);
        if (it != itemFrequency.end()) {
            matchedName = it->first;
            return it->second;
        }

        // Step 2: prefix match so "apple" finds "Apples"
        string lowerSearch = ToLower(item);
        for (const auto& entry : itemFrequency) {
            string lowerKey = ToLower(entry.first);
            if (lowerKey.find(lowerSearch) == 0) {
                matchedName = entry.first;
                return entry.second;
            }
        }

        matchedName = item;   // keep original for "not found" message
        return 0;             // no match found
    }

    // ============================================================
    // PrintAllFrequencies  (Menu Option 2)
    // Prints every item paired with its numeric frequency count.
    // ============================================================
    void PrintAllFrequencies() const {
        cout << "\n------ Item Purchase Frequencies ------\n";
        for (const auto& entry : itemFrequency) {
            // Left-align item name in a 15-char field for readability
            cout << entry.first;
            // Pad with spaces so the numbers line up neatly
            int padding = 15 - static_cast<int>(entry.first.size());
            for (int i = 0; i < padding; ++i) {
                cout << " ";
            }
            cout << entry.second << "\n";
        }
        cout << "---------------------------------------\n";
    }

    // ============================================================
    // PrintHistogram  (Menu Option 3)
    // Prints each item name followed by one '*' per purchase.
    // ============================================================
    void PrintHistogram() const {
        cout << "\n------- Item Frequency Histogram -------\n";
        for (const auto& entry : itemFrequency) {
            cout << entry.first;
            // Pad so asterisks start in a consistent column
            int padding = 15 - static_cast<int>(entry.first.size());
            for (int i = 0; i < padding; ++i) {
                cout << " ";
            }
            // Print one '*' for each purchase
            for (int i = 0; i < entry.second; ++i) {
                cout << "*";
            }
            cout << "\n";
        }
        cout << "----------------------------------------\n";
    }
};


// Display Menu
// Prints the four numbered options to the console.
void DisplayMenu() {
    cout << "\n========== Corner Grocer Tracker ==========\n";
    cout << "  1. Search for a specific item's frequency\n";
    cout << "  2. Display all item frequencies\n";
    cout << "  3. Display frequency histogram\n";
    cout << "  4. Exit\n";
    cout << "===========================================\n";
    cout << "Enter your choice (1-4): ";
}


// ============================================================
// GetValidMenuChoice
// Reads an integer from cin, validates it is 1-4, and returns
// the valid choice.  Loops and re-prompts on bad input, so the
// program never crashes from non-numeric or out-of-range input.
// ============================================================
int GetValidMenuChoice() {
    int choice;

    while (true) {
        cin >> choice;

        if (cin.fail()) {
            // Non-numeric input: clear the error flag and discard the bad token
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Please enter a number between 1 and 4: ";
        }
        else if (choice < 1 || choice > 4) {
            // Numeric but out of range
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Choice must be 1, 2, 3, or 4. Try again: ";
        }
        else {
            // Valid input - consume trailing newline and return
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}


// ============================================================
// main
// Entry point: loads data, creates backup, runs menu loop.
// ============================================================
int main() {

    // File names as constants for easy future maintenance
    const string INPUT_FILE = "CS210_Project_Three_Input_File.txt";
    const string BACKUP_FILE = "frequency.dat";

    // Instantiate the tracker object
    GroceryTracker tracker;

    // Load purchase data from the daily input file.
    // If the file cannot be opened, report and exit gracefully.
    if (!tracker.LoadFromFile(INPUT_FILE)) {
        cout << "\nProgram cannot continue without the input file.\n";
        cout << "Make sure \"" << INPUT_FILE << "\" is in the same\n";
        cout << "folder as the .exe and restart the program.\n";
        return 1;   // Non-zero exit signals an error to the OS
    }

    // Automatically save the backup file (frequency.dat).
    // No user action is required for this step.
    if (tracker.SaveBackup(BACKUP_FILE)) {
        cout << "[INFO] Data backed up successfully to \"" << BACKUP_FILE << "\"\n";
    }

    // Run the interactive menu until the user chooses 4.
    int choice = 0;

    while (choice != 4) {
        DisplayMenu();
        choice = GetValidMenuChoice();

        switch (choice) {

            // individual item look-up
        case 1: {
            cout << "Enter the item name to search: ";
            string itemName;
            getline(cin, itemName);   // getline captures names with spaces

            // matchedName holds the actual map key found so the correct
            // full name is displayed even if the user typed a partial name
            string matchedName;
            int freq = tracker.GetFrequency(itemName, matchedName);

            if (freq > 0) {
                cout << "\n\"" << matchedName << "\" was purchased "
                    << freq << " time(s) today.\n";
            }
            else {
                cout << "\n\"" << itemName
                    << "\" was not found in today's purchase records.\n";
            }
            break;
        }

              // full frequency list -----
        case 2:
            tracker.PrintAllFrequencies();
            break;

            // text histogram
        case 3:
            tracker.PrintHistogram();
            break;

            // Exit
        case 4:
            cout << "\nThank you for using the Corner Grocer Tracker. Goodbye!\n";
            break;

            // This branch is unreachable (GetValidMenuChoice guarantees 1-4)
            // but is kept for defensive completeness.
        default:
            cout << "[ERROR] Unexpected error. Please restart the program.\n";
            break;
        }
    }

    return 0;
}