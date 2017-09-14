#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>

#define QUEENS 40
#define DEPTH 2048
#define ITERATIONS 700
#define MATE_PCT 0.4
#define TOP_PCT 0.1

using namespace std;
template <class T>
void GA_swap(T& a, T& b){
    T c(a);
    a = b;
    b = c;
}
class Solution{
    vector<int> places;
    int fitness;
public:
    Solution(){
        int queens[QUEENS] = {0};
        for(int i=0;i<QUEENS;++i){
            queens[i] = i;
        }
        int len = QUEENS;
        for(int i=0; i<QUEENS; ++i){
            int index = rand() % len;
            int add = queens[index];
            places.push_back(add);
            queens[index] = queens[len-1];
            len--;
        }
        fitness = calc_fitness();
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
    Solution* mate(Solution s){
        Solution* res = new Solution;
        int rand_hist[QUEENS] = {0};
        int len_rand = QUEENS;
        for(int i=0;i<len_rand;++i){
            rand_hist[i] = i;
        }
        int hist[QUEENS] = {0};
        int hist_index[QUEENS] = {0};
        int to_keep = rand()%(QUEENS-1);
        /*
        int to_keep = rand() % QUEENS;
        to_keep = (to_keep < QUEENS/2) ? QUEENS/2 : to_keep;
        */
        for(int i=0; i<to_keep && len_rand>=1; ++i){
            int i_hist = rand() % len_rand;
            int index = rand_hist[i_hist];
            GA_swap(rand_hist[i_hist],rand_hist[len_rand-1]);
            len_rand--;
            hist[places[index]]++;
            hist_index[index]++;
        }
        int last = 0;
        for(int i=0;i<QUEENS;i++){
            if(hist_index[i]==0){
                for(int j=last;j<QUEENS;j++){
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
            //delete solutions[i];
            solutions[i] = b.solutions[i];
        }
        return *this;
    }
    void sort(){
        int min, i_min;
        for(int i=0; i<DEPTH*TOP_PCT; ++i){
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
    void elite(Board& buffer){
        Board temp = *this;
        temp.sort();
        for(int i=0; i<DEPTH*TOP_PCT; ++i){
            buffer.solutions[i] = temp.solutions[i];
        }
    }
    void mate(Board& buffer){
        /*
        for(int i=0;i<DEPTH*TOP_PCT;++i){
            buffer.solutions[i] = solutions[i];
        }
        */
        elite(buffer);
        double weight_sum = 0;
        for(int i=0; i<DEPTH; ++i){
            int fitness = solutions[i]->get_fitness();
            if(!fitness) cout << "FOUND!!";
            weight_sum += 1/(double)fitness;
        }
        for(int i=DEPTH*TOP_PCT;i<DEPTH;i++){
            int mate1 = rouletteSelect(weight_sum);
            int mate2 = rouletteSelect(weight_sum);
            //if(mate1<0 || mate2<0) return;
            //if(mate1 == mate2) continue;
            assert(mate1 >= 0 && mate2 >= 0);
            buffer.solutions[i] = (*(solutions[mate1])).mate(*(solutions[mate2]));
        }
    }
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
    void mutate(){
        for(int i=0;i<DEPTH;++i){
                /*
            int problem = 0;
            for(unsigned int i=0 ; i < QUEENS-1 ; ++i){
                unsigned int j = i+1;
                int first = i;
                problem = j;
                int diff = 1;
                bool prob_found = false;
                while(j < places.size()){
                    int next = places[j];
                    if(abs(places[first]-next) == diff){
                        problem = j;
                        prob_found = true;
                        break;
                    }
                    ++diff;
                    ++j;
                }
                if(prob_found) break;
            }
            int switch_index = rand() % QUEENS;
            int temp = places[problem];
            places[problem] = places[switch_index];
            places[switch_index] = temp;
            */
            if(solutions[i]->get_fitness() == 0) return;
            int first = rand() % QUEENS;
            int second = rand() % QUEENS;
            int temp = (*solutions[i])[first];
            (*solutions[i])[first] = (*solutions[i])[second];
            (*solutions[i])[second] = temp;
            (*solutions[i]).updateFitness();
        }
    }
    void merge(Board a, Board b){
        int ia, ib, ic;
        for(ia=0,ib=0,ic=0;ia<DEPTH&&ib<DEPTH&&ic<DEPTH;){
            int a_fit = a.solutions[ia]->get_fitness();
            int b_fit = b.solutions[ib]->get_fitness();
            if(a_fit<=b_fit){
                solutions[ic++] = a.solutions[ia++];
            }
            else{
                solutions[ic++] = a.solutions[ib++];
            }
        }
        if(ic<DEPTH&&ia<DEPTH){
            while(ic<DEPTH&&ia<DEPTH)
                solutions[ic++] = a.solutions[ia++];
        }
        else if(ic<DEPTH&&ib<DEPTH)
            while(ic<DEPTH&&ib<DEPTH)
                solutions[ic++] = a.solutions[ib++];
    }
};
/*
// Returns a uniformly distributed double value between 0.0 and 1.0
double randUniformPositive() {
	// easiest implementation
	return ((double) rand() / (RAND_MAX));
}

int rouletteSelect(Board& board, double weight_sum){
    int to_mate = MATE_PCT*DEPTH;
    to_mate = (to_mate%2) ? to_mate-1 : to_mate;
    double value = randUniformPositive() * weight_sum;
    for(int i=0; i<to_mate; ++i){
        double fitness = (double)((board[i])->get_fitness());
        if(!fitness) return -1;
        value -= 1/fitness;
        if(value <= 0)
            return i;
    }
    return to_mate-1;

    for(int i=0; i<to_mate-1 && i<DEPTH-1; i+=2){
        (*((*board)[i])).mate(*((*board)[i+1]));
    }
}

void mate_board(Board& board){
    int to_mate = MATE_PCT*DEPTH;
    to_mate = (to_mate%2) ? to_mate-1 : to_mate;
    double weight_sum = 0;
    for(int i=0; i<to_mate; ++i){
        double fitness = (double)((board[i])->get_fitness());
        if(!fitness) return;
        weight_sum += 1/fitness;
    }
    for(int i=0;i<to_mate/2;i++){
        int mate1 = rouletteSelect(board, weight_sum);
        int mate2 = rouletteSelect(board, weight_sum);
        if(mate1<0 || mate2<0) return;
        if(mate1 == mate2) continue;
        (*(board[mate1])).mate(*(board[mate2]));
    }
}
*/
int main()
{
    srand(time(NULL));
    Board board1;
    Board buffer;
    //board1.sort();
    int it = 0;
    while(board1[board1.best()]->get_fitness() && it<ITERATIONS){
        board1.mate(buffer);
        //buffer.sort();
        board1 = buffer;
        if(((double) rand() / (RAND_MAX))<0.25){
            board1.mutate();
            //board1.sort();
        }
        //board1.sort();
        ++it;
        cout << "best yet: " << *(board1[board1.best()]);
    }
    cout << "The places are: " << *(board1[0]) << endl;
    cout << "Iterations: " << it << endl;
    return 0;
}
