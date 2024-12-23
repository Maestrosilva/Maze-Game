#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

int toInt(const std::string& str);
std::string getValue(const std::string& str);

const std::string PADDING = "   ";
const std::string USERS_DATA_FILE = "files//users.data.txt";
const std::string LEVEL_DATA_FILE = "files//users.level.";
const short LINES_OF_DATA_PER_USER = 8;

struct Level {
    unsigned numberOfLevel;
    char** map;
    unsigned coins;
    bool keyFound;

    static std::vector<Level> levels;
    // Constructor that sets up a new level
    Level(unsigned level) {
        map = getMap(level);
        coins = getCoins(map);
        keyFound = containsKey(map);
    }

    // Constructor to copy state from another Level (used for restoring state)
    Level(Level* currentLevelAddress) {
        map = currentLevelAddress->map;  // Reuse the existing map
        coins = currentLevelAddress->coins;  // Restore state (coins)
        keyFound = currentLevelAddress->keyFound;  // Restore state (key found)
    }

    ~Level() {
        if (map) {
            for (unsigned i = 0; map[i] != nullptr; ++i) {
                delete[] map[i];
            }
            delete[] map;
        }
    }

    // Static function to load a level's map from a file
    static char** getMap(unsigned level) {
        std::ifstream inputFile(LEVEL_DATA_FILE + std::to_string(level) + ".txt");
        std::string current;
        std::getline(inputFile, current);
        unsigned size = toInt(current);
        char** map = new char* [size + 1];
        unsigned row = 0;
        while (std::getline(inputFile, current) && row < size) {
            std::strcpy(map[row], current.c_str());
            ++row;
        }
        map[row] = nullptr;
        inputFile.close();
        return map;
    }

    unsigned getCoins(char** map) {
        unsigned counter = 0;
        for (unsigned i = 0; map[i] != nullptr; ++i) {
            for (unsigned j = 0; map[i][j] != '\0'; ++j) {
                counter += map[i][j] == 'C';
            }
        }
        return counter;
    }

    bool containsKey(char** map) {
        for (unsigned i = 0; map[i] != nullptr; ++i) {
            for (unsigned j = 0; map[i][j] != '\0'; ++j) {
                if (map[i][j] == '&') {
                    return true;
                }
            }
        }
        return false;
    }
};

struct User {
    // User Game Data
    unsigned levelReached;
    Level* levelOneState = nullptr;
    Level* levelTwoState = nullptr;
    Level* levelThreeState = nullptr;
    unsigned totalCoins;

    // Authentication Data
    std::string username;
    std::string password;

    static std::vector<User> userList;  // Declare static member

    static void loadAllUsers() {
        userList.clear();
        std::ifstream inputFile(USERS_DATA_FILE);
        std::string current;

        while (std::getline(inputFile, current)) {
            User user;
            user.username = getValue(current);
            std::getline(inputFile, current);
            user.password = getValue(current);
            std::getline(inputFile, current);
            user.levelReached = toInt(getValue(current));

            // Load level one state
            std::getline(inputFile, current);
            if (current != "null") {
                unsigned levelAddress = toInt(current);  // This should be an address or index
                // Here, you should retrieve the corresponding Level object from the 'levels' vector
                user.levelOneState = &Level::levels[levelAddress];
            }

            // Load level two state
            std::getline(inputFile, current);
            if (current != "null") {
                unsigned levelAddress = toInt(current);  // This should be an address or index
                user.levelTwoState = &Level::levels[levelAddress];
            }

            // Load level three state
            std::getline(inputFile, current);
            if (current != "null") {
                unsigned levelAddress = toInt(current);  // This should be an address or index
                user.levelThreeState = &Level::levels[levelAddress];
            }

            // Load total coins
            std::getline(inputFile, current);
            user.totalCoins = toInt(getValue(current));

            userList.push_back(user);
        }

        inputFile.close();
    }

    static User* getUser(const std::string& username) {
        for (auto& user : userList) {
            if (user.username == username) {
                return &user;
            }
        }
        std::cerr << "User not found!" << std::endl;
        return nullptr;  // Return nullptr if user not found
    }
};

// Define the static member outside the struct
std::vector<User> User::userList;  // This resolves the LNK2001 error

int toInt(const std::string& str) {
    int number = 0;
    bool negative = false;
    unsigned index = 0;
    if (str[index] == '-') {
        negative = true;
        index++;
    }
    while (index < str.size()) {
        char c = str[index++];
        if (c < '0' || c > '9') {
            std::cerr << "Not a number!" << std::endl;
            return 0;  // Return 0 if the input is invalid
        }
        number = number * 10 + (c - '0');
    }
    return negative ? -number : number;
}

std::string getValue(const std::string& str) { // "<filed_name>: <field_value>" -> "<field_value>"
    size_t pos = str.find(": ");
    if (pos != std::string::npos) {
        return str.substr(pos + 2);
    }
    return str;
}

int main() {
    User::loadAllUsers();
}
