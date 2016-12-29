#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000
#define TIME1 0.95
#define TIME2 9.0
#define TIME3 58.0
#define BEES_NUMBER 20
#define GOOD_SOLUTIONS 15
#define ITERATIONS_TO_IMPROVE 10

using namespace std;

struct tree{
    // x,y - position on map
    // h - height
    // d - diameter
    // c - weight per volume unit
    // p - price/value per volume unit
    // volume = h * d
    int x, y, h, d, c, p;
};

string directions[] = {"down", "right", "up", "left"};

int timeLimit, mapSize, treeCount;

vector<tree> trees;
int forest[MAXN][MAXN];
int cut_value[MAXK+1][4];

int calculate_distance(int i, int j) {
    return abs(trees[i].x-trees[j].x) + abs(trees[i].y-trees[j].y);
}
int tree_value(int i) {
    return trees[i].h * trees[i].d * trees[i].p;
}
int tree_weight(int i) {
    return trees[i].h * trees[i].d * trees[i].c;
}
void calculate_cut_values() {
    for(int i = 1; i <= treeCount; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int sum = tree_value(i);
            int height = trees[i].h;
            int x = trees[i].x;
            int y = trees[i].y;
            int cur_tree = i;
            int dx = 0;
            int	dy = 0;
            if(j == 0)
            {
                dy = -1;
            }
            if(j == 1)
            {
                dx = 1;
            }
            if(j == 2)
            {
                dy = 1;
            }
            if(j == 3)
            {
                dx = -1;
            }
            height--;
            while(height > 0)
            {
                x += dx;
                y += dy;
                if(x < 0 || y < 0 || x >= mapSize || y >= mapSize)
                    break;
                if(forest[x][y] > 0)
                {
                    if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
                    {
                        sum += tree_value(forest[x][y]);
                        height = trees[forest[x][y]].h;
                        cur_tree = forest[x][y];
                    }
                    else
                        break;
                }
                height--;
            }
            cut_value[i][j] = sum;
            //  cout << sum <<" ";
        }
        // cout <<endl;
    }
}

struct solution{
    int order[MAXK+1];
    int direction[MAXK+1];
    bool cutted[MAXK+1];
    bool used[MAXK+1];
    int lastPosition;
    int result = 0;
    int timeLeft;
    int iterationsLeft = ITERATIONS_TO_IMPROVE;
    bool operator < (const solution& sol) const
    {
        return (result < sol.result);
    }
    void init(){
        for(int i = 0; i <= treeCount; i++){
            used[i] = false;
        }
        timeLeft = timeLimit;
        iterationsLeft = ITERATIONS_TO_IMPROVE;
    }
    void prepare(){
        timeLeft = timeLimit;
        lastPosition = 0;
        for(int i = 0; i <= treeCount; i++){
            cutted[i] = false;
        }
        cutted[0] = true;
        result = 0;
    }
    void calculate_cut_values(int i) {
        for(int j = 0; j < 4; j++)
        {
            int sum = tree_value(i);
            int height = trees[i].h;
            int x = trees[i].x;
            int y = trees[i].y;
            int cur_tree = i;
            int dx = 0;
            int	dy = 0;
            if(j == 0)
            {
                dy = -1;
            }
            if(j == 1)
            {
                dx = 1;
            }
            if(j == 2)
            {
                dy = 1;
            }
            if(j == 3)
            {
                dx = -1;
            }
            height--;
            while(height > 0)
            {
                x += dx;
                y += dy;
                if(x < 0 || y < 0 || x >= mapSize || y >= mapSize)
                    break;
                if(forest[x][y] > 0 && !cutted[forest[x][y]])
                {
                    if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
                    {
                        sum += tree_value(forest[x][y]);
                        height = trees[forest[x][y]].h;
                        cur_tree = forest[x][y];
                    }
                    else
                        break;
                }
                height--;
            }
            cut_value[i][j] = sum;
            //  cout << sum <<" ";
        }
    }
    int cutTree(int i, int direction) {
        int sum = tree_value(i);
        int height = trees[i].h;
        int x = trees[i].x;
        int y = trees[i].y;
        int cur_tree = i;
        cutted[cur_tree] = true;
        int dx, dy;
        if(direction == 0)
        {
            dx = 0;
            dy = -1;
        }
        if(direction == 1)
        {
            dx = 1;
            dy = 0;
        }
        if(direction == 2)
        {
            dx = 0;
            dy = 1;
        }
        if(direction == 3)
        {
            dx = -1;
            dy = 0;
        }
        height--;
        while(height > 0)
        {
            x += dx;
            y += dy;
            if(x < 0 || y < 0 || x >= mapSize || y >= mapSize)
                break;
            if(forest[x][y] > 0 && !cutted[forest[x][y]])
            {
                if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
                {
                    sum += tree_value(forest[x][y]);
                    height = trees[forest[x][y]].h;
                    cur_tree = forest[x][y];
                    cutted[cur_tree] = true;
                }
                else
                    break;
            }
            height--;
        }
        return sum;
    }
    int slowCalculateResult(){
        prepare();
        for(int i = 1; i <= treeCount; i++){
            int cost = calculate_distance(order[i-1], order[i])+trees[order[i]].d;
            if(cost <= timeLeft && !cutted[order[i]]){

                //new
                calculate_cut_values(order[i]);
                direction[i] = (int)distance(cut_value[order[i]], max_element(cut_value[order[i]], cut_value[order[i]]+4));
                //
                result += cutTree(order[i], direction[i]);
                timeLeft -= cost;
                lastPosition++;
            } else {
                if(cutted[order[i]] && cost <= timeLeft){
                    result = -1;
                    return -1;
                } else {
                    return result;
                }
            }
        }
    }
    int calculateResult(int x = 0, int y = 0){
        prepare();
        for(int i = 1; i <= treeCount; i++){
            int cost = calculate_distance(order[i-1], order[i])+trees[order[i]].d;
            if(cost <= timeLeft && !cutted[order[i]]){
                if(x == i || y == i){
                    calculate_cut_values(order[i]);
                    direction[i] = (int)distance(cut_value[order[i]], max_element(cut_value[order[i]], cut_value[order[i]]+4));
                }
                result += cutTree(order[i], direction[i]);
                timeLeft -= cost;
                lastPosition++;
            } else {
                if(cutted[order[i]] && cost <= timeLeft){
                    result = -1;
                    return -1;
                } else {
                    return result;
                }
            }
        }
    }
    void createRandomSolution(){
        prepare();
        order[0] = 0;
        vector<int> possibilities;
        while(timeLeft > 0){
            for(int j = 1; j <= treeCount; j++){
                int cost = trees[j].d+calculate_distance(order[lastPosition], j);
                int direction = (int)distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
                if(!cutted[j] && timeLeft >= cost)
                {
                    possibilities.push_back(j);
                }
            }
            if(possibilities.size() > 0){
                int chosen_tree = possibilities[int(rand() % possibilities.size())];
                lastPosition++;
                used[chosen_tree] = true;
                order[lastPosition] = chosen_tree;
                direction[lastPosition] = (int)distance(cut_value[chosen_tree], max_element(cut_value[chosen_tree], cut_value[chosen_tree]+4));
                int cost = trees[chosen_tree].d+calculate_distance(order[lastPosition-1], chosen_tree);
                timeLeft -= cost;
                result += cutTree(order[lastPosition], direction[lastPosition]);
                possibilities.clear();
            }
            else{
                break;
            }
        }
        int temp_position = lastPosition+1;
        for(int i = 1; i <= treeCount; i++){
            if(!used[i]){
                order[temp_position] = i;
                direction[temp_position] = (int) distance(cut_value[i], max_element(cut_value[i], cut_value[i] + 4));
                temp_position++;
            }
        }
    }
    void createGreadySolution(){
        prepare();
        order[0] = 0;
        while(timeLeft > 0){
            double best_value = -1;
            int best_tree = -1;
            int best_direction = -1;
            for(int j = 1; j <= treeCount; j++)
            {
                calculate_cut_values(j);
                int cost = trees[j].d+calculate_distance(order[lastPosition], j);
                int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
                if(!cutted[j] && timeLeft >= cost)
                {
                    double res = cut_value[j][direction]/(double)cost;
                    if(res > best_value)
                    {
                        best_value = res;
                        best_tree = j;
                        best_direction = direction;
                    }
                }
            }
            if(best_value < 0)
                break;
            else{
                lastPosition++;
                used[best_tree] = true;
                order[lastPosition] = best_tree;
                direction[lastPosition] = best_direction;
                int cost = trees[best_tree].d+calculate_distance(order[lastPosition-1], best_tree);
                timeLeft -= cost;
                result += cutTree(order[lastPosition], direction[lastPosition]);
            }
        }
        int temp_position = lastPosition+1;
        for(int i = 1; i <= treeCount; i++){
            ::calculate_cut_values();
            if(!used[i]){
                order[temp_position] = i;
                direction[temp_position] = (int) distance(cut_value[i], max_element(cut_value[i], cut_value[i] + 4));
                temp_position++;
            }
        }
    }
    void mutate(){
        int rand1, rand2;
        if(lastPosition == 0)
            rand1 = 1;
        else
            rand1 = ( rand() % lastPosition ) + 1;
        do{
            rand2 = ( rand() % treeCount ) + 1;
        }while(rand1 == rand2);

        swap(order[rand1], order[rand2]);
        swap(direction[rand1], direction[rand2]);
    }
    void printResult(){

        timeLeft = timeLimit;
        for(int i = 1; i <= lastPosition; i++)
        {
            int horizontal = trees[order[i]].x-trees[order[i-1]].x;
            int vertical = trees[order[i]].y-trees[order[i-1]].y;
            while(horizontal > 0 && timeLeft > 0)
            {
                cout << "move right\n";
                horizontal--;
                timeLeft--;
            }
            while(horizontal < 0 && timeLeft > 0)
            {
                cout << "move left\n";
                horizontal++;
                timeLeft--;
            }
            while(vertical > 0 && timeLeft > 0)
            {
                cout << "move up\n";
                vertical--;
                timeLeft--;
            }
            while(vertical < 0 && timeLeft > 0)
            {
                cout << "move down\n";
                vertical++;
                timeLeft--;
            }
            if(trees[order[i]].d <= timeLeft){
                cout << "cut "<<directions[direction[i]]<<"\n";
                timeLeft -= trees[order[i]].d;
            }
            else
                return;
        }
    }
};

solution bestSolution;

void loadFromStream(istream &stream) {
    stream >> timeLimit >> mapSize >> treeCount;
    trees.resize(treeCount + 1);
    for (int i = 1; i <= treeCount; i++)
    {
        int x, y, h, d, c, p;
        stream >> x >> y >> h >> d >> c >> p;
        forest[x][y] = i; // assign tree index from trees vector to map field
        trees[i] = { x, y, h, d, c, p };
    }
}

int main(int argc, char** argv)
{
    const clock_t begin_time = clock();
    loadFromStream(cin);
    srand(static_cast<unsigned int>(time(0)));
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
    srand( time( NULL ) );
    calculate_cut_values();
    bestSolution.init();
    bestSolution.createGreadySolution();
    bestSolution.calculateResult();

    vector<solution> bees;

    for(int i = 0; i < BEES_NUMBER; i++){
        solution bee;
        if(!i){
            bee = bestSolution;
        }
        else
        {
            bee.init();
            bee.createRandomSolution();
            bee.calculateResult();
        }
        bees.push_back(bee);
    }
    sort(bees.rbegin(), bees.rend());
    int counter = 0;
    while(true)
    {
        counter++;
        for(int i = 0; i < BEES_NUMBER; i++){
            if(i > GOOD_SOLUTIONS || bees[i].iterationsLeft == 0){
                bees[i].init();
                bees[i].createRandomSolution();
            } else {
                solution temp = bees[i];
                solution currentBest = temp;
                //temp.mutate();
                //temp.calculateResult();
                int rand1;
                if(temp.lastPosition == 0)
                    rand1 = 1;
                else
                    rand1 = ( rand() % temp.lastPosition ) + 1;
                for(int j = 1; j <= treeCount; j++){
                    if(j == rand1)
                        continue;
                    temp = bees[i];
                    swap(temp.order[i], temp.order[rand1]);
                    swap(temp.direction[i], temp.direction[rand1]);
                    temp.calculateResult(i, rand1);
                    if(temp.result > currentBest.result)
                        currentBest = temp;
                }
                if(bees[i].result >= currentBest.result){
                    bees[i].iterationsLeft--;
                }
                else {
                    bees[i] = currentBest;
                    bees[i].iterationsLeft = ITERATIONS_TO_IMPROVE;
                }
            }
            if(bees[i].result > 100000)
                cout << bees[i].result<<"\n";
        }
        sort(bees.rbegin(), bees.rend());
        if(bestSolution.result < bees[0].result){
            bestSolution = bees[0];
        }
        if(
                (treeCount <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
                (treeCount <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
                (treeCount <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
                ){
            //cout <<counter<<"\n";
            cout << bestSolution.result <<"\n";
            //bestSolution.printResult();
            return 0;
        }
    }
}