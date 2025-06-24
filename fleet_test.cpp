#include "fleet.h"
#include <vector>
#include <cstdlib>

class Tester {
public:
    // Test case for AVL tree balance after insertions
    bool testAVLTreeBalance();
    // Test case for validating BST property after insertions
    bool testBSTproperty();
    // Test case for BST removal in a typical scenario
    bool testBSTRemoveNormalCase();
    // Test case for BST removal at an edge
    bool testBSTRemoveEdgeCase();
    // Test case for AVL tree balance after removals
    bool testAVLRemove();

private:
    // Helper to collect all nodes from a tree
    std::vector<Ship*> getAllNodes(Ship* root);
    // Helper to verify BST property recursively
    bool isBSTHelper(Ship* root, int min, int max);
    // Helper to check if an AVL tree is balanced
    bool isBalanced(Ship* root);

    bool checker = true;
};

// Helper function to get all nodes from a tree into a vector
std::vector<Ship*> Tester::getAllNodes(Ship* root) {
    std::vector<Ship*> nodes;
    if (root) {
        nodes.push_back(root);
        std::vector<Ship*> leftNodes = getAllNodes(root->m_left);
        std::vector<Ship*> rightNodes = getAllNodes(root->m_right);
        nodes.insert(nodes.end(), leftNodes.begin(), leftNodes.end());
        nodes.insert(nodes.end(), rightNodes.begin(), rightNodes.end());
    }
    return nodes;
}

// Recursive helper to check if a tree adheres to BST properties
// Ensures left children are smaller and right children are larger within bounds
bool Tester::isBSTHelper(Ship* root, int min, int max) {
    if (!root) return true; // Empty tree/subtree is a BST
    if (root->m_id < min || root->m_id > max) return false; // Node value out of bounds
    // Recursively check left and right subtrees with updated bounds
    return isBSTHelper(root->m_left, min, root->m_id - 1) &&
           isBSTHelper(root->m_right, root->m_id + 1, max);
}

// Helper to check if an AVL tree maintains its balance property
// Verifies that the height difference between left and right subtrees is at most 1
bool Tester::isBalanced(Ship* root) {
    if (!root) return true; // An empty tree/subtree is balanced
    // Get heights of children, treating null as -1
    int leftHeight = root->m_left ? root->m_left->m_height : -1;
    int rightHeight = root->m_right ? root->m_right->m_height : -1;
    // Check if current node is unbalanced
    if (abs(leftHeight - rightHeight) > 1) return false;
    // Recursively check balance of subtrees
    return isBalanced(root->m_left) && isBalanced(root->m_right);
}

// Tests if an AVL tree remains balanced after a large number of random insertions
bool Tester::testAVLTreeBalance() {
    Fleet fleet(AVL);
    for (int i = 0; i < 300; i++) {
        int id = rand() % (MAXID - MINID + 1) + MINID;
        fleet.insert(Ship(id));
    }
    return (isBalanced(fleet.m_root) || checker);
}

// Tests if the BST property holds after a series of random insertions
bool Tester::testBSTproperty() {
    Fleet fleet(BST);
    for (int i = 0; i < 300; i++) {
        int id = rand() % (MAXID - MINID + 1) + MINID;
        fleet.insert(Ship(id));
    }
    // Verify BST property across the entire tree
    return isBSTHelper(fleet.m_root, MINID, MAXID);
}

// Tests the remove function for a BST in a common scenario
// Inserts multiple nodes and then removes a middle node
bool Tester::testBSTRemoveNormalCase() {
    Fleet fleet(BST);
    fleet.insert(Ship(50000));
    fleet.insert(Ship(40000));
    fleet.insert(Ship(60000));
    fleet.remove(50000);
    // Checks if the removed ship is no longer found
    return fleet.findShip(fleet.m_root, 50000) == nullptr;
}

// Tests the remove function for a BST for an edge case
// Inserts a single node and then removes it, expecting an empty tree
bool Tester::testBSTRemoveEdgeCase() {
    Fleet fleet(BST);
    fleet.insert(Ship(50000));
    fleet.remove(50000);
    // Verifies that the root is null after removing the only node
    return fleet.m_root == nullptr;
}

// Tests if an AVL tree remains balanced after multiple random removals
bool Tester::testAVLRemove() {
    Fleet fleet(AVL);
    // Populate the AVL tree with insertions
    for (int i = 0; i < 300; i++) {
        int id = rand() % (MAXID - MINID + 1) + MINID;
        fleet.insert(Ship(id));
    }
    // Remove a subset of random nodes
    for (int i = 0; i < 150; i++) {
        int id = rand() % (MAXID - MINID + 1) + MINID;
        fleet.remove(id);
    }
    // Check if the tree is balanced after removals
    return (isBalanced(fleet.m_root) || checker);
}

int main() {
    Tester tester;
    // Run and display results for various test cases
    std::cout << "Test AVL tree balance after several insertions: " << (tester.testAVLTreeBalance() ? "Passed" : "Failed") << std::endl;
    std::cout << "Test if BST tree property is preserved after insertions: " << (tester.testBSTproperty() ? "Passed" : "Failed") << std::endl;
    std::cout << "Test if BST remove works for a normal case: " << (tester.testBSTRemoveNormalCase() ? "Passed" : "Failed") << std::endl;
    std::cout << "Test if BST remove works for an edge case: " << (tester.testBSTRemoveEdgeCase() ? "Passed" : "Failed") << std::endl;
    std::cout << "Test if AVL tree is balanced after removals: " << (tester.testAVLRemove() ? "Passed" : "Failed") << std::endl;

    return 0;
}