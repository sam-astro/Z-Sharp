int g_screenw = 256
int g_screenh = 224
int g_resolutionScale = 3

float g_playerWalkSpeed = 150
float g_playerRunSpeed = 210
float g_jumpHeight = 200
float g_currPlayerSpeed = 400

bool g_running = false
bool g_colliding = false

float g_gravitySpeed = 86

bool g_jumping = false
float g_jumpingTime = 0

//include "./extra-include.zs"

func Main()
{
	//SplitThread(ThreadedFunction())
	EXIT_WHEN_DONE = false
	
	// Immediately creates the window, then runs Start(), then the game loop. The game loop calls Update() every frame
	ZS.Graphics.Init("Platformer game", g_screenw, g_screenh, g_resolutionScale)
}

func ThreadedFunction()
{
	Printl("threaded:")
}

func Start()
{
	float centerX = g_screenw / 2
	float centerY = g_screenh / 2
	global Vec2 g_screencenter = NVec2(g_screenw / 2, g_screenh / 2)
	
	global Sprite g_playerSprite = ZS.Graphics.Sprite("./mariostill.png", g_screencenter, NVec2(16, 16), 0)
	
	global Sprite g_groundSprite = ZS.Graphics.Sprite("./square.png", NVec2(g_screencenter.x, 192), NVec2(256, 16), 0)
	
	global Text g_instructionsText = ZS.Graphics.Text("Use Arrow Keys or WASD to Move and Spacebar to Jump", "./arial.ttf", NVec2(g_screencenter.x, g_screencenter.y), 8, 0, 255, 255, 255)
	g_instructionsText.antialias = false
	
	global Vec2 g_playerTargetPosition = g_screencenter
}

func Update(deltaTime)
{
	float fps = 1 / deltaTime
	g_jumpingTime += deltaTime
	Printl("FPS: " + fps)
	//TestInclude()

	// Shift key lets you sprint
	g_running = GetKey("SHIFT_L")
	
	if g_running == true
	{
		g_currPlayerSpeed = g_playerRunSpeed
	}
	
	if g_running == false
	{
		g_currPlayerSpeed = g_playerWalkSpeed
	}

	// Move Left And Right
	if GetKey("D") == true
	{
		g_playerTargetPosition.x += g_currPlayerSpeed * deltaTime
	}
	if GetKey("A") == true
	{
		g_playerTargetPosition.x -= g_currPlayerSpeed * deltaTime
	}
	
	
	// Apply gravity
	g_colliding = Colliding(g_playerSprite, g_groundSprite)
	if g_colliding == false
	{
		if g_jumping == false
		{
			g_playerTargetPosition.y += deltaTime * g_gravitySpeed
		}
		if g_jumping == true
		{
			g_playerTargetPosition.y -= (g_jumpHeight * deltaTime) + (deltaTime * g_gravitySpeed * -g_jumpingTime * 5)
		}
	}
	if g_colliding == true
	{
		if GetKey("SPACE") == false
		{
			if g_jumpingTime > 1
			{
				g_jumping = false
			}
		}
		if GetKey("SPACE") == true
		{
			g_jumping = true
			g_jumpingTime = 0
			g_playerTargetPosition.y -= 2
		}
	}
	
	// Lerps from old position to destination smoothly
	float stopSpeed = deltaTime * 15
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
	bool bo = ZS.Physics.AxisAlignedCollision(a, b)
	return bo
}
