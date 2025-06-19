#include "fleet.h"

// Default constructor
Fleet::Fleet() {
    m_root = nullptr;
    m_type = NONE;
}

// Constructor with tree type
Fleet::Fleet(TREETYPE type) {
    m_root = nullptr;
    m_type = type;
}

// Destructor: cleans up all Ship nodes
Fleet::~Fleet() {
    m_root = cleanFleet(m_root); // Helper to delete nodes
    m_type = NONE;
}

// Clears all Ship objects from the fleet
void Fleet::clear() {
    m_root = cleanFleet(m_root);
    m_type = NONE;
}

// Inserts a new Ship, handling ID validation and duplicates
void Fleet::insert(const Ship& ship) {
    Ship* newShip = new Ship(ship.m_id, ship.m_type, ship.m_state);

    if (newShip->m_id < MINID || newShip->m_id > MAXID || findShip(m_root, newShip->m_id)) {
        delete newShip; // Discard invalid or duplicate ship
        return;
    }

    // Insert based on tree type
    if (m_type == BST) {
        m_root = insertBST(m_root, newShip);
    } else if (m_type == AVL) {
        m_root = insertAVL(m_root, newShip);
    } else if (m_type == SPLAY) {
        m_root = insertBST(m_root, newShip); // Initial BST insert
        m_root = splay(m_root, newShip->m_id); // Splay to root
    }
}

// Removes a Ship by ID, based on tree type
void Fleet::remove(int id) {
    if (m_type == BST) {
        m_root = removeBST(m_root, id);
    } else if (m_type == AVL) {
        m_root = removeAVL(m_root, id);
    } else if (m_type == SPLAY) {
        removeSplay(id);
    }
}

// Assignment operator for deep copy
const Fleet& Fleet::operator=(const Fleet& rhs) {
    if (this == &rhs) return *this; // Self-assignment check
    clear();
    m_type = rhs.m_type;
    m_root = copyTree(rhs.m_root);
    return *this;
}

// Returns the current tree type
TREETYPE Fleet::getType() const {
    return m_type;
}

// Sets the tree type, rebalancing if changing to AVL
void Fleet::setType(TREETYPE type) {
    if (type == AVL && m_type != AVL) {
        m_root = nodeTransfer(m_root); // Rebalance for AVL
        m_type = AVL;
    } else if (type == NONE) {
        clear();
        m_type = NONE;
    } else if (type == BST || type == SPLAY) {
        m_type = type;
    }
}

// Dumps the tree structure (in-order)
void Fleet::dumpTree() const {
    dump(m_root);
}

// Recursive helper for in-order tree dump
void Fleet::dump(Ship* aShip) const {
    if (aShip != nullptr) {
        cout << "(";
        dump(aShip->m_left);
        cout << aShip->m_id << ":" << aShip->m_height;
        dump(aShip->m_right);
        cout << ")";
    }
}

// Recursive helper to deallocate all nodes in a tree
Ship* Fleet::cleanFleet(Ship* theRoot) {
    if (!theRoot) return nullptr;
    theRoot->m_left = cleanFleet(theRoot->m_left);
    theRoot->m_right = cleanFleet(theRoot->m_right);
    delete theRoot;
    return nullptr; // Ensure pointer is nullified
}

// Finds a Ship node by ID
Ship* Fleet::findShip(Ship* node, int id) {
    if (!node) return nullptr;
    if (id == node->m_id) return node;
    return (id < node->m_id) ? findShip(node->m_left, id) : findShip(node->m_right, id);
}

// Finds the minimum node in a subtree
Ship* Fleet::findMin(Ship* node) {
    if (!node) return nullptr;
    return (node->m_left == nullptr) ? node : findMin(node->m_left);
}

// Updates a node's height based on its children's heights
void Fleet::updateHeight(Ship* node) {
    if (!node) return;
    int leftHeight = (node->m_left) ? node->m_left->m_height : -1;
    int rightHeight = (node->m_right) ? node->m_right->m_height : -1;
    node->m_height = std::max(leftHeight, rightHeight) + 1;
}

// Removes a node from a BST
Ship* Fleet::removeBST(Ship* root, int id) {
    if (root == nullptr) return nullptr;

    if (id < root->m_id) {
        root->m_left = removeBST(root->m_left, id);
    } else if (id > root->m_id) {
        root->m_right = removeBST(root->m_right, id);
    } else { // Node to be deleted
        if (root->m_left == nullptr) {
            Ship* temp = root->m_right;
            delete root;
            root = temp;
        } else if (root->m_right == nullptr) {
            Ship* temp = root->m_left;
            delete root;
            root = temp;
        } else {
            Ship* minRight = findMin(root->m_right);
            root->m_id = minRight->m_id; // Copy successor's data
            root->m_state = minRight->m_state;
            root->m_right = removeBST(root->m_right, minRight->m_id); // Remove successor
        }
    }
    if (root) updateHeight(root);
    return root;
}

// Inserts a node into a BST
Ship* Fleet::insertBST(Ship* root, Ship* node) {
    if (!root) return node;
    if (node->m_id < root->m_id)
        root->m_left = insertBST(root->m_left, node);
    else
        root->m_right = insertBST(root->m_right, node);
    updateHeight(root);
    return root;
}

// Deep copies a tree structure
Ship* Fleet::copyTree(Ship* root) {
    if (!root) return nullptr;
    Ship* newRoot = new Ship(root->m_id, root->m_type, root->m_state);
    newRoot->m_height = root->m_height;
    newRoot->m_left = copyTree(root->m_left);
    newRoot->m_right = copyTree(root->m_right);
    return newRoot;
}

// Calculates a node's balance factor
int Fleet::BalanceFactor(Ship* root) {
    int leftHeight = (root->m_left) ? root->m_left->m_height : -1;
    int rightHeight = (root->m_right) ? root->m_right->m_height : -1;
    return (leftHeight - rightHeight);
}

// Inserts a node into an AVL tree, performing rotations to maintain balance
Ship* Fleet::insertAVL(Ship* root, Ship* newElement) {
    if (!root) return newElement;
    if (newElement->m_id < root->m_id)
        root->m_left = insertAVL(root->m_left, newElement);
    else
        root->m_right = insertAVL(root->m_right, newElement);

    updateHeight(root); // Update height

    // Rebalance based on balance factor and new element's position
    int balance = BalanceFactor(root);
    if (balance > 1 && newElement->m_id < root->m_left->m_id) return rightRotate(root);
    if (balance < -1 && newElement->m_id > root->m_right->m_id) return leftRotate(root);
    if (balance > 1 && newElement->m_id > root->m_left->m_id) {
        root->m_left = leftRotate(root->m_left);
        return rightRotate(root);
    }
    if (balance < -1 && newElement->m_id < root->m_right->m_id) {
        root->m_right = rightRotate(root->m_right);
        return leftRotate(root);
    }
    return root;
}

// Performs a right rotation
Ship* Fleet::rightRotate(Ship* node) {
    if (!node || !node->m_left) return node;
    Ship* A = node->m_left;
    Ship* B = A->m_right;
    A->m_right = node;
    node->m_left = B;
    updateHeight(node); // Update heights of rotated nodes
    updateHeight(A);
    return A;
}

// Performs a left rotation
Ship* Fleet::leftRotate(Ship* node) {
    if (!node || !node->m_right) return node;
    Ship* A = node->m_right;
    Ship* B = A->m_left;
    A->m_left = node;
    node->m_right = B;
    updateHeight(node); // Update heights of rotated nodes
    updateHeight(A);
    return A;
}

// Removes a node from an AVL tree, rebalancing after deletion
Ship* Fleet::removeAVL(Ship* root, int id) {
    if (root == nullptr) return nullptr;

    if (id < root->m_id) {
        root->m_left = removeAVL(root->m_left, id);
    } else if (id > root->m_id) {
        root->m_right = removeAVL(root->m_right, id);
    } else { // Node to be deleted
        if (root->m_left == nullptr) {
            Ship* temp = root->m_right;
            delete root;
            root = temp;
        } else if (root->m_right == nullptr) {
            Ship* temp = root->m_left;
            delete root;
            root = temp;
        } else {
            Ship* minRight = findMin(root->m_right);
            root->m_id = minRight->m_id;
            root->m_state = minRight->m_state;
            root->m_right = removeAVL(root->m_right, minRight->m_id);
        }
    }
    if (root) {
        updateHeight(root); // Update height after removal
        // Rebalance the tree if necessary
        int balance = BalanceFactor(root);
        if (balance > 1 && BalanceFactor(root->m_left) >= 0) root = rightRotate(root);
        if (balance < -1 && BalanceFactor(root->m_right) <= 0) root = leftRotate(root);
        if (balance > 1 && BalanceFactor(root->m_left) < 0) {
            root->m_left = leftRotate(root->m_left);
            root = rightRotate(root);
        }
        if (balance < -1 && BalanceFactor(root->m_right) > 0) {
            root->m_right = rightRotate(root->m_right);
            root = leftRotate(root);
        }
    }
    return root;
}

// Splay function: brings the node with 'id' to the root
Ship* Fleet::splay(Ship* root, int id) {
    if (!root || root->m_id == id) return root;

    if (id < root->m_id) { // Key is in left subtree
        if (!root->m_left) return root;
        if (id < root->m_left->m_id) { // Zig-Zig
            root->m_left->m_left = splay(root->m_left->m_left, id);
            root = rightRotate(root);
        } else if (id > root->m_left->m_id) { // Zig-Zag
            root->m_left->m_right = splay(root->m_left->m_right, id);
            if (root->m_left->m_right) root->m_left = leftRotate(root->m_left);
        }
        return (root->m_left) ? rightRotate(root) : root;
    } else { // Key is in right subtree
        if (!root->m_right) return root;
        if (id > root->m_right->m_id) { // Zag-Zag
            root->m_right->m_right = splay(root->m_right->m_right, id);
            root = leftRotate(root);
        } else if (id < root->m_right->m_id) { // Zag-Zig
            root->m_right->m_left = splay(root->m_right->m_left, id);
            if (root->m_right->m_left) root->m_right = rightRotate(root->m_right);
        }
        return (root->m_right) ? leftRotate(root) : root;
    }
}

// Converts existing tree nodes into an AVL-balanced structure
Ship* Fleet::nodeTransfer(Ship* root) {
    if (!root) return nullptr;
    root->m_left = nodeTransfer(root->m_left);
    root->m_right = nodeTransfer(root->m_right);
    return rebalance(root); // Rebalance after children are processed
}

// Helper to rebalance a single node in an AVL tree
Ship* Fleet::rebalance(Ship* root) {
    if (!root) return nullptr;
    int balance = BalanceFactor(root);

    // Apply rotations if unbalanced
    if (balance > 1 && BalanceFactor(root->m_left) >= 0) return rightRotate(root);
    if (balance < -1 && BalanceFactor(root->m_right) <= 0) return leftRotate(root);
    if (balance > 1 && BalanceFactor(root->m_left) < 0) {
        root->m_left = leftRotate(root->m_left);
        return rightRotate(root);
    }
    if (balance < -1 && BalanceFactor(root->m_right) > 0) {
        root->m_right = rightRotate(root->m_right);
        return leftRotate(root);
    }
    return root;
}

// Removes a node from a Splay tree
void Fleet::removeSplay(int id) {
    if (!m_root) return;

    m_root = splay(m_root, id); // Splay the node to be removed to the root

    if (m_root->m_id != id) return; // Key not found

    if (!m_root->m_left) { // No left child
        Ship* temp = m_root;
        m_root = m_root->m_right;
        delete temp;
    } else { // Has a left child
        Ship* temp = m_root;
        m_root = splay(m_root->m_left, id); // Splay max of left subtree to root
        m_root->m_right = temp->m_right; // Attach original right subtree
        delete temp;
    }
}