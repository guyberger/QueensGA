#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <stdexcept>

#define DEPTH 2048      // Amount of genes in a single generation.
#define ITERATIONS 700  // Number of tries to find a solution.
#define ELITE 0.1     // The elite precentage of the genes to be moved to the next generation.

int QC = 20;    // Queens Count

using namespace std;
template <class T>
void GA_swap(T& a, T& b){
    T c(a);
    a = b;
    b = c;
}
/*
    A solution holds a vector of places for the N-Queens. It is a gene in a generation, each with
    it's appropriate fitness.
*/
class Solution{
    vector<int> places;
    int fitness;
public:
    /*
        The constructor randomly selects places the queens and then calculates the solutions's fitness.
    */
    Solution(){
        int* queens = new int[QC];
        for(int i=0;i<QC;++i){
            queens[i] = i;
        }
        int len = QC;
        for(int i=0; i<QC; ++i){
            int index = rand() % len;
            int add = queens[index];
            places.push_back(add);
            queens[index] = queens[len-1];
            len--;
        }
        fitness = calc_fitness();
        delete[] queens;
    }
    int calc_fitness(){
        int count = 0;
        for(unsigned int i=0 ; i < places.size()-1 ; ++i){
            unsigned int j = i;
            int curr = places[j];
            int diff = 1;
            while(j < places.size()-1){
                int next = places[j+1];
                if(curr == next || abs(curr-next) == diff)
                    count++;
                ++diff;
                ++j;
            }
        }
        return count;
    }
    int get_fitness(){
        return fitness;
    }
    void updateFitness(){
        fitness = calc_fitness();
    }
    int& operator[](int i){
        return places[i];
    }
    unsigned int size(){
        return places.size();
    }
    /*
        Mates 2 genes and return a child. The mating algorithm chooses a number of locaindexes from the
        first parent, copies them and the appropriate queens to the same indexes of the child and then
        takes the rest of the queens from the other parent and put them in the free indexes of the child
        while keeping their relative order.
    */
    Solution* mate(Solution s){
        Solution* res = new Solution;
        int* rand_hist = new int[QC];
        int len_rand = QC;
        for(int i=0;i<len_rand;++i){
            rand_hist[i] = i;
        }
        int* hist = new int[QC];
        int* hist_index = new int[QC];
        int to_keep = rand()%(QC-1);
        for(int i=0; i<to_keep && len_rand>=1; ++i){
            int i_hist = rand() % len_rand;
            int index = rand_hist[i_hist];
            GA_swap(rand_hist[i_hist],rand_hist[len_rand-1]);
            len_rand--;
            hist[places[index]]++;
            hist_index[index]++;
        }
        int last = 0;
        for(int i=0;i<QC;i++){
            if(hist_index[i]==0){
                for(int j=last;j<QC;j++){
                    if(hist[s.places[j]]==0){
                        res->places[i] = s.places[j];
                        last = j+1;
                        break;
                    }
                }
            }
            else{
                res->places[i] = places[i];
            }
        }
        res->updateFitness();
        delete[] rand_hist;
        delete[] hist;
        delete[] hist_index;
        return res;
    }
};
ostream& operator<<(ostream& output, Solution s){
    for(unsigned int i=0;i<s.size();++i){
        output << s[i]+1 << " ";
    }
    output << endl;
    return output;
}
class Board{
    vector<Solution*> solutions;
public:
    Board(){
        for(int i=0; i<DEPTH; ++i){
            Solution* s1 = new Solution();
            solutions.push_back(s1);
        }
    }
    Solution* operator[](int i){
        return solutions[i];
    }
    Board& operator=(const Board& b){
        for(int i=0;i<DEPTH;++i){
            solutions[i] = b.solutions[i];
        }
        return *this;
    }
    /*
        The sort function sorts only the Elite genes.
    */
    void sort(){
        int min, i_min;
        for(int i=0; i<DEPTH*ELITE; ++i){
            min = (*(solutions[i])).get_fitness();
            i_min = i;
            for(int j=i+1; j<DEPTH; ++j){
                int curr = (*(solutions[j])).get_fitness();
                if(min>curr){
                    i_min = j;
                    min = curr;
                }
            }
            Solution* temp = solutions[i];
            solutions[i] = solutions[i_min];
            solutions[i_min] = temp;
        }
    }
    /*
        Finds the strongest gene in the gene (with minimum fitness).
    */
    int best(){
        int min = (*(solutions[0])).get_fitness(), i_min = 0;
        for(int i=1; i<DEPTH; ++i){
            int curr = (*(solutions[i])).get_fitness();
            if(min>curr){
                i_min = i;
                min = curr;
            }
        }
        return i_min;
    }
    /*
        Copies the elite chromosomes to the buffer.
    */
    void elite(Board& buffer){
        Board temp = *this;
        temp.sort();
        for(int i=0; i<DEPTH*ELITE; ++i){
            buffer.solutions[i] = temp.solutions[i];
        }
    }
    /*
        Mating has 2 steps:
            1. Go through the entire generation and select 2 genes to mate in each iteration, using
            Roulette Selection.
            2. Mate the selected 2 genes and add the child to the buffer.
    */
    void mate(Board& buffer){
        elite(buffer);
        double weight_sum = 0;
        for(int i=0; i<DEPTH; ++i){
            int fitness = solutions[i]->get_fitness();
            if(!fitness) cout << "FOUND!!";
            weight_sum += 1/(double)fitness;
        }
        for(int i=DEPTH*ELITE;i<DEPTH;i++){
            int mate1 = rouletteSelect(weight_sum);
            int mate2 = rouletteSelect(weight_sum);
            assert(mate1 >= 0 && mate2 >= 0);
            buffer.solutions[i] = (*(solutions[mate1])).mate(*(solutions[mate2]));
        }
    }
    /*
        Each gene has it's fitness, and the stronger it is the more likely the gene will be
        chosen to mate. @param - Weight_sum = sum of all the fitnesses in the generation.
        Randomly select a number between 0 and the weight_sum, then keep subtracting the gene's
        fitness until the number is <= 0.
    */
    int rouletteSelect(double weight_sum){
        double value = ((double) rand() / (RAND_MAX)) * weight_sum;
        for(int i=0; i<DEPTH; ++i){
            int fitness = solutions[i]->get_fitness();
            if(!fitness) cout << "FOUND222222";
            value -= 1/(double)fitness;
            if(value <= 0)
                return i;
        }
        return DEPTH-1;
    }
    /*
        Mutate a gene iterate through all the genes and switches the places of 2 random queens.
    */
    void mutate(){
        for(int i=0;i<DEPTH;++i){
            if(solutions[i]->get_fitness() == 0) return;
            int first = rand() % QC;
            int second = rand() % QC;
            int temp = (*solutions[i])[first];
            (*solutions[i])[first] = (*solutions[i])[second];
            (*solutions[i])[second] = temp;
            (*solutions[i]).updateFitness();
        }
    }
};
int main(int argc, char** argv)
{
    if(argc <= 1 || argc >= 3){
        cout << "Run \"Queens <board_size>\"" << endl;
        return 0;
    }
    std::string arg = argv[1];
    try {
    std::size_t pos;
    QC = std::stoi(arg, &pos);
    if (pos < arg.size()) {
        std::cerr << "Trailing characters after number: " << arg << '\n';
        return 1;
    }
    } catch (std::invalid_argument const &ex) {
        std::cerr << "Invalid number: " << arg << '\n';
        return 1;
    } catch (std::out_of_range const &ex) {
        std::cerr << "Number out of range: " << arg << '\n';
        return 1;
    }
    srand(time(NULL));

/*
    Initializing board1 to be the main board and the buffer to hold the next generation.
*/
    Board board1;
    Board buffer;

    int it = 0;     // iterations counter.
/*
    The genes mate and mutate until the best gene has fitness of 0 = solved for N-Queens.
*/
    while(board1[board1.best()]->get_fitness() && it<ITERATIONS){
        board1.mate(buffer);
        board1 = buffer;
/*
    The gene mutate with probability of 0.25.
*/
        if(((double) rand() / (RAND_MAX))<0.25){
            board1.mutate();
        }
        ++it;
        cout << "best yet: " << *(board1[board1.best()]);
    }
    cout << "The places are: " << *(board1[0]) << endl;
    cout << "Iterations: " << it << endl;
    return 0;
}
