#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <ctime>
#include <algorithm>
/*
 * 4_2. Порядковые статистики. Дано число N и N строк. Каждая строка содержит команду добавления или удаления натуральных чисел,
 * а также запрос на получение k-ой порядковой статистики.
 * Команда добавления числа A задается положительным числом A, команда удаления числа A задается отрицательным числом “-A”.
 * Запрос на получение k-ой порядковой статистики задается числом k.
Требования: скорость выполнения запроса - O(log n).
*/
using namespace std;

template<typename T>
class AvlTree {
    struct Node {
        Node(const T &value) : value(value), left(nullptr), right(nullptr), height(1), treeSize(1) {}

        T value;
        Node *left;
        Node *right;
        int height;
        int treeSize;
    };

public:
    AvlTree() : root(nullptr) {}

    ~AvlTree() {
        destroyTree(root);
    }

    bool Has(const T &value) {
        Node *tmp = root;
        while (tmp) {
            if (tmp->value == value)
                return true;
            if (tmp->value < value)
                tmp = tmp->right;
            else
                tmp = tmp->left;
        }
        return false;
    }

    T findKStat(unsigned k) {
        if(k>root->treeSize){
            throw "ERRRRROR";
        }
        int curPos = getTreeSize(root->left);
        Node *curNode = root;
        int pos = 0;
        while (curPos != k) {
            //влево
            if (k < curPos) {
                curNode = curNode->left;
            }
                //вправо
            else if (k > curPos) {
                k = k - curPos - 1;
                curNode = curNode->right;

            }
            curPos = getTreeSize(curNode->left);
        }
        return curNode->value;
    }

    void Add(const T &value) {
        root = addInternal(root, value);
    }

    void Remove(const T &value) {
        root = removeInternal(root, value);

    }

    void printTree(Node *tree, int level) {
        if (tree) {
            printTree(tree->left, level + 1);
            for (int i = 0; i < level; i++) cout << "    ";
            cout << tree->value << "(" << tree->treeSize << ")" << endl;
            printTree(tree->right, level + 1);
        }
    }

    Node *root;
private:

    Node *removeInternal(Node *node, const T &value) {

        if (!node)
            return nullptr;
        if (node->value < value) {
            node->right = removeInternal(node->right, value);
        } else if (node->value > value) {
            node->left = removeInternal(node->left, value);
        } else {
            //node - вершина, которую надо удалить
            //если нет ни правого ни левого поддерева
            if (node->left == nullptr && node->right== nullptr) {
                delete node;
                return nullptr;
            }

            else if (node->left == nullptr && node->right!= nullptr){// Если нет левого поддерева.
                Node* right = node->right; // Подставляем правое, может быть 0.
                node->value = right->value;
                node->left = right->left;
                node->right = right->right;
                delete right;
                return doBalance(node);
            }
            else if (node->left != nullptr && node->right == nullptr) { // Если нет правого поддерева.
                Node* left = node->left; // Подставляем левое.
                node->value = left->value;
                node->left = left->left;
                node->right = left->right;
                delete left;
                return doBalance(node);
            }
            else {
              //Есть оба поддерева.
              //Ищем минимальный элемент в правом поддереве и его родителя.
                Node *minParent = node;
                Node *min = node->right;
                while (min->left != nullptr) {
                    minParent = min;
                    minParent->treeSize-=1;
                    min = min->left;
                }
                // Переносим значение.
                node->value = min->value;
                // Удаляем min, подставляя на его место min->Right.
                (minParent->left == min ? minParent->left : minParent->right)
                        = min->right;
                delete min;
            }

        }
        return doBalance(node);
    }

    Node *findMin(Node *node) {
        while (node->left)
            node = node->left;
        return node;
    }

    Node *removeMin(Node *node) {
        if (!node->left)
            return node->right;
        node->left = removeMin(node->left);
        return doBalance(node);
    }

    Node *addInternal(Node *node, const T &value) {
        if (!node)
            return new Node(value);
        if (node->value <= value) {
            node->right = addInternal(node->right, value);
        } else {
            node->left = addInternal(node->left, value);
        }

        return doBalance(node);
    }

    Node *doBalance(Node *node) {
        fixHeight(node);
        fixTreeSize(node);
        switch (getBalance(node)) {
            case 2: {
                if (getBalance(node->right) < 0)
                    node->right = rotateRight(node->right);
                return rotateLeft(node);
            }
            case -2: {
                if (getBalance(node->left) > 0)
                    node->left = rotateLeft(node->left);
                return rotateRight(node);
            }
            default:
                return node;
        }
    }

    int getBalance(Node *node) {
        return getHeight(node->right) - getHeight(node->left);
    }

    void fixHeight(Node *node) {
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    int getHeight(Node *node) {
        return node ? node->height : 0;
    }

    int getTreeSize(Node *node) {
        return node ? node->treeSize : 0;
    }

    void fixTreeSize(Node *node) {
        node->treeSize = getTreeSize(node->left) + getTreeSize(node->right) + 1;
    }

    Node *rotateLeft(Node *node) {
        Node *tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;

        fixHeight(node);
        fixHeight(tmp);
        fixTreeSize(node);
        fixTreeSize(tmp);
        return tmp;
    }

    Node *rotateRight(Node *node) {
        Node *tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;
        fixHeight(node);
        fixHeight(tmp);
        fixTreeSize(node);
        fixTreeSize(tmp);
        return tmp;
    }

    void destroyTree(Node *node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }


};


void randomTest(){
    srand( time(0) );
    for(int i =0; i< 10000; ++i) {
        AvlTree<int> tree;
        //n - количество элементов
        int n = 1 + rand() % 15;
        vector<int> a;
        //заполняем дерево и вектор
        for (int k = 0; k < n; ++k) {
            int val = rand() % 400 + 1;
            a.push_back(val);
            tree.Add(val);
        }
        std::sort(a.begin(), a.end());
        //ищем все статистики и сверяем с отсторитрованным вектором
        for(int j = 0; j<n; j++){
            int stat = tree.findKStat(j);
            assert (a[j]==stat);
        }

        //проверка статистик после удаления
        for(int j =0; j < n-1; j++){
            int pos = rand()%(a.size());
            int el = a[pos];
            a.erase(a.begin()+pos);
            tree.Remove(el);
            std::sort(a.begin(), a.end());
            //ищем статистики после удаления и сверяем с отсортированным вектором
            for(int k=0; k<a.size(); k++){
                int stat = tree.findKStat(k);
                assert (a[k]==stat);
            }
        }
    }
    cout<<"all test OK";
}


void run(std::istream &in, std::ostream &out) {
    int n;
    in >> n;
    AvlTree<int> tree;
    int num, pos;
    for (int i=0;i<n;i++) {
        in >> num >> pos;
        if (num > 0) {
            tree.Add(num);
        }
        if (num < 0) {
            tree.Remove(-num);
        }
        cout << tree.findKStat(pos) << endl;
    }
}

int main() {
    //randomTest();
    run(cin, cout);
    return 0;
}