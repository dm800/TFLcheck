#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <complex>
#include <fstream>

std::map<int, std::string> letters = {
    {0, "c"},
    {1, "b"},
    {2, "a"}
};

int ss = letters.size();
int len = 0;

std::vector<std::string> rulesleft = {};

std::vector<std::string> rulesright = {};

auto llo = [](const std::string& a, const std::string& b){
    if (a.size() != b.size()) return a.size() < b.size();
    return a > b;
};


void parse_rules() {
    std::ifstream rules("data/rules.txt");
    std::string rule;
    while (std::getline(rules, rule)) {
        if (rule[0] == '#') {
            continue;
        }
        int index = rule.find(" -> ");
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
    len = std::atoi(letter.c_str());
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
}

std::string generate(int n, int len) {
    if (len == 0) {
        return "";
    }
    return generate(n / ss, len - 1) + letters[n % ss];
}

std::vector<int> find_subs(const std::string& src, const std::string& pat) {
    std::vector<int> result = {};
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
        std::vector<int> indexes = find_subs(starting, key);
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


int main() {
    parse_rules();
    parse_letters();
    size_t count = 0;
    std::map<std::string, std::string> to_add = {};
    for (int i = 0; i < std::pow(3, len); i++) {
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
    } else {
        std::cout << "add rules bro\n";
        for (const auto& pairs : to_add) {
            std::cout << pairs.first << " -> " << pairs.second << std::endl;
        }
        std::cout << "add automatically? (not reliable now, not recommended)\n";
        std::string answer;
        std::cin >> answer;
        if (answer == "Y" || answer == "Yes" || answer == "yes" || answer == "y") {
            std::ofstream rulesout("data/rules.txt");
            for (int i = 0; i < rulesleft.size(); i++) {
                rulesout << rulesleft[i] << " -> " << rulesright[i] << std::endl;
            }
            for (const auto& pairs : to_add) {
                rulesout << pairs.first << " -> " << pairs.second << std::endl;
            }
            std::cout << "done\n";
            rulesout.close();
        }
    }
    return 0;
}