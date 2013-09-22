#include "gui/napplication.hpp"
#include "gui/nwidget.hpp"

int main (int argc, char * argv[]) {
	NApplication & a = NApplication::instance();
	NWidget w;
	w.keyPressed().connect([&a](int keyCode)->void {
		if (keyCode == 'q') {
			a.quit();
		}
	});
	NWidget * c = new NWidget(&w);
	return a.exec();
}
