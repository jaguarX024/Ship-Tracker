#include "fleet.h" 
#include <math.h>
#include <algorithm> 
#include <random> 
#include <vector> 
using namespace std;

// Enum to define different types of random number distributions
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};

// Random class for generating various types of random numbers and sequences
class Random {
public:
    Random(){} // Default constructor

    // Constructor to initialize the random number generator with specific parameters
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            // Initialize for normal distribution to generate integer numbers
            m_generator = std::mt19937(m_device()); // Uses a non-deterministic seed from hardware
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            // Initialize for uniform integer distribution
            // Using a fixed seed value generates always the same sequence of pseudorandom numbers
            m_generator = std::mt19937(10); // 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { // For generating real numbers with uniform distribution
            m_generator = std::mt19937(10); // 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { // For shuffling, uses a non-deterministic seed
            m_generator = std::mt19937(m_device());
        }
    }

    // Allows setting a custom seed for the random number generator
    void setSeed(int seedNum){
        m_generator = std::mt19937(seedNum);
    }

    // Initializes the random generator for uniform integer distribution with new min/max
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10); // 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }

    // Populates a vector with numbers from min to max and shuffles them in a random order
    void getShuffle(vector<int> & array){
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator); // Randomly shuffles the elements
    }

    // Populates an array with numbers from min to max and shuffles them in a random order
    void getShuffle(int array[]){
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    // Generates a random integer based on the selected distribution type
    int getRandNum(){
        int result = 0;
        if(m_type == NORMAL){
            // Returns a random number from a normal distribution, constrained by min and max values
            result = m_min - 1; // Initialize to an invalid value to ensure loop runs
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            // Generates a random integer between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    // Generates a random real number, rounded to two decimal places
    double getRealRandNum(){
        double result = m_uniReal(m_generator);
        result = std::floor(result*100.0)/100.0; // Rounds down to two decimal places
        return result;
    }

    // Generates a random string of a specified length
    string getRandString(int size){
        // The parameter size specifies the length of string we ask for
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum(); // Appends random ASCII characters
        }
        return output;
    }
    
    // Getter for minimum value
    int getMin(){return m_min;}
    // Getter for maximum value
    int getMax(){return m_max;}
private:
    int m_min; // Minimum value for random generation
    int m_max; // Maximum value for random generation
    RANDOM m_type; // Type of random distribution
    std::random_device m_device; // Non-deterministic random number generator source
    std::mt19937 m_generator; // Mersenne Twister pseudo-random number generator
    std::normal_distribution<> m_normdist; // Normal distribution object
    std::uniform_int_distribution<> m_unidist; // Integer uniform distribution object
    std::uniform_real_distribution<double> m_uniReal; // Real uniform distribution object

};

// Placeholder Tester class 
class Tester{};

// Main function to demonstrate Fleet (tree) functionality
int main(){
    // Initialize random number generators for Ship IDs and types
    Random idGen(MINID,MAXID);
    Random typeGen(0,4); // Assuming SHIPTYPE enum ranges from 0 to 4 (DEFAULT_TYPE to SHIPTYPE_LAST)

    { // Block for BST (Binary Search Tree) operations
        Fleet team(BST); // Create a Fleet object using a Binary Search Tree
        int teamSize = 10; // Number of ships to insert
        int tempID = 0; // To store an ID for later removal
        int ID = 0;
        SHIPTYPE shipType = DEFAULT_TYPE;

        // Insert ships with random IDs and types into the BST
        for(int i=0;i<teamSize;i++){
            ID = idGen.getRandNum();
            shipType = static_cast<SHIPTYPE>(typeGen.getRandNum());
            if (i == teamSize / 2) tempID = ID; // Store the ID of the middle ship for removal
            Ship ship(ID,shipType);
            team.insert(ship); // Insert the ship into the fleet
        }
        cout << "\nDump the BST tree after inserting " << teamSize << " nodes:\n\n";
        team.dumpTree(); // Display the structure of the BST
        cout << endl;
        
        team.remove(tempID); // Remove the stored ship ID from the BST
        cout << "\nDump after removig the node with ID: " << tempID << "\n\n";
        team.dumpTree(); // Display the BST after removal
        cout << endl;
        
        team.setType(AVL); // Convert the existing tree from BST to AVL
        cout << "\nDump after converting the tree from BST to AVL: " << "\n\n";
        team.dumpTree(); // Display the tree after conversion
        cout << endl;
    }

    { 
        Fleet avlTeam(AVL); // Create another Fleet object, initialized as an AVL tree
        int avlTeamSize = 15; // Number of ships to insert into the AVL tree

        // Insert ships with random IDs and types into the AVL tree
        for (int i = 0; i < avlTeamSize; i++) {
            int ID = idGen.getRandNum();
            SHIPTYPE shipType = static_cast<SHIPTYPE>(typeGen.getRandNum());
            Ship ship(ID, shipType);
            avlTeam.insert(ship); // AVL tree should automatically balance upon insertion
        }

        // We can only check balance by observing the tree structure via dumpTree()
        // Visual inspection is needed to see if the AVL tree looks balanced 
        // We cannot use BalanceFactor or any other private info directly from this test file.
        cout << "\nAVL tree after " << avlTeamSize << " insertions:\n\n";
        avlTeam.dumpTree(); // Display the AVL tree structure
        cout << endl;
    }
    return 0; // Indicate successful execution
}