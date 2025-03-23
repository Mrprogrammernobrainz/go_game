#include <iostream>
#include <vector>
#include <queue>

const int SIZE = 9;

struct Point {
    int x, y;
};

class Board {
public:
    char grid[SIZE][SIZE]; 

    Board() {
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                grid[i][j] = '.';  //создание пустой доски
    }

    void print() const {
        std::cout << "  "; //вывод
        for (int j = 0; j < SIZE; ++j) {
            if (j + 1 < 10) std::cout << " "; 
            std::cout << j + 1 << " ";
        }
        std::cout << std::endl;

        for (int i = 0; i < SIZE; ++i) {
            if (i + 1 < 10) std::cout << " "; 
            std::cout << i + 1 << " ";
            for (int j = 0; j < SIZE; ++j)
                std::cout << grid[i][j] << "  ";
            std::cout << std::endl;
        }
    }

    bool placeStone(int x, int y, char color) {
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || grid[x][y] != '.')
            return 0; // ход невозможен
        grid[x][y] = color; // ставим фишку соотвествующего цвета
        return true;
    }

    char get(int x, int y) const { 
        return grid[x][y]; } //возвращает значение клетки в точке ху
    void set(int x, int y, char c) { 
        grid[x][y] = c; } //устанавливает значение клетки в точке ху
};

bool hasLiberties(const Board& board, int x, int y, char color) { //проверка есть ли у группы точек свобода если нет умирают
    std::vector<std::vector<bool>> visited(SIZE, std::vector<bool>(SIZE, false));
    std::queue<Point> q;
    Point p = { x, y }; //начальная точка
    q.push(p);//очередь для бфс
    visited[x][y] = 1;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        int dx[] = { -1, 1, 0, 0 }; //смещения для проверки соседей
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
                if (board.get(nx, ny) == '.') return true; // нашли свободу
                if (board.get(nx, ny) == color && !visited[nx][ny]) {
                    visited[nx][ny] = 1;
                    Point next = { nx, ny };
                    q.push(next);
                }
            }
        }
    }
    return false; // нет свободы
}

void removeGroup(Board& board, int x, int y, char color) {
    std::vector<std::vector<bool>> visited(SIZE, std::vector<bool>(SIZE, false));
    std::queue<Point> q;
    Point p = { x, y };
    q.push(p); //аналогичная логика как выше
    visited[x][y] = 1;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();
        board.set(current.x, current.y, '.'); // удаляем фишку

        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE &&
                board.get(nx, ny) == color && !visited[nx][ny]) {
                visited[nx][ny] = 1;
                Point next = { nx, ny };
                q.push(next);
            }
        }
    }
}

void calculateScore(const Board& board, int& blackScore, int& whiteScore) {
    std::vector<std::vector<bool>> visited(SIZE, std::vector<bool>(SIZE, false));

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board.get(i, j) == '.' && !visited[i][j]) {//если клетка пустая и не была посмещена начинаем подсчет
                std::queue<Point> q;
                q.push({ i, j });
                visited[i][j] = true;

                char surrounding = 0; // ни черные, ни белые камни пока не окружили
                bool isTerritory = true;
                int territorySize = 0;

                while (!q.empty()) {
                    Point current = q.front();
                    q.pop();
                    territorySize++;

                    int dx[] = { -1, 1, 0, 0 };
                    int dy[] = { 0, 0, -1, 1 };
                    for (int k = 0; k < 4; ++k) {
                        int nx = current.x + dx[k];
                        int ny = current.y + dy[k];
                        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
                            if (board.get(nx, ny) == '.') {
                                if (!visited[nx][ny]) {
                                    visited[nx][ny] = true;
                                    q.push({ nx, ny });
                                }
                            }
                            else if (board.get(nx, ny) == '*') {
                                if (surrounding == '@') {
                                    isTerritory = false;
                                }
                                surrounding = '*';
                            }
                            else if (board.get(nx, ny) == '@') {
                                if (surrounding == '*') {
                                    isTerritory = false;
                                }
                                surrounding = '@';
                            }
                        }
                    }
                }

                if (isTerritory) {
                    if (surrounding == '*') blackScore += territorySize;
                    else if (surrounding == '@') whiteScore += territorySize;
                }
            }
        }
    }

    // добавляем захваченные фишки
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board.get(i, j) == '*') blackScore++;
            else if (board.get(i, j) == '@') whiteScore++;
        }
    }
}

int main() {
    Board board;
    bool blackTurn = true;
    bool gameOver = false;
    int passCount = 0;
    std::cout << "Go game!" << std::endl;
    std::cout << "Black - *" << std::endl;
    std::cout << "White - @" << std::endl;
    while (gameOver == 0) {
        board.print();
        std::cout << (blackTurn ? "Black's turn (*)" : "White's turn (@)") << std::endl;
        std::cout << "Enter row and column (1-9) or 0 0 to pass: ";

        int x, y;
        std::cin >> x >> y;

        if (x == 0 && y == 0) {
            if (++passCount >= 2) gameOver = true;
            blackTurn = !blackTurn;
            continue;
        }

        x--; y--;
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
            std::cout << "Invalid coordinates!" << std::endl;
            continue;
        }

        char color = blackTurn ? '*' : '@';
        Board temp = board;

        if (!temp.placeStone(x, y, color)) {
            std::cout << "Invalid move!" << std::endl;
            continue;
        }

        // проверка на запрещенный по правилам самоубийственный ход
        if (!hasLiberties(temp, x, y, color)) {
            bool captured = false;
            char enemy = (color == '*') ? '@' : '*';

            int dx[] = { -1, 1, 0, 0 };
            int dy[] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE &&
                    temp.get(nx, ny) == enemy &&
                    !hasLiberties(temp, nx, ny, enemy)) {
                    captured = true;
                    break;
                }
            }

            if (captured == 0) {
                std::cout << "Suicide move is not allowed!" << std::endl;
                continue;
            }
        }

        // удалить группы
        char enemy;
        if (color == '*') {
            enemy = '@';
        }
        else {
            enemy = '*';
        }
        std::vector<Point> toRemove;
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                if (temp.get(i, j) == enemy && !hasLiberties(temp, i, j, enemy))
                    toRemove.push_back({ i, j });

        for (Point p : toRemove)
            removeGroup(temp, p.x, p.y, enemy);

        // ход
        board = temp;
        passCount = 0;
        blackTurn = !blackTurn;
    }

    // подсчет очков
    int blackScore = 0, whiteScore = 0;
    calculateScore(board, blackScore, whiteScore);

    // определение победителя
    std::cout << "Game over!" << std::endl;
    std::cout << "Black score: " << blackScore << std::endl;
    std::cout << "White score: " << whiteScore << std::endl;
    if (blackScore > whiteScore) {
        std::cout << "Black wins!" << std::endl;
    }
    else if (whiteScore > blackScore) {
        std::cout << "White wins!" << std::endl;
    }
    else {
        std::cout << "Draw!" << std::endl;
    }
    //ох как я устал это писать
    return 0;
}
