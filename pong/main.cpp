#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>
#include <iostream>
#include <utility>//Pair
#include <thread>

#include "ball.hpp"
#include "paddle.hpp"

/*
    I may be linking the wrong SFML, one sec.
    I made another project not that long ago, let me check where it is.
    It seems to be right... hmm.
    All the DLL's!
    I'll just take them all for simplicity sakes.
    Well, this stinks.

    For now, we will be using my previous project to load the files.
    For time saving purposes.

    Alright, so the idea is that we will need one or more balls, with two paddles.
    There will be four global bounds that will trigger upon ball intact (misspelled?)

    So we just built one ball.
    Making walls now to rebound the ball. We could just evaluate beyound the screen
    but we will be using the walls to make things easier for us.

    Will be using mt19337 twister random for c++

    Time to slow the game down

    Note: This was not rehearsed XD

    So we will make an invisible wall for left and right to evaluate out of bounds.
    This will reset the position of the ball, and evaluate points for our player.

    Time to add movement.
*/
int main() {

    // obtain a seed from the system clock:
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count());

    std::mt19937 rand(seed);

    sf::Clock clock;
    float delta = 0;
    std::pair<int, int> score{ 0,0 };

    int width = 800, height = 600;
    //This will give our application a window to work with.
    sf::RenderWindow window(sf::VideoMode(width, height), "Pong");

    //Declare our ball now.
    my::Ball ball{ 10.f,sf::Vector2f{100,100} };
    ball.shape.setPosition(width / 2.f, height / 2.f);
    ball.velocity.x = rand() % 2 ? ball.velocity.x : -ball.velocity.x;
    ball.velocity.y = rand() % 2 ? ball.velocity.y : -ball.velocity.y;

    //No need to draw these:
    sf::RectangleShape top_wall{ sf::Vector2f{static_cast<float>(width),height / 20.f } },
        bot_wall{ sf::Vector2f{static_cast<float>(width),height / 20.f } };

    sf::RectangleShape left_bound{ sf::Vector2f{width / 40.f,static_cast<float>(height) } },
        right_bound{ sf::Vector2f{width / 40.f,static_cast<float>(height) } };
    right_bound.setPosition(width - right_bound.getSize().x, 0);
    //top_wall.setPosition();//Correct spot
    bot_wall.setPosition(0, height - bot_wall.getSize().y);

    my::Paddle p1{ sf::Vector2f{width / 100.f, height / 10.f }, 500.f },
        p2{ sf::Vector2f{width / 100.f, height / 10.f }, 500.f };

    p1.shape.setPosition(p1.shape.getSize().x * 2, height / 2.f);
    p2.shape.setPosition(width - p2.shape.getSize().x * 4, height / 2.f);

    p1.key_map[sf::Keyboard::Up] = std::pair<my::Key, bool>{ my::Key::Up,false };
    //p1.key_map[sf::Keyboard::Down] = { my::Key::Down,false };//Being lazy now.
    //p1.key_map[sf::Keyboard::Left] = { my::Key::Left,false };
    p1.key_map[sf::Keyboard::Right] = { my::Key::Right,false };

    p1.event_map[my::Key::Up] = [&](float delta)->void {
        if (!p1.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()) ||
            !p1.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()) ||
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()));
        p1.shape.setPosition(p1.shape.getPosition().x , p1.shape.getPosition().y - delta * p1.velocity);
    };
    p1.event_map[my::Key::Down] = [&](float delta)->void {
        if (!p1.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()) ||
            !p1.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()) ||
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()));
        p1.shape.setPosition(p1.shape.getPosition().x, p1.shape.getPosition().y + delta * p1.velocity);
    };


    //This variable is the running condition variable.
    bool isRunning = true;

    //Timer for delta times and limits:
    std::pair<float, float> update(0.f, 1 / 120.f), 
        //We will be updating once every 1000/120 = ~8ish mil seconds
                            draw(0.f, 1 / 60.f);
        //We will be updating once every 1000/120 = ~8ish mil seconds

    //The game loop
    while (isRunning) {
        delta = clock.getElapsedTime().asSeconds();
        clock.restart();

        //A variable to store the events this cycle.
        sf::Event event;

        //Poll our events
        while (window.pollEvent(event)) {
            //The default close operation
            if (event.type == sf::Event::Closed)
                isRunning = false;
            for (const auto& map : p1.key_map) {
                if (sf::Keyboard::isKeyPressed(map.first))
                    p1.key_map[map.first].second = true;
                else
                    p1.key_map[map.first].second = false;

            }
        }

        update.first += delta;
        if (update.first > update.second) {
            if (ball.shape.getGlobalBounds().intersects(left_bound.getGlobalBounds())) {
                ball.shape.setPosition(width / 2.f, height / 2.f);
                ball.velocity.x = rand() % 2 ? ball.velocity.x : -ball.velocity.x;
                ball.velocity.y = rand() % 2 ? ball.velocity.y : -ball.velocity.y;
                ++score.first;
                std::cout << "Hit left\n";
            }
            else if (ball.shape.getGlobalBounds().intersects(right_bound.getGlobalBounds())) {
                ball.shape.setPosition(width / 2.f, height / 2.f);
                ball.velocity.x = rand() % 2 ? ball.velocity.x : -ball.velocity.x;
                ball.velocity.y = rand() % 2 ? ball.velocity.y : -ball.velocity.y;
                ++score.first;
                std::cout << "Hit right\n";
            }
            if (ball.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()))
                ball.velocity.y = -ball.velocity.y;
            else if (ball.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()))
                ball.velocity.y = -ball.velocity.y;
            ball.shape.setPosition(ball.shape.getPosition().x +
                                   ball.velocity.x * update.first, 
                                   ball.shape.getPosition().y +
                                   ball.velocity.y * update.first);
            std::cout << ball.velocity.y << '\n';

            for (const auto& key : p1.key_map)
                if (key.second.second)
                    p1.event_map[key.second.first](update.first);

            //Modulus doesn't work for floats
            while(update.first > update.second)
                update.first -= update.second;
        }

        //This should make the ball move out of the screen.
        //And it's gone.

        window.clear();
        window.draw(ball.shape);
        window.draw(top_wall);
        window.draw(bot_wall);
        window.draw(p1.shape);
        window.draw(p2.shape);

        //Place holders for debug
        //window.draw(left_bound);
        //window.draw(right_bound);
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 240));
        //std::this_thread::yield();
    }

    return 0;
}