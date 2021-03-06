/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller
Copyright © 2013 Henrik Andersson
Copyright © 2013 Kurt Rinnert

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
 * Utils
 *
 * Various utility structures, enums, function
 */

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <stdint.h>
#include <string>

class Point {
public:
	int x, y;
	Point() : x(0), y(0) {}
	Point(int _x, int _y) : x(_x), y(_y) {}
};

class FPoint {
public:
	float x, y;
	FPoint(Point _p) : x((float)_p.x), y((float)_p.y) {}
	FPoint() : x(0), y(0) {}
	FPoint(float _x, float _y) : x(_x), y(_y) {}
};

class Rect {
public:
	int x, y, w, h;
	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(SDL_Rect _r) : x(_r.x), y(_r.y), w(_r.w), h(_r.h) {}
	operator SDL_Rect() {
		SDL_Rect r;
		r.x = x;
		r.y = y;
		r.w = w;
		r.h = h;
		return r;
	}
};

class Color {
public:
	Uint8 r, g, b, a;
	Color() : r(0), g(0), b(0), a(255) {}
	Color(Uint8 _r, Uint8 _g, Uint8 _b) : r(_r), g(_g), b(_b), a(255) {}
	Color(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a) : r(_r), g(_g), b(_b), a(_a) {}
	operator SDL_Color() {
		SDL_Color c;
		c.r = r;
		c.g = g;
		c.b = b;
#if SDL_VERSION_ATLEAST(2,0,0)
        c.a = a;
#else
		// Warning: SDL 1.2 only
		c.unused = a;
#endif
		return c;
	}
};

class Event_Component {
public:
	std::string type;
	std::string s;
	int x;
	int y;
	int z;
	int a;
	int b;

	Event_Component()
		: type("")
		, s("")
		, x(0)
		, y(0)
		, z(0)
		, a(0)
		, b(0)
	{}
};

Point floor(FPoint fp);
FPoint screen_to_map(int x, int y, float camx, float camy);
Point map_to_screen(float x, float y, float camx, float camy);
Point center_tile(Point p);
Point map_to_collision(FPoint p);
FPoint collision_to_map(Point p);
FPoint calcVector(FPoint pos, int direction, float dist);
float calcDist(FPoint p1, FPoint p2);
float calcTheta(float x1, float y1, float x2, float y2);
int calcDirection(float x0, float y0, float x1, float y1);
int calcDirection(const FPoint &src, const FPoint &dst);
bool isWithin(FPoint center, float radius, FPoint target);
bool isWithin(Rect r, Point target);

std::string abbreviateKilo(int amount);
void alignToScreenEdge(std::string alignment, Rect *r);
void alignFPoint(FPoint *pos);

class Image * loadTextureImage(std::string filename);

#endif
