#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>
#include <iostream>
#include <utility>//Pair
#include <thread>
#include <cmath>//Float modulus

#include "ball.hpp"
#include "paddle.hpp"
/*
    Twitch stream coding.

    We created two walls, a ball, two boundaries for restart and 
    two paddles for each player.

    Doing further offscreen editing of code.

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

    //Top wall and bottom wall: argument is their sizes
    sf::RectangleShape top_wall{ sf::Vector2f{static_cast<float>(width),height / 20.f } },
        bot_wall{ sf::Vector2f{static_cast<float>(width),height / 20.f } };

    //Set wall positions: (top is already at the correct position)
    bot_wall.setPosition(0, height - bot_wall.getSize().y);

    //Left boudary and right boundary: args are their sizes (they aren't drawn). 
    sf::RectangleShape left_bound{ sf::Vector2f{width / 80.f,static_cast<float>(height) } },
        right_bound{ sf::Vector2f{width / 80.f,static_cast<float>(height) } };

    //Set boundary positions: (Left is already at the correct location)
    right_bound.setPosition(width - right_bound.getSize().x, 0);

    //Create the two players: args are their sizes
    my::Paddle p1{ sf::Vector2f{width / 100.f, height / 10.f }, 500.f },
        p2{ sf::Vector2f{width / 100.f, height / 10.f }, 500.f };

    //Set player positions: 
    //  p1 being twice his position from the wall
    //  p2 being twich his position from the wall as well (origin is top left)
    p1.shape.setPosition(p1.shape.getSize().x * 2, height / 2.f);
    p2.shape.setPosition(width - p2.shape.getSize().x * 3, height / 2.f);

    //Map key input to key identity and state: Left and Right are unused
    p1.key_map[sf::Keyboard::Up] = std::pair<my::Key, bool>{ my::Key::Up,false };
    p1.key_map[sf::Keyboard::Down] = { my::Key::Down,false };//Being lazy now.
    //p1.key_map[sf::Keyboard::Left] = { my::Key::Left,false };
    //p1.key_map[sf::Keyboard::Right] = { my::Key::Right,false };

    //Map key identity to function
    p1.event_map[my::Key::Up] = [&](float delta)->void {
        if (!p1.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()) &&
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
        p1.shape.setPosition(p1.shape.getPosition().x , p1.shape.getPosition().y - delta * p1.velocity);
    };
    p1.event_map[my::Key::Down] = [&](float delta)->void {
        //If player does not intersects top wall, bot wall, or ball: move
        //false or false = true hmm wait... not true? false and false would be true.
        if (!p1.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()) && 
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
            p1.shape.setPosition(p1.shape.getPosition().x, p1.shape.getPosition().y + delta * p1.velocity);
    };

    //Container of the shapes: (manage our drawables at the cost of some pointers)
    std::vector<sf::Shape*> shapes;
    shapes.reserve(50);//Reserve more than enough memory our pointer to address
    shapes.push_back(&ball.shape);
    shapes.push_back(&top_wall);
    shapes.push_back(&bot_wall);
    shapes.push_back(&p1.shape);
    shapes.push_back(&p2.shape);

    //This variable is the running condition variable.
    bool isRunning = true;

    //Timer for delta times and limits:
    std::pair<float, float> update(0.f, 1 / 120.f), 
        //We will be updating once every 1000/120 = ~8ish mil seconds
                            draw(0.f, 1 / 60.f);
        //We will be updating once every 1000/60 = ~16ish mil seconds

    //The game loop
    while (isRunning) {
        //Get our delta time
        delta = clock.getElapsedTime().asSeconds();//Get time since this call
        clock.restart();//Reset the clock 

        //A variable to store the events this cycle.
        sf::Event event;

        //Poll our events
        while (window.pollEvent(event)) {//Only entered if windows polled something
            //The default close operation
            if (event.type == sf::Event::Closed)
                isRunning = false;
            //Iterate through keys and update state
            for (const auto& map : p1.key_map) {
                if (sf::Keyboard::isKeyPressed(map.first))
                    p1.key_map[map.first].second = true;
                else
                    p1.key_map[map.first].second = false;

            }
        }

        update.first += delta;
        if (update.first > update.second) {//Process updates if ms reached
            //Check if a player has scored
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
                ++score.second;
                std::cout << "Hit right\n";
            }
            //Check if ball has hit the top or bottom
            if (ball.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()))
                ball.velocity.y = -ball.velocity.y;
            else if (ball.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()))
                ball.velocity.y = -ball.velocity.y;
            else if (ball.shape.getGlobalBounds().intersects(p1.shape.getGlobalBounds()) || 
                     ball.shape.getGlobalBounds().intersects(p2.shape.getGlobalBounds()))
                ball.velocity.x = -ball.velocity.x;

            //Moves the ball
            ball.shape.setPosition(ball.shape.getPosition().x +
                                   ball.velocity.x * update.first, 
                                   ball.shape.getPosition().y +
                                   ball.velocity.y * update.first);
            //Move the player
            for (const auto& key : p1.key_map)
                if (key.second.second)
                    p1.event_map[key.second.first](update.first);

            //Modulus doesn't work for floats
            update.first = std::fmod(update.first, update.second);//Credit ever everx80
        }

        window.clear();
        for (const auto& shape_ptr : shapes)
            window.draw(*shape_ptr);

        //Place holders for debug (These are not drawn)
        //window.draw(left_bound);
        //window.draw(right_bound);

        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 240));
        //std::this_thread::yield();
    }

    return 0;
}