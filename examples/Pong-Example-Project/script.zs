int SCREENW = 900
int SCREENH = 600

int scoreOne = 0
int scoreTwo = 0

float ballSpeed = 400

float paddleMoveSpeed = 600

float lerpSpeed = 14

bool aiOn = false

// Main is always run at the VERY BEGINNING. Start() is the start of GRAPHICS
// so if you never call ZS.Graphics.Init, then Start won't run
func Main()
{
	EXIT_WHEN_DONE = false
	
	// Immediately creates the window, then Start(), then the game loop. The game loop calls Update() every frame
	ZS.Graphics.Init("This is a pong game", SCREENW, SCREENH)
}

func Start()
{
	float centerX = SCREENW / 2
	float centerY = SCREENH / 2
	global Vec2 centerOfScreen = NVec2(centerX, centerY)
	
	Vec2 ballScale = NVec2(10, 10)
	Vec2 ballPos = centerOfScreen
	
	Vec2 paddleScale = NVec2(4, 70)
	float yPosPaddle = centerOfScreen.y
	
	Vec2 lPaddlePosition = NVec2(15, yPosPaddle)
	global Vec2 lPaddleTargetPosition = NVec2(15, yPosPaddle)
	
	float rOffset = SCREENW - 15
	Vec2 rPaddlePosition = NVec2(rOffset, yPosPaddle)
	global Vec2 rPaddleTargetPosition = NVec2(rOffset, yPosPaddle)
	
	global Sprite ballSpr = ZS.Graphics.Sprite("./ball.png", ballPos, ballScale, 0)
	global Sprite lPaddle = ZS.Graphics.Sprite("./square.png", lPaddlePosition, paddleScale, 0)
	global Sprite rPaddle = ZS.Graphics.Sprite("./square.png", rPaddlePosition, paddleScale, 0)
	
	Vec2 netScale = NVec2(1, SCREENH)
	global Sprite net = ZS.Graphics.Sprite("./net.png", centerOfScreen, netScale, 0)
	
	float leftOffset = SCREENW / 4
	Vec2 scoreOnePos = NVec2(leftOffset, 30)
	global Text scoreTextOne = ZS.Graphics.Text("0", "./arial.ttf", scoreOnePos, 60, 0, 255, 255, 255)
	float rightOffset = SCREENW - (SCREENW / 4)
	Vec2 scoreTwoPos = NVec2(rightOffset, 30)
	global Text scoreTextTwo = ZS.Graphics.Text("0", "./arial.ttf", scoreTwoPos, 60, 0, 255, 255, 255)
	
	Vec2 instructionsPos = NVec2(centerOfScreen.x, SCREENH - 60)
	global Text instructionsText = ZS.Graphics.Text("Use 'W' and 'S' or 'UP' and 'DOWN' arrows to control or press 'ENTER' to toggle AI", "./arial.ttf", instructionsPos, 20, 0, 255, 255, 255)
	
	global Vec2 ballVelocity = NVec2(ballSpeed, ballSpeed)
}

func Update(deltaTime)
{
	//float FPS = 1 / deltaTime
	//Printl("FPS: " + FPS)

	// Handles Left Paddle Movement
	//
	if GetKey("W") == true
	{
		// Subtract from Y to move up, because vertical coordinates are reversed
		float newY = lPaddleTargetPosition.y - paddleMoveSpeed * deltaTime
		newY = Clamp(newY, 0 + lPaddle.scale.y / 2, SCREENH - lPaddle.scale.y / 2)
		lPaddleTargetPosition.y = newY
	}
	if GetKey("S") == true
	{
		// Add to Y to move down, because vertical coordinates are reversed
		float newY = lPaddleTargetPosition.y + paddleMoveSpeed * deltaTime
		newY = Clamp(newY, 0 + lPaddle.scale.y / 2, SCREENH - lPaddle.scale.y / 2)
		lPaddleTargetPosition.y = newY
	}
	// Lerps from old position to destination smoothly
	float oldY = lPaddle.position.y
	float stopSpeed = deltaTime * lerpSpeed
	float newY = lPaddleTargetPosition.y
	float lerpedY = Lerp(oldY, newY, stopSpeed)
	lPaddle.position = NVec2(lPaddle.position.x, lerpedY)
	
	// Handles Right Paddle Movement
	//
	if aiOn == false
	{
		if GetKey("UP") == true
		{
			// Subtract from Y to move up, because vertical coordinates are reversed
			float newY = rPaddleTargetPosition.y - paddleMoveSpeed * deltaTime
			newY = Clamp(newY, 0 + rPaddle.scale.y / 2, SCREENH - rPaddle.scale.y / 2)
			rPaddleTargetPosition.y = newY
		}
		if GetKey("DOWN") == true
		{
			// Add to Y to move down, because vertical coordinates are reversed
			float newY = rPaddleTargetPosition.y + paddleMoveSpeed * deltaTime
			newY = Clamp(newY, 0 + rPaddle.scale.y / 2, SCREENH - rPaddle.scale.y / 2)
			rPaddleTargetPosition.y = newY
		}
	}
	if aiOn == true
	{
		if rPaddle.position.y < ballSpr.position.y
		{
			// Add to Y to move down, because vertical coordinates are reversed
			float newY = rPaddleTargetPosition.y + paddleMoveSpeed * deltaTime
			newY = Clamp(newY, 0 + rPaddle.scale.y / 2, SCREENH - rPaddle.scale.y / 2)
			rPaddleTargetPosition.y = newY
		}
		if rPaddle.position.y > ballSpr.position.y
		{
			// Subtract from Y to move up, because vertical coordinates are reversed
			float newY = rPaddleTargetPosition.y - paddleMoveSpeed * deltaTime
			newY = Clamp(newY, 0 + rPaddle.scale.y / 2, SCREENH - rPaddle.scale.y / 2)
			rPaddleTargetPosition.y = newY
		}
	}
	// Lerps from old position to destination smoothly
	float oldY = rPaddle.position.y
	float stopSpeed = deltaTime * lerpSpeed
	float newY = rPaddleTargetPosition.y
	float lerpedY = Lerp(oldY, newY, stopSpeed)
	rPaddle.position = NVec2(rPaddle.position.x, lerpedY)
	
	if GetKey("ENTER") == true
	{
		bool changeTo = false
		if aiOn == true
		{
			changeTo = false
		}
		if aiOn == false
		{
			changeTo = true
		}
		aiOn = changeTo
	}
	
	Vec2 scaledVelocity = ballVelocity
	scaledVelocity *= deltaTime
	ballSpr.position += scaledVelocity
	
	// Finally draws all of the sprites
	ZS.Graphics.Draw(ballSpr)
	ZS.Graphics.Draw(lPaddle)
	ZS.Graphics.Draw(rPaddle)
	
	ZS.Graphics.Draw(net)
	
	ZS.Graphics.DrawText(scoreTextOne)
	ZS.Graphics.DrawText(scoreTextTwo)
	
	ZS.Graphics.DrawText(instructionsText)
	
	HandleBallBounce()
}

func HandleBallBounce()
{
	float ballX = ballSpr.position.x
	float ballY = ballSpr.position.y
	float scaleY = ballSpr.scale.y
	
	float topEdge = ballY - scaleY/2
	// Checks if the ball is touching the ceiling
	if topEdge <= 0
	{
		float vX = ballVelocity.x
		float vY = ballVelocity.y
		vY *= -1
		ballVelocity = NVec2(vX, vY)
		return 0
	}
	float bottomEdge = ballY + scaleY/2
	// Checks if the ball is touching the floor
	if bottomEdge >= SCREENH
	{
		float vX = ballVelocity.x
		float vY = ballVelocity.y
		vY *= -1
		ballVelocity = NVec2(vX, vY)
		return 0
	}
	
	// Checks if ball is in player 1 goal
	if ballX < 0
	{
		Vec2 ballPos = centerOfScreen
		ballPos -= ballSpr.scale
		ballPos.x -= SCREENW / 3
		scoreTwo += 1
		ballSpr.position = ballPos
		ballVelocity = NVec2(ballSpeed, 0)
		scoreTextTwo.content = Round(scoreTwo)
	}
	// Checks if ball is in player 2 goal
	if ballX > SCREENW
	{
		Vec2 ballPos = centerOfScreen
		ballPos -= ballSpr.scale
		ballPos.x += SCREENW / 3
		scoreOne += 1
		ballSpr.position = ballPos
		ballVelocity = NVec2(-ballSpeed, 0)
		scoreTextOne.content = Round(scoreOne)
	}
	
	// Checks if colliding with left paddle
	bool coll = Colliding(ballSpr, lPaddle)
	if coll == true
	{
		float difference = lPaddle.position.y
		difference -= ballY
		float paddleHeight = lPaddle.scale.y
		float normalizedRelativeIntersectionY = difference / (paddleHeight / 2)
		float bounceAngle = normalizedRelativeIntersectionY * 0.523599
		float ballVx = ballSpeed
		ballVx *= Cos(bounceAngle)
		float ballVy = ballSpeed
		ballVy *= Sin(bounceAngle)
		ballVy *= -1
		ballVelocity = NVec2(ballVx, ballVy)
	}
	// Checks if colliding with right paddle
	bool coll = Colliding(ballSpr, rPaddle)
	if coll == true
	{
		float difference = rPaddle.position.y
		difference -= ballY
		float paddleHeight = rPaddle.scale.y
		float normalizedRelativeIntersectionY = difference / (paddleHeight / 2)
		float bounceAngle = normalizedRelativeIntersectionY * -0.523599
		float ballVx = ballSpeed
		ballVx *= Cos(bounceAngle)
		ballVx *= -1
		float ballVy = ballSpeed
		ballVy *= Sin(bounceAngle)
		ballVelocity = NVec2(ballVx, ballVy)
	}
}

func Colliding(a, b)
{
	bool b = ZS.Physics.AxisAlignedCollision(a, b)
	return b
}
