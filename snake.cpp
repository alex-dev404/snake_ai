#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include <queue>
#include <climits>
#include <cstdlib>

struct SnakeSegment {
    int x, y;
};

enum class Direction { None, Up, Down, Left, Right };

const int tileSize = 25;
const int boardSize = 25;
const int screenWidth = tileSize * boardSize;
const int screenHeight = tileSize * boardSize;

std::vector<std::vector<int>> distances(boardSize, std::vector<int>(boardSize, INT_MAX));

void updateDistances(const sf::Vector2i& newApplePosition) {
    std::queue<sf::Vector2i> q;
    q.push(newApplePosition);
    distances[newApplePosition.y][newApplePosition.x] = 0;

    while (!q.empty()) {
        sf::Vector2i current = q.front();
        q.pop();

        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize && distances[ny][nx] == INT_MAX) {
                distances[ny][nx] = distances[current.y][current.x] + 1;
                q.push(sf::Vector2i(nx, ny));
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Snake Game");
    sf::Font font;
    font.loadFromFile("minecraft_font.ttf");
    std::deque<SnakeSegment> snake{{10, 10}, {9, 10}, {8, 10}};
    Direction dir = Direction::Right;

    sf::Vector2i applePosition(std::rand() % boardSize, std::rand() % boardSize);

    updateDistances(applePosition);

    sf::Clock clock;
    float moveTimer = 0.0f;
    float moveDelay = 0.1f;

    while (window.isOpen()) {
        sf::Event event;
        window.clear(sf::Color::Black);
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir != Direction::Down) {
            dir = Direction::Up;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir != Direction::Up) {
            dir = Direction::Down;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir != Direction::Right) {
            dir = Direction::Left;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir != Direction::Left) {
            dir = Direction::Right;
        }

        if (clock.getElapsedTime().asSeconds() > moveDelay) {
            moveTimer += clock.restart().asSeconds();

            if (moveTimer >= moveDelay) {
                moveTimer = 0.0f;

                SnakeSegment newSegment = snake.front();
                switch (dir) {
                    case Direction::Up:
                        newSegment.y--;
                        break;
                    case Direction::Down:
                        newSegment.y++;
                        break;
                    case Direction::Left:
                        newSegment.x--;
                        break;
                    case Direction::Right:
                        newSegment.x++;
                        break;
                    default:
                        break;
                }

                if (newSegment.x == applePosition.x && newSegment.y == applePosition.y) {
                    snake.push_front(newSegment);
                    applePosition = sf::Vector2i(std::rand() % boardSize, std::rand() % boardSize);
                    updateDistances(applePosition);
                } else {
                    snake.push_front(newSegment);
                    snake.pop_back();
                }

                if (newSegment.x < 0 || newSegment.y < 0 || newSegment.x >= boardSize || newSegment.y >= boardSize) {
                    window.close();
                }

                for (auto it = std::next(snake.begin()); it != snake.end(); ++it) {
                    if (newSegment.x == it->x && newSegment.y == it->y) {
                        window.close();
                    }
                }
            }
        }

        for (int y = 0; y < boardSize; ++y) {
            for (int x = 0; x < boardSize; ++x) {
                if (distances[y][x] != INT_MAX) {
                    sf::Text text(std::to_string(distances[y][x]), font, 12);
                    text.setPosition(x * tileSize, y * tileSize);
                    text.setFillColor(sf::Color::White);
                    window.draw(text);
                }
            }
        }

        for (const auto &segment : snake) {
            sf::RectangleShape bodyPart(sf::Vector2f(tileSize - 1, tileSize - 1));
            bodyPart.setPosition(segment.x * tileSize, segment.y * tileSize);
            bodyPart.setFillColor(sf::Color::Green);
            window.draw(bodyPart);
        }

        sf::RectangleShape apple(sf::Vector2f(tileSize - 1, tileSize - 1));
        apple.setPosition(applePosition.x * tileSize, applePosition.y * tileSize);
        apple.setFillColor(sf::Color::Red);
        window.draw(apple);

        window.display();
    }

    return 0;
}
