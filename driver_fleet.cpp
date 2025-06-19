#include "fleet.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(){}
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { 
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        m_generator = std::mt19937(seedNum);
    }
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10);// 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }
    void getShuffle(vector<int> & array){
        // this function provides a list of all values between min and max
        // in a random order
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

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

    int getRandNum(){
        // this function returns integer numbers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        double result = m_uniReal(m_generator);
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    int getMin(){return m_min;}
    int getMax(){return m_max;}
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester{};
int main(){
    Random idGen(MINID,MAXID);
    Random typeGen(0,4);
    {
        Fleet team(BST);
        int teamSize = 10;
        int tempID = 0;
        int ID = 0;
        SHIPTYPE shipType = DEFAULT_TYPE;
        for(int i=0;i<teamSize;i++){
            ID = idGen.getRandNum();
            shipType = static_cast<SHIPTYPE>(typeGen.getRandNum());
            if (i == teamSize / 2) tempID = ID;//we store this ID for later use
            Ship ship(ID,shipType);
            team.insert(ship);
        }
        cout << "\nDump the BST tree after inserting " << teamSize << " nodes:\n\n";
        team.dumpTree();
        cout << endl;
        
        team.remove(tempID);
        cout << "\nDump after removig the node with ID: " << tempID << "\n\n";
        team.dumpTree();
        cout << endl;
        
        team.setType(AVL);
        cout << "\nDump after converting the tree from BST to AVL: " << "\n\n";
        team.dumpTree();
        cout << endl;
    }

    // AVL Balance Test
    {
        Fleet avlTeam(AVL);
        int avlTeamSize = 15;
        for (int i = 0; i < avlTeamSize; i++) {
            int ID = idGen.getRandNum();
            SHIPTYPE shipType = static_cast<SHIPTYPE>(typeGen.getRandNum());
            Ship ship(ID, shipType);
            avlTeam.insert(ship);
        }

        // We can only check balance by observing the tree structure via dumpTree()
        // Visual inspection is needed to see if the AVL tree looks balanced.
        // We cannot use BalanceFactor or any other private info.
        cout << "\nAVL tree after " << avlTeamSize << " insertions:\n\n";
        avlTeam.dumpTree();
        cout << endl;
    }
    return 0;
}