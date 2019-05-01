#ifndef PADDLE_HPP
#define PADDLE_HPP
#include <SFML/Graphics.hpp>//Pretty sure they've got header guards.
#include <unordered_map>
#include <functional>
#include <utility>
#include "my.hpp"
namespace my {
    //Hmm, Paddle will have ... a shape, input handler, event handler? (maybe).
    class Paddle {
    public:
        sf::RectangleShape shape;
        float velocity;


        //Is it better to just have three maps or one key to two values? plus another
        //  for the key binding.
        //Brute force for now

        std::unordered_map < sf::Keyboard::Key, std::pair<my::Key, bool>> key_map;//Binding keys
        std::unordered_map<my::Key, std::function<void(float)>> event_map;//Binding events
        Paddle(const sf::Vector2f& size, float velocity = 1) : shape{ size }, velocity { velocity }{}
    };
}
#endif // !PADDLE_HPP
