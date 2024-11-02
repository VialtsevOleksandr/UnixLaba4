#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>

#define RESET   "\033[0m" 
#define RED     "\033[31m" 
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

using namespace std;

// Структура для представлення недетермінованого скінченого автомата
struct NFA {
    unordered_set<char> alphabet; // Множина символів вхідного алфавіту
    unordered_set<int> states; // Множина станів S
    unordered_set<int> finalStates; // Множина фінальних станів F
    int startState; // Початковий стан s0
    unordered_map<int, unordered_map<char, vector<int>>> transitions; // Функція переходів f
};

// Функція для завантаження автомата з файлу
bool loadNFA(const string& filename, NFA& nfa) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open the file.\n";
        return false;
    }

    // Зчитування потужності алфавіту
    int numAlphabetSymbols;
    file >> numAlphabetSymbols;

    // Заповнення алфавіту англійськими малими літерами
    for (int i = 0; i < numAlphabetSymbols && i < 26; ++i) {
        nfa.alphabet.insert('a' + i);
    }

    // Зчитування кількості станів і додавання їх до множини
    int numStates;
    file >> numStates;
    for (int i = 0; i < numStates; ++i) {
        nfa.states.insert(i);
    }

    // Зчитування початкового стану
    file >> nfa.startState;

    // Зчитування кількості фінальних станів і самих фінальних станів
    int numFinalStates;
    file >> numFinalStates;
    for (int i = 0; i < numFinalStates; ++i) {
        int finalState;
        file >> finalState;
        nfa.finalStates.insert(finalState);
    }

    // Зчитування переходів
    int stateFrom, stateTo;
    char symbol;
    while (file >> stateFrom >> symbol >> stateTo) {
        nfa.transitions[stateFrom][symbol].push_back(stateTo);
    }

    file.close();
    return true;
}

// Функція для перевірки, чи автомат допускає слово
bool acceptsWord(const NFA& nfa, const string& word) {
    unordered_set<int> currentStates = { nfa.startState };

    for (char symbol : word) {
        unordered_set<int> nextStates;
        for (int state : currentStates) {
            if (nfa.transitions.count(state) && nfa.transitions.at(state).count(symbol)) {
                const vector<int>& transitions = nfa.transitions.at(state).at(symbol);
                nextStates.insert(transitions.begin(), transitions.end());
            }
        }
        if (nextStates.empty()) {
            return false; // Якщо немає можливих переходів
        }
        currentStates = nextStates;
    }

    // Перевірка, чи один із поточних станів є фінальним
    for (int state : currentStates) {
        if (nfa.finalStates.find(state) != nfa.finalStates.end()) {
            return true;
        }
    }

    return false;
}

void printINFO()
{
    cout << YELLOW << "-----------------------------\n";
    cout << "Menu:\n";
    cout << "1. Select file\n";
    cout << "2. Print NFA information\n";
    cout << "3. Exit\n";
    cout << "-----------------------------\n" << RESET;
    cout << "Enter your choice: ";
}

int main() {
    NFA nfa;
    string filename;
    bool fileLoaded = false;

    while (true) {
        printINFO();
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter the filename with the NFA description: ";
            cin >> filename;
            if (loadNFA(filename, nfa)) {
                fileLoaded = true;
                cout << "File loaded successfully.\n";
            }
            else {
                cerr << "Error loading the NFA.\n";
                fileLoaded = false;
            }
            break;
        case 2:
            printINFO();
            break;
        case 3:
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }

        if (fileLoaded) {
            cout << YELLOW << "The NFA works with the following symbols: ";
            for (char c : nfa.alphabet) {
                cout << c << " ";
            }
            cout << RESET << endl;

            string inputWord;
            while (true) {
                cout << "Enter the word to check (or type 'exit' to return to menu): ";
                cin >> inputWord;
                if (inputWord == "exit") {
                    break;
                }
                bool validWord = true;
                string invalidSymbols;
                for (char c : inputWord) {
                    if (nfa.alphabet.find(c) == nfa.alphabet.end()) {
                        validWord = false;
                        invalidSymbols += c;
                        invalidSymbols += " ";
                    }
                }

                if (!validWord) {
                    cout << RED << "The word contains invalid symbols for NFA: " << invalidSymbols << RESET << endl;
                    continue;
                }
                if (acceptsWord(nfa, inputWord)) {
                    cout << GREEN << "The word is accepted by the NFA." << RESET << endl;
                }
                else {
                    cout << RED << "The word is not accepted by the NFA." << RESET << endl;
                }
            }
        }
    }

    return 0;
}
