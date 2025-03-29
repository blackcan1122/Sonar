/*
 Copyright (C) [Year] [Your Name]

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

/*
 Version: 1.5.0
 Author: Marcel Schulz
 Description:
  A learning tool utilizing Object-Oriented Programming techniques to explore various game development concepts.

 Features:
 - Learning-oriented game modes including Pong
 - Physics engine implementation 
 - Dynamic GameMode loading via DLLs
 - Time tracker for monitoring overtime hours
 - Modular and extensible architecture

 Contact: https://github.com/blackcan1122

 Note for Contributors:
 Contributions are welcome! Please fork the repository and submit your pull requests.
*/

#include "Base/GameInstance.h"
int main (int args, char* argv[])
{
	bool isDebug = false;

	if (args > 1 && strcmp(argv[1], "-debug") == 0)
	{
		isDebug = true;
	}

#if DEBUG
	if (isDebug == false)
		isDebug = true;
#endif

	std::cout << isDebug << std::endl;
	GameInstance::InitGameInstance(WindowProperties(1280, 720, 60, false, isDebug));
	
	CloseWindow();
	return 0;
}
