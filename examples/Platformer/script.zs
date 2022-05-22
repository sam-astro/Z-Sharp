int g_screenw = 900
int g_screenh = 600

float g_playerMoveSpeed = 400
float g_jumpHeight = 20

bool g_running = false

func Main()
{
	// Immediately creates the window, then Start(), then the game loop. The game loop calls Update() every frame
	ZS.Graphics.Init("Platformer game", g_screenw, g_screenh)
}

func Start()
{
	float centerX = g_screenw / 2
	float centerY = g_screenh / 2
	global Vec2 g_screencenter = NVec2(centerX, centerY)
	
	Vec2 playerPos = NVec2(0, 0)
	Vec2 playerScale = NVec2(16, 16)
	global Sprite g_playerSprite = ZS.Graphics.Sprite("./square.png", playerPos, playerScale, 0)
	
	Vec2 groundPos = NVec2(0, -17)
	Vec2 groundScale = NVec2(256, 16)
	global Sprite g_groundSprite = ZS.Graphics.Sprite("./square.png", groundPos, groundScale, 0)
	
	Vec2 instructionsPos = NVec2(centerOfScreen.x, g_screenh - 60)
	global Text g_instructionsText = ZS.Graphics.Text("Use Arrow Keys or WASD to Move, and Spacebar to Jump", "./arial.ttf", instructionsPos, 20, 0, 255, 255, 255)
	
	global Vec2 g_playerTargetPosition = playerPos
}

func Update(deltaTime)
{
	float fps = 1 / deltaTime
	print "FPS: " + fps
	
	// Test automatic conversion from bool to int
	int c = 0 + GetKey("A")
	print "Test: " + 0 + c

	// Move Left And Right
	//
	if GetKey("A") == true
	{
		float newY = g_playerSprite.position.y
		
		float newX = g_playerTargetPosition.x - g_playerMoveSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, newY)
	}
	if GetKey("D") == true
	{
		float newY = g_playerSprite.position.y
		
		float newX = g_playerTargetPosition.x + g_playerMoveSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, newY)
	}
	// Lerps from old position to destination smoothly
	float oldX = g_playerSprite.position.x
	float newX = g_playerTargetPosition.x
	float stopSpeed = deltaTime * lerpSpeed
	float lerpedX = Lerp(oldX, newX, stopSpeed)
	g_playerSprite.position = NVec2(lerpedX, newY)
	
	
	// Finally draws all of the sprites
	ZS.Graphics.Draw(playerSprite)
	ZS.Graphics.Draw(groundSprite)
	
	ZS.Graphics.DrawText(instructionsText)
	
	HandleBallBounce()
}

func Colliding(a, b)
{
	bool b = ZS.Physics.AxisAlignedCollision(a, b)
	return b
}
