# Slang, or StuLang
Slang, or StuLang, which stands for ***Stu****pid  **Lang**uage* is a custom programming language I made because I don't like c++ very much (Slang's interpreter is written in c++ though). Slang scripts have the file extension .slg. The base syntax and formatting I would say is quite similar to C#, but differs as task complexity increases. It has support for graphics using SDL2, and by default is not enabled. Here is some example code.
```c#
// Comments are indicated by two forward slashes
// They can only be on their own line
//    int j = 4 // <- This is invalid comment placement

// All programs start with a main function
func Main()
{
    int i = 0
    string s = "r"
    
    i += 2
    i -= 1
    i /= 3
    i *= 2
    
    while i < 10
    {
        i += 1
    }
    
    if s == "r"
    {
        print s + " is r"
    }
    
    int functionNumber = ExampleFunction("A", s)
    ExampleFunction(1, 3)
    
    GlobalFunction()
}

// Declare new function with 'func', then it's name, and the names of any input variables.
// The input variables don't need type, as those are automatic. Also, they don't need to
/// be assigned at all on execute and can be left blank
func ExampleFunction(inputA, inputB)
{
    print "In A is: " + inputA
    print "In B is: " + inputB
    
    // Return a value to the valling location
    return 4
}

func GlobalFunction()
{
    // Create variables that can be accessed from anywhere (ex. in Main or ExampleFunction) with the 'global' keyword before type
    global int x = 12
    global string y = "Y String"
}
```
Here is how to use graphics:
```c#
func Main()
{
    int screenWidth = 500
    int screenHeight = 500
    CPP.Graphics.Init("Title of window", screenWidth, screenHeight)
    // After graphics are initialized, the main function will not finish.
    // Instead, Start() will be called a single time, then Update() every frame after that.
}

// Runs once at start of graphics initialization
func Start()
{
    // Vec2 are initialized using function 'NVec2(x, y)'
    Vec2 position = NVec2(250, 250)
    Vec2 scale = NVec2(20, 20)
    float rotation = 0

    // Sprite object, stores (and loads from file) the texture, location, scale, and rotation
    global Sprite exampleSprite = CPP.Graphics.Sprite("./square.png", position, scale, rotation)
}

// Executes each frame
func Update()
{
    // Draws the image created in Start(). This is usually at the end of update.
    CPP.Graphics.Draw(exampleSprite)   
}
```
