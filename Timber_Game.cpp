// Timber_Game.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <sstream>
#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include "Timber_Game.h"
using namespace sf;

// function declaration 
void updateBranches(int speed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main()
{
    // Create a video mode object
    VideoMode vm(1920, 1080);

    // Create and open a window for the game
    RenderWindow window(vm, "TIMBER!!!", Style::Fullscreen);

    // Create a texture to hold a graphic on the GPU
    Texture textureBackground;

    // Load a graphic into the texture
    textureBackground.loadFromFile("graphics/background.png");
    
    // Create a sprite
    Sprite spriteBackground;

    // Attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);

    // Set the spriteBackground to cover the screen
    spriteBackground.setPosition(0, 0);

    // Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    // Prepare the tree
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 800);

    // Is the bee currently moving?
    bool beeActive = false;

    // How fast can fly bee?
    float speedBee = 0.0f;

    // Make 3 cloud sprites
    Texture textureClouds;

    // Load 1 new texture
    textureClouds.loadFromFile("graphics/cloud.png");

    // 3 new sprites with the same texture
    Sprite spriteCloud1;
    Sprite spriteCloud2;
    Sprite spriteCloud3;
    spriteCloud1.setTexture(textureClouds);
    spriteCloud2.setTexture(textureClouds);
    spriteCloud3.setTexture(textureClouds);

    // position the clouds on the left side of the screen
    // at different heights
    spriteCloud1.setPosition(0, 0);
    spriteCloud2.setPosition(0, 150);
    spriteCloud3.setPosition(0, 300);

    // are the clouds currently on the screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;

    // how fast is each other clouds?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    // variables to control time itself
    Clock clock;

    // track wehter the game is running
    bool paused = true;

    // Draw some text
    int score = 0;

    // Text messageText
    Text messageText;
    Text scoreText;

    // We need to chose a font
    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    // Set font to our message
    messageText.setFont(font);
    scoreText.setFont(font);

    // assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    // make it really big
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // chose color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // position the text 
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(
        textRect.left +
        textRect.width / 2.0f,
        textRect.top +
        textRect.height / 2.0f);

    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
    scoreText.setPosition(20, 20);

    // Time bar
    RectangleShape timebar;
    float timeBarStartWidth = 400.0f;
    const float timeBarHeight = 80.0f;
    timebar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timebar.setFillColor(Color::Red);
    timebar.setPosition((1920 / 2.0f) - timeBarStartWidth / 2.0f, 980);
    
    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Prepare 6 branches 
    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");

    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);

        // Set the sprites origin to dead centre
        // We can then spin it round without changing it's position

        branches[i].setOrigin(220, 20);
    }

    updateBranches(1);
    updateBranches(2);
    updateBranches(3);
    updateBranches(4);
    updateBranches(5);

    // Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);

    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600, 860);

    // Prepare the axe
    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700, 830);

    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // Prepare the flying log
    Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);

    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // control the player input
    bool acceptInput = false;

    // prepare the sound
    // the player chopping sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    // the player has met his end under a branch
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    // out of time
    SoundBuffer outBuffer;
    outBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(outBuffer);

    while (window.isOpen())
    {
        /*
        ***************************************************
        Handle the players input
        ***************************************************
        */

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == event.KeyPressed && !paused)
            {
                // Listen for key presses again
                acceptInput = true;

                // hide the axe 
                spriteAxe.setPosition(2000,
                    spriteAxe.getPosition().y
                    );
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        // Start the game
        if (Keyboard::isKeyPressed(Keyboard::Return))
        {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 6;

            // Make all the branches disappear - 
            // starting in the second position
            for (int i = 1; i < NUM_BRANCHES; i++)
            {
                branchPositions[i] = side::NONE;
            }

            // Make sure the gravestorne is hidden
            spriteRIP.setPosition(675, 2000);

            // Move the player into position 
            spritePlayer.setPosition(580, 720);

            acceptInput = true;

        }

        // Wrap the player controls to
        // Make sure we are acepting input
        if (acceptInput)
        {
            // more code here...

            // first handle pressing the right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                // make sure player is on the right
                playerSide = side::RIGHT;
                score++;

                // add to the amount of time remaining 
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(
                    AXE_POSITION_RIGHT,
                    spriteAxe.getPosition().y
                );

                spritePlayer.setPosition(1200, 720);

                // update the branches
                updateBranches(score);

                // set the log flying to the left
                spriteLog.setPosition(810, 720);
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                // Play a chop sound
                chop.play();
            }

            // handle the left cursor key

            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                // Make sure the player is on the left 
                playerSide = side::LEFT;
                score++;

                // Add to the amount of time remaining 
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(
                    AXE_POSITION_LEFT,
                    spriteAxe.getPosition().y
                );

                spritePlayer.setPosition(580, 720);

                // update the branches
                updateBranches(score);

                // set the log flying
                spriteLog.setPosition(810, 720);
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                // Play a chop sound
                chop.play();
            }
        }

        /*
        ***************************************************
        Update the scene
        ***************************************************
        */

        if (!paused)
        {
            // Measure time
            Time dt = clock.restart();

            // Subtract from the amount of time remaining 
            timeRemaining -= dt.asSeconds();

            // resize up time bar 
            timebar.setSize(Vector2f(timeBarWidthPerSecond *
                timeRemaining, timeBarHeight));

            if (timeRemaining <= 0)
            {
                // pause the game
                paused = true;

                // change the message show to the player
                messageText.setString("Out of time!");

                // reposition the text based on it's new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(
                    textRect.left +
                    textRect.width / 2.0f,
                    textRect.top +
                    textRect.height / 2.0f
                );

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                // Play the out of time sound
                outOfTime.play();

            }

            // Setup the bee 
            if (!beeActive)
            {
                // how fast is the bee
                srand((int)time(0));
                speedBee = (rand() % 200) + 200;
                // how hight is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;
            }
            else
                // Move the bee
            {
                spriteBee.setPosition(
                    spriteBee.getPosition().x -
                    (speedBee * dt.asSeconds()),
                    spriteBee.getPosition().y);

                // Has the bee reached the left-hand edge of the screen
                if (spriteBee.getPosition().x < -100)
                {
                    // Set it up ready to be a whole new bee next frame
                    beeActive = false;
                }
            }

            // Manage the clouds 
            if (!cloud1Active)
            {
                // How fast is the cloud
                srand((int)time(0) * 10);
                cloud1Speed = (rand() % 200);
                // How hight is the cloud 
                srand((int)time(0) * 10);
                float height = (rand() % 150);
                spriteCloud1.setPosition(-200, height);
                cloud1Active = true;
            }
            // Move the cloud
            else
            {
                spriteCloud1.setPosition(
                    spriteCloud1.getPosition().x +
                    (cloud1Speed * dt.asSeconds()),
                    spriteCloud1.getPosition().y
                );
                if (spriteCloud1.getPosition().x > 1920)
                {
                    cloud1Active = false;
                }
            }

            // Manage is the cloud 2
            if (!cloud2Active)
            {
                srand((int)time(0) * 20);
                cloud2Speed = (rand() % 200);
                srand((int)time(0) * 20);
                float height = (rand() % 300) - 150;
                spriteCloud2.setPosition(-200, height);
                cloud2Active = true;
            }
            // Move the cloud 2
            else
            {
                spriteCloud2.setPosition(
                    spriteCloud2.getPosition().x +
                    (cloud2Speed * dt.asSeconds()),
                    spriteCloud2.getPosition().y
                );
                if (spriteCloud2.getPosition().x > 1920)
                {
                    cloud2Active = false;
                }
            }

            if (!cloud3Active)
            {
                srand((int)time(0) * 30);
                cloud3Speed = (rand() % 300);
                srand((int)time(0) * 30);
                float height = (rand() % 450) - 150;
                spriteCloud3.setPosition(-200, height);
                cloud3Active = true;
            }
            else
            {
                spriteCloud3.setPosition(
                    spriteCloud3.getPosition().x +
                    (cloud3Speed * dt.asSeconds()),
                    spriteCloud3.getPosition().y
                );

                if (spriteCloud3.getPosition().x > 1920)
                {
                    cloud3Active = false;
                }
            }

            // update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites 
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;
                if (branchPositions[i] == side::LEFT)
                {
                    // Move the sprite to the left side
                    branches[i].setPosition(610, height);

                    // Flip the sprite round the other way
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    // move the sprite to the right side
                    branches[i].setPosition(1330, height);

                    // Flip the sprite round the other way
                    branches[i].setRotation(0);
                }
                else
                {
                    // hide the branch
                    branches[i].setPosition(3000, height);
                }
            }

            // Handle a flying log
            if (logActive)
            {
                spriteLog.setPosition(
                    spriteLog.getPosition().x +
                    (logSpeedX * dt.asSeconds()),

                    spriteLog.getPosition().y +
                    (logSpeedY * dt.asSeconds())
                );

                // Has the log reached the right hand edge?
                if (spriteLog.getPosition().x < -100 ||
                    spriteLog.getPosition().y > 2000)
                {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }
            }

            // Has the player been squished by branch?
            if (branchPositions[5] == playerSide)
            {
                // death
                paused = true;
                acceptInput = false;

                // draw the gravestone
                spriteRIP.setPosition(525, 760);

                // hide the player
                spritePlayer.setPosition(2000, 660);

                // change the text of the message
                messageText.setString("SQUISHED!!");

                // center it on the screen
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(
                    textRect.left +
                    textRect.width / 2.0f,
                    textRect.top +
                    textRect.height / 2.0f
                );

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                // Player the death sound
                death.play();
            }

        } // End if(!paused)

        /*
        ***************************************************
        Draw the scene
        ***************************************************
        */

        // Clear everything 
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);
        
        // Draw the clouds
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        // Draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            window.draw(branches[i]);
        }

        // Draw the tree
        window.draw(spriteTree);

        // Draw the player
        window.draw(spritePlayer);

        // Draw the axe
        window.draw(spriteAxe);

        // Draw the flying log
        window.draw(spriteLog);

        // Draw the grawstone
        window.draw(spriteRIP);

        // Draw the bee
        window.draw(spriteBee);

        // Draw the time bar
        window.draw(timebar);

        // Draw the score 
        window.draw(scoreText);
        if (paused)
        {
            // Draw our message
            window.draw(messageText);
        }

        // Show everything
        window.display();

    }
    return 0;
}

void updateBranches(int seed)
{
    // move all the branches down one place
    for (int j = NUM_BRANCHES - 1; j > 0; j--)
    {
        branchPositions[j] = branchPositions[j - 1];
    }

    // Spawn a new branches at position 0
    // LEFT, RIGHT, NONE
    srand((int)time(0) + seed);
    int r = (rand() % 5);

    switch (r)
    {
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
        
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}