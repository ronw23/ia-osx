#ifndef MANUAL_H
#define MANUAL_H

#include <vector>
#include <string>

using namespace std;

class Engine;

class Manual
{
    public:
        Manual(Engine* engine) : eng(engine) {
			readFile();
		}

        void run();

    private:
		void readFile();

		void drawManualInterface();

		vector<string> lines;

        Engine* eng;
};

#endif
