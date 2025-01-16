#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

int main();
struct Level;
struct User;
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
std::string registrationAttempt();
std::string loginAttempt();
void sortList();
unsigned getCoinsCount(char** map);
size_t getMapSize(unsigned level, std::string path);
char** getMap(unsigned level, std::string path);
std::string scoreboard(unsigned length, std::string username);
std::string movement(std::string move, Level& level, User& user);
void updateData();
void loadLevels(User user);
unsigned* findPlayerCoords(char** map, unsigned size);
User* getUser(const std::string& username);
std::string livesToStr(unsigned short lives);
void deleteFile(const std::string& filePath);
void swap(char& a, char& b);
bool strToBool(std::string value);
unsigned* findNextPortal(unsigned short row, Level level);
bool isAround(const Level level, const char a);

const std::string PADDING = "    ";
const std::string USERS_DATA_FILE = "files//users.data.txt";
const std::string INITIAL_LEVEL_FILE = "files//level";
const std::string LEVELS_STATE_FILE = "files//level//id";
const short NUMBER_OF_LEVELS = 5;
const short NUMBER_OF_DISCOVERIES = 8;
const std::string LEVEL_MESSAGES = "Choose the number of a level to start playing the corresponding level!";
const std::string MENU_MESSAGES = "Welcome to Maze Game \n\n\nChoose one of the following by entering the corresponding number :\n\n1.Register\n\n2.Login\n\n";

struct Level {
    size_t id;
    unsigned numberOfLevel;
    char** map;
    unsigned short levelCoinsLeft;
    unsigned short levelCoinsTaken;
    bool keyFound;
    size_t size;
    unsigned short livesLeft;
    unsigned short playerX;
    unsigned short playerY;

    Level() : id(0), numberOfLevel(0), map(nullptr), levelCoinsLeft(0), levelCoinsTaken(0), 
        keyFound(false), size(0), livesLeft(0), playerX(0), playerY(0) {}

    Level(unsigned level) {
        if (level < 0 || level > 5) {
            std::cerr << "Invalid level!" << std::endl;
            return;
        }
        std::string path = INITIAL_LEVEL_FILE + std::to_string(level);
        path += rand() % 2 ? "_" + std::to_string(level) : "";
        path += ".txt";
        numberOfLevel = level;
        map = getMap(level, path);
        levelCoinsLeft = getCoinsCount(map);
        keyFound = false;
        size = getMapSize(level, path);
        levelCoinsTaken = 0;
        livesLeft = 5;
        unsigned* coords = findPlayerCoords(map, size);
        playerX = coords[0];
        playerY = coords[1];
    }

    void save() {
        std::string path = LEVELS_STATE_FILE + std::to_string(id) + ".txt";
        std::fstream file(path, std::ios::out);
        if (!file) {
            std::cerr << "Error opening the file for saving level!" << std::endl;
            return;
        }
        file << toString();
        file.close();
    }

    std::string toString() const {
        std::string mapStr = "";
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                mapStr += map[i][j];
            }
            mapStr += "\n";
        }
        return
            "Size: " + std::to_string(size) + "\n" +
            "Level: " + std::to_string(numberOfLevel) + "\n" +
            "LevelCoinsLeft: " + std::to_string(levelCoinsLeft) + "\n" +
            "LevelCoinsTaken: " + std::to_string(levelCoinsTaken) + "\n" +
            "KeyFound: " + (keyFound ? "Yes" : "No") + "\n" +
            "LivesLeft: " + std::to_string(livesLeft) + "\n" +
            "PlayerX: " + std::to_string(playerX) + "\n" + 
            "PlayerY: " + std::to_string(playerY) + "\n" + 
            mapStr;
    }
};

unsigned getCoinsCount(char** map) {
    unsigned counter = 0;
    for (unsigned i = 0; map[i] != nullptr; i++) {
        for (unsigned j = 0; map[i][j] != '\0'; ++j) {
            counter += map[i][j] == 'C';
        }
    }
    return counter;
}

size_t getMapSize(unsigned level, std::string path) {
    std::ifstream inputFile(path);
    if (!inputFile) {
        std::cerr << "Error: Could not open level file!" << std::endl;
        return -1;
    }
    std::string current;
    std::getline(inputFile, current);
    return toInt(current);
}

char** getMap(unsigned level, std::string path) {
    std::ifstream inputFile(path);
    if (!inputFile) {
        std::cerr << "Error: Could not open level file!" << std::endl;
        return nullptr;
    }
    std::string current;
    std::getline(inputFile, current);
    unsigned size = toInt(getValue(current));
    char** map = new char* [size + 1];
    for (unsigned row = 0; row < size; ++row) {
        std::getline(inputFile, current);
        map[row] = new char[current.size() + 1];
        strCopy(map[row], current);
    }
    map[size] = nullptr;
    inputFile.close();
    return map;
}

Level* getLevelById(size_t id) {
    std::string path = LEVELS_STATE_FILE + std::to_string(id) + ".txt";
    std::ifstream inputFile(path);
    if (!inputFile) {
        return nullptr;
    }
    std::string current;
    Level* level = new Level();
    (*level).id = id;
    std::getline(inputFile, current);
    (*level).size = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).numberOfLevel = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).levelCoinsLeft = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).levelCoinsTaken = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).keyFound = strToBool(getValue(current));
    std::getline(inputFile, current);
    (*level).livesLeft = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).playerX = toInt(getValue(current));
    std::getline(inputFile, current);
    (*level).playerY = toInt(getValue(current));
    (*level).map = new char* [(*level).size];
    for (int i = 0; i < (*level).size; ++i) {
        std::getline(inputFile, current);
        (*level).map[i] = new char[current.size() + 1];
        strCopy((*level).map[i], current);
    }
    inputFile.close();
    return level;
}

unsigned* findPlayerCoords(char** map, unsigned size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (map[i][j] == '@') {
                unsigned* arr = new unsigned[2];
                arr[0] = i;
                arr[1] = j;
                return arr;
            }
        }
    }
}

unsigned* findNextPortal(unsigned short row, Level level) {
    for (size_t i = row + 1; i < level.size; i++) {
        for (size_t j = 1; j < level.size - 1; j++) {
            if (level.map[i][j] == '%') {
                unsigned* coords = new unsigned[2];
                coords[0] = i;
                coords[1] = j;
                return coords;
            }
        }
    }
    return findNextPortal(0, level);
}

struct User {
    unsigned short levelReached;
    std::vector<size_t> levelIds;
    std::vector<bool> discovered;
    unsigned short totalCoins;
    std::string username;
    std::string password;
    bool staysOnPortal;

    User()
        : levelReached(1), levelIds(NUMBER_OF_LEVELS, 0), discovered(NUMBER_OF_DISCOVERIES, false), totalCoins(0), username(""), password(""), staysOnPortal(false) {}

    User(std::string passedUsername, std::string passedPassword) {
        username = passedUsername;
        password = passedPassword;
        totalCoins = 0;
        levelReached = 1;
        for (size_t i = 0; i < NUMBER_OF_LEVELS; i++) {
            size_t id = 0;
            levelIds.push_back(id);
        }
        for (size_t i = 0; i < NUMBER_OF_DISCOVERIES; i++) {
            discovered.push_back(false);
        }
        staysOnPortal = false;
    }

    void enterlevelsMenu() {
        loadLevels(*getUser(username));
    }

    std::string toString() {
        std::string levelStatesStr;
        std::string discoveriesStr;
        for (unsigned i = 0; i < levelIds.size(); ++i) {
            levelStatesStr += "Level" + std::to_string(i + 1) + "Id: " + std::to_string((long long)(levelIds[i])) + "\n";
        }
        discoveriesStr += "Discovered 'move': " + boolToString(discovered[0]) + "\n";
        discoveriesStr += "Discovered '#': " + boolToString(discovered[1]) + "\n";
        discoveriesStr += "Discovered '@': " + boolToString(discovered[2]) + "\n";
        discoveriesStr += "Discovered '%': " + boolToString(discovered[3]) + "\n";
        discoveriesStr += "Discovered '% work': " + boolToString(discovered[4]) + "\n";
        discoveriesStr += "Discovered '&': " + boolToString(discovered[5]) + "\n";
        discoveriesStr += "Discovered 'X': " + boolToString(discovered[6]) + "\n";
        discoveriesStr += "Discovered 'C': " + boolToString(discovered[7]) + "\n";
        return "Username: " + username + "\n"
            + "Password: " + password + "\n"
            + "LevelReached: " + std::to_string(levelReached) + "\n"
            + levelStatesStr
            + "TotalCoins: " + std::to_string(totalCoins) + "\n"
            + discoveriesStr 
            + "StaysOnPortal: " + boolToString(staysOnPortal) + "\n";
    }
};

std::vector<User> userList;

void visualizeLevel(Level& level, User& user) {
    User& userFromList = *getUser(user.username);
    std::string move;
    level.save();
    std::string message = movement("", level, userFromList);
    updateData();
    while (true) {
        clearConsole();
        printWithPadding(scoreboard(3, user.username));
        printWithPadding("Coins Left: " + std::to_string(level.levelCoinsLeft));
        printWithPadding("Key Found: " + boolToString(level.keyFound), 1);
        printWithPadding("Lives: " + livesToStr(level.livesLeft), 1);
        for (int i = 0; i < level.size; i++) {
            printWithPadding(" ", 0, false);
            for (int j = 0; j < level.size; j++) {
                std::string toAdd = level.map[i][j] == '#' ? "#" : " ";
                std::cout << level.map[i][j] << toAdd;
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        if (message == "stop") {
            break;
        }
        printWithPadding(message, 1);
        std::cin >> move;
        if (move == "exit") {
            clearConsole();
            break;
        }
        if (move == "back") {
            clearConsole();
            loadLevels(user);
            break;
        }
        message = movement(move, level, userFromList);
        level.save();
        updateData();
    }
}

User* getUser(const std::string& username) {
    for (unsigned i = 0; i < userList.size(); ++i) {
        userList[i];
        if (userList[i].username == username) {
            return &userList[i];
        }
    }
    return nullptr;
}

std::string livesToStr(unsigned short lives) {
    std::string result = "";
    for (size_t i = 0; i < lives; i++) {
        result += " <3";
    }
    return result;
}

void loadAllUsers() {
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
        for (unsigned i = 0; i < NUMBER_OF_DISCOVERIES; i++) {
            std::getline(inputFile, current);
            user.discovered[i] = strToBool(getValue(current));
        }
        std::getline(inputFile, current);
        user.staysOnPortal = strToBool(getValue(current));
        std::getline(inputFile, current);
        current;
        userList.push_back(user);
    }
    inputFile.close();
}

void clearConsole() {
    system("cls");
}

bool isAround(const Level level, const char a) {
    size_t x = level.playerX;
    size_t y = level.playerY;
    return level.map[x - 1][y] == a || level.map[x + 1][y] == a
        || level.map[x][y - 1] == a || level.map[x][y + 1] == a;
}

void updateData() {
    std::fstream file(USERS_DATA_FILE, std::ios::app);
    clearFileContent(USERS_DATA_FILE);
    int size = userList.size();
    for (int i = 0; i < size; i++) {
        userList[i].totalCoins;
        file << userList[i].toString();
        file << "------------------------" << "\n";
    }
    file.close();
    loadAllUsers();
}

void clearFileContent(const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        std::cerr << "Error opening the file!" << std::endl;
        return;
    }
    file.close();
}


void deleteFile(const std::string& filePath) {
    int status = remove(filePath.c_str());  // Convert std::string to C-style string using .c_str()
    if (status != 0) {
        std::cout << "Failed to delete file. Error code: " << errno << "\n";  // errno gives error details
    }
}


void strCopy(char* arr, std::string str) {
    for (int i = 0; i < str.size(); i++) {
        arr[i] = str[i];
    }
    arr[str.size()] = 0;
}

std::string boolToString(bool value) {
    return (value ? "YES" : "NO");
}

int toInt(const std::string& str) {
    int number = 0;
    for (char c : str) {
        if (c < '0' || c > '9') return -1;
        number = number * 10 + (c - '0');
    }
    return number;
}

bool strToBool(std::string value) {
    return value == "YES" || value == "true";
}


std::string getValue(const std::string& str) { //<field_name>: <fileld_value> ==> <field_value>
    size_t pos = str.find(": ");
    if (pos != std::string::npos) {
        return str.substr(pos + 2);
    }
    return str;
}

void printWithPadding(const std::string message, const unsigned emptyRows, const bool newRow) {
    size_t start = 0;
    for (size_t i = 0; i <= message.size(); ++i) {
        if (i == message.size() || message[i] == '\n') {
            std::cout << PADDING << message.substr(start, i - start);
            if (newRow) std::cout << std::endl;
            start = i + 1;
        }
    }
    for (unsigned i = 0; i < emptyRows; ++i) {
        std::cout << std::endl;
    }
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

void failedInput(std::string message) {
    printWithPadding(message);
    promptAndClearConsole("reload");
}

std::string loadMenu() {
    std::string optionStr;
    unsigned option;
    while (true) {
        printWithPadding("type \"back\" to go back or \"exit\" to exit the program", 2);
        printWithPadding(MENU_MESSAGES);
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

void registration(User* user) {
    std::ofstream outputFile(USERS_DATA_FILE, std::ios::app);
    outputFile << (*user).toString();
    outputFile << "------------------------" << "\n";
    outputFile.close();
    loadAllUsers();
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
        User* user = getUser(username);
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
    registration(&user);
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
        user = getUser(username);
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
            //delete user;
            failedInput("Wrong password!");
            loginAttempt();
            return username;
        }
        else {
            break;
        }
    }
    return username;
}

bool exists(size_t id) {
    std::string path = LEVELS_STATE_FILE + std::to_string(id) + ".txt";
    std::fstream file(path);
    if (file) {
        return 1;
    }
    return 0;
}

size_t generateRandomId() {
    size_t id;
    while (exists(id = rand()));
    return id;
}

void loadLevels(User user) {
    Level* currentLevelPtr = nullptr;
    User& userFromList = *getUser(user.username);
    while (true) {
        printWithPadding("Type \"back\" to go back or \"exit\" to exit the program", 2);
        printWithPadding(LEVEL_MESSAGES, 2);
        for (unsigned i = 0; i < user.levelIds.size(); ++i) {
            std::string status = (i + 1 < user.levelReached) ? "Completed" : "Not Completed";
            printWithPadding("Level: " + std::to_string(i + 1) + " - " + status);
        }
        std::string optionStr;
        std::cin >> optionStr;
        if (optionStr == "back") {
            clearConsole();
            main();
            break;
        }
        if (optionStr == "exit") {
            clearConsole();
            return;
        }
        unsigned short option = toInt(optionStr);
        if (option == 0 || option > user.levelIds.size() || option < 1) {
            failedInput("Invalid number");
            continue;
        }
        if (option > user.levelReached) {
            failedInput("Level not reached");
            continue;
        }
        currentLevelPtr = getLevelById(user.levelIds[option - 1]);
        if (currentLevelPtr != nullptr) {
            clearConsole();
            printWithPadding("Do you want to continue? (enter \"yes\" or \"no\")");
            std::string continueLevel;
            std::cin >> continueLevel;
            if (continueLevel == "yes") {
                visualizeLevel(*currentLevelPtr, *getUser(user.username));
                delete currentLevelPtr;
                return;
            }
            else if (continueLevel == "no") {
                userFromList.totalCoins -= (*currentLevelPtr).levelCoinsTaken;
                userFromList.staysOnPortal = false;
                // removes the previouly-stored state of the level
                deleteFile(LEVELS_STATE_FILE + std::to_string((*currentLevelPtr).id) + ".txt");
                updateData();
            }
            else {
                failedInput("Invalid input");
                continue;
            }
        }
        delete currentLevelPtr;
        Level level(option);
        level.id = generateRandomId();
        userFromList.levelIds[level.numberOfLevel - 1] = level.id; // stores the id of the n-th level in levelIds[n - 1]
        updateData();
        visualizeLevel(level, user);
        return;
    }
}

std::string movement(std::string move, Level& level, User& user) {
    std::string message = "";

    // Initial instructions for the user
    if (!user.discovered[2]) {
        user.discovered[2] = true;
        message += "'@' - this is you.\n";
    }
    if (!user.discovered[0]) {
        user.discovered[0] = true;
        message += "Enter 'A', 'S', 'D' or 'W' (all case insensitive) to move!\n";
    }

    // Determine movement direction
    unsigned short newX = level.playerX;
    unsigned short newY = level.playerY;
    if (move == "W" || move == "w") newX--;
    else if (move == "S" || move == "s") newX++;
    else if (move == "A" || move == "a") newY--;
    else if (move == "D" || move == "d") newY++;

    char* next = &level.map[newX][newY];

    // Handle wall collision
    if (*next == '#') {
        if (!user.discovered[1]) {
            user.discovered[1] = true; message += "'#' - these are walls. Walls hurt. Don't bump into them!\n";
        } level.livesLeft--;
        level.map[level.playerX][level.playerY] = '@';
    }
    // Restore teleport when moving off it
    else if (user.staysOnPortal && *next != '#' && (move == "W" || move == "w" || move == "S" || move == "s"
        || move == "A" || move == "a" || move == "D" || move == "d")) {
        user.staysOnPortal = false;
        level.map[level.playerX][level.playerY] = '%';
    }
    else {
        level.map[level.playerX][level.playerY] = ' ';
    }

    // Handle empty space movement
    if (*next == ' ') {
        level.playerX = newX;
        level.playerY = newY;
        level.map[newX][newY] = '@';
    }

    // Handle coin collection
    if (*next == 'C') {
        if (!user.discovered[7]) {
            user.discovered[7] = true;
            message += "'C' - this is a coin. Collect as many of them as you can to go up the scoreboard list!\n";
        }
        level.levelCoinsTaken++;
        level.levelCoinsLeft--;
        user.totalCoins++;
        level.playerX = newX;
        level.playerY = newY;
        level.map[newX][newY] = '@';
    }

    // Handle key collection
    if (*next == '&') {
        if (!user.discovered[6]) {
            user.discovered[6] = true;
            message += "'&' - this is a key. You need it to open the treasure.\n";
        }
        level.keyFound = true;
        level.playerX = newX;
        level.playerY = newY;
        level.map[newX][newY] = '@';
    }

    // Handle teleportation
    if (*next == '%') {
        unsigned* coords = findNextPortal(newX, level);
        level.map[level.playerX][level.playerY] = ' ';  // Restore previous teleport
        level.playerX = coords[0];
        level.playerY = coords[1];
        user.staysOnPortal = true;
        level.map[level.playerX][level.playerY] = '@';
        delete[] coords;

        if (!user.discovered[4]) {
            user.discovered[4] = true;
            message += "You teleport to the next portal (downwise if exists, else top-most one)\n";
        }
    }

    // Handle treasure without key
    if (*next == 'X' && !level.keyFound) {
        message += "You have to find the key first!\n";
    }

    // Handle level completion
    if (*next == 'X' && level.keyFound) {
        user.levelReached = user.levelReached > level.numberOfLevel + 1 ? user.levelReached : level.numberOfLevel;
        clearConsole();
        printWithPadding("", 2);
        printWithPadding("CONGRATULATIONS, you passed level " + std::to_string(level.numberOfLevel), 1);
        unsigned reward = rand() % (100 * level.numberOfLevel);
        user.totalCoins += reward;
        printWithPadding("You get " + std::to_string(reward) + " coins from the treasure!", 1);
        promptAndClearConsole("continue...");
        user.levelIds[level.numberOfLevel - 1] = 0;
        deleteFile(LEVELS_STATE_FILE + std::to_string(level.id) + ".txt");
        updateData();
        loadLevels(user);
        return "stop";
    }

    // Handle player death
    if (level.livesLeft == 0) {
        clearConsole();
        printWithPadding("", 2);
        printWithPadding("YOU DIED!", 1);
        promptAndClearConsole("Start Again!");
        user.levelIds[level.numberOfLevel - 1] = 0;
        user.totalCoins -= level.levelCoinsTaken;
        deleteFile(LEVELS_STATE_FILE + std::to_string(level.id) + ".txt");
        updateData();
        loadLevels(user);
        return "stop";
    }

    // Discovery hints
    if (!user.discovered[3] && isAround(level, 'X')) {
        user.discovered[3] = true;
        message += "'X' - this is the treasure. You need to open it with a key to finish the level.\n";
    }
    if (!user.discovered[3] && isAround(level, '%')) {
        user.discovered[3] = true;
        message += "'%' - this is a portal.\n";
    }

    return message;
}


std::string scoreboard(unsigned length, std::string username) {
    loadAllUsers();
    sortList();
    std::string result = "Leadreboard: \n\n";
    int size = userList.size();
    for (int i = 0; i < size; i++) {
        if (userList[i].username == username) {
            if(i >= length){
                result += "\n";
            }
            result += PADDING + std::to_string(i + 1) + ". " + username + " coins: " + std::to_string(userList[i].totalCoins) + "\n";
        }
        else if (i < length) {
            result += PADDING + std::to_string(i + 1) + ". " + userList[i].username + " coins: " + std::to_string(userList[i].totalCoins) + "\n";
        }
    }
    result += "\n<------------------------------------>\n";
    return result;
}

void swap(char& a, char& b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

void sortList() {
    size_t size = userList.size();
    for (int i = 0; i < size; i++) {
        int index = i;
        for (int j = i + 1; j < size; j++) {
            if (userList[j].totalCoins > userList[index].totalCoins) {
                index = j;
            }
        }
        if (index != i) {
            User temp = userList[i];
             userList[i] = userList[index];
             userList[index] = temp;
        }
    }
}

int main() {
    srand(time(0));
    loadAllUsers();
    std::string username = loadMenu();
    if (username == "") {
        return 0;
    }
    User user = *getUser(username);
    clearConsole();
    user.enterlevelsMenu();
}
