<img src="https://raw.githubusercontent.com/sam-astro/Z-Sharp/master/ExtraResources/ZS-Gem-Icon-Small.png"/><img src="https://raw.githubusercontent.com/sam-astro/Z-Sharp/master/ExtraResources/ZS-Logo-Light-Small.png"/>

## Introduction
Z-Sharp is a custom programming language I made because I don't like c++ very much (Z-Sharp's interpreter is written in c++ though). It used to be called Slang, standing for "Stupid Lang", but another programming language called Slang already exists :(. Z-Sharp scripts have the file extension .zs. The base syntax and formatting I would say is quite similar to C#, but differs as task complexity increases. It has support for graphics using SDL2, and by default is not enabled.

## Installation
Downloading or installing is very simple, here is how depending on your version and operating system:
### Windows
1. Navigate to [the most recent release](https://github.com/sam-astro/Z-Sharp/releases) and download `ZSharp-Win-Installer.zip`.
2. Unzip `ZSharp-Win-Installer.zip` and open the unzipped folder.
3. Inside is a single file titled `ZSharp-Setup.exe`. Run it, and follow the setup instructions.
4. Now that it is installed, there are a few ways to use it:
    * (recommended) Any ZSharp file that ends with .ZS will automatically be associated with the interpreter. Just double-click it, and the interpreter will run.
    * Drag and drop any .ZS script directly onto the executable.
    * Use command line, providing path to interpreter and then to script like so:
    `> ./ZSharp.exe ./Pong-Example-Project/script.zs`
5. Feel free to use and edit the `Pong-Example-Project`. It is a single script called `script.zs`, and you can open it with any of the methods above. It is also located on the releases page.
> If you don't want to install ZSharp on your device, or you want easier acces to the executable and .DLLs, another version is provided called `ZS_Win_Base_Raw.zip`. This just contains all of the files the installer puts on your computer.
### Linux
1. Navigate to [the most recent release](https://github.com/sam-astro/Z-Sharp/releases) and download `ZSharp-Linux.zip`.
2. Unzip `ZSharp-Linux.zip` and open the unzipped folder.
3. You will see some files. The Z# interpreter is `ZSharp`. Any time you want to execute a script, this is the program that will be used. You can use it like so:
    * Use terminal, providing path to executable and then to script like so:
    `$ ./ZSharp ./Pong-Example-Project/script.zs`
4. Feel free to use and edit the included `Pong-Example-Project`. It is a single script called `script.zs`, and you can open it with any of the methods above.


## Here is some example code:
```c++
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
```c++
func Main()
{
    int screenWidth = 500
    int screenHeight = 500
    ZS.Graphics.Init("Title of window", screenWidth, screenHeight)
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
    global Sprite exampleSprite = ZS.Graphics.Sprite("./square.png", position, scale, rotation)
}

// Executes each frame
func Update(deltaTime)
{
    // Draws the image created in Start(). This is usually at the end of update.
    ZS.Graphics.Draw(exampleSprite)   
}
```
Currently, ZSharp is ***VERY*** strict with formatting, and can throw an error if you forget to put a space somewhere.
Also, speaking of errors, if your code has any it will show in the console. Errors are colored red, and warnings are colored yellow. A line number will also usually be provided. This is ***Not*** the line relative to the *documents* beginning, but rather the *functions* beginning.
Example:
```
ERROR: line 5 in function Main
```
This is the 5th line *inside of Main*.
```c++
func Main()
{
   // line 1
   // line 2
   // line 3
   // line 4
   int g = "s"
   // ^ above line is the error, since it is line 5
}
```
I am planning to change how error reporting works to report the document line number as well, but this is how it is for now.
