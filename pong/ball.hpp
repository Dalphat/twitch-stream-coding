#ifndef BALL_HPP
#define BALL_HPP
#include <SFML/Graphics.hpp>
namespace my {
    //Ball will have a sprite and velocity vector(2) and other stuff.
    //Our sprite will be a circle shape.
    class Ball {
    public:
        sf::CircleShape shape;
        sf::Vector2f velocity;      //Kinematic with respects to x and y degrees.
        //Constructor with initializer list
        Ball(float radius, const sf::Vector2f& velocity) : shape{ radius }, velocity{ velocity }{}
    };
}
#endif // !BALL_HPP
