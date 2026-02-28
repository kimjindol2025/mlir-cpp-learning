/**
 * Exercise 2: 이진 탐색 트리 (Binary Search Tree)
 *
 * 학습 목표:
 * - 클래스 설계
 * - 포인터를 통한 동적 구조
 * - 재귀 함수
 * - 메모리 관리
 *
 * 난이도: ⭐⭐ 중급
 */

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

class Node {
public:
    int value;
    Node* left;
    Node* right;

    Node(int val) : value(val), left(nullptr), right(nullptr) {
        cout << "노드 생성: " << val << endl;
    }

    ~Node() {
        cout << "노드 소멸: " << value << endl;
    }
};

class BinarySearchTree {
private:
    Node* root;

    // 재귀적으로 노드 삽입
    Node* insertHelper(Node* node, int value) {
        if (node == nullptr) {
            return new Node(value);
        }

        if (value < node->value) {
            node->left = insertHelper(node->left, value);
        } else if (value > node->value) {
            node->right = insertHelper(node->right, value);
        }
        // 중복은 무시

        return node;
    }

    // 재귀적으로 노드 검색
    Node* searchHelper(Node* node, int value) const {
        if (node == nullptr) {
            return nullptr;
        }

        if (value == node->value) {
            return node;
        } else if (value < node->value) {
            return searchHelper(node->left, value);
        } else {
            return searchHelper(node->right, value);
        }
    }

    // 중위 순회 (Inorder Traversal): 정렬된 순서
    void inorderHelper(Node* node) const {
        if (node == nullptr) return;

        inorderHelper(node->left);
        cout << node->value << " ";
        inorderHelper(node->right);
    }

    // 전위 순회 (Preorder Traversal)
    void preorderHelper(Node* node) const {
        if (node == nullptr) return;

        cout << node->value << " ";
        preorderHelper(node->left);
        preorderHelper(node->right);
    }

    // 후위 순회 (Postorder Traversal)
    void postorderHelper(Node* node) const {
        if (node == nullptr) return;

        postorderHelper(node->left);
        postorderHelper(node->right);
        cout << node->value << " ";
    }

    // 높이 계산
    int heightHelper(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + max(heightHelper(node->left), heightHelper(node->right));
    }

    // 노드 개수
    int sizeHelper(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + sizeHelper(node->left) + sizeHelper(node->right);
    }

    // 전체 트리 삭제
    void deleteHelper(Node* node) {
        if (node == nullptr) return;

        deleteHelper(node->left);
        deleteHelper(node->right);
        delete node;
    }

    // 최솟값 찾기
    int findMinHelper(Node* node) const {
        if (node == nullptr) return INT_MAX;
        if (node->left == nullptr) return node->value;
        return findMinHelper(node->left);
    }

    // 최댓값 찾기
    int findMaxHelper(Node* node) const {
        if (node == nullptr) return INT_MIN;
        if (node->right == nullptr) return node->value;
        return findMaxHelper(node->right);
    }

public:
    BinarySearchTree() : root(nullptr) {}

    ~BinarySearchTree() {
        deleteHelper(root);
        cout << "트리 소멸 완료" << endl;
    }

    // 값 삽입
    void insert(int value) {
        root = insertHelper(root, value);
    }

    // 값 검색
    bool search(int value) const {
        return searchHelper(root, value) != nullptr;
    }

    // 중위 순회 출력
    void printInorder() const {
        cout << "중위 순회: ";
        inorderHelper(root);
        cout << endl;
    }

    // 전위 순회 출력
    void printPreorder() const {
        cout << "전위 순회: ";
        preorderHelper(root);
        cout << endl;
    }

    // 후위 순회 출력
    void printPostorder() const {
        cout << "후위 순회: ";
        postorderHelper(root);
        cout << endl;
    }

    // 트리 높이
    int height() const {
        return heightHelper(root);
    }

    // 노드 개수
    int size() const {
        return sizeHelper(root);
    }

    // 최솟값
    int findMin() const {
        return findMinHelper(root);
    }

    // 최댓값
    int findMax() const {
        return findMaxHelper(root);
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 2: 이진 탐색 트리            ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl << endl;

    // 테스트 1: 기본 삽입과 순회
    {
        cout << "📝 테스트 1: 기본 삽입과 순회" << endl;
        BinarySearchTree bst;

        int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35};
        cout << "\n삽입할 값: ";
        for (int v : values) cout << v << " ";
        cout << endl << endl;

        for (int v : values) {
            bst.insert(v);
        }

        cout << "\n트리 정보:" << endl;
        cout << "크기: " << bst.size() << endl;
        cout << "높이: " << bst.height() << endl;
        cout << "최솟값: " << bst.findMin() << endl;
        cout << "최댓값: " << bst.findMax() << endl;

        cout << "\n순회 결과:" << endl;
        bst.printInorder();
        bst.printPreorder();
        bst.printPostorder();

        cout << endl;
    }

    // 테스트 2: 검색 기능
    {
        cout << "📝 테스트 2: 검색 기능" << endl;
        BinarySearchTree bst;

        int values[] = {15, 10, 20, 8, 12, 17, 25};
        for (int v : values) {
            bst.insert(v);
        }

        bst.printInorder();

        cout << "\n검색 테스트:" << endl;
        int searchValues[] = {10, 15, 25, 30, 5, 100};
        for (int v : searchValues) {
            cout << v << ": " << (bst.search(v) ? "찾음 ✓" : "찾지 못함 ✗") << endl;
        }

        cout << endl;
    }

    // 테스트 3: 큰 트리
    {
        cout << "📝 테스트 3: 큰 트리 (1000개 노드)" << endl;
        BinarySearchTree bst;

        srand(42);
        for (int i = 0; i < 1000; i++) {
            bst.insert(rand() % 10000);
        }

        cout << "크기: " << bst.size() << endl;
        cout << "높이: " << bst.height() << endl;
        cout << "최솟값: " << bst.findMin() << endl;
        cout << "최댓값: " << bst.findMax() << endl;
        cout << "평균 높이/크기 비율: " << fixed << setprecision(2)
             << (double)bst.height() / bst.size() << endl;

        cout << "\n중위 순회 (처음 10개): ";
        bst.printInorder();

        cout << endl;
    }

    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  ✅ 모든 테스트 완료!                  ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    return 0;
}

/**
 * 학습 포인트:
 *
 * 1. 클래스 설계:
 *    - Node: 트리의 개별 노드
 *    - BinarySearchTree: 전체 트리 관리
 *
 * 2. 포인터 사용:
 *    - left, right 포인터로 자식 노드 연결
 *    - new/delete로 메모리 관리
 *
 * 3. 재귀 함수:
 *    - insertHelper: 삽입 재귀
 *    - searchHelper: 검색 재귀
 *    - 순회: inorder, preorder, postorder
 *
 * 4. 트리 분석:
 *    - 높이, 크기, 최솟값, 최댓값
 *
 * 심화 과제:
 * 1. delete() 함수 추가 (노드 삭제)
 * 2. isBalanced() 함수 (균형 확인)
 * 3. isBST() 함수 (유효한 BST 확인)
 * 4. levelOrder() 함수 (레벨별 순회)
 */
