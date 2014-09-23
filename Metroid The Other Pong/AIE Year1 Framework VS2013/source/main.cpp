#include "AIE.h"
#include <iostream>
// background stuff
const int BackgroundWidth = 800;
const int BackgroundHeight = 600;

enum GAMESTATES
{
	eMENU,
	eGAMEPLAY,
	eWIN, 
	eLOSE
};

enum DIR
{
	eTOP,
	eBOTTOM,
	eRANDOM
};
struct collision
{
	float Xmax;
	float Ymax;
	float Xmin;
	float Ymin;
};

struct Ball
{
	unsigned int spriteID;
	float ballWidth;
	float ballHeight;
	float xSpeed = 200;
	float ySpeed = 200;
	float Radius;
	collision ballBox;
	float x;
	float y;
	int SamusBallX = BackgroundWidth / 2;
	int SamusBallY = BackgroundHeight / 2;

	void SetPosition(float a_x, float a_y)
	{
		x = a_x;
		y = a_y;
	}

	void SetSpeed(float x_BallSpeed, float y_BallSpeed)
	{	
		x += x_BallSpeed;
		x += y_BallSpeed;
		MoveSprite(spriteID, x, y);
		DrawSprite(spriteID);
	}
	void move(float aDelta)
	{
		x += xSpeed * aDelta;
		y += ySpeed * aDelta;
	}
	int GetLeftSideX()
	{
		return x - BackgroundWidth / 2;
	}
	int GetRightSideX()
	{
		return x + BackgroundWidth / 2;
	}
	void setBox()
	{
		ballBox.Xmax = x + (ballWidth * .5);
		ballBox.Ymax = y + (ballHeight * .5);
		ballBox.Xmin = x - (ballWidth * .5);
		ballBox.Ymin = y - (ballHeight * .5);
	}
};

struct MetroidBar
{
	unsigned int iSpriteID;
	float fHeight = BackgroundHeight;
	float fWidth = BackgroundWidth;
	collision barBox;
	float x;
	float y;
	float speed = 500.f;
	void SetSize(float a_fWidth, float a_fHeight)
	{
		fWidth = a_fWidth;
		fHeight = a_fHeight;
	}
	unsigned int iMoveUpKey;
	unsigned int iMoveDownKey;
	void SetMovementKeys(unsigned int a_moveLeft, unsigned int a_moveRight)
	{
		iMoveUpKey = a_moveLeft;
		iMoveDownKey = a_moveRight;
	}
	unsigned int iUpMovementExtreme;
	unsigned int iDownMovementExtreme;
	void SetMovementExtremes(unsigned int a_leftExtreme, unsigned int a_RightExtreme)
	{
		iUpMovementExtreme = a_leftExtreme;
		iDownMovementExtreme = a_RightExtreme;
	}
	void Move(float a_fTimeStep)
	{
		{
			if (IsKeyDown(iMoveUpKey))
			{
				y += a_fTimeStep * speed;
				if (y > (iUpMovementExtreme))
				{
					y = (iUpMovementExtreme);
				}
			}
			if (IsKeyDown(iMoveDownKey))
			{
				y -= a_fTimeStep * speed;
				if (y < (iDownMovementExtreme))
				{
					y = (iDownMovementExtreme);
				}
			}
			MoveSprite(iSpriteID, x, y);
		}
	}
	void setBox()
	{
		barBox.Xmax = x + (fWidth * .5);
		barBox.Ymax = y + (fHeight * .5);
		barBox.Xmin = x - (fWidth * .5);
		barBox.Ymin = y - (fHeight * .5);
	}
};

bool CollisionCheckBox(collision box1, collision box2)
{
	if (box1.Xmin < box2.Xmax && box1.Ymin < box2.Ymax && box1.Xmin > box2.Xmin && box1.Ymin > box2.Ymin)
	{
		return true;
	}
	if (box1.Xmax < box2.Xmax && box1.Ymax < box2.Ymax && box1.Xmax > box2.Xmin && box1.Ymax > box2.Ymin)
	{
		return true;
	}
	if (box1.Xmin < box2.Xmax && box1.Ymax < box2.Ymax && box1.Xmin > box2.Xmin && box1.Ymax > box2.Ymin)
	{
		return true;
	}
	if (box1.Xmax < box2.Xmax && box1.Ymax < box2.Ymin && box1.Xmax > box2.Xmin && box1.Ymin > box2.Ymin)
	{
		return true;
	}
	return false;
}

	// dimensions of ball and bars
	int SamusAirBallX = 320;
	int SamusAirBallY = 240;

	Ball ball;
	MetroidBar bar1, bar2;

	// other stuff
	int LeftScreenEnd;
	int RightScreenEnd;
	int Score = 0;
	int Score2 = 0;
	float fDelta;

	// chars
	const char* pressButton = "Press Space to Play";
	const char* winGame = "You Won!";
	const char* loseGame = "You Suck!";
	char ScoreText[3];
	char ScoreText2[3];

	int main(int argc, char* argv[])
	{
		Initialise(BackgroundWidth, BackgroundHeight, false, "Metroid: The Other Pong");

		SetBackgroundColour(SColour(0, 0, 0, 0));

		//create the actual images
		int TheBall = CreateSprite("./images/ball.png", 50, 50, true);
		int TheBar = CreateSprite("./images/bar.png", 40, 200, true);
		int TheBar2 = CreateSprite("./images/bar2.png", 40, 200, true);
		int TheLogo = CreateSprite("./images/Metroid-Other-M-Logo.png", 500, 200, true);
		int p1WinGame = CreateSprite("./images/SamusWin.jpg", 500, 235, true);
		int p1LoseGame = CreateSprite("./images/SamusLose.jpg", 500, 700, true);

		// move p1 bar, right side
		bar1.fHeight = 32.f;
		bar1.fWidth = 64.f;
		bar1.iSpriteID = CreateSprite("./images/bar.png", bar1.fWidth, bar1.fHeight, true);
		bar1.x = BackgroundWidth * 0.5f;
		bar1.y = 80.f;
		bar1.SetSize(64.f, 32.f);
		bar1.setBox();
		bar1.SetMovementKeys('O', 'L');
		bar1.SetMovementExtremes(600, 0);

		// move p2 bar
		bar2.fHeight = 32.f;
		bar2.fWidth = 64.f;
		bar2.iSpriteID = CreateSprite("./images/bar.png", bar2.fWidth, bar2.fHeight, true);
		bar2.x = BackgroundWidth * 0.5f;
		bar2.y = 80.f;
		bar2.SetSize(64.f, 32.f);
		bar2.setBox();
		bar2.SetMovementKeys('W', 'S');
		bar2.SetMovementExtremes(600, 0);


		bar1.x = BackgroundWidth * 0.85f;
		bar1.y = BackgroundHeight * 0.85f;
		MoveSprite(TheBar, bar1.x, bar1.y);
		bar2.x = BackgroundWidth * 0.15f;
		bar2.y = BackgroundHeight * 0.15f;
		MoveSprite(TheBar2, bar2.x, bar2.y);
		if (bar1.y = BackgroundHeight)
		{
			bar1.y = BackgroundHeight / 2;
		}

		ball.x = BackgroundWidth / 2;
		ball.y = BackgroundHeight / 2;
		ball.ballWidth = 50;
		ball.ballHeight = 50;
		ball.setBox();

		GAMESTATES eCurrentState = eMENU;
		//Game Loop
		do
		{
			fDelta = GetDeltaTime();

			ClearScreen();

			switch (eCurrentState)
			case eMENU:
			{
				void UpdateMainMenu();
				{
					MoveSprite(TheLogo, 400, 390);
					DrawSprite(TheLogo);
					DrawString(pressButton, BackgroundWidth / 2.9, BackgroundHeight / 4);
				}
				if (IsKeyDown(' '))
				{
					eCurrentState = eGAMEPLAY;
				}
				break;
			case eGAMEPLAY:
			{
							  bar1.Move(fDelta);
							  bar2.Move(fDelta);
							  ball.move(fDelta);
							  bar1.setBox();
							  bar2.setBox();
							  ball.setBox();

							  if (CollisionCheckBox(ball.ballBox, bar1.barBox) && ball.xSpeed > 0)
							  {
								  ball.xSpeed *= -1;
							  }
							  if (CollisionCheckBox(ball.ballBox, bar2.barBox) && ball.xSpeed < 0)
							  {
								  ball.xSpeed *= -1;
							  }
							  // if ball hits the top of the screen
							  if (ball.y > BackgroundHeight)
							  {
								  ball.ySpeed *= -1;
							  }
							  if (ball.y < 0)
							  {
								  ball.ySpeed = -ball.ySpeed;
							  }
							  //if ball hits width sent back to starting point
							  if (ball.x > BackgroundWidth)
							  {
								  ball.x = BackgroundWidth / 2;
								  ball.y = BackgroundHeight / 2;
								  ball.xSpeed = ball.ySpeed;
								  Score++;
								  itoa(Score, ScoreText, 5);
							  }
							  if (ball.x < 0)
							  {
								  ball.x = BackgroundWidth / 2;
								  ball.y = BackgroundHeight / 2;
								  ball.xSpeed = ball.ySpeed;
								  Score2++;
								  itoa(Score2, ScoreText2, 5);
							  }
							  // switch between Win state and Lose State.

							  // draw the pictures and stuff
							  MoveSprite(TheBar, bar1.x, bar1.y);
							  DrawSprite(TheBar);
							  MoveSprite(TheBar2, bar2.x, bar2.y);
							  DrawSprite(TheBar2);
							  MoveSprite(ball.spriteID, ball.x, ball.y);
							  DrawSprite(TheBall);
							  DrawString(ScoreText, BackgroundWidth * .025f, BackgroundHeight - 2);
							  DrawString(ScoreText2, BackgroundWidth * .95f, BackgroundHeight - 2);
							  if (Score == 5)
							  {
								  eCurrentState = eWIN;
							  }
							  if (Score2 == 5)
							  {
								  eCurrentState = eLOSE;
							  }

							  break;
			}
			case eWIN:
			{
						 DrawString(winGame, BackgroundWidth / 2, BackgroundHeight / 2);
						 MoveSprite(p1WinGame, 400, 235);
						 DrawSprite(p1WinGame);

						 if (IsKeyDown('T'))
						 {
							 eCurrentState = eMENU;
						 }
						 break;
			}
			case eLOSE:
			{
						  DrawString(loseGame, BackgroundWidth / 2, BackgroundHeight / 2);
						  MoveSprite(p1LoseGame, BackgroundWidth / 2, BackgroundHeight / 2);
						  DrawSprite(p1LoseGame);
						  if (IsKeyDown('T'))
						  {
							  eCurrentState = eMENU;
						  }
			}
				break;
			}
		
		} while (!FrameworkUpdate());

		Shutdown();

		return 0;
	}

	