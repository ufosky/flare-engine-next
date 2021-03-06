/*
Copyright © 2014 Igor Paliychuk
Copyright © 2014 Henrik Andersson

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class MenuBook
 */

#include "FileParser.h"
#include "MenuBook.h"
#include "SharedResources.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "SharedGameResources.h"

#include <climits>

using namespace std;

MenuBook::MenuBook()
	: book_name("")
	, book_loaded(false) {

	closeButton = new WidgetButton("images/menus/buttons/button_x.png");

	tablist = TabList();
	tablist.add(closeButton);
}

void MenuBook::loadBook() {
	if (book_loaded) return;

	// Read data from config file
	FileParser infile;

	if (infile.open(book_name)) {
		while (infile.next()) {
			if (parseMenuKey(infile.key, infile.val))
				continue;

			infile.val = infile.val + ',';

			if(infile.key == "close") {
				closeButton->pos.x = popFirstInt(infile.val);
				closeButton->pos.y = popFirstInt(infile.val);
			}
			else if (infile.key == "background") {
			        setBackground(popFirstString(infile.val));
			}

			if (infile.new_section) {

				// for sections that are stored in collections, add a new object here
				if (infile.section == "text") {
					text.push_back(NULL);
					textData.push_back("");
					textColor.push_back(Color());
					justify.push_back(0);
					textFont.push_back("");
					size.push_back(Rect());
				}
				else if (infile.section == "image") {
					image.push_back(NULL);
					image_dest.push_back(Point());
				}

			}
			if (infile.section == "text")
				loadText(infile);
			else if (infile.section == "image")
				loadImage(infile);
		}

		infile.close();
	}

	// setup image dest
	for (unsigned i=0; i < image.size(); i++) {
	       image[i]->setDest(image_dest[i]);
	}

	// render text to surface
	for (unsigned i=0; i<text.size(); i++) {
		font->setFont(textFont[i]);
		Point pSize = font->calc_size(textData[i], size[i].w);
		Image *graphics = render_device->createAlphaSurface(size[i].w, pSize.y);

		if (justify[i] == JUSTIFY_CENTER)
			font->render(textData[i], size[i].w/2, 0, justify[i], graphics, size[i].w, textColor[i]);
		else if (justify[i] == JUSTIFY_RIGHT)
			font->render(textData[i], size[i].w, 0, justify[i], graphics, size[i].w, textColor[i]);
		else
			font->render(textData[i], 0, 0, justify[i], graphics, size[i].w, textColor[i]);
		text[i] = graphics->createSprite();
		graphics->unref();
	}

	align();
	alignElements();

	book_loaded = true;
}

void MenuBook::loadImage(FileParser &infile) {
	if (infile.key == "image_pos") {
		image_dest.back() = toPoint(infile.val);
	}
	else if (infile.key == "image") {
	        Image *graphics;
		graphics = render_device->loadGraphicSurface(popFirstString(infile.val));
		if (graphics) {
		  image.back() = graphics->createSprite();
		  graphics->unref();
		}
	}
}

void MenuBook::loadText(FileParser &infile) {
	if (infile.key == "text_pos") {
		size.back().x = popFirstInt(infile.val);
		size.back().y = popFirstInt(infile.val);
		size.back().w = popFirstInt(infile.val);
		std::string _justify = popFirstString(infile.val);

		if (_justify == "left") justify.back() = JUSTIFY_LEFT;
		else if (_justify == "center") justify.back() = JUSTIFY_CENTER;
		else if (_justify == "right") justify.back() = JUSTIFY_RIGHT;
	}
	else if (infile.key == "text_font") {
		Color color;
		color.r = popFirstInt(infile.val);
		color.g = popFirstInt(infile.val);
		color.b = popFirstInt(infile.val);
		textColor.back() = color;
		textFont.back() = popFirstString(infile.val);
	}
	else if (infile.key == "text") {
		textData.back() = infile.val;
		// remove comma from the end
		textData.back() = textData.back().substr(0, textData.back().length() - 1);
	}
}

void MenuBook::alignElements() {
	closeButton->pos.x += window_area.x;
	closeButton->pos.y += window_area.y;

	Rect clip;
	clip.x = clip.y = 0;
	clip.w = window_area.w;
	clip.h = window_area.h;

	setBackgroundClip(clip);
	setBackgroundDest(window_area);

	for (unsigned i=0; i<text.size(); i++) {
		text[i]->setDestX(size[i].x + window_area.x);
		text[i]->setDestY(size[i].y + window_area.y);
	}
	for (unsigned i=0; i<image.size(); i++) {
		image[i]->setDestX(image[i]->getDest().x + window_area.x);
		image[i]->setDestY(image[i]->getDest().y + window_area.y);
	}
}

void MenuBook::clearBook() {
	for (unsigned i=0; i<text.size(); i++) {
		delete text[i];
	}
	text.clear();

	textData.clear();
	textColor.clear();
	textFont.clear();
	size.clear();
	image_dest.clear();

	for (unsigned i=0; i<image.size(); i++) {
		delete image[i];
	}
	image.clear();
}

void MenuBook::logic() {
	if (book_name == "") return;
	else {
		loadBook();
		visible = true;
	}

	if (NO_MOUSE) {
		tablist.logic();
	}
	if (closeButton->checkClick() || (inpt->pressing[ACCEPT] && !inpt->lock[ACCEPT])) {
		if (inpt->pressing[ACCEPT]) inpt->lock[ACCEPT] = true;

		clearBook();

		snd->play(sfx_close);

		visible = false;
		book_name = "";
		book_loaded = false;
	}
}

void MenuBook::render() {
	if (!visible) return;

	Menu::render();

	closeButton->render();
	for (unsigned i=0; i<text.size(); i++) {
		render_device->render(text[i]);
	}
	for (unsigned i=0; i<image.size(); i++) {
		render_device->render(image[i]);
	}
}

MenuBook::~MenuBook() {
	delete closeButton;
	clearBook();

}
