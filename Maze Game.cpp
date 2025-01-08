#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

int toInt(const std::string& str);
std::string getValue(const std::string& str);
void strCopy(char* arr, std::string str);
void printWithPadding(const std::string message, const unsigned emptyRows = 0, const bool newRow = true);
void clearFileContent(const std::string& filename);
std::string loadMenu();
void clearConsole();
void promptAndClearConsole(std::string message);
void failedInput(std::string message);
std::string boolToString(bool value);

const std::string PADDING = "    ";
const std::string USERS_DATA_FILE = "files//users.data.txt";
const std::string INITIAL_LEVEL_FILE = "files//level";
const std::string LEVELS_STATE_FILE = "files//level//id";
const short NUMBER_OF_LEVELS = 5;
const std::string levelMessages = "Choose the number of a level to start playing the corresponding level!";
const std::string menuMessages = "Welcome to Maze Game \n\n\nChoose one of the following by entering the corresponding number :\n\n1.Register\n\n2.Login\n\n";

struct Level {
    size_t id;
    unsigned numberOfLevel;
    char** map;
    unsigned coins;
    bool keyFound;
    unsigned size;

    static std::vector<Level> levels;

    Level(unsigned level, size_t currentId = generateRandomId()) {
        numberOfLevel = level;
        map = getMap(level);
        coins = getCoins(map);
        keyFound = false;
        size = getSize(level);
        id = currentId;
    }

    static Level* getLevelById(size_t id) {
        std::string path = LEVELS_STATE_FILE + std::to_string(id);
        std::ifstream inputFile(path);

        if (!inputFile) {
            return nullptr;
        }

        std::string current;
        std::getline(inputFile, current);
        Level level(toInt(current));  // Use the first line as level number

        // Read the size, coins, and keyFound properties
        std::getline(inputFile, current);
        level.size = toInt(current);
        std::getline(inputFile, current);
        level.coins = toInt(current);
        std::getline(inputFile, current);
        level.keyFound = toInt(current);

        // Allocate memory for the map
        char** map = new char* [level.size];
        for (int i = 0; i < level.size; ++i) {
            std::getline(inputFile, current);
            map[i] = new char[current.size() + 1];  // Allocate memory for each row
            strCopy(map[i], current);              // Copy row into map
        }
        level.map = map;
        inputFile.close();
        for (int i = 0; i < level.size; ++i) {
            delete[] map[i];
        }
        delete[] map;
        return &level;
    }

    static size_t generateRandomId() {
        size_t id;
        while (exists(id = rand()));
        return id;
    }

    static bool exists(size_t id) {
        std::string path = LEVELS_STATE_FILE + std::to_string(id);
        std::fstream file(path);
        if (file) {
            return 1;
        }
        return 0;
    }

    static void visualizeLevel(Level level) {
        clearConsole();
        printWithPadding("Coins: " + std::to_string(level.coins));
        printWithPadding("Key Found: " + boolToString(level.keyFound), 2);
        for (int i = 0; i < level.size; i++) {
            printWithPadding(" ", 0, false);
            for (int j = 0; j < level.size; j++) {
                std::cout << level.map[i][j] <<  " ";
            }
            std::cout << "\n";
        }
    }

    static void loadLevels(std::string username, unsigned levelReached, unsigned totalCoins, std::vector<size_t> levelIds) {
        printWithPadding("", 3);
        printWithPadding(levelMessages, 2);
        for (unsigned i = 0; i < levelIds.size(); ++i) {
            //std::string status = i + 1 < level ? "Completed" : "NOT COMPLETED";
            printWithPadding("Level: " + std::to_string(i + 1));
        }
        unsigned option;
        std::cin >> option;
        Level* current = Level::getLevelById(levelIds[option - 1]);
        if (current != nullptr) {
            Level::visualizeLevel(*current);
            return;
        }
        Level level(levelReached);
        Level::visualizeLevel(level);
        std::cout;
    }

    int getSize(unsigned level) {
        std::string add = INITIAL_LEVEL_FILE + std::to_string(level) + ".txt";
        std::ifstream inputFile(add);
        if (!inputFile) {
            std::cerr << "Error: Could not open level file!" << std::endl;
            return -1;
        }
        std::string current;
        std::getline(inputFile, current);
        return toInt(current);
    }

    static char** getMap(unsigned level) {
        std::string add = INITIAL_LEVEL_FILE + std::to_string(level) + ".txt";
        std::ifstream inputFile(add);
        if (!inputFile) {
            std::cerr << "Error: Could not open level file!" << std::endl;
            return nullptr;
        }
        std::string current;
        std::getline(inputFile, current);
        unsigned size = toInt(current);

        char** map = new char* [size + 1];  // +1 for nullptr at the end

        unsigned row = 0;
        while (std::getline(inputFile, current) && row < size) {
            map[row] = new char[current.size() + 1];
            strCopy(map[row], current);
            ++row;
        }

        map[row] = nullptr;
        inputFile.close();
        return map;
    }

    unsigned getCoins(char** map) {
        unsigned counter = 0;
        for (unsigned i = 0; map[i] != nullptr; i++) {
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

    void save() {
        std::string path = LEVELS_STATE_FILE + std::to_string(id);
        std::ofstream outputFile(path);
        outputFile << toString();
        outputFile.close();
    }

    std::string toString() const {
        std::string levelData = "Level: " + std::to_string(numberOfLevel) + "\n";
        levelData += "Size: " + std::to_string(size) + "\n";
        levelData += "Coins: " + std::to_string(coins) + "\n";
        levelData += "KeyFound: " + std::to_string(keyFound ? 1 : 0) + "\n";
        levelData += "Map:\n";

        for (unsigned i = 0; map[i] != nullptr; ++i) {
            levelData += map[i];
            levelData += "\n";
        }

        return levelData;
    }
};

struct User {
    unsigned levelReached;
    std::vector<size_t> levelIds;
    unsigned totalCoins;
    std::string username;
    std::string password;

    static std::vector<User> userList;

    User(std::string username = "", std::string password = "") {
        this->username = username;
        this->password = password;
        totalCoins = 0;
        levelReached = 1;
        for (int i = 0; i < NUMBER_OF_LEVELS; i++) {
            Level level(i + 1);
            size_t id = 0;
            levelIds.push_back(id);
        }
    }

    static void loadAllUsers() {
        userList.clear();
        std::ifstream inputFile(USERS_DATA_FILE);
        if (!inputFile) {
            std::cerr << "Error opening users file!" << std::endl;
            return;
        }

        std::string current;
        while (std::getline(inputFile, current)) {
            User user;
            user.username = getValue(current);
            std::getline(inputFile, current);
            user.password = getValue(current);
            std::getline(inputFile, current);
            user.levelReached = toInt(getValue(current));

            for (unsigned i = 0; i < NUMBER_OF_LEVELS; ++i) {
                std::getline(inputFile, current);
                user.levelIds[i] = toInt(getValue(current));
            }

            std::getline(inputFile, current);
            user.totalCoins = toInt(getValue(current));
            std::getline(inputFile, current);
            current;
            userList.push_back(user);
        }

        inputFile.close();
    }

    static User* getUser(const std::string& username) {
        for (unsigned i = 0; i < userList.size(); ++i) {
            if (userList[i].username == username) {
                return &userList[i];
            }
        }
        //std::cerr << "User not found!" << std::endl;
        return nullptr;
    }

    static void registration(User* user) {
        std::ofstream outputFile(USERS_DATA_FILE, std::ios::app);
        outputFile << (*user).toString();
        outputFile << "---------------------" << "\n";
        outputFile.close();
        loadAllUsers();
    }

    void enterlevelsMenu() {
        Level::loadLevels(username, levelReached, totalCoins, levelIds);
    }

    std::string toString() {
        std::string levelStatesStr;
        for (unsigned i = 0; i < levelIds.size(); ++i) {
            levelStatesStr += "level" + std::to_string(i + 1) + "Id: " + std::to_string((long long)(levelIds[i])) + "\n";
        }
        return "username: " + username + "\n"
            + "password: " + password + "\n"
            + "levelReached: " + std::to_string(levelReached) + "\n"
            + levelStatesStr
            + "totalCoins: " + std::to_string(totalCoins) + "\n";
    }
};

std::vector<Level> Level::levels;
std::vector<User> User::userList;

void clearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get info for the size of the console buffer
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Calculate the amount of symbols in the console buffer
    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the buffer with spaces
    COORD topLeft = { 0, 0 };
    DWORD charsWritten;
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, topLeft, &charsWritten);

    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, topLeft, &charsWritten); // set the default attributes (color)

    SetConsoleCursorPosition(hConsole, topLeft); // Moves the cursor to the top left corner
    std::cout << std::endl; // leaved one row top padding
}

void promptAndClearConsole(std::string message) {
    // Clears the error state and ignore the invalid input
    std::cin.clear();
    std::string dummy;
    std::getline(std::cin, dummy);
    printWithPadding("Press ENTER to " + message);
    std::cin.get(); // waits for user to press ENTER
    clearConsole();  // clears the console before reloading
}

std::string registrationAttempt() {
    std::string username, password;
    while (true) {
        printWithPadding("type \"back\" to go back or \"exit\" to exit the program", 2);
        printWithPadding("REGISTRATION", 2);
        printWithPadding("Username:");
        std::cin >> username;
        if (username == "back") {
            clearConsole();
            return loadMenu();
        }
        if (username == "exit") {
            clearConsole();
            return "";
        }
        User* user = User::getUser(username);
        if (user != nullptr) {
            failedInput("Username already exists!");
        }
        else if (username.length() > 50) {
            failedInput("Username is too long (2-50 characters)!");
        }
        else if (username.length() < 2) {
            failedInput("Username is too short (2-50 characters)!");
        }
        else {
            break;
        }
    }
    printWithPadding("Password:");
    std::cin >> password;
    std::cout << std::endl;
    User user(username, password);
    User::registration(&user);
    printWithPadding("Successful registration!");
    std::cout << PADDING << "Welcome, " << username << "!" << std::endl;
    std::cout << std::endl;
    promptAndClearConsole("start your adventure");
    return username;
}

std::string loginAttempt() {
    std::string username, password;
    User* user = nullptr;
    while (true) {
        printWithPadding("type \"back\" to go back or \"exit\" to exit the program", 2);
        printWithPadding("LOGIN", 2);
        printWithPadding("Username:");
        std::cin >> username;
        if (username == "back") {
            clearConsole();
            return loadMenu();
        }
        if (username == "exit") {
            clearConsole();
            return "";
        }
        user = User::getUser(username);
        if (user == nullptr) {
            failedInput("Username does't exist!");
        }
        else {
            break;
        }
    }
    while (true) {
        printWithPadding("Password:");
        std::cin >> password;
        if ((*user).password != password) {
            failedInput("Wrong password!");
            loginAttempt();
            return username;
        }
        else {
            break;
        }
    }
    delete user;
    return username;
}

void clearFileContent(const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        std::cerr << "Error opening the file!" << std::endl;
        return;
    }
    file.close();
}

void strCopy(char* arr, std::string str) {
    for (int i = 0; i < str.size(); i++) {
        arr[i] = str[i];
    }
    arr[str.size()] = 0;
}

int toInt(const std::string& str) {
    for (char c : str) {
        if (c < '0' || c > '9') {
            //std::cerr << "Not a number! String: " << str << std::endl;
            return -1;
        }
    }
    int number = 0;
    for (char c : str) {
        number = number * 10 + (c - '0');
    }

    return number;
}

std::string getValue(const std::string& str) {
    size_t pos = str.find(": ");
    if (pos != std::string::npos) {
        return str.substr(pos + 2);
    }
    return str;
}

std::string boolToString(bool value) {
    return (value ? "true" : "false");
}

void printWithPadding(const std::string message, const unsigned emptyRows, const bool newRow) {
    std::vector<std::string> lines;
    std::string line;

    for (size_t i = 0; i < message.size(); ++i) {
        if (message[i] == '\n') {
            lines.push_back(line);
            line.clear();
        }
        else {
            line += message[i];
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }

    for (unsigned i = 0; i < lines.size(); ++i) {
        std::cout << PADDING << lines[i];
        if (newRow) {
            std::cout << std::endl;
        }
    }

    for (unsigned i = 0; i < emptyRows; i++) {
        std::cout << std::endl;
    }
}

void failedInput(std::string message) {
    printWithPadding(message);
    promptAndClearConsole("reload");
}


std::string loadMenu() {
    std::string optionStr;
    unsigned option;
    while (true) {
        printWithPadding("type \"back\" to go back or \"exit\" to exit the program", 2);
        printWithPadding(menuMessages);
        printWithPadding("Enter your choice: ", 0, false);
        std::cin >> optionStr;
        if (optionStr == "exit" || optionStr == "back") {
            clearConsole();
            return "";
        }
        option = toInt(optionStr);
        if (option != -1 && (option == 1 || option == 2)) {
            break;
        }
        failedInput("Invalid number!");
    }
    if (option == 1) {
        clearConsole();
        return registrationAttempt();
    }
    else if (option == 2) {
        clearConsole();
        return loginAttempt();
    }
}

int main() {
    srand(time(0));
    User::loadAllUsers();
    std::string username = loadMenu();
    if (username == "") {
        return 0;
    }
    User user = *User::getUser(username);
    clearConsole();
    //user.enterlevelsMenu();
}
