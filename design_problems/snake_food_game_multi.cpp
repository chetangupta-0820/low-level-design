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

    Player(string name, shared_ptr<MovementStrategy> movementStrategy): name(std::move(name)), movementStrategy(movementStrategy) {
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
    vector<deque<pair<int, int>>> snakes;
    unordered_map<int, unordered_map<int, bool>> posMap; // to check snake pos in 0(1)
    vector<shared_ptr<Player>> players;
    bool gameEnded;

    SnakeGame(shared_ptr<Board> board, vector<shared_ptr<Player>> players, vector<shared_ptr<Food>> foodItems) : board(board), players(players), foodItems(foodItems) {
        gameEnded = false;
        snakes.resize(players.size()); // Use resize instead of reserve to actually create the deques

        // Start snakes at different corners
        vector<pair<int, int>> startPositions = {
            {0, 0},              // top left
            {0, board->width-1}, // top right
            {board->length-1, 0}, // bottom left
            {board->length-1, board->width-1} // bottom right
        };

        for(int i=0; i < players.size(); ++i){
            auto pos = startPositions[i % startPositions.size()];
            snakes[i].push_back(pos);
            posMap[pos.first][pos.second] = true;
        }
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

    bool isCollision(const pair<int, int>& pos, int pid, bool isHead) {
        auto [i, j] = pos;
        
        // Check boundary collision
        if (i < 0 || i >= board->length || j < 0 || j >= board->width) {
            return true;
        }

        // Check snake collision (including other snakes)
        if (posMap[i][j]) {
            // Allow tail position if we're checking head movement
            if (isHead && pos == snakes[pid].back()) {
                return false;
            }
            return true;
        }
        
        return false;
    }

    void turn(int pid){
        auto& snake = snakes[pid];
        auto& player = players[pid];

        // get snake next pos
        auto curHead = snake.front();
        auto nextPos = player->move(curHead);
        auto& [nextI, nextJ] = nextPos;
    
        // check for any collision
        if(isCollision(nextPos, pid, true)){
            gameEnded = true;
            return;
        }

        int foodIdx = checkFood(nextPos);
        
        // update food Items map
        if(foodIdx !=-1){
            player->score += foodItems[foodIdx]->getPoints();
            foodItems.erase(foodItems.begin() + foodIdx);
        }
        else{
            auto [tailI, tailJ] = snake.back();
            posMap[tailI][tailJ] = false;
            snake.pop_back();
        }
        
        // update snake deque
        snake.push_front(nextPos);
        posMap[nextI][nextJ] = true;

        if(foodItems.empty())
            gameEnded = true;
    };
    
    void printState() const{
        cout << "\n=== Game State ===\n";
        // Print food items
        cout << "Food items remaining: " << foodItems.size() << "\n";
        cout << "Food positions: ";
        for (const auto& food : foodItems) {
            cout << "(" << food->row << "," << food->col << ") ";
        }
        cout << "\n\n";

        // Print player states
        for(size_t i=0; i < players.size(); ++i){
            cout << "Player: " << players[i]->name 
                 << "\n  Score: " << players[i]->score
                 << "\n  Head: (" << snakes[i].front().first << "," << snakes[i].front().second << ")"
                 << "\n  Length: " << snakes[i].size() << "\n";
        }
        cout << "================\n";
    }

};

int main(){

    auto board = make_shared<Board>(10, 10);
    auto foodItems = vector<shared_ptr<Food>>{
        make_shared<NormalFood>(1, 0),
        make_shared<NormalFood>(2, 0),
        make_shared<HealthyFood>(3, 0)
    };
    auto players = vector<shared_ptr<Player>>{
        make_shared<Player>("chetan", make_shared<HumanMovement>()),
        make_shared<Player>("bot", make_shared<ComputerMovement>())
    };

    auto game = make_shared<SnakeGame>(board, players, foodItems);
    int pid = 0, n = players.size();

    while (!game->gameEnded)
    {
        game->turn(pid);
        game->printState();
        pid = (pid+1)%n;
    }

    return  0;

}