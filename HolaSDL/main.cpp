
#include "SDL.h"
#include "SDL_image.h"
#include "checkML.h"
#include <iostream>
#include <vector>

using namespace std;

using uint = unsigned int;

const int IMAGINARY_NEGATIVE_MARGIN = -1;
const int IMAGINARY_POSITIVE_MARGIN = 1;

const int REAL_NEGATIVE_MARGIN = -2;
const int REAL_POSITIVE_MARGIN = 1;

const int ITERATIONS_PER_PIXEL = 255;

//Red, green, blue, alpha
struct pixel {
	int red;
	int green;
	int blue;
	int alpha;
};

//imaginary, real
struct compl {
	double real;
	double img;

	//we declare the multiplication and sum of complex numbers. This is one more reason to separate it into a separate class
	compl operator*(compl other) {
		return compl{(real* other.real) - (img * other.img), (real* other.img) + (other.real * img)};
	}

	compl operator+(compl other) {
		return compl{real + other.real, img + other.img};
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

//this should be in another class and be the constructor of complexplane, but im lazy and this isn't serious, don't do this kids
void generateComplexPlane(complexPlane& cp, int height, int width, compl center, double scale) {
	cp.height = height;
	cp.width = width;
	cp.table = vector<vector<compl>>(height, vector<compl>(width)); // we know how big it needs to be
	cp.center = center;
	cp.scale = scale;

	compl topLeftComplex; //first we calculate the top-left corner number, to later add increments to it and obtain the full chart
	compl increment;	//what we are going to add

	topLeftComplex.img = cp.center.img + ((IMAGINARY_POSITIVE_MARGIN - IMAGINARY_NEGATIVE_MARGIN) / 2) / cp.scale;
	topLeftComplex.real = cp.center.real - ((REAL_POSITIVE_MARGIN - REAL_NEGATIVE_MARGIN) / 2) / cp.scale;

	//this is negative because we go topleft -> down
	increment.img = -((double)(IMAGINARY_POSITIVE_MARGIN - IMAGINARY_NEGATIVE_MARGIN) / cp.scale) / cp.height;
	increment.real = ((double)(REAL_POSITIVE_MARGIN - REAL_NEGATIVE_MARGIN) / cp.scale) / cp.height;

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
	while (c.img < IMAGINARY_POSITIVE_MARGIN && c.img > IMAGINARY_NEGATIVE_MARGIN &&
		c.real < REAL_POSITIVE_MARGIN && c.real > REAL_NEGATIVE_MARGIN && counter < ITERATIONS_PER_PIXEL) {
		c = c * c + c;	//mandelbrot formula
		counter++;
	}
	return counter;
}


//you input a complexplane, it gives you a canvas. ez pz
void generateMandelbrot(complexPlane& cp, canvas& cv) {
	if (cp.height == cv.height && cp.width == cv.width) {
		for (int j = 0; j < cp.height; j++) {
			for (int i = 0; i < cp.width; i++) {
				cv.canv[j][i].red = cv.canv[j][i].green = cv.canv[j][i].blue = isInMandelbrot(cp.table[j][i]);
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

	char input;

	uint winWidth = 1920; //1920
	uint winHeight = 1080; //1080

	cout << "Debug\n";
	//cin >> input;
	if (true || input == 'a') cout << "Debug_Mode\n";
	else {
		cout << "How tall do you want the window?\n";
		cin >> winHeight;
		cout << "How wide do you want the window?";
		cin >> winWidth;
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("First test with SDL", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	/*
	canvas canvas{
		{
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{255,0,255,255},{255,0,255,255}, },
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255}, {255,0,255,255},{255,0,255,255},},
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255}, {255,0,255,255},{255,0,255,255},},
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255}, {255,0,255,255},{255,0,255,255},},
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255}, {255,0,255,255},{255,0,255,255},},
		{{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255},{0,255,255,255}, {255,0,255,255},{255,0,255,255},},
		{{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},},
		{{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},{255,0,255,255},},
		},
		8,
		10,
	};
	*/

	compl a = {23.567000000, 105.5600000 };
	a = a*a;
	cout << a.real;
	cout << "+";
	cout << a.img;
	cout << "i\n";

	canvas canvas = {
	vector<vector<pixel>>(winHeight, vector<pixel>(winWidth)),
	winHeight,
	winWidth,
	};
	complexPlane cPTest = {};
	compl centerTest = { -1.2,0 };

	generateComplexPlane(cPTest, winHeight, winWidth, centerTest, 1);
	generateMandelbrot(cPTest, canvas);

	if (window == nullptr || renderer == nullptr)
		cout << "Error cargando SDL" << endl;
	else {
		SDL_Event eventInput;
		SDL_PollEvent(&eventInput);
		while (eventInput.type != SDL_QUIT) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			renderCanvas(canvas, renderer);

			SDL_RenderPresent(renderer);
			SDL_PollEvent(&eventInput);
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