#include <iostream>
#include <vector>

class BorVertex {
public:
    BorVertex(BorVertex* parent, char symbol) {
        this->parent = parent;
        this->symbol = symbol;

        suffix = nullptr;
        shortLink = nullptr;
        autoJump.assign(kAlphabetSize, nullptr);
        children.assign(kAlphabetSize, nullptr);

        if(parent != nullptr) {
            parent->children[symbol] = this;
        }
    }

    ~BorVertex() {
        autoJump.clear();
        children.clear();
    }

    BorVertex* findChild(char symbol) {
        return children[symbol];
    }

    BorVertex* jump(char symbol) {
        if(autoJump[symbol] == nullptr) {
            if(findChild(symbol)) {
                autoJump[symbol] = findChild(symbol);
            }
            else if(this->symbol == '\0') {// \0 - root symbol
                autoJump[symbol] =  this;
            }
            else {
                autoJump[symbol] =  getSuffix()->jump(symbol);
            }
        }
        return autoJump[symbol];
    }

    BorVertex* getShortLink() {
        if(shortLink == nullptr) {
            if(getSuffix()->symbol == '\0') {
                shortLink = getSuffix();
            }
            else if(getSuffix()->terminalFlag) {// \0 - root symbol
                shortLink = getSuffix();
            }
            else {
                shortLink = getSuffix()->getShortLink();
            }
        }
        return shortLink;
    }

    BorVertex* getSuffix() {
        if(suffix == nullptr) {
            if (symbol == '\0') {// \0 - root symbol
                suffix = this;
            }
            else if (parent->symbol == '\0') {
                suffix = parent;
            }
            else {
                suffix = parent->getSuffix()->jump(symbol);
            }
        }
        return suffix;
    }

    std::vector<BorVertex*> const& getChildren() const {
        return children;
    }

    std::vector<size_t> const& getPositions() const {
        return positions;
    }

    bool isTerminal() const{
        return terminalFlag;
    }

    void becomeTerminal(long position) {
        terminalFlag = true;
        positions.push_back(position);
    }

public:
    std::vector<BorVertex*> children;
    BorVertex* parent;
    BorVertex* suffix;
    BorVertex* shortLink;
    std::vector<BorVertex*> autoJump;
    std::vector<size_t> positions;

    char symbol;
    bool terminalFlag;

    const short kAlphabetSize = 256;
};


class Bor {
public:
    Bor() {
        root = new BorVertex(nullptr, '\0');
        numberSubpatterns = 0;
        sizePattern = 0;
    }

    ~Bor() {
        m_DFSdelete(root);
    }

    void addPattern(std::string_view pattern) {
        sizePattern = pattern.size();

        std::string word;
        for(size_t i = 0; i < pattern.size(); ++i) {
            if(pattern[i] != '?') {
                word += pattern[i];
            }
            else {
                m_addWord(word, i);
            }
        }
        m_addWord(word, pattern.size());
    }

    std::vector<size_t> findPatterns(std::string_view text) {
        std::vector<uint> appearances(text.size(), 0);
        std::vector<size_t> result;
        auto cursor = root;

        for(size_t i = 0; i < text.size(); ++i) {
            cursor = cursor->jump(text[i]);
            for(auto v = cursor; v != root; v = v->getShortLink()) {
                if(v->isTerminal()) {
                    for(auto position: v->getPositions()) {
                        if (i >= position) {
                            ++appearances[i - position];
                        }
                    }
                }
            }
        }

        for(size_t i = 0; i < appearances.size(); ++i) {
            if((appearances[i] == numberSubpatterns) &&
               (appearances.size() >= sizePattern + i)) {
                result.push_back(i);
            }
        }
        return result;
    }
private:
    void m_DFSdelete(BorVertex* vertex) {
        if(vertex != nullptr) {
            for (auto b: vertex->getChildren()) {
                if (b != nullptr) {
                    m_DFSdelete(b);
                }
            }
            delete vertex;
        }
    }

    void m_addWord(std::string& word, size_t placeInWord) {
        if(!word.empty()) {
            m_add_word_bor(word, placeInWord);
            word = "";
            numberSubpatterns++;
        }
    }

    void m_add_word_bor(std::string_view word, size_t placeInWord) {
        auto tempVertex = root;
        for(auto c: word) {
            if(tempVertex->findChild(c)) {
                tempVertex = tempVertex->findChild(c);
            }
            else {
                tempVertex = new BorVertex(tempVertex, c);
            }
        }
        tempVertex->becomeTerminal(placeInWord - 1);
    }

    BorVertex* root;
    size_t sizePattern;
    size_t numberSubpatterns;
};

int main() {
    Bor bor;
    std::string pattern, text;
    std::cin >> pattern;
    std::cin >> text;

    bor.addPattern(pattern);
    for(auto p: bor.findPatterns(text)) {
        std::cout << p << ' ';
    }
    std::cout << std::endl;

    return 0;
}
