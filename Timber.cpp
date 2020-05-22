// Timber.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
//Make code easier to type with "using namespace"
using namespace sf;

//Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Where is the player/btanch?
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

//Setup the bee1
int SpriteBee1Time = 0;
int SpriteBee2Time = 0;
// 			
int main()
{
	//Create a video mode object
	VideoMode vm(1920, 1080);
	//Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);
	//Create a texture to hold a graphic on the GPU
	Texture textureBackground;
	//Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");
	//Create a sprite
	Sprite spriteBackground;
	//Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);
	//Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);
	//Make a flower sprite
	Texture textureFlower;
	textureFlower.loadFromFile("graphics/flower.png");
	Sprite spriteFlower;
	spriteFlower.setTexture(textureFlower);
	spriteFlower.setPosition(425, 650);

	// Prepare the bee
	Texture textureBee1;
	textureBee1.loadFromFile("graphics/bee.png");
	Texture textureBee2;
	textureBee2.loadFromFile("graphics/bee2.png");
	Sprite spriteBee1;
	Sprite spriteBee2;
	spriteBee1.setTexture(textureBee1);
	spriteBee1.setPosition(0, 800);
	spriteBee2.setTexture(textureBee2);
	spriteBee2.setPosition(0, 600);
	// Is the bee currently moving?
	bool beeActive1 = false;
	bool beeActive2 = false;
	// How fast can the bee fly
	float beeSpeed1 = 0.0f;
	float beeSpeed2 = 0.0f;


	// make 3 cloud sprites from 1 texture
	Texture textureCloud;
	// Load 1 new texture
	textureCloud.loadFromFile("graphics/cloud.png");
	// 3 New sprites with the same texture
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	// Position the clouds on the left of the screen
	// at different heights
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	// Are the clouds currently on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	// How fast is each cloud?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	//Variable to control time itself
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	//Track whether the game is running
	bool paused = true;

	// Draw some text
	int score = 0;
	Text messageText;
	Text scoreText;
	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);
	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	// Make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	// Choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	//Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	//Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		//Set the sprite's origin to dead centre
		//We can spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	// The player starts on the left
	side playerSide = side::LEFT;
	// Prepare the gravestone
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

	// Control the player input
	bool acceptInput = false;

	// Prepare the sounds
	// The player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);
	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen()) {
		//Handle the player's input

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				// Listen for key presses again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		//Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
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
			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);
			// Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		// Wrap the player controls to
		// Make sure we are accepting input
		if (acceptInput)
		{
			// More code here next...
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);
				// Update the branches
				updateBranches(score);
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;

				// Play a chop sound
				chop.play();
			}
			// Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Make sure the player is on the left
				playerSide = side::LEFT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
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

		//Update the scene
		if (!paused)
		{
			//Measure time
			Time dt = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));
			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;
				// Change the message shown to the player
				messageText.setString("Out of time!!");
				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the out of time sound
				outOfTime.play();
			}

			//Setup the bee1
// 			int SpriteBee1Time = 0;
// 			int SpriteBee2Time = 0;

			if (!beeActive1)
			{
				//How fast is the bee
				srand((int)time(0));
				beeSpeed1 = (rand() % 200) + 200;
				//How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee1.setPosition(2000, height);
				beeActive1 = true;
				SpriteBee1Time = 0;
			}
			else
			{
				float y = spriteBee1.getPosition().y;
				float x = spriteBee1.getPosition().x;
				float x_new = spriteBee1.getPosition().x - (beeSpeed1 * dt.asSeconds());
				spriteBee1.setPosition(x_new, y);

				if (SpriteBee1Time < 1000)
				{
					if (x < 725 && x>425)
					{
						if (y < 950 && y>350)  // 改成渐变飞过去
						{
							if (y > 650)
								y = spriteBee1.getPosition().y - (beeSpeed1 * dt.asSeconds());
							else
							{
								if (y < 650)
									y = spriteBee1.getPosition().y + (beeSpeed1 * dt.asSeconds());
								else
									y = spriteBee1.getPosition().y;
							}
							spriteBee1.setPosition(x_new, y);
						}
					}
					else if (x <= 425 && y < 950 && y>350)  // 【修订】添加判定条件：y < 950 && y>350
					{
						SpriteBee1Time++;
						spriteBee1.setPosition(x, y);
					}
				}

				//Move the bee
				//spriteBee1.setPosition(spriteBee1.getPosition().x - (beeSpeed1 * dt.asSeconds()), spriteBee1.getPosition().y);
				//Had the bee reached the left-hand edge of the screen?
				if (spriteBee1.getPosition().x < -100)
				{
					//Set it up ready to be a whole new bee next frame
					beeActive1 = false;
				}
			}

			//Setup the bee2
			if (!beeActive2) {
				//How fast is the bee
				srand((int)time(0));
				beeSpeed2 = (rand() % 200) + 200;
				//How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee2.setPosition(0, height);
				beeActive2 = true;
				SpriteBee2Time = 0;
			}
			//
			else
			{
				float y2 = spriteBee2.getPosition().y;
				float x2 = spriteBee2.getPosition().x;
				float x2_new = spriteBee2.getPosition().x + (beeSpeed2 * dt.asSeconds());
				spriteBee2.setPosition(x2_new, spriteBee2.getPosition().y);
				
				if (SpriteBee2Time < 1000)
				{
					if (x2 <= 425 && x2>=125)
					{
						if (y2 <= 950 && y2>=350)  // 改成渐变飞过去
						{
							if (spriteBee2.getPosition().y > 650)
								y2 = spriteBee2.getPosition().y - (beeSpeed2 * dt.asSeconds());
							else if (spriteBee2.getPosition().y < 650)
								y2 = spriteBee2.getPosition().y + (beeSpeed2 * dt.asSeconds());
							else
								y2 = spriteBee2.getPosition().y;

							spriteBee2.setPosition(x2_new, y2);
						}
					}
					else if(x2>425 && y2 <= 950 && y2 >= 350)  // 【修订】添加判定条件：y2 <= 950 && y2=>350
					{
						SpriteBee2Time++;
						spriteBee2.setPosition(x2, y2);
					}
				}

				//Move the bee
				//spriteBee1.setPosition(spriteBee1.getPosition().x - (beeSpeed1 * dt.asSeconds()), spriteBee1.getPosition().y);
				//Had the bee reached the left-hand edge of the screen?
				if (spriteBee2.getPosition().x > 2000)
				{
					//Set it up ready to be a whole new bee next frame
					beeActive2 = false;
				}
			}

			// Manage the clouds
			// Cloud 1
			if (!cloud1Active) {
				// How fast is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x +
					(cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y);
				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud1.getPosition().x > 1920)
				{
					// Set it up ready to be a whole new cloud next frame
					cloud1Active = false;
				}
			}

			// Cloud 2
			if (!cloud2Active) {
				// How fast is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x +
					(cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y);
				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud2.getPosition().x > 1920)
				{
					// Set it up ready to be a whole new cloud next frame
					cloud2Active = false;
				}
			}
			if (!cloud3Active) {
				// How fast is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x +
					(cloud3Speed * dt.asSeconds()),
					spriteCloud3.getPosition().y);
				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud3.getPosition().x > 1920)
				{
					// Set it up ready to be a whole new cloud next frame
					cloud3Active = false;
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					//Move the sprite to the left side
					branches[i].setPosition(610, height);
					//Flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else {
					//Hide the branch
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
					(logSpeedY * dt.asSeconds()));
				// Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000)
				{
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPositions[5] == playerSide)
			{
				// death
				paused = true;
				acceptInput = false;
				// Draw the gravestone
				spriteRIP.setPosition(525, 760);
				// hide the player
				spritePlayer.setPosition(2000, 660);
				// Change the text of the message
				messageText.setString("SQUISHED!!");
				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				// Play the death sound
				death.play();
			}
		}
		//Draw the scene

		//Clear everything from the last frame
		window.clear();

		//Draw our game scene here
		window.draw(spriteBackground);
		// Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		//Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}
		// Draw the tree
		window.draw(spriteTree);
		// Draw the flower
		window.draw(spriteFlower);
		// Draw the player
		window.draw(spritePlayer);
		// Draw the axe
		window.draw(spriteAxe);
		// Draw the flying log
		window.draw(spriteLog);
		// Draw the gravestone
		window.draw(spriteRIP);
		// Draw the insect
		window.draw(spriteBee1);
		window.draw(spriteBee2);

		// Draw the score
		window.draw(scoreText);
		// Draw the timebar
		window.draw(timeBar);
		if (paused)
		{
			// Draw our message
			window.draw(messageText);
		}

		//Show everything wo just drew
		window.display();
	}//while(window.isOpen())

	return 0;
}

void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
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

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
