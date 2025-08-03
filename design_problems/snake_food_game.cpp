#include<bits/stdc++.h>
using namespace std;

class Food{
public:
    int row, col;
    virtual ~Food() = default;
    Food() = default;
    Food(int row,int col) : row(row), col(col) {}
    virtual int getPoints() = 0;
};

class NormalFood: public Food{
public:
    NormalFood() = default;
    NormalFood(int row, int col) : Food(row, col) {}
    int getPoints(){
        return 1;
    }
};

class HealthyFood: public Food{
public:
    HealthyFood() = default;
    HealthyFood(int row, int col) : Food(row, col) {}
    int getPoints(){
        return 3;
    }
};

class Board{
public:
    int length, width;
    Board(int length, int width) : length(length), width(width){}
};

class MovementStrategy{
public:
    virtual pair<int, int> move(pair<int, int> cur) = 0;
};

class HumanMovement: public MovementStrategy{
public:
    pair<int, int> move(pair<int, int> current){
        char dir; cin>>dir;
        auto [i, j] = current;

        switch (dir)
        {
            case 'D' : return {i+1, j};
            case 'U' : return {i-1, j};
            case 'L' : return {i, j-1};
            case 'R' : return {i, j+1};  
        }

        return current;
    }
};


class ComputerMovement: public MovementStrategy{
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
public:
    pair<int, int> move(pair<int, int> current){
        int idx = rand()%4;
        return {current.first + directions[idx].first, current.second + directions[idx].second};
    }
};

class Player{
    shared_ptr<MovementStrategy> movementStrategy;
public:
    string name;
    int score;

    Player(string name, shared_ptr<MovementStrategy> movementStrategy): name(name), movementStrategy(movementStrategy) {
        score = 0;
    }

    pair<int, int> move(pair<int, int> current){
        return movementStrategy->move(current);
    }
};

class SnakeGame{
public:
    shared_ptr<Board> board;
    vector<shared_ptr<Food>> foodItems;
    deque<pair<int, int>> snake;
    unordered_map<int, unordered_map<int, bool>> posMap; // to check snake pos in 0(1)
    int foodIdx;
    shared_ptr<Player> player;
    bool gameEnded;

    SnakeGame(shared_ptr<Board> board, shared_ptr<Player> player, vector<shared_ptr<Food>> foodItems) : board(board), player(player), foodItems(foodItems) {
        snake.push_back({0, 0});
        posMap[0][0] = true;
        foodIdx = 0;
        gameEnded = false;
    }

    // all food items at the same time
    int checkFood(pair<int, int> curHead){
        auto [i, j] = curHead;
        for(int idx = 0; idx < foodItems.size(); ++idx){
            if(foodItems[idx]->row == i && foodItems[idx]->col == j)
                return idx;
        }
        return -1;
    }

    // check if current food item is consumed
    bool checkCurFood(pair<int, int> curHead){
        auto [i, j] = curHead;
        if(foodItems[foodIdx]->row == i && foodItems[foodIdx]->col == j)
            return true;
        return false;
    }

    void turn(){
        // get snake next pos
        // update pos map
        auto curHead = snake.front();
        auto nextPos = player->move(curHead);
        auto& [nextI, nextJ] = nextPos;
    
        // check if it touches boundary or itself
        bool crosesBoundary = nextI<0 || nextI >= board->length || nextJ<0 || nextJ>=board->width ; 
        bool bitesItself = (nextPos != snake.back()) && posMap[nextI][nextJ] ;

        // set game state
        if(crosesBoundary || bitesItself){
            gameEnded = true;
            return;
        }
        
        // update food Items map
        if(checkCurFood(nextPos)){
            player->score += foodItems[foodIdx]->getPoints();
            foodIdx++;
        }
        else{
            auto [tailI, tailJ] = snake.back();
            posMap[tailI][tailJ] = false;
            snake.pop_back();
        }
        
        // update snake deque
        snake.push_front(nextPos);
        posMap[nextI][nextJ] = true;

        if(foodIdx == foodItems.size())
            gameEnded = true;
    };

    void printState(){
        auto [i, j] = snake.front();
        cout<<"Cur Snake Pos: "<< i<<", "<<j<<endl;
        cout<<"Player score: "<<player->score<<endl;
        cout<<"Food Index: "<<foodIdx<<endl;
    }

};

int main(){

    auto board = make_shared<Board>(10, 10);
    auto foodItems = vector<shared_ptr<Food>>{
        make_shared<NormalFood>(1, 0),
        make_shared<NormalFood>(2, 0),
        make_shared<HealthyFood>(3, 0)
    };
    string name = "chetan";
    auto chetan = make_shared<Player>(name, make_shared<HumanMovement>());

    auto game = make_shared<SnakeGame>(board, chetan, foodItems);

    while (!game->gameEnded)
    {
        game->turn();
        game->printState();
    }

    return  0;

}