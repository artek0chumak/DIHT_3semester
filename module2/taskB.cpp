#include <map>
#include <iostream>
#include <limits>


template<typename T>
class SuffixTree {
public:
    struct Node {
        T start;
        T end;
        std::map<char, Node*> children;
        Node* suffixLink;
        SuffixTree* suffixTree;

        Node(T start, T end, SuffixTree* suffixTree): start(start), end(end), suffixTree(suffixTree)
        {}

        T getEdgeLength() {
            if(end == suffixTree->kLastPosition) {
                return suffixTree->currentLength - start;
            }
            else {
                return end - start;
            }
        }
    };

    struct ImplicitNode {
        Node* vertex;
        char ch;
        T edgeLength;

        void canonize() {
            if(edgeLength == 0) {
                return;
            }
            Node* child = vertex->children[ch];
            T actualEdgeLength = child->getEdgeLength();
            while(actualEdgeLength <= edgeLength) {
                vertex = child;
                edgeLength -= actualEdgeLength;
                if(edgeLength != 0) {
                    ch = vertex->suffixTree->text[vertex->suffixTree->currentLength - edgeLength];
                    child = vertex->children[ch];
                    actualEdgeLength = child->getEdgeLength();
                }
            }
        }

        void moveToSuffix() {
            if(vertex != vertex->suffixTree->root) {
                vertex = vertex->suffixLink;
            }
            else if (edgeLength > 0) {
                --edgeLength;
                ch = vertex->suffixTree->text[vertex->suffixTree->currentLength - edgeLength];
            }
        }

        void moveForward() {
            ++edgeLength;
            if(edgeLength == 1) {
                ch = vertex->suffixTree->text[vertex->suffixTree->currentLength];
            }
        }
    };

    SuffixTree(): root(new Node(0, 0, this)), activePoint{root, '\0', 0},
        currentLength(0), size(1) {
        splitPosition = text.find('$');
        m_ukkonen();
    }

    ~SuffixTree() {
        m_clean(root);
    }

    void dfs() {
        T id = 0;
        std::cout << size << std::endl;
        m_dfs(root, id, id);
    }

    T currentLength;
private:
    Node* root;
    ImplicitNode activePoint;
    std::string text;
    T splitPosition;
    size_t size;

    static const T kLastPosition = std::numeric_limits<T>::max();

    void m_ukkonen() {
        T i = 0;
        char c;
        std::scanf("%c", &c);
        while(c != '#') {
            if(c != '\n') {
                if(c == '$') {
                    splitPosition = i;
                }

                text += c;
                m_addLetter(i);
                ++i;
            }
            std::scanf("%c", &c);
        }
        text += '#';
        m_addLetter(i);
    }

    void m_addLetter(T position) {
        Node* parent;
        bool endPoint;
        Node* lastNewNode = root;
        do {
            endPoint = m_testAndSplit(position, parent);
            if(lastNewNode != root) {
                lastNewNode->suffixLink = parent;
            }

            if(!endPoint) {
                auto newLeaf = new Node(position, kLastPosition, this);
                parent->children[text[position]] = newLeaf;
                ++size;

                lastNewNode = parent;
                activePoint.moveToSuffix();
            }
        }
        while(!endPoint && (parent != root));

        if(endPoint) {
            activePoint.moveForward();
        }
        ++currentLength;
    }

    bool m_testAndSplit(T position, Node*& newNode) {
        char newChar = text[position];
        activePoint.canonize();
        Node* parent = activePoint.vertex;

        if(activePoint.edgeLength == 0) {
            newNode = parent;
            return parent->children[newChar] != nullptr;
        }

        Node* child = parent->children[activePoint.ch];
        T splitPosition = child->start + activePoint.edgeLength;

        if(text[splitPosition] == newChar) {
            return true;
        }
        else {
            newNode = new Node(child->start, splitPosition, this);
            newNode->children[text[splitPosition]] = child;
            parent->children[activePoint.ch] = newNode;
            ++size;
            child->start = splitPosition;
            return false;
        }
    }


    void m_dfs(Node* root, T parentID, T& prevID) {
        T start = root->start;
        T end = start + root->getEdgeLength();
        T gap = 0;
        T id = prevID;

        bool strID = start > splitPosition;
        if(start > splitPosition) {
            gap = splitPosition + 1;
        }
        else if(end > splitPosition) {
            end = splitPosition + 1;
        }

        if(root != this->root) {
            std::printf("%d %d %d %d\n", parentID, strID, start - gap, end - gap);
        }

        for(auto child: root->children) {
            m_dfs(child.second, id, ++prevID);
        }
    }

    void m_clean(Node* vertex) {
        for(auto child: vertex->children) {
            m_clean(child.second);
        }
        delete vertex;
    }
};


int main() {
    SuffixTree<int> suffixTree;
    suffixTree.dfs();
    return 0;
}