This repo contains the entire VS2012 solution as well as a compiled executable file.

I've tested it on Windows 7 and Windows XP and it runs fine on either as long as the VS2012 C++ redistributables are installed, which you can download from http://www.microsoft.com/en-us/download/details.aspx?id=30679

If the project doesn't import correctly for you:

Create a new console application project named CellAutomata
Uncheck the 'precompiled header' option and the 'Security Development Lifecycle checks' option
Add the .cpp and .header files from the CellularAutomata folder to the new project you've created, overwriting the CellAutomata.cpp file that was created when setting up the project.


Visual Studio 2012 can't build for XP without an update, available at http://blogs.msdn.com/b/vcblog/archive/2012/10/08/10357555.aspx along with directions for using it. If there's a more updated computer available for testing that is probably simpler.