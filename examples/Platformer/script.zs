int g_screenw = 256
int g_screenh = 224
int g_resolutionScale = 3

float g_playerWalkSpeed = 400
float g_playerRunSpeed = 700
float g_jumpHeight = 20
float g_currPlayerSpeed = 400

bool g_running = false

float g_gravitySpeed = -86

include "./extra-include.zs"

func Main()
{
	//SplitThread(ThreadedFunction())
	
	// Immediately creates the window, then Start(), then the game loop. The game loop calls Update() every frame
	ZS.Graphics.Init("Platformer game", g_screenw, g_screenh, g_resolutionScale)
}

func ThreadedFunction()
{
	print "threaded:"
}

func Start()
{
	float centerX = g_screenw / 2
	float centerY = g_screenh / 2
	global Vec2 g_screencenter = NVec2(centerX, centerY)
	
	Vec2 playerPos = g_screencenter
	Vec2 playerScale = NVec2(16, 16)
	global Sprite g_playerSprite = ZS.Graphics.Sprite("./mariostill.png", playerPos, playerScale, 0)
	
	Vec2 groundPos = NVec2(g_screencenter.x, 192)
	Vec2 groundScale = NVec2(256, 16)
	global Sprite g_groundSprite = ZS.Graphics.Sprite("./square.png", groundPos, groundScale, 0)
	
	Vec2 instructionsPos = NVec2(centerOfScreen.x, centerOfScreen.y)
	global Text g_instructionsText = ZS.Graphics.Text("Use Arrow Keys or WASD to Move, and Spacebar to Jump", "./arial.ttf", instructionsPos, 20, 0, 255, 255, 255)
	
	global Vec2 g_playerTargetPosition = playerPos
	
	int i = 0
	while i < 10 {
		i += 1
		print "while iter : " + i
	}
}

func Update(deltaTime) {
	float fps = 1 / deltaTime
	print "FPS: " + fps
	//TestInclude()
	
	//// Test automatic conversion from bool to int
	//int c = GetKey("A")
	//print "Test: " + c

	// Shift key lets you sprint
	g_running = GetKey("SHIFT_L")
	
	if g_running == true{
		g_currPlayerSpeed = g_playerRunSpeed
	}
	if g_running == false
	{
		g_currPlayerSpeed = g_playerWalkSpeed
	}

	// Move Left And Right
	if GetKey("A") == true
	{
		float newY = g_playerSprite.position.y
		
		float newX = g_playerTargetPosition.x - g_currPlayerSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, newY)
	}
	if GetKey("D") == true
	{
		float newY = g_playerSprite.position.y
		
		float newX = g_playerTargetPosition.x + g_currPlayerSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, newY)
	}
	// Lerps from old position to destination smoothly
	float oldX = g_playerSprite.position.x
	float newX = g_playerTargetPosition.x
	float stopSpeed = deltaTime * 7
	float lerpedX = Lerp(oldX, newX, stopSpeed)
	g_playerSprite.position = NVec2(lerpedX, newY)
	
	
	// Finally draws all of the sprites
	ZS.Graphics.Draw(g_playerSprite)
	ZS.Graphics.Draw(g_groundSprite)
	
	ZS.Graphics.DrawText(g_instructionsText)
}

func Colliding(a, b)
{
	bool b = ZS.Physics.AxisAlignedCollision(a, b)
	return b
}
