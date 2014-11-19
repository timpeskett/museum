museum
======

I managed to recover a lot of the programs that I wrote during my high school days. Putting the more interesting of these into one repo shows a nice progression of my programming ability, I suppose. 


When these programs were written I had no formal training in programming. It's interesting to note that the biggest thing that has changed over time is simply naming conventions and program organisation. Most of the code is fairly readable and well indented. Unfortunately I didn't see much value in comments at the time


All of this code was written for Windows XP. Most of it still runs on newer versions of Windows I believe (all screengrabs were taken on Windows 7). All graphical examples use GDI and win32 API. I never got around to learning MFC or anything like that.

The following programs are ordered by last modification date of the final executable. A lot of the flicker in the gifs is due to a low framerate so as to keep the filesizes down. Where the actual program has lag or flickers a lot it will be noted. 

I have not spent any time making any of this code easy to compile as I don't expect anyone to want to actually compile any of it. It should all actually compile, though. I originally used Code::blocks or Dev-C++ with MinGW as my IDE.


#DeskDraw
####A program to draw on the desktop
####October, 2009

This program was (most likely) inspired by the new smartboards that our school installed. It also seems like it could be an interesting idea but I don't really remember ever finding this useful. I suppose it could be used where there is a projector without a touch screen but I should think there are probably better open source alternatives now.

![DeskDraw Demonstration](demos/deskdraw.gif)

Holding down the middle mouse button is used to draw.

I think basically how this is achieved is just by getting the device context of the dispay and then drawing on top of it.

There's not really a great deal to say about the functionality, it does what it says. I never got around to implementing erasing so the lines only go away when the window they are 'above' is updated. The lines also don't stay there when windows are moved around. I don't think it ever got finished because I realised I would never use it.


#Fake BSOD
####A fake blue screen of death as a prank
####October, 2009

I wrote this intended as a prank. I suppose in theory this is technically a trojan but it doesn't really do anything too nefarious. 
The program creates a fullscreen window modelled to look like a Windows blue screen of death. To exit the window (without using the task manager) a three-letter passcode can be set. Holding all three letters of this passcode will close the window. The default is T, I, M (creative, right?).

![Blue screen of death screenshot](demos/bsod.png)

Funnily enough, a minute or two after taking this screenshot Windows gave me an actual blue screen of death. Maybe it works a little too well...


#BitmapInvert
####Applies some basic filters to bitmaps
#### November, 2009

This program just inverts/changes some channels of a bitmap image. From the looks of things the program works with both 8-bit and 24-bit bitmaps, which is suprising. I was fairly interested in image processing at the time and I think I also built a program to take a webcam feed and apply a (very basic) line filter to it. I haven't been able to find that one though.

The program is run from the command line and the desired filter is supplied there. Only one filter can be supplied per execution of the program but as it reads and writes complete bitmaps there is nothing stopping using multiple filters by running the program several times.

According to the inbuilt documentation the filters available are:

	* i - Inverse Filter -- All pixels values take 255 - value.
	* l - Left Shift -- All pixel values left shifted by 1.
	* r - Right Shift -- All pixel values right shifted by 1.
	* h - Halfway Filter -- Don't even ask 
	* c - cycles RGB values for pixels -- Red value becomes blue, blue value becomes green, green becomes red.
	* g - Greyscale -- Turns the image greyscale
	* R/G/B - All Red/Green/Blue -- Eliminates two of the channels

These are some examples of applying filters to an image:

![Original Image](BitmapInvert/TinyTim.bmp) ![Filtered1](BitmapInvert/TinyTim2.bmp) ![Filtered2](BitmapInvert/TinyTim3.bmp) ![Filtered3](BitmapInvert/TinyTim4.bmp)

The first here is the original. The second looks inverted. The third looks like it has been made all blue. The fourth looks like it has been cycled left or right.

The most interesting thing here is just the manual reading/writing of bitmap files. Very little of this code is Windows dependent.


#Padman
#### A Pacman clone for the ClassPad calculator
#### January, 2010

The ClassPad calculator was a pretty nifty CAS graphics calculator with a large screen and a touch screen. I don't think it was nearly as popular as the TI calculators so there weren't very many games available for it (maybe 10-20 that I could find at the time). Pacman was a pretty obvious choice to build because it offered more complexity than I was used to and the dimensions of the screen are perfect for it. The game ended up having about ten levels, one of which was designed by a friend.

This was one of my first ventures into C++ so the code is pretty horrendous and impossible to follow. The windowing system of the ClassPad (PEG) was also knew to me and so was another thing to learn.

![Padman](demos/padman.gif)

This shows the very first level (which is an exact clone of the original Pacman level). There was a menu with options and credits but I must have removed that for this version for some reason. Unfortunately the PC emulation crashes on button presses but the version that runs on the actual calculator worked fine and was bug tested and finished. This was probably one of the first non-trivial programs I ever wrote that I would really consider complete. I remember this taking me a long time. I think I worked on and off for a few months on it.

The AI is somewhat notable I suppose. From memory it uses a recursive algorithm to search the entire maze for the most efficient path to player. Unfortunately that would be too perfect, though, so it will only update this path once it travels half the tiles toward the player. This means that it is always travelling to where the player was several tiles ago until it is right next to the player where it will directly follow the player.

The power pills are also sort of interesting. The ClassPad is only black and white (no greyscale) and there needed to be a way to indicate that the ghosts could be eaten. To implement a 'pseudo grayscale' the ghosts are only rendered on every second or third frame when a powerpill has been eaten. This idea is essentially the same as PCM used to make lights look dimmer.


#DeskPOP
#### The Prince of Persia running along the bottom of the desktop.
#### February, 2010

This is a program that renders the Prince of Persia sprite from the original game running along the bottom of the desktop. Once he reaches the edge of the screen he will turn around and run the other way.

![DeskPOP](demos/deskPOP.gif)

There was always a lot of flicker in the program. It did work fairly well in Windows XP though. Windows 7 completely broke it and he is hardly visible. Funnily enough I was only able to see him running when he was being screengrabbed. I always thought this was a pretty cool idea and it's disappointing that I never got a realiable version working.


#TicTacToe
#### A simple tic tac toe game
#### March, 2010

A very simple tic tac toe game for windows. I think I wanted to make this so that I could build an AI for it. From memory I never actually got an unbeatable AI working (I wanted to develop the algorithm myself so no minimax used) but I wasn't able to beat it when I was taking the screengrab. Maybe I just suck at Tic Tac Toe.

![Tic Tac Toe](demos/tictactoe.gif)

The mouse is really difficult to see here. That's an artifact of the screengrabbing software that I used. I am the Xs and the AI is the Os.



#There are still more to come.
