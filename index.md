Note to sam-astro when merging (if you decide to do so): this will need to be compiled using a rust package named mdbook into html. In the meantime, doc pages can be written in just markdown (it doesn't make a difference) and can be compiled for each release of Z#.
# Z-Sharp (Z#)

Z# is an interpreted programming language written in C++ and inspired by the popular language, C#.
Z# mainly removes things from C#, but there are a few additions, mainly the new way to make methods.

Here's a snippet of Z# code:
```c++
func Main() // the main method is the start of the program.
{
    print "Print text to the console with this!"
    int i = 0 // semicolons are not required.
    while i < 11 // brackets for statements have been removed
    {
        i -= 1
        print "Hello " + i + " times!"
    }
    // foreach and for loops do not exist in Z#, but can be easily emulated using a while loop.
}
```
# Running your code

### Windows 

1. Navigate to [the most recent release](https://github.com/sam-astro/Z-Sharp/releases) and download `ZSharp-Win-Installer.zip`. 

2. Unzip `ZSharp-Win-Installer.zip` and open the unzipped folder. 

3. Inside is a single file titled `ZSharp-Setup.exe`. Run it, and follow the setup instructions. 

4. Now that it is installed, there are a few ways to use it: * (recommended) Any ZSharp file that ends with .ZS will automatically be associated with the interpreter. Just double-click it, and the interpreter will run. * Drag and drop any .ZS script directly onto the executable. * Use command line, providing path to interpreter and then to script like so: `> ./ZSharp.exe ./Pong-Example-Project/script.zs` 

5. Feel free to use and edit the `Pong-Example-Project`. It is a single script called `script.zs`, and you can open it with any of the methods above. It is also located on the releases page. > If you don't want to install ZSharp on your device, or you want easier acces to the executable and .DLLs, another version is provided called `ZS_Win_Base_Raw.zip`. This just contains all of the files the installer puts on your computer. 

### Linux 

1. Install requirements: * You need the package ***libsdl2-dev***. For Debian based operating systems you can install it with ```$ sudo apt install libsdl2-dev``` * You also need the package ***libsdl2-image-dev***. For Debian based operating systems you can install it with ```$ sudo apt install libsdl2-image-dev``` * You also need the package ***libsdl2-ttf-dev***. For Debian based operating systems you can install it with ```$ sudo apt install libsdl2-ttf-dev``` 

2. Navigate to [the most recent release](https://github.com/sam-astro/Z-Sharp/releases) and download `ZSharp-Linux.zip`. 

3. Unzip `ZSharp-Linux.zip` and open the unzipped folder. 


4. You will see some files. The Z# interpreter is `ZSharp`. Any time you want to execute a script, this is the program that will be used. You can use it like so: * Use terminal, providing path to executable and then to script like so: `$ ./ZSharp ./Pong-Example-Project/script.zs` 

5. Feel free to use and edit the included `Pong-Example-Project`. It is a single script called `script.zs`, and you can open it with any of the methods above. 

# The Docs

This section will probably be removed.

The docs will use a package called mdbook to turn markdown into HTML (it can be formated as a documentation).

Each major release of Z#, the md files in the docs/ directory will be compiled into html format.
