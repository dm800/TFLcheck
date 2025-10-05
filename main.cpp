#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <complex>
#include <fstream>

std::map<size_t, std::string> letters = {};

size_t ss = 0;
int len = 0;
bool aut = false;
int autlen = 0;
bool error = false;

std::vector<std::string> rulesleft = {};

std::vector<std::string> rulesright = {};

auto llo = [](const std::string& a, const std::string& b){
    if (a.size() != b.size()) return a.size() < b.size();
    return a < b;
};


void parse_rules() {
    std::ifstream rules("data/rules.txt");
    std::string rule;
    rulesleft.clear();
    rulesright.clear();
    while (std::getline(rules, rule)) {
        if (rule[0] == '#') {
            continue;
        } if (rule.empty()) {
            continue;
        }
        size_t index = rule.find(" -> ");
        std::string left = rule.substr(0, index);
        std::string right = rule.substr(index + 4, std::string::npos);
        rulesleft.push_back(left);
        rulesright.push_back(right);
    }
    rules.close();
}

void parse_letters() {
    std::ifstream alphabet("data/alphabet.txt");
    std::string letter;
    std::vector<std::string> all = {};
    std::getline(alphabet, letter);
    try {
        if (letter.size() >= 4 && letter.substr(0, 4) == "auto") {
            std::cout << "auto mod, starting from 1\n";
            aut = true;
            autlen = std::stoi(letter.substr(5, std::string::npos));
            len = 1;
        } else {
            len = std::stoi(letter);
            autlen = len;
        }
    } catch (std::invalid_argument&) {
        std::cout << "first line of data/alphabet.txt should be either a number or 'auto N', where N is a number\n";
        error = true;
        return;
    }
    while (std::getline(alphabet, letter)) {
        if (letter[0] == '#') {
            continue;
        }
        all.push_back(letter);
    }
    std::sort(all.begin(), all.end(), llo);
    for (int i = 0; i < all.size(); i++) {
        letters[i] = all[i];
    }
    alphabet.close();
    ss = letters.size();
}

std::string generate(size_t n, int length) {
    if (length == 0) {
        return "";
    }
    return generate(n / ss, length - 1) + letters[n % ss];
}

std::vector<size_t> find_subs(const std::string& src, const std::string& pat) {
    std::vector<size_t> result = {};
    size_t ind = src.find(pat);
    while (ind != std::string::npos) {
        result.push_back(ind);
        ind += 1;
        ind = src.find(pat, ind);
    }
    return result;
}

std::map<std::string, std::vector<std::string>> stories = {};

std::vector<std::string> normals(const std::string& starting, std::vector<std::string> story) {
    story.push_back(starting);
    std::vector<std::string> norms = {};
    bool found = false;
    for (int rulesind = 0; rulesind != rulesleft.size(); rulesind++) {
        const std::string& key = rulesleft[rulesind];
        std::vector<size_t> indexes = find_subs(starting, key);
        if (!indexes.empty()) {
            found = true;
            for (auto ind : indexes) {
                std::string nstr = starting;
                nstr.replace(ind, key.size(), rulesright[rulesind]);
                const std::vector<std::string>& nstory = story;
                std::vector<std::string> another = normals(nstr, nstory);
                for (const auto& elem : another) {
                    if (!std::ranges::contains(norms, elem)) {
                        norms.push_back(elem);
                    }
                }
            }
        }
    }
    if (!found) {
        norms.push_back(starting);
        stories[story.at(story.size() - 1)] = story;
    }
    return norms;
}

void optimize_rules() {
    size_t size = rulesleft.size();
    for (int i = 0; i < size; i++) {
        std::string starting = rulesleft[i];
        std::string left = rulesleft[i];
        std::string right = rulesright[i];
        rulesleft.erase(rulesleft.begin() + i);
        rulesright.erase(rulesright.begin() + i);
        std::vector<std::string> normforms = normals(starting, {});
        if (!std::ranges::contains(normforms, right)) {
            rulesleft.emplace(rulesleft.begin() + i, left);
            rulesright.emplace(rulesright.begin() + i, right);
        } else {
            size--;
        }
    }
}


int main() {
    parse_letters();
    parse_rules();
    optimize_rules();
    size_t count = 0;
    int iter_count = 0;
    std::map<std::string, std::string> to_add = {};
    while (!error && len <= autlen) {
        while (true) {
            iter_count++;
            for (int i = 0; i < std::pow(ss, len); i++) {
                std::string generated = generate(i, len);
                stories = {};
                std::vector<std::string> normforms = normals(generated, {});
                if (normforms.size() != 1) {
                    count++;
                    std::cout << generated << " has some problems: \n";
                    for (const auto& story : stories) {
                        std::cout << story.first << ": ";
                        for (const auto& str : story.second) {
                            std::cout << str << " >> ";
                        }
                        std::cout << "END" << std::endl;
                    }
                    std::sort(normforms.begin(), normforms.end(), llo);
                    if (normforms.size() >= 2) {
                        for (size_t k = 0; k + 1 < normforms.size(); k++) {
                            to_add[normforms[k + 1]] = normforms[k];
                        }
                    }
                }
                else {
                    std::cout << generated << " is clear (" << generated << " -> " << normforms[0] << ")" << std::endl;
                }
            }
            if (count == 0) {
                std::cout << "cool stuff\n";
                break;
            }
            std::cout << "add rules bro\n";
            for (const auto& pairs : to_add) {
                std::cout << pairs.first << " -> " << pairs.second << std::endl;
            }
            if (iter_count == 1 && aut == false) {
                std::string answer;
                while (answer != "fast" && answer != "slow" && answer != "skip") {
                    std::cout << "add automatically? (fast/slow/skip)\n";
                    std::cin >> answer;
                }
                if (answer == "fast") {
                    std::ofstream rulesout("data/rules.txt");
                    for (int i = 0; i < rulesleft.size(); i++) {
                        rulesout << rulesleft[i] << " -> " << rulesright[i] << std::endl;
                    }
                    for (const auto& pairs : to_add) {
                        rulesout << pairs.first << " -> " << pairs.second << std::endl;
                    }
                    std::cout << "done\n";
                    rulesout.close();
                    break;
                }
                if (answer != "slow") {
                    std::cout << "done";
                    break;
                }
            }
            if (to_add.empty()) {
                break;
            }
            std::ofstream rulesout("data/rules.txt");
            for (int i = 0; i < rulesleft.size(); i++) {
                rulesout << rulesleft[i] << " -> " << rulesright[i] << std::endl;
            }
            std::pair<std::string, std::string> firstrule = *to_add.begin();
            rulesout << firstrule.first << " -> " << firstrule.second << std::endl;
            rulesout.close();
            std::cout << firstrule.first << " -> " << firstrule.second << " added" << std::endl;
            std::cout << "completed iteration " << iter_count << std::endl;
            parse_rules();
            optimize_rules();
            to_add.clear();
            count = 0;
        }
        len++;
    }
    return 0;
}