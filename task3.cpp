#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int blockSize = 20;
const int width = 800;
const int height = 600;
const int cols = width / blockSize;
const int rows = height / blockSize;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Segment {
    int x, y;
    Segment(int x, int y) : x(x), y(y) {}
};

class SnakeGame {
private:
    sf::RenderWindow window;
    sf::RectangleShape block;
    std::vector<Segment> snake;
    Direction dir;
    Segment food;
    int score;
    int speed;
    sf::Font font;
    sf::Text scoreText;
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    sf::Sound eatSound, gameOverSound;

public:
    SnakeGame()
        : window(sf::VideoMode(width, height), "Snake Game"),
          food(rand() % cols, rand() % rows),
          score(0),
          speed(150)
    {
        srand(static_cast<unsigned>(time(0)));
        dir = RIGHT;
        snake.emplace_back(5, 5);
        block.setSize(sf::Vector2f(blockSize - 2, blockSize - 2));
        block.setFillColor(sf::Color::Green);

        if (!font.loadFromFile("arial.ttf"))
            throw std::runtime_error("Failed to load font (arial.ttf)");
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);

        if (!eatBuffer.loadFromFile("eat.wav"))
            throw std::runtime_error("Failed to load eat.wav");
        if (!gameOverBuffer.loadFromFile("gameover.wav"))
            throw std::runtime_error("Failed to load gameover.wav");

        eatSound.setBuffer(eatBuffer);
        gameOverSound.setBuffer(gameOverBuffer);
    }

    void run() {
        sf::Clock clock;
        float timer = 0;
        float delay = speed / 1000.0f;

        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;

            handleEvents();

            if (timer > delay) {
                timer = 0;
                update();
            }

            render();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir != DOWN)
            dir = UP;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir != UP)
            dir = DOWN;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir != RIGHT)
            dir = LEFT;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir != LEFT)
            dir = RIGHT;
    }

    void update() {
        Segment head = snake.front();
        switch (dir) {
            case UP:    head.y--; break;
            case DOWN:  head.y++; break;
            case LEFT:  head.x--; break;
            case RIGHT: head.x++; break;
        }

        if (head.x < 0 || head.y < 0 || head.x >= cols || head.y >= rows || isSelfCollision(head)) {
            gameOverSound.play();
            sf::sleep(sf::seconds(2));
            reset();
            return;
        }

        snake.insert(snake.begin(), head);

        if (head.x == food.x && head.y == food.y) {
            score++;
            eatSound.play();
            spawnFood();
            if (score % 5 == 0 && speed > 50)
                speed -= 10;
        } else {
            snake.pop_back();
        }
    }

    bool isSelfCollision(const Segment& head) {
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[i].x == head.x && snake[i].y == head.y)
                return true;
        }
        return false;
    }

    void spawnFood() {
        bool valid = false;
        while (!valid) {
            food.x = rand() % cols;
            food.y = rand() % rows;
            valid = true;
            for (const auto& seg : snake) {
                if (seg.x == food.x && seg.y == food.y) {
                    valid = false;
                    break;
                }
            }
        }
    }

    void render() {
        window.clear();

        for (const auto& seg : snake) {
            block.setPosition(seg.x * blockSize, seg.y * blockSize);
            window.draw(block);
        }

        block.setFillColor(sf::Color::Red);
        block.setPosition(food.x * blockSize, food.y * blockSize);
        window.draw(block);
        block.setFillColor(sf::Color::Green);

        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        window.display();
    }

    void reset() {
        snake.clear();
        snake.emplace_back(5, 5);
        dir = RIGHT;
        score = 0;
        speed = 150;
        spawnFood();
    }
};

int main() {
    try {
        SnakeGame game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
