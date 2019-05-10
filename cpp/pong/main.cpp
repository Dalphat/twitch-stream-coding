#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>
#include <iostream>
#include <utility>//Pair
#include <thread>
#include <cmath>//Float modulus
#include <array>

#include "ball.hpp"
#include "paddle.hpp"
/*
    Twitch stream coding.

    We created two walls, a ball, two boundaries for restart and 
    two paddles for each player.

    Doing further offscreen editing of code.

*/
int main() {

    const std::size_t win_value = 10;

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
    p1.key_map[sf::Keyboard::W] = std::pair<my::Key, bool>{ my::Key::Up,false };
    p1.key_map[sf::Keyboard::S] = { my::Key::Down,false };//Being lazy now.
    //p1.key_map[sf::Keyboard::Left] = { my::Key::Left,false };
    //p1.key_map[sf::Keyboard::Right] = { my::Key::Right,false };

    p2.key_map[sf::Keyboard::Up] = std::pair<my::Key, bool>{ my::Key::Up, false };
    p2.key_map[sf::Keyboard::Down] = { my::Key::Down, false };//Being lazy now.
    //p1.key_map[sf::Keyboard::Left] = { my::Key::Left,false };
    //p1.key_map[sf::Keyboard::Right] = { my::Key::Right,false };

    //Lambdas as a place holder to allow users to further change behavior of input

    //Map key identity to function
    p1.event_map[my::Key::Up] = [&](float delta)->void {
        if (!p1.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()) &&
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
            p1.shape.setPosition(p1.shape.getPosition().x, p1.shape.getPosition().y - delta * p1.velocity);
    };
    p1.event_map[my::Key::Down] = [&](float delta)->void {
        //If player does not intersects top wall, bot wall, or ball: move
        //false or false = true hmm wait... not true? false and false would be true.
        if (!p1.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()) &&
            !p1.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
            p1.shape.setPosition(p1.shape.getPosition().x, p1.shape.getPosition().y + delta * p1.velocity);
    };
    p2.event_map[my::Key::Up] = [&](float delta)->void {
        if (!p2.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds()) &&
            !p2.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
            p2.shape.setPosition(p2.shape.getPosition().x, p2.shape.getPosition().y - delta * p1.velocity);
    };
    p2.event_map[my::Key::Down] = [&](float delta)->void {
        //If player does not intersects top wall, bot wall, or ball: move
        //false or false = true hmm wait... not true? false and false would be true.
        if (!p2.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds()) &&
            !p2.shape.getGlobalBounds().intersects(ball.shape.getGlobalBounds()))
            p2.shape.setPosition(p2.shape.getPosition().x, p2.shape.getPosition().y + delta * p1.velocity);
    };

    //Container of players: Possibly ease future event and updates.
    std::array<my::Paddle*, 2> players{//Two players for now, vector if dynamic
        &p1,
        &p2
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
                            draw(0.f, 1 / 60.f),
                            print(0.f,1.f);
        //We will be updating once every 1000/60 = ~16ish mil seconds

    std::cout << "Scores: \t" << score.first << '\t' << score.second << '\n';

    //The game loop
    while (isRunning && score.first < win_value && score.second < win_value) {
        //Get our delta time
        delta = clock.getElapsedTime().asSeconds();//Get time since this call
        clock.restart();//Reset the clock 
        
        print.first += delta;
        if (print.first > print.second) {//Place holder for things per second.
            print.first = std::fmod(print.first, print.second);
        }

        //A variable to store the events this cycle.
        sf::Event event;

        //Poll our events
        while (window.pollEvent(event)) {//Only entered if windows polled something
            //The default close operation
            if (event.type == sf::Event::Closed)
                isRunning = false;
            //Iterate through keys and update state
            for(std::size_t i = 0; i < players.size(); ++i)
                for (const auto& map : players[i]->key_map) {
                    if (sf::Keyboard::isKeyPressed(map.first))
                        players[i]->key_map[map.first].second = true;
                    else
                        players[i]->key_map[map.first].second = false;

            }
        }

        update.first += delta;
        if (update.first > update.second) {//Process updates if ms reached
            //Moves the ball
            ball.shape.setPosition(ball.shape.getPosition().x +
                ball.velocity.x * update.first,
                ball.shape.getPosition().y +
                ball.velocity.y * update.first);
            //Check if a player has scored
            if (ball.shape.getGlobalBounds().intersects(left_bound.getGlobalBounds())) {
                ball.shape.setPosition(width / 2.f, height / 2.f);
                ball.velocity.x = rand() % 2 ? ball.velocity.x : -ball.velocity.x;
                ball.velocity.y = rand() % 2 ? ball.velocity.y : -ball.velocity.y;
                ++score.first;
                std::cout << "Scores: \t" << score.first << '\t' << score.second << '\n';
            }
            else if (ball.shape.getGlobalBounds().intersects(right_bound.getGlobalBounds())) {
                ball.shape.setPosition(width / 2.f, height / 2.f);
                ball.velocity.x = rand() % 2 ? ball.velocity.x : -ball.velocity.x;
                ball.velocity.y = rand() % 2 ? ball.velocity.y : -ball.velocity.y;
                ++score.second;
                std::cout << "Scores: \t" << score.first << '\t' << score.second << '\n';
            }
            //Check if ball has hit the top or bottom
            if (ball.shape.getGlobalBounds().intersects(top_wall.getGlobalBounds())) {
                ball.velocity.y = -ball.velocity.y;
                //Un-Moves the ball
                ball.shape.setPosition(ball.shape.getPosition().x +
                    ball.velocity.x * update.first,
                    ball.shape.getPosition().y +
                    ball.velocity.y * update.first);
            }
            else if (ball.shape.getGlobalBounds().intersects(bot_wall.getGlobalBounds())) {
                ball.velocity.y = -ball.velocity.y;
                //Un-Moves the ball
                ball.shape.setPosition(ball.shape.getPosition().x +
                    ball.velocity.x * update.first,
                    ball.shape.getPosition().y +
                    ball.velocity.y * update.first);
            }
            else if (ball.shape.getGlobalBounds().intersects(p1.shape.getGlobalBounds()) ||
                ball.shape.getGlobalBounds().intersects(p2.shape.getGlobalBounds())) {
                ball.velocity.x = -ball.velocity.x;
                //Un-Moves the ball
                ball.shape.setPosition(ball.shape.getPosition().x +
                    ball.velocity.x * update.first,
                    ball.shape.getPosition().y +
                    ball.velocity.y * update.first);
            }

            //Move the player
            for (std::size_t i = 0; i < players.size(); ++i)
                for (const auto& key : players[i]->key_map)
                    if (key.second.second)
                        players[i]->event_map[key.second.first](update.first);

            //Modulus doesn't work for floats
            update.first = std::fmod(update.first, update.second);//Credit ever everx80
        }

        draw.first += delta;
        if (draw.first > draw.second) {
            window.clear();
            for (const auto& shape_ptr : shapes)
                window.draw(*shape_ptr);

            //Place holders for debug (These are not drawn)
            //window.draw(left_bound);
            //window.draw(right_bound);

            window.display();
            draw.first = std::fmod(draw.first, draw.second);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 240));
    }
    window.close();
    if (score.first >= win_value)
        std::cout << "Left player wins\n";
    else if (score.second >= win_value)
        std::cout << "Right player wins\n";
    else
        std::cout << "No winners this game (GG)\n";
    std::cout << "\nApplication terminating in 5 seconds\n\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << ".\t";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << '\n';

    return 0;
}