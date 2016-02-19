#define WIN32

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>

// demonstrate a scrollable browser of widgets where children follow size
// erco 05/20/05
//
// dialog
// -------------------------------------------
// |   scroll                                  |
// |   -------------------------------------   |
// |  | fixed   |  stretch               | ^|  |
// |  |_________|________________________|--|  |
// |  | fixed   |  stretch               |  |  |
// |  |_________|________________________|  |  |
// |  | fixed   |  stretch               |  |  |
// |  |_________|________________________|  |  |
// |  | fixed   |  stretch               |  |  |
// |  |_________|________________________|__|  |
// |  |__________________________________| v|  |
// |                                      --   |
// |                              _______      |
// |                             |  ADD  |     |
// |                              -------      |
// |___________________________________________|
//


const int fixedWidth = 25;
const int defaultHeight = 25;

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class ScrollItem : public Fl_Group {
	Fl_Check_Button *fixedBox;
	Fl_Box *stretchBox;
public:
	ScrollItem(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
		begin();
		// Fixed width box
		fixedBox = new Fl_Check_Button(X, Y, fixedWidth, defaultHeight);
		// Stretchy box
		stretchBox = new Fl_Box(X + fixedWidth, Y, W - fixedWidth, defaultHeight, "Stretch");
		stretchBox->box(FL_UP_BOX);
		resizable(stretchBox);
		end();
	}
};

// Custom scroll that tells children to follow scroll's width when resized
class MyScroll : public Fl_Scroll {
	int nchild;
public:
	MyScroll(int X, int Y, int W, int H, const char* L = 0) : Fl_Scroll(X, Y, W, H, L) {
		nchild = 0;
	}
	void resize(int X, int Y, int W, int H) {
		// Tell children to resize to our new width
		for (int t = 0; t<nchild; t++) {
			Fl_Widget *w = child(t);
			w->resize(w->x(), w->y(), W - 20, w->h());    // W-20: leave room for scrollbar
		}
		// Tell scroll children changed in size
		init_sizes();
		Fl_Scroll::resize(X, Y, W, H);
	}

	// Append new scrollitem to bottom
	//     Note: An Fl_Pack would be a good way to do this, too
	//
	void AddItem() {
		int X = x() + 1,
			Y = y() - yposition() + (nchild*defaultHeight) + 1,
			W = w() - 20,                           // -20: compensate for vscroll bar
			H = defaultHeight;
		add(new ScrollItem(X, Y, W, H));
		redraw();
		nchild++;
	}
};

// Callback to add new item to scroll
void add_cb(Fl_Widget*, void *data) {
	MyScroll *scroll = (MyScroll*)data;
	scroll->AddItem();
}

// Main
int main() {
	Fl_Double_Window *win = new Fl_Double_Window(300, 300);
	MyScroll *scroll = new MyScroll(10, 10, win->w() - 20, win->h() - 60);
	scroll->box(FL_BORDER_BOX);
	scroll->end();
	Fl_Button *add_butt = new Fl_Button(win->w() - 150, win->h() - 40, 100, 25, "Add");
	add_butt->callback(add_cb, (void*)scroll);
	// Create a few widgets to start with
	for (int t = 0; t<4; t++) {
		scroll->AddItem();
	}
	win->resizable(scroll);
	win->show();
	return(Fl::run());
}