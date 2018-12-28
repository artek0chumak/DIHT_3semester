#include <iostream>
#include <vector>


template<typename T>
class FinderOfSubstrings {
public:
    FinderOfSubstrings(const std::string_view patern): patern(patern) {
        prefixValues.assign(patern.size(), 0);
    }

    void solve(std::istream& inputStream, std::ostream& outputStream) {
        m_prefixFunction();
        m_findSubstrings(inputStream, outputStream);
    }
private:
    void m_prefixFunction() {
        for(auto i = 1; i < patern.size(); ++i) {
            prefixValues[i] = prefixValues[i - 1];
            while((prefixValues[i] > 0) && (patern[i] != patern[prefixValues[i]])) {
                prefixValues[i] = prefixValues[prefixValues[i] - 1];
            }
            if(patern[i] == patern[prefixValues[i]]) {
                ++prefixValues[i];
            }
        }
    }

    void m_findSubstrings(std::istream &inputStream, std::ostream &outputStream) {
        T prefix = 0;
        T i = 0;
        T answerBuffer[kBufferSize];
        T itBuffer = 0;

        char symbol = inputStream.get();
        if(symbol == '\n') {
            symbol = inputStream.get();
        }

        while(symbol != '\n') {
            // Обычный префикс функция с запоминанием последнего значения
            while((prefix > 0) && (symbol != patern[prefix])) {
                prefix = prefixValues[prefix - 1];
            }
            if(symbol == patern[prefix]) {
                ++prefix;
            }

            // Если значение стало равно размеру шаблона, то мы нашли нужную
            // под строку.
            if(prefix == patern.size()) {
                answerBuffer[itBuffer] = i - patern.size() + 1;
                ++itBuffer;

                if(itBuffer == kBufferSize) {
                    itBuffer = 0;
                    for(auto j = 0; j < kBufferSize; ++j) {
                        std::cout << answerBuffer[j] << ' ';
                    }
                }
            }

            symbol = inputStream.get();
            ++i;
        }

        for(auto j = 0; j < itBuffer; ++j) {
            std::cout << answerBuffer[j] << ' ';
        }
        outputStream << std::endl;
    }

    std::string patern;
    std::vector<T> prefixValues;
    const size_t kBufferSize = 50;
};


int main() {
    std::string patern;
    std::cin >> patern;

    FinderOfSubstrings<size_t> finder(patern);
    finder.solve(std::cin, std::cout);
}
