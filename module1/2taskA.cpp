#include <iostream>
#include <vector>
#include <algorithm>

class StringFunctions {
public:
    std::vector<uint> stringToPrefix(std::string_view str) {
        std::vector<uint> prefixValues(str.size(), 0);
        for(auto i = 1; i < str.size(); ++i) {
            prefixValues[i] = prefixValues[i - 1];
            while((prefixValues[i] > 0) && (str[i] != str[prefixValues[i]])) {
                prefixValues[i] = prefixValues[prefixValues[i] - 1];
            }
            if(prefixValues[i] == str[prefixValues[i]]) {
                ++prefixValues[i];
            }
        }

        return prefixValues;
    }

    std::vector<uint> stringToZ(std::string_view str) {
        std::vector<uint> zValues(str.size(), 0);
        uint left = 0, right = 0;
        zValues[0] = str.size();
        for(uint i = 1; i < str.size(); ++i) {
            zValues[i] = std::max(static_cast<uint>(0),
                                  std::min(right - i, zValues[i - left]));
            while((i + zValues[i] < str.size()) &&
                  (str[zValues[i]] == str[i + zValues[i]])) {
                ++zValues[i];
            }
            if(i + zValues[i] > right) {
                left = i;
                right = i + zValues[i];
            }
        }

        return zValues;
    }

    std::string prefixToString(std::vector<uint> prefixValues) {
        std::string answer;
        std::vector<bool> usedSymbols(kSizeAlphabet, false);

        uint itAlphabet = -1;
        uint maxSymbol = 0;
        uint prevPrefix = 0;

        for(auto p: prefixValues) {
            if(p == 0) {
                usedSymbols.assign(kSizeAlphabet, false);
                itAlphabet = -1;

                while(prevPrefix > 0) {
                    usedSymbols[answer[prevPrefix] - 'a'] = true;
                    prevPrefix = prefixValues[prevPrefix - 1];
                }

                for(auto i = 1; i < maxSymbol; ++i) {
                    if(!usedSymbols[i]) {
                        itAlphabet = i;
                        break;
                    }
                }
                if(itAlphabet == -1) {
                    itAlphabet = ++maxSymbol;
                }

                answer += kAlphabet[itAlphabet];
            }
            else {
                answer += answer[p - 1];
            }

            prevPrefix = p;
        }

        return answer;
    }

    std::string zToString(std::vector<uint>& zValues) {
        return prefixToString(zToPrefix(zValues));
    }

    std::vector<uint> zToPrefix(std::vector<uint>& zValues) {
        std::vector<uint> prefixValues(zValues.size(), 0);

        for(auto i = 1; i < zValues.size(); ++i) {
            for(int j = zValues[i] - 1; j > -1; --j) {
                if(prefixValues[i + j] > 0) {
                    break;
                }
                else {
                    prefixValues[i + j] = j + 1;
                }
            }
        }

        return prefixValues;
    }

    std::vector<uint> prefixToZ(std::vector<uint>& prefixValues) {
        std::vector<uint> zValues(prefixValues.size(), 0);
        zValues[prefixValues.size() - 1] = prefixValues.size();
        for(auto i = 1; i < prefixValues.size(); ++i) {
            if(prefixValues[i] > 0) {
                zValues[i - prefixValues[i] + 1] = prefixValues[i];
            }
        }

        auto i = 1;
        while(i < prefixValues.size()) {
            auto t = i;
            if(zValues[i] > 0) {
                for(auto j = 1; j < zValues[i]; ++j) {
                    if(zValues[i + j] > zValues[j]) {
                        break;
                    }
                    zValues[i + j] = std::min(zValues[j], zValues[i] - j);
                    t = i + j;
                }
            }
            i = t + 1;
        }

        return zValues;
    }

private:
    const std::string kAlphabet = "abcdefghijklmnopqrstuvwxyz";
    const size_t kSizeAlphabet = 26;
};

int main() {
    StringFunctions functions;
    std::vector<uint> prefixValues;
    uint prefix;

    while(std::cin >> prefix) {
        prefixValues.push_back(prefix);
    }
    std::cout << functions.prefixToString(prefixValues) << std::endl;
}
