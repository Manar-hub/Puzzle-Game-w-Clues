#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <set>

/*Bulls mean correct number in correct position
Cows mean correct number but in incorrect position*/
struct BC { int bulls; int cows; };

//To ensure that digits don't repeat
bool unique3(const std::vector<int>& d) {
    return d[0] != d[1] && d[0] != d[2] && d[1] != d[2];
}

//To treat the digits individually for easier manipulation (to track uniqueness, and count of bulls and cows)
std::vector<int> numToVec(int n) {
    return { (n / 100) % 10, (n / 10) % 10, n % 10 };
}

int vecToNum(const std::vector<int>& v) {
    return v[0] * 100 + v[1] * 10 + v[2];
}

BC countBC(const std::vector<int>& g, const std::vector<int>& s) { //vector g for the clue number, vector s for the secret number
    BC bc{ 0,0 };
    for (int i = 0; i < 3; ++i) {
        if (g[i] == s[i]) ++bc.bulls;
        else if (std::find(s.begin(), s.end(), g[i]) != s.end()) ++bc.cows;
    }
    return bc;
}

void printClues(int num, BC bc) {
    std::cout << num << " - ";
    if (bc.bulls == 0 && bc.cows == 0) { std::cout << "none are correct\n"; return; }
    if (bc.bulls)  std::cout << bc.bulls << " correct and in correct position";
    if (bc.cows)   std::cout << (bc.bulls ? ", " : "") << bc.cows
        << " correct but in wrong position";
    std::cout << "\n";
}

int main() {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr))); //algo for randomness, rng more modern and effecient, using time to compute more random results

    //all valid 3-digit numbers with unique digits
    std::vector<std::vector<int>> all;
    for (int a = 0; a <= 9; ++a)
        for (int b = 0; b <= 9; ++b) if (b != a)
            for (int c = 0; c <= 9; ++c) if (c != a && c != b)
                all.push_back({ a,b,c });

    std::uniform_int_distribution<int> pick(0, all.size() - 1); //object pick to generate all possible numbers
    std::vector<int> secret = all[pick(rng)];//picking a random number

    struct Clue { int num; BC bc; };
    std::vector<Clue> clues;
    std::set<int> usedNums;
    std::uniform_int_distribution<int> rnd100(100, 999);

    //generate unique clues for the puzzle
    while (true) {
        //we clear to start from scratch in case the process didn't work
        clues.clear();
        usedNums.clear();

        while (clues.size() < 5) {
            int guessNum;
            std::vector<int> guess;
            do {
                guessNum = rnd100(rng);
                guess = numToVec(guessNum);
            } while (!unique3(guess) || guess == secret || usedNums.count(guessNum));
            usedNums.insert(guessNum);

            BC feedback = countBC(guess, secret);
            if (feedback.bulls == 3) continue;

            clues.push_back({ guessNum, feedback });

            std::vector<std::vector<int>> candidates = all;
            for (const Clue& cl : clues) {
                std::vector<std::vector<int>> next;
                for (const auto& cand : candidates) {
                    BC res = countBC(numToVec(cl.num), cand);
                    if (res.bulls == cl.bc.bulls && res.cows == cl.bc.cows)
                        next.push_back(cand);
                }
                candidates = next;
            }

            //if == 1 then we found our secret number
            if (candidates.size() == 1) {
                //checking if all digits of the secret number are mentioned in the clues
                std::set<int> secretDigits(secret.begin(), secret.end());
                std::set<int> clueDigits;
                for (const Clue& cl : clues) {
                    for (int d : numToVec(cl.num)) clueDigits.insert(d);
                }
                bool allSeen = true;
                for (int d : secretDigits)
                    if (clueDigits.find(d) == clueDigits.end())
                        allSeen = false;

                if (allSeen) goto startGame;
            }
        }
    }

startGame:
    std::cout << "Deduce the secret 3-digit number based on these clues:\n";
    for (const Clue& cl : clues)
        printClues(cl.num, cl.bc);

    std::cout << "\nStart guessing the number (digits must be unique)!\n";

    while (true) {
        std::cout << "Your guess: ";
        int g;
        std::cin >> g;

        if (g < 100 || g > 999) {
            std::cout << "Enter a valid 3-digit number!\n";
            continue;
        }

        std::vector<int> guess = numToVec(g);
        if (!unique3(guess)) {
            std::cout << "Digits must be unique!\n";
            continue;
        }

        BC bc = countBC(guess, secret);
        if (bc.bulls == 3) {
            std::cout << "Correct! The secret number was " << vecToNum(secret) << "!\n";
            break;
        }
        else {
            std::cout << g << " - ";
            if (bc.bulls > 0) std::cout << bc.bulls << " correct and in correct position";
            if (bc.cows > 0) {
                if (bc.bulls > 0) std::cout << ", ";
                std::cout << bc.cows << " correct but in wrong position";
            }
            if (bc.bulls == 0 && bc.cows == 0) std::cout << "none are correct";
            std::cout << "\n";
        }
    }

    return 0;
}
