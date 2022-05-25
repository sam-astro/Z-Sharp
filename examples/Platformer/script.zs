int g_screenw = 256
int g_screenh = 224
int g_resolutionScale = 3

float g_playerWalkSpeed = 400
float g_playerRunSpeed = 700
float g_jumpHeight = 20
float g_currPlayerSpeed = 400

bool g_running = false
bool g_colliding = false

float g_gravitySpeed = -86

include "./extra-include.zs"

func Main()
{
	//SplitThread(ThreadedFunction())
	
	// Immediately creates the window, then runs Start(), then the game loop. The game loop calls Update() every frame
	ZS.Graphics.Init("Platformer game", g_screenw, g_screenh, g_resolutionScale)
}

func ThreadedFunction()
{
	print "threaded:"
}

func Start()
{
	global Vec2 g_screencenter = NVec2(g_screenw / 2, g_screenh / 2)
	
	global Sprite g_playerSprite = ZS.Graphics.Sprite("./mariostill.png", g_screencenter, NVec2(16, 16), 0)
	
	global Sprite g_groundSprite = ZS.Graphics.Sprite("./square.png", NVec2(g_screencenter.x, 192), NVec2(256, 16), 0)
	
	global Text g_instructionsText = ZS.Graphics.Text("Use Arrow Keys or WASD to Move and Spacebar to Jump", "./arial.ttf", NVec2(centerOfScreen.x, centerOfScreen.y), 11, 0, 255, 255, 255)
	
	global Vec2 g_playerTargetPosition = playerPos
}

func Update(deltaTime) {
	float fps = 1 / deltaTime
	print "FPS: " + fps + "\r"
	//TestInclude()
	
	//// Test automatic conversion from bool to int
	//int c = GetKey("A")
	//print "Test: " + c

	// Shift key lets you sprint
	g_running = GetKey("SHIFT_L")
	
	if g_running == true {
		g_currPlayerSpeed = g_playerRunSpeed
	}
	if g_running == false {
		g_currPlayerSpeed = g_playerWalkSpeed
	}

	// Move Left And Right
	if GetKey("A") == true
	{
		float newX = g_playerTargetPosition.x - g_currPlayerSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, g_playerSprite.position.y)
	}
	if GetKey("D") == true
	{		
		float newX = g_playerTargetPosition.x + g_currPlayerSpeed * deltaTime
		g_playerTargetPosition = NVec2(newX, g_playerSprite.position.y)
	}
	
	// Apply gravity
	g_colliding = Colliding(g_playerSprite, g_groundSprite)
	if g_colliding == false {
		g_playerTargetPosition.y -= deltaTime * g_gravitySpeed
	}
	
	// Lerps from old position to destination smoothly
	float stopSpeed = deltaTime * 7
	float lerpedX = Lerp(g_playerSprite.position.x, g_playerTargetPosition.x, stopSpeed)
	g_playerSprite.position = NVec2(lerpedX, g_playerTargetPosition.y)
	
	// Finally draws all of the sprites
	ZS.Graphics.Draw(g_playerSprite)
	ZS.Graphics.Draw(g_groundSprite)
	
	// Draw the text
	ZS.Graphics.DrawText(g_instructionsText)
}

func Colliding(a, b)
{
	bool b = ZS.Physics.AxisAlignedCollision(a, b)
	return b
}
