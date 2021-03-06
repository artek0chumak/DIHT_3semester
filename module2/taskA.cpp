#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>

template<typename T>
class SuffixArray {
public:
    SuffixArray<T>(std::string_view text) {
        array.assign(text.size(), 0);
        std::vector<T> eqClass(text.size(), 0);
        m_sortByText(array, text);
        size_t classes = 0;
        for(size_t i = 1; i < array.size(); ++i) {
            if(text[array[i]] != text[array[i - 1]]) {
                ++classes;
            }
            eqClass[array[i]] = classes;
        }

        std::vector<T> array_2_k(text.size()), newEqClass(text.size());
        for(size_t h = 1; h < text.size(); h<<=1) {
            m_sortByEqClass(array, eqClass, classes, h);
            classes = 0;
            for(size_t i = 1; i < array.size(); ++i) {
                if((eqClass[array[i]] != eqClass[array[i - 1]]) ||
                   (eqClass[(array[i] + h) % text.size()] != eqClass[(array[i - 1] + h) % text.size()])) {
                    ++classes;
                }
                newEqClass[array[i]] = classes;
            }
            std::swap(newEqClass, eqClass);
        }
    }

    T const& operator[](size_t it) const {
        return array[it];
    }

    size_t size() const {
        return array.size();
    }

    typename std::vector<T>::iterator begin() {
        return array.begin();
    }

    typename std::vector<T>::iterator end() {
        return array.end();
    }

private:
    static void m_sortByText(std::vector<T>& array, std::string_view text) {
        std::vector<T> count((256 < text.size()) ? text.size() : 256);

        for(size_t i = 0; i < text.size(); ++i) {
            ++count[text[i]];
        }
        for(size_t i = 1; i < 256; ++i) {
            count[i] += count[i - 1];
        }
        for(size_t i = 0; i < text.size(); ++i) {
            array[--count[text[i]]] = i;
        }
    }

    static void m_sortByEqClass(std::vector<T>& array, std::vector<T> eqClass, size_t classes, size_t length) {
        std::vector<T> array_2_k(array.size());
        size_t size = array.size();
        for(size_t i = 0; i < size; ++i) {
            array_2_k[i] = array[i] - length;
            if(array_2_k[i] < 0) {
                array_2_k[i] += size;
            }
        }

        std::vector<size_t> count(classes + 1, 0);
        for(size_t i = 0; i < size; ++i) {
            ++count[eqClass[array_2_k[i]]];
        }
        for(size_t i = 1; i < classes + 1; ++i) {
            count[i] += count[i - 1];
        }
        for(size_t i = size; i > 0; --i) {
            array[--count[eqClass[array_2_k[i - 1]]]] = array_2_k[i - 1];
        }
    }

    const unsigned int kAlpahbetSize = 256;
    std::vector<T> array;
};


template<typename T>
class LCP {
public:
    LCP(std::string_view text, SuffixArray<T>& suffixArray) {
        array.assign(text.size(), 0);

        std::vector<size_t> positionOfSuffix(text.size(), 0);
        for(size_t i = 0; i < suffixArray.size(); ++i) {
            positionOfSuffix[suffixArray[i]] = i;
        }

        size_t equalLetters = 0;
        for(size_t i = 0; i < positionOfSuffix.size(); ++i) {
            if(equalLetters > 0) {
                --equalLetters;
            }

            if(positionOfSuffix[i] == suffixArray.size() - 1) {
                array[positionOfSuffix[i]] = 0;
                equalLetters = 0;
            }
            else {
                size_t j = suffixArray[positionOfSuffix[i] + 1];
                while ((std::max(i + equalLetters, j + equalLetters) < suffixArray.size()) &&
                       (text[i + equalLetters] == text[j + equalLetters])) {
                    ++equalLetters;
                }
                array[positionOfSuffix[i]] = equalLetters;
            }
        }
    }

    T const& operator[](size_t it) const {
        return array[it];
    }

    size_t size() const {
        return array.size();
    }

    std::vector<T> const& value() const {
        return array;
    }

    typename std::vector<T>::iterator begin() {
        return array.begin();
    }

    typename std::vector<T>::iterator end() {
        return array.end();
    }

private:
    std::vector<T> array;
};


int main() {
    std::string text;
    std::cin >> text;
    text += '$';

    SuffixArray<int> suffixArray(text);
    LCP<int> lcp(text, suffixArray);

    std::cout << std::accumulate(suffixArray.begin(), suffixArray.end(), 0) -
                 std::accumulate(lcp.begin(), lcp.end(), 0) << std::endl;

    return 0;
}
