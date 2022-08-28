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

func Start()
{
	float centerX = g_screenw / 2
	float centerY = g_screenh / 2
	
	
	global Vec2 v_aa = NVec2(g_screenw / 2 + -40, g_screenh / 2 + -40)
	global Sprite s_aa = ZS.Graphics.Sprite("./mariostill.png", v_aa, NVec2(16, 16), 0)
	
	global Vec2 v_ab = NVec2(g_screenw / 2 + -30, g_screenh / 2 + -40)
	global Sprite s_ab = ZS.Graphics.Sprite("./mariostill.png", v_ab, NVec2(16, 16), 0)
	
	global Vec2 v_ac = NVec2(g_screenw / 2 + -20, g_screenh / 2 + -40)
	global Sprite s_ac = ZS.Graphics.Sprite("./mariostill.png", v_ac, NVec2(16, 16), 0)
	
	global Vec2 v_ad = NVec2(g_screenw / 2 + -10, g_screenh / 2 + -40)
	global Sprite s_ad = ZS.Graphics.Sprite("./mariostill.png", v_ad, NVec2(16, 16), 0)
	
	global Vec2 v_ae = NVec2(g_screenw / 2 + -0, g_screenh / 2 + -40)
	global Sprite s_ae = ZS.Graphics.Sprite("./mariostill.png", v_ae, NVec2(16, 16), 0)
	
	global Vec2 v_af = NVec2(g_screenw / 2 + 10, g_screenh / 2 + -40)
	global Sprite s_af = ZS.Graphics.Sprite("./mariostill.png", v_af, NVec2(16, 16), 0)
	
	global Vec2 v_ag = NVec2(g_screenw / 2 + 20, g_screenh / 2 + -40)
	global Sprite s_ag = ZS.Graphics.Sprite("./mariostill.png", v_ag, NVec2(16, 16), 0)
	
	global Vec2 v_ah = NVec2(g_screenw / 2 + 30, g_screenh / 2 + -40)
	global Sprite s_ah = ZS.Graphics.Sprite("./mariostill.png", v_ah, NVec2(16, 16), 0)
	
	global Vec2 v_ai = NVec2(g_screenw / 2 + 40, g_screenh / 2 + -40)
	global Sprite s_ai = ZS.Graphics.Sprite("./mariostill.png", v_ai, NVec2(16, 16), 0)
	
	global Vec2 v_aj = NVec2(g_screenw / 2 + -40, g_screenh / 2 + -30)
	global Sprite s_aj = ZS.Graphics.Sprite("./mariostill.png", v_aj, NVec2(16, 16), 0)
	
	global Vec2 v_ak = NVec2(g_screenw / 2 + -30, g_screenh / 2 + -30)
	global Sprite s_ak = ZS.Graphics.Sprite("./mariostill.png", v_ak, NVec2(16, 16), 0)
	
	global Vec2 v_al = NVec2(g_screenw / 2 + -20, g_screenh / 2 + -30)
	global Sprite s_al = ZS.Graphics.Sprite("./mariostill.png", v_al, NVec2(16, 16), 0)
	
	
}

func Update(deltaTime)
{
	float fps = 1 / deltaTime
	Printl("FPS: " + fps)

	// // Shift key lets you sprint
	// g_running = GetKey("SHIFT_L")
	
	// if g_running == true
	// {
		// g_currPlayerSpeed = g_playerRunSpeed
	// }
	
	// if g_running == false
	// {
		// g_currPlayerSpeed = g_playerWalkSpeed
	// }

	// // Move Left And Right
	// if GetKey("D") == true
	// {
		// g_playerTargetPosition.x += g_currPlayerSpeed * deltaTime
	// }
	// if GetKey("A") == true
	// {
		// g_playerTargetPosition.x -= g_currPlayerSpeed * deltaTime
	// }
	
	
	// // Apply gravity
	// g_colliding = Colliding(g_playerSprite, g_groundSprite)
	// if g_colliding == false
	// {
		// if g_jumping == false
		// {
			// g_playerTargetPosition.y += deltaTime * g_gravitySpeed
		// }
		// if g_jumping == true
		// {
			// g_playerTargetPosition.y -= (g_jumpHeight * deltaTime) + (deltaTime * g_gravitySpeed * -g_jumpingTime * 5)
		// }
	// }
	// if g_colliding == true
	// {
		// if GetKey("SPACE") == false
		// {
			// if g_jumpingTime > 1
			// {
				// g_jumping = false
			// }
		// }
		// if GetKey("SPACE") == true
		// {
			// g_jumping = true
			// g_jumpingTime = 0
			// g_playerTargetPosition.y -= 2
		// }
	// }
	
	// // Lerps from old position to destination smoothly
	// float stopSpeed = deltaTime * 15
	// float lerpedX = Lerp(g_playerSprite.position.x, g_playerTargetPosition.x, stopSpeed)
	// s_aa.position = NVec2(lerpedX, g_playerTargetPosition.y)
	
	// Finally draws all of the sprites
	ZS.Graphics.Draw(s_aa)
	ZS.Graphics.Draw(s_ab)
	ZS.Graphics.Draw(s_ac)
	ZS.Graphics.Draw(s_ad)
	ZS.Graphics.Draw(s_ae)
	ZS.Graphics.Draw(s_af)
	ZS.Graphics.Draw(s_ag)
	ZS.Graphics.Draw(s_ah)
	ZS.Graphics.Draw(s_ai)
	ZS.Graphics.Draw(s_aj)
	ZS.Graphics.Draw(s_ak)
	ZS.Graphics.Draw(s_al)
}

func Colliding(a, b)
{
	bool bo = ZS.Physics.AxisAlignedCollision(a, b)
	return bo
}
