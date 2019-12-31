
#include "SDL.h"
#include "SDL_image.h"
#include "checkML.h"
#include <iostream>
#include <vector>

using namespace std;

using uint = unsigned int;

const double IMAGINARY_NEGATIVE_MARGIN = -1.3;
const double IMAGINARY_POSITIVE_MARGIN = 1.3;

const double REAL_NEGATIVE_MARGIN = -2;
const double REAL_POSITIVE_MARGIN = 1;

const int ITERATIONS_PER_PIXEL = 650;

const vector <const char*> nameFiles = {
	"frame001.bmp","frame002.bmp","frame003.bmp","frame004.bmp","frame005.bmp",
	"frame006.bmp","frame007.bmp","frame008.bmp","frame009.bmp","frame010.bmp",
	"frame011.bmp","frame012.bmp","frame013.bmp","frame014.bmp","frame015.bmp",
	"frame016.bmp","frame017.bmp","frame018.bmp","frame019.bmp","frame020.bmp",
	"frame021.bmp","frame022.bmp","frame023.bmp","frame024.bmp","frame025.bmp",
	"frame026.bmp","frame027.bmp","frame028.bmp","frame029.bmp","frame030.bmp",
	"frame031.bmp","frame032.bmp","frame033.bmp","frame034.bmp","frame035.bmp",
	"frame036.bmp","frame037.bmp","frame038.bmp","frame039.bmp","frame040.bmp",
};

//Red, green, blue, alpha
struct pixel {
	int red;
	int green;
	int blue;
	int alpha;
};

//real, imaginary
struct compl {
	double real;
	double img;

	//we declare the multiplication and sum of complex numbers. 
	//This is one more reason to separate it into a separate class

	compl operator * (compl const& other) {
		compl res;
		res.real = (real * other.real) - (img * other.img);
		res.img = (real * other.img) + (other.real * img);
		return res;
	}

	compl operator + (compl const& other) {
		compl res;
		res.real = real + other.real;
		res.img = img + other.img;
		return res;
	}


};


//vector, height, width
struct canvas {
	vector<vector<pixel>> canv;
	int height;
	int width;
};

//table, height, width, center, scale
struct complexPlane {
	vector<vector<compl>> table;
	int height;
	int width;
	compl center;
	double scale;
};

void renderCanvas(canvas& c, SDL_Renderer* renderer) {
	for (int j = 0; j < c.height; j++) {
		for (int i = 0; i < c.width; i++) {
			SDL_SetRenderDrawColor(renderer, c.canv[j][i].red, c.canv[j][i].blue, c.canv[j][i].green, c.canv[j][i].alpha);
			SDL_RenderDrawPoint(renderer, i, j);
		}
	}
}

void clear() {//this cleans the screen and moves the cursor to 0,0
	// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
	std::cout << "\x1B[2J\x1B[H";
}

//this should be in another class and be the constructor of complexplane, but im lazy and this isn't serious, don't do this kids
void generateComplexPlane(complexPlane& cp, int height, int width, compl center, double scale) {
	cp.height = height;
	cp.width = width;
	cp.table = vector<vector<compl>>(height, vector<compl>(width)); // we know how big it needs to be
	cp.center = center;
	cp.scale = scale;

	compl topLeftComplex; //first we calculate the top-left corner number, to later add increments to it and obtain the full chart
	compl increment;	//what we are going to add every time

	topLeftComplex.img = cp.center.img + ((IMAGINARY_POSITIVE_MARGIN - IMAGINARY_NEGATIVE_MARGIN) / 2) / cp.scale;
	topLeftComplex.real = cp.center.real - ((REAL_POSITIVE_MARGIN - REAL_NEGATIVE_MARGIN) / 2) / cp.scale;

	//this is negative because we go topleft -> down
	increment.img = -((double)(IMAGINARY_POSITIVE_MARGIN - IMAGINARY_NEGATIVE_MARGIN) / cp.scale) / cp.width;
	increment.real = ((double)(REAL_POSITIVE_MARGIN - REAL_NEGATIVE_MARGIN) / cp.scale) / cp.width;

	for (int j = 0; j < cp.height; j++) {
		for (int i = 0; i < cp.width; i++) {
			cp.table[j][i].img = topLeftComplex.img + increment.img * j;
			cp.table[j][i].real = topLeftComplex.real + increment.real * i;
		}
	}
}

//given a number, it calculates how many iterations it needs to leave our scope. Right now it isn't very efficient, we could add
//more finishing conditions, for example, if a number goes to (1, -1i) then to (-1, 1i) and back to (1, -1i), we know it isn't
//going to change, and can return the max without waiting
int isInMandelbrot(compl c) {
	int counter = 0;
	compl temp;
	compl seed = { 0.285, -0.01 };	//julia->{ 0.285, -0.01 } raices -> { -1.3, 0.00525 }
	while (c.img <= 1e+40 && c.img >= -1e+40 &&
		c.real <= 1e+40 && c.real >= -1e+40 && counter < ITERATIONS_PER_PIXEL) {
		temp = c * c + seed;	//mandelbrot formula
		c = temp;
		counter++;
	}
	return counter;
}



//you input a complexplane, it gives you a canvas. ez pz
void generateMandelbrot(complexPlane& cp, canvas& cv) {
	int color;
	int colormode = 1;
	if (cp.height == cv.height && cp.width == cv.width) {
		for (int j = 0; j < cp.height; j++) {
			for (int i = 0; i < cp.width; i++) { //1529

				if (colormode == 0) {
					color = (int)(((double)isInMandelbrot(cp.table[j][i]) / (double)ITERATIONS_PER_PIXEL) * 8000);
					switch (color / 255)
					{
					case 0:
						cv.canv[j][i].red = 255;
						cv.canv[j][i].green = color % 255;
						cv.canv[j][i].blue = 0;
						break;
					case 1:
						cv.canv[j][i].red = 255 - color % 255;
						cv.canv[j][i].green = 255;
						cv.canv[j][i].blue = 0;
						break;
					case 2:
						cv.canv[j][i].red = 0;
						cv.canv[j][i].green = 255;
						cv.canv[j][i].blue = color % 255;
						break;
					case 3:
						cv.canv[j][i].red = 0;
						cv.canv[j][i].green = 255 - color % 255;
						cv.canv[j][i].blue = 255;
						break;
					case 4:
						cv.canv[j][i].red = color % 255;
						cv.canv[j][i].green = 0;
						cv.canv[j][i].blue = 255;
						break;
					case 5:
						cv.canv[j][i].red = 255;
						cv.canv[j][i].green = 0;
						cv.canv[j][i].blue = 255 - color % 255;
						break;
					case 6:
						cv.canv[j][i].red = 254;
						cv.canv[j][i].green = 0;
						cv.canv[j][i].blue = 0;
						break;
					default:
						break;
					}
				}
				else if (colormode == 1) {
					color = (int)(((double)isInMandelbrot(cp.table[j][i]) / (double)ITERATIONS_PER_PIXEL) * 1000000);
					cv.canv[j][i].green = color / 3;
					cv.canv[j][i].red = color / 2;
					cv.canv[j][i].blue = color / 7;
				}
				else if (colormode == 2) {
					color = (int)(((double)isInMandelbrot(cp.table[j][i]) / (double)ITERATIONS_PER_PIXEL) * 255);
					cv.canv[j][i].green = cv.canv[j][i].red = cv.canv[j][i].blue = color;
				}
			}
		}
	}
	else cout << "You cant generate a canvas with a complexPlane of a different size! (void generateMandelbrot)\n";
}


//program per se (really kids, organize your code, i'm a horrible person, but you shouldn't do this to you)
void mandelbrot() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Check Memory Leaks
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	char input = ' ';

	uint winWidth = 1000; //1920
	uint winHeight = 800; //1080

	int renderMode = 0;

	cout << "Debug? [Y/N]\n";

	while (input != 'y' && input != 'Y' && input != 'n' && input != 'N') {
		cin >> input;
		if (input == 'y' || input == 'Y') cout << "Debug_Mode\n";
		else if (input == 'n' || input == 'N') {
			cout << "How tall (in pixels) do you want the window? (recommended:1080)\n";
			cin >> winHeight;
			cout << "How wide (in pixels) do you want the window? (recommended:1920)";
			cin >> winWidth;
		}
	}
	const int numFrames = 40;

	double zoom = 0.7;

	canvas cfractal = {
	vector<vector<pixel>>(winHeight, vector<pixel>(winWidth)),
	winHeight,
	winWidth,
	};
	complexPlane cPTest = {};
	compl centerTest = { -0.47098750,0.19021300 };

	canvas video[numFrames];
	int actualFrame = 0;

	if (renderMode == 1) {
		clear();
		cout << "Rendering fractal video\n         ";

		for (int i = 0; i < numFrames; i++) {
			cout << "_";
		}

		cout << "\nProgress:";


		for (int i = 0; i < numFrames; i++) {
			generateComplexPlane(cPTest, winHeight, winWidth, centerTest, zoom);
			generateMandelbrot(cPTest, cfractal);
			video[i] = cfractal;
			zoom += 0.5 * zoom * zoom / (zoom / 2);
			cout << "*";
		}
		int actualFrame = 0;
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("First test with SDL", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (window == nullptr || renderer == nullptr)
		cout << "Error cargando SDL" << endl;
	else {
		SDL_Event eventInput;
		SDL_PollEvent(&eventInput);

		zoom = 1; //348611

		while (eventInput.type != SDL_QUIT) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			if (renderMode == 1 && actualFrame < numFrames) {
				renderCanvas(video[actualFrame % numFrames], renderer); 
			
				SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, winWidth, winHeight, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

				if (pScreenShot)
				{
					// Read the pixels from the current render target and save them onto the surface
					SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window), pScreenShot->pixels, pScreenShot->pitch);

					// Create the bmp screenshot file
					SDL_SaveBMP(pScreenShot, nameFiles[actualFrame % numFrames]);

					// Destroy the screenshot surface
					SDL_FreeSurface(pScreenShot);
				}
			
			}
			else if (renderMode == 0){

				generateComplexPlane(cPTest, winHeight, winWidth, centerTest, zoom);
				generateMandelbrot(cPTest, cfractal);
				renderCanvas(cfractal, renderer);

				// Create an empty RGB surface that will be used to create the screenshot bmp file
				SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, winWidth, winHeight, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

				if (pScreenShot)
				{
					// Read the pixels from the current render target and save them onto the surface
					SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window), pScreenShot->pixels, pScreenShot->pitch);

					// Create the bmp screenshot file
					SDL_SaveBMP(pScreenShot, "Screenshot.bmp");

					// Destroy the screenshot surface
					SDL_FreeSurface(pScreenShot);
				}


				zoom += 0.4 * zoom * zoom / (zoom / 2); //0.15 * zoom * zoom / (zoom / 2)
				cout << zoom << "\n";			

			}

			SDL_RenderPresent(renderer);
			SDL_PollEvent(&eventInput);
			actualFrame++;
			
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}




int main(int argc, char* argv[]) {
	mandelbrot();
	return 0;
}