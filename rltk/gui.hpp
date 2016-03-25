/* RLTK (RogueLike Tool Kit) 1.00
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the LGPL - see LICENSE file.
 *
 * Provides support for complicated GUIs.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>

#include "virtual_terminal.hpp"

namespace rltk {

/*
 * Base type for retained-mode GUI controls.
 */
struct gui_control_t {
	virtual void render(virtual_terminal * console)=0;
};

/*
 * A renderable layer. You won't use this type directly.
 */
struct layer_t {
	/* This specialization is for generic consoles */
	layer_t(const int X, const int Y, const int W, const int H, std::string font_name, std::function<void(layer_t *,int,int)> resize_fun, bool render_background=true) :
		x(X), y(Y), w(W), h(H), font(font_name), resize_func(resize_fun), has_background(render_background) 
	{
		console = std::make_unique<virtual_terminal>(font_name, x, y, has_background);
	    console->resize_pixels(w, h);
	}

	/* This specialization is for owner-draw panels */
	layer_t(const int X, const int Y, const int W, const int H, std::function<void(layer_t *,int,int)> resize_fun, std::function<void(layer_t *, sf::RenderTexture &)> owner_draw_fun) :
		x(X), y(Y), w(W), h(H), resize_func(resize_fun), owner_draw_func(owner_draw_fun)
	{
	}

	int x;
	int y;
	int w;
	int h;
	std::string font;

	std::function<void(layer_t *,int,int)> resize_func;
	std::function<void(layer_t *, sf::RenderTexture &)> owner_draw_func;
	std::unique_ptr<virtual_terminal> console;
	bool has_background;
	std::vector<gui_control_t> controls;
	std::unique_ptr<sf::RenderTexture> backing; // Used for owner-draw layers

	void make_owner_draw_backing();
	void on_resize(const int width, const int height);
	void render(sf::RenderWindow &window);
};

/*
 * The overall GUI - holds layers and handles render calls. Access via rltk::gui
 */
struct gui_t {
public:
	gui_t(const int w, const int h) : screen_width(w), screen_height(h) {}
	void on_resize(const int w, const int h);
	void render(sf::RenderWindow &window);

	// Specialization for adding console layers
	void add_layer(const int handle, const int X, const int Y, const int W, const int H, std::string font_name, std::function<void(layer_t *,int,int)> resize_fun, bool has_background=true, int order=-1);
	
	// Specialization for adding owner-draw layers
	void add_owner_layer(const int handle, const int X, const int Y, const int W, const int H, std::function<void(layer_t *,int,int)> resize_fun, std::function<void(layer_t *, sf::RenderTexture &)> owner_draw_fun, int order=-1);
	void delete_layer(const int handle);
	layer_t * get_layer(const int handle);

private:
	int screen_width;
	int screen_height;
	int render_order = 0;

	std::unordered_map<int, layer_t> layers;
};

}