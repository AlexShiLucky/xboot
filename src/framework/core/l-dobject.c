/*
 * framework/core/l-dobject.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <framework/core/l-image.h>
#include <framework/core/l-ninepatch.h>
#include <framework/core/l-shape.h>
#include <framework/core/l-text.h>
#include <framework/core/l-dobject.h>

enum {
	MFLAG_TRANSLATE					= (0x1 << 0),
	MFLAG_ROTATE					= (0x1 << 1),
	MFLAG_SCALE						= (0x1 << 2),
	MFLAG_SKEW						= (0x1 << 3),
	MFLAG_ANCHOR					= (0x1 << 4),
	MFLAG_LOCAL_MATRIX				= (0x1 << 5),
	MFLAG_GLOBAL_MATRIX				= (0x1 << 6),
};

static inline cairo_matrix_t * dobject_local_matrix(struct ldobject_t * o)
{
	cairo_matrix_t * m = &o->local_matrix;
	if(o->mflag & MFLAG_LOCAL_MATRIX)
	{
		if((o->mflag & (MFLAG_TRANSLATE | MFLAG_ROTATE | MFLAG_SCALE | MFLAG_SKEW | MFLAG_ANCHOR)) == MFLAG_TRANSLATE)
		{
			m->xx = 1; m->yx = 0;
			m->xy = 0; m->yy = 1;
			m->x0 = o->x; m->y0 = o->y;
		}
		else
		{
			if(o->mflag & (MFLAG_ROTATE | MFLAG_SKEW))
			{
				double rx = o->rotation + o->skewy;
				double ry = o->rotation - o->skewx;
				m->xx = cos(rx);
				m->yx = sin(rx);
				m->xy = -sin(ry);
				m->yy = cos(ry);
				if(o->mflag & MFLAG_SCALE)
				{
					m->xx *= o->scalex;
					m->yx *= o->scalex;
					m->xy *= o->scaley;
					m->yy *= o->scaley;
				}
			}
			else
			{
				m->xx = o->scalex; m->yx = 0;
				m->xy = 0; m->yy = o->scaley;
			}
			if(o->mflag & MFLAG_ANCHOR)
			{
				double anchorx = o->anchorx * o->width;
				double anchory = o->anchory * o->height;
				m->x0 = o->x - (anchorx * m->xx + anchory * m->xy);
				m->y0 = o->y - (anchorx * m->yx + anchory * m->yy);
			}
			else
			{
				m->x0 = o->x;
				m->y0 = o->y;
			}
		}
		o->mflag &= ~MFLAG_LOCAL_MATRIX;
	}
	return m;
}

static inline cairo_matrix_t * dobject_global_matrix(struct ldobject_t * o)
{
	cairo_matrix_t * m = &o->global_matrix;
	if(o->mflag & MFLAG_GLOBAL_MATRIX)
	{
		memcpy(m, dobject_local_matrix(o), sizeof(cairo_matrix_t));
		while(o->parent)
		{
			o = o->parent;
			cairo_matrix_multiply(m, m, dobject_local_matrix(o));
		}
		o->mflag &= ~MFLAG_GLOBAL_MATRIX;
	}
	return m;
}

enum layout_direction_t {
	LAYOUT_DIRECTION_ROW 			= 0,
	LAYOUT_DIRECTION_ROW_REVERSE	= 1,
	LAYOUT_DIRECTION_COLUMN			= 2,
	LAYOUT_DIRECTION_COLUMN_REVERSE	= 3,
};

enum layout_justify_t {
	LAYOUT_JUSTIFY_START			= 0,
	LAYOUT_JUSTIFY_END				= 1,
	LAYOUT_JUSTIFY_CENTER			= 2,
	LAYOUT_JUSTIFY_BETWEEN			= 3,
	LAYOUT_JUSTIFY_AROUND			= 4,
	LAYOUT_JUSTIFY_EVENLY			= 5,
};

enum layout_align_t {
	LAYOUT_ALIGN_START				= 0,
	LAYOUT_ALIGN_END				= 1,
	LAYOUT_ALIGN_CENTER				= 2,
	LAYOUT_ALIGN_STRETCH			= 3,
};

enum layout_align_self_t {
	LAYOUT_ALIGN_SELF_AUTO			= 0,
	LAYOUT_ALIGN_SELF_START			= 1,
	LAYOUT_ALIGN_SELF_END			= 2,
	LAYOUT_ALIGN_SELF_CENTER		= 3,
	LAYOUT_ALIGN_SELF_STRETCH		= 4,
};

static inline void dobject_layout_set_direction(struct ldobject_t * o, enum layout_direction_t direction)
{
	o->layout.style &= ~(0x3 << 2);
	o->layout.style |= direction << 2;
}

static inline enum layout_direction_t dobject_layout_get_direction(struct ldobject_t * o)
{
	return (o->layout.style >> 2) & 0x3;
}

static inline void dobject_layout_set_justify(struct ldobject_t * o, enum layout_justify_t justify)
{
	o->layout.style &= ~(0xf << 4);
	o->layout.style |= justify << 4;
}

static inline enum layout_justify_t dobject_layout_get_justify(struct ldobject_t * o)
{
	return (o->layout.style >> 4) & 0xf;
}

static inline void dobject_layout_set_align(struct ldobject_t * o, enum layout_align_t align)
{
	o->layout.style &= ~(0xf << 8);
	o->layout.style |= align << 8;
}

static inline enum layout_align_t dobject_layout_get_align(struct ldobject_t * o)
{
	return (o->layout.style >> 8) & 0xf;
}

static inline void dobject_layout_set_align_self(struct ldobject_t * o, enum layout_align_self_t align)
{
	o->layout.style &= ~(0xf << 12);
	o->layout.style |= align << 12;
}

static inline enum layout_align_self_t dobject_layout_get_align_self(struct ldobject_t * o)
{
	return (o->layout.style >> 12) & 0xf;
}

static inline double dobject_layout_main_leading_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.left;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.top;
		default:
			break;
		}
	}
	return o->margin.left;
}

static inline double dobject_layout_cross_leading_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.top;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.left;
		default:
			break;
		}
	}
	return o->margin.top;
}

static inline double dobject_layout_main_trailing_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.right;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.bottom;
		default:
			break;
		}
	}
	return o->margin.right;
}

static inline double dobject_layout_cross_trailing_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.bottom;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.right;
		default:
			break;
		}
	}
	return o->margin.bottom;
}

static inline double dobject_layout_main_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.left + o->margin.right;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.top + o->margin.bottom;
		default:
			break;
		}
	}
	return o->margin.left + o->margin.right;
}

static inline double dobject_layout_cross_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->margin.top + o->margin.bottom;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->margin.left + o->margin.right;
		default:
			break;
		}
	}
	return o->margin.top + o->margin.bottom;
}

static inline double dobject_layout_main_size(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	double basis = o->layout.basis;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			if(basis <= 0.0)
			{
				if(isnan(o->layout.width))
					o->layout.width = o->width;
				basis = o->layout.width;
			}
			else if(basis <= 1.0)
			{
				basis = parent->width * basis;
			}
			break;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			if(basis <= 0.0)
			{
				if(isnan(o->layout.height))
					o->layout.height = o->height;
				basis = o->layout.height;
			}
			else if(basis <= 1.0)
			{
				basis = parent->height * basis;
			}
			break;
		default:
			break;
		}
	}
	return basis;
}

static inline double dobject_layout_cross_size(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			if(isnan(o->layout.height))
				o->layout.height = o->height;
			return o->layout.height;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			if(isnan(o->layout.width))
				o->layout.width = o->width;
			return o->layout.width;
		default:
			break;
		}
	}
	if(isnan(o->layout.height))
		o->layout.height = o->height;
	return o->layout.height;
}

static inline double dobject_layout_container_main_size(struct ldobject_t * o)
{
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
	case LAYOUT_DIRECTION_ROW_REVERSE:
		return o->width;
	case LAYOUT_DIRECTION_COLUMN:
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		return o->height;
	default:
		break;
	}
	return o->width;
}

static inline double dobject_layout_container_cross_size(struct ldobject_t * o)
{
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
	case LAYOUT_DIRECTION_ROW_REVERSE:
		return o->height;
	case LAYOUT_DIRECTION_COLUMN:
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		return o->width;
	default:
		break;
	}
	return o->height;
}

static void dobject_layout(struct ldobject_t * o)
{
	struct ldobject_t * pos;
	double consumed, grow, shrink, cms, ccs;
	double space, offset, between;
	double basis, ms, cp, cs;
	enum layout_direction_t direction;
	enum layout_align_t align;
	int count;

	if(list_empty(&o->children))
		return;

	consumed = 0;
	grow = 0;
	shrink = 0;
	count = 0;
	list_for_each_entry(pos, &o->children, entry)
	{
		if(pos->layoutable)
		{
			basis = dobject_layout_main_size(pos);
			consumed += basis + dobject_layout_main_margin(pos);
			grow += pos->layout.grow;
			shrink += pos->layout.shrink * basis;
			count++;
		}
	}

	if(count > 0)
	{
		cms = dobject_layout_container_main_size(o);
		ccs = dobject_layout_container_cross_size(o);
		direction = dobject_layout_get_direction(o);
		align = dobject_layout_get_align(o);
		space = cms - consumed;
		offset = 0;
		between = 0;
		if((space > 0) && (grow == 0))
		{
			switch(dobject_layout_get_justify(o))
			{
			case LAYOUT_JUSTIFY_START:
				break;
			case LAYOUT_JUSTIFY_END:
				offset = space;
				break;
			case LAYOUT_JUSTIFY_CENTER:
				offset = space / 2;
				break;
			case LAYOUT_JUSTIFY_BETWEEN:
				if(count > 1)
					between = space / (count - 1);
				break;
			case LAYOUT_JUSTIFY_AROUND:
				between = space / count;
				offset = between / 2;
				break;
			case LAYOUT_JUSTIFY_EVENLY:
				between = space / (count + 1);
				offset = between;
				break;
			default:
				break;
			}
		}

		list_for_each_entry(pos, &o->children, entry)
		{
			if(pos->layoutable)
			{
				switch(dobject_layout_get_align_self(pos))
				{
				case LAYOUT_ALIGN_SELF_AUTO:
					break;
				case LAYOUT_ALIGN_SELF_START:
					align = LAYOUT_ALIGN_START;
					break;
				case LAYOUT_ALIGN_SELF_END:
					align = LAYOUT_ALIGN_END;
					break;
				case LAYOUT_ALIGN_SELF_CENTER:
					align = LAYOUT_ALIGN_CENTER;
					break;
				case LAYOUT_ALIGN_SELF_STRETCH:
					align = LAYOUT_ALIGN_STRETCH;
					break;
				default:
					break;
				}

				switch(align)
				{
				case LAYOUT_ALIGN_START:
					cs = dobject_layout_cross_size(pos);
					cp = dobject_layout_cross_leading_margin(pos);
					break;
				case LAYOUT_ALIGN_END:
					cs = dobject_layout_cross_size(pos);
					cp = ccs - (cs + dobject_layout_cross_trailing_margin(pos));
					break;
				case LAYOUT_ALIGN_CENTER:
					cs = dobject_layout_cross_size(pos);
					cp = (ccs - (cs + dobject_layout_cross_margin(pos))) / 2 + dobject_layout_cross_leading_margin(pos);
					break;
				case LAYOUT_ALIGN_STRETCH:
					cs = ccs - dobject_layout_cross_margin(pos);
					cp = dobject_layout_cross_leading_margin(pos);
					break;
				default:
					cs = dobject_layout_cross_size(pos);
					cp = dobject_layout_cross_leading_margin(pos);
					break;
				}

				ms = basis = dobject_layout_main_size(pos);
				if((space >= 0) && (pos->layout.grow > 0))
					ms += space * (pos->layout.grow / grow);
				else if((space < 0) && (pos->layout.shrink > 0))
					ms += space * (pos->layout.shrink * basis / shrink);

				switch(direction)
				{
				case LAYOUT_DIRECTION_ROW:
					pos->layout.h = cs;
					pos->layout.y = cp;
					pos->layout.w = ms;
					pos->layout.x = offset + dobject_layout_main_leading_margin(pos);
					break;
				case LAYOUT_DIRECTION_ROW_REVERSE:
					pos->layout.h = cs;
					pos->layout.y = cp;
					pos->layout.w = ms;
					pos->layout.x = cms - (offset + dobject_layout_main_trailing_margin(pos) + ms);
					break;
				case LAYOUT_DIRECTION_COLUMN:
					pos->layout.w = cs;
					pos->layout.x = cp;
					pos->layout.h = ms;
					pos->layout.y = offset + dobject_layout_main_leading_margin(pos);
					break;
				case LAYOUT_DIRECTION_COLUMN_REVERSE:
					pos->layout.w = cs;
					pos->layout.x = cp;
					pos->layout.h = ms;
					pos->layout.y = cms - (offset + dobject_layout_main_trailing_margin(pos) + ms);
					break;
				default:
					break;
				}
				offset = offset + ms + dobject_layout_main_margin(pos) + between;

				pos->layout.x = round(pos->layout.x);
				pos->layout.y = round(pos->layout.y);
				pos->layout.w = round(pos->layout.w);
				pos->layout.h = round(pos->layout.h);
				if(pos->layout.w < 1)
					pos->layout.w = 1;
				if(pos->layout.h < 1)
					pos->layout.h = 1;
			}
		}
	}

	list_for_each_entry(pos, &o->children, entry)
	{
		if(pos->layoutable)
		{
			pos->mflag = MFLAG_LOCAL_MATRIX | MFLAG_GLOBAL_MATRIX;
			pos->rotation = 0;
			pos->skewx = 0;
			pos->skewy = 0;
			pos->anchorx = 0;
			pos->anchory = 0;
			pos->x = pos->layout.x;
			pos->y = pos->layout.y;
			if((pos->x == 0.0) && (pos->y == 0.0))
				pos->mflag &= ~MFLAG_TRANSLATE;
			else
				pos->mflag |= MFLAG_TRANSLATE;
			switch(pos->dtype)
			{
			case DOBJECT_TYPE_CONTAINER:
				pos->scalex = 1.0;
				pos->scaley = 1.0;
				pos->width = pos->layout.w;
				pos->height = pos->layout.h;
				break;
			case DOBJECT_TYPE_IMAGE:
				if(pos->width != 0.0 && pos->height != 0.0)
				{
					pos->scalex = pos->layout.w / pos->width;
					pos->scaley = pos->layout.h / pos->height;
				}
				break;
			case DOBJECT_TYPE_NINEPATCH:
				pos->scalex = 1.0;
				pos->scaley = 1.0;
				pos->width = pos->layout.w;
				pos->height = pos->layout.h;
				ninepatch_stretch(pos->priv, pos->width, pos->height);
				break;
			case DOBJECT_TYPE_SHAPE:
				if(pos->width != 0.0 && pos->height != 0.0)
				{
					pos->scalex = pos->layout.w / pos->width;
					pos->scaley = pos->layout.h / pos->height;
				}
				break;
			case DOBJECT_TYPE_TEXT:
				if(pos->width != 0.0 && pos->height != 0.0)
				{
					pos->scalex = pos->layout.w / pos->width;
					pos->scaley = pos->layout.h / pos->height;
				}
				break;
			default:
				break;
			}
			if((pos->scalex == 1.0) && (pos->scaley == 1.0))
				pos->mflag &= ~MFLAG_SCALE;
			else
				pos->mflag |= MFLAG_SCALE;
		}
		dobject_layout(pos);
	}
}

static inline void dobject_draw_background(cairo_t * cr, struct ldobject_t * o)
{
	if(o->background.enable)
	{
		double x = o->background.border_width / 2;
		double y = o->background.border_width / 2;
		double w = o->width - o->background.border_width;
		double h = o->height - o->background.border_width;
		double radius = o->background.border_radius;
		cairo_save(cr);
		cairo_set_matrix(cr, dobject_global_matrix(o));
		cairo_set_line_width(cr, o->background.border_width);
		if(radius > 0.0)
		{
			cairo_move_to(cr, x + radius, y);
			cairo_line_to(cr, x + w - radius, y);
			cairo_arc(cr, x + w - radius, y + radius, radius, - M_PI / 2, 0);
			cairo_line_to(cr, x + w, y + h - radius);
			cairo_arc(cr, x + w - radius, y + h - radius, radius, 0, M_PI / 2);
			cairo_line_to(cr, x + radius, y + h);
			cairo_arc(cr, x + radius, y + h - radius, radius, M_PI / 2, M_PI);
			cairo_arc(cr, x + radius, y + radius, radius, M_PI, M_PI + M_PI / 2);
		}
		else
		{
			cairo_rectangle(cr, x, y, w, h);
		}
		if(o->background.border_color_alpha != 0.0)
		{
			cairo_set_source_rgba(cr, o->background.border_color_red, o->background.border_color_green, o->background.border_color_blue, o->background.border_color_alpha);
			cairo_stroke_preserve(cr);
		}
		if(o->background.alpha != 0.0)
		{
			cairo_set_source_rgba(cr, o->background.red, o->background.green, o->background.blue, o->background.alpha);
			cairo_fill(cr);
		}
		cairo_restore(cr);
	}
}

static void dobject_draw_image(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct limage_t * img = o->priv;
	cairo_t * cr = disp->cr;
	dobject_draw_background(cr, o);
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	cairo_set_source_surface(cr, img->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_ninepatch(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct lninepatch_t * ninepatch = o->priv;
	cairo_t * cr = disp->cr;
	dobject_draw_background(cr, o);
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	if(ninepatch->lt)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, 0);
		cairo_set_source_surface(cr, ninepatch->lt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, 0);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, 0);
		cairo_set_source_surface(cr, ninepatch->rt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lm)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->lm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->top);
		cairo_scale(cr, ninepatch->__sx, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->mm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->rm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lb)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->lb, 0, 0);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	if(ninepatch->mb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->__h - ninepatch->bottom);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mb, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->rb, 0, 0);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	cairo_restore(cr);
}

static void dobject_draw_shape(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct lshape_t * shape = o->priv;
	cairo_t * cr = disp->cr;
	dobject_draw_background(cr, o);
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	cairo_set_source_surface(cr, shape->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_text(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct ltext_t * text = o->priv;
	cairo_matrix_t * m = dobject_global_matrix(o);
	cairo_t * cr = disp->cr;
	dobject_draw_background(cr, o);
	cairo_save(cr);
	cairo_set_scaled_font(cr, text->font);
	cairo_move_to(cr, m->x0, m->y0);
	cairo_set_matrix(cr, m);
	cairo_move_to(cr, 0, text->metric.height);
	cairo_text_path(cr, text->utf8);
	cairo_set_source(cr, text->pattern);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void dobject_draw_container(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	cairo_t *cr = disp->cr;
	dobject_draw_background(cr, o);
}

static int l_dobject_new(lua_State * L)
{
	struct ldobject_t * o = lua_newuserdata(L, sizeof(struct ldobject_t));

	o->parent = NULL;
	init_list_head(&o->entry);
	init_list_head(&o->children);

	o->width = luaL_optnumber(L, 1, 0);
	o->height = luaL_optnumber(L, 2, 0);
	o->x = 0;
	o->y = 0;
	o->rotation = 0;
	o->scalex = 1;
	o->scaley = 1;
	o->skewx = 0;
	o->skewy = 0;
	o->anchorx = 0;
	o->anchory = 0;
	o->alpha = 1;
	o->margin.left = 0;
	o->margin.top = 0;
	o->margin.right = 0;
	o->margin.bottom = 0;
	o->layout.style = 0;
	o->layout.grow = 0;
	o->layout.shrink = 1;
	o->layout.basis = 0;
	o->layout.width = NAN;
	o->layout.height = NAN;
	o->background.enable = 0;
	o->background.red = 0;
	o->background.green = 0;
	o->background.blue = 0;
	o->background.alpha = 0;
	o->background.border_width = 0;
	o->background.border_radius = 0;
	o->background.border_color_red = 0;
	o->background.border_color_green = 0;
	o->background.border_color_blue = 0;
	o->background.border_color_alpha = 0;
	o->ctype = COLLIDER_TYPE_NONE;
	o->visible = 1;
	o->touchable = 1;
	o->layoutable = 0;
	o->mflag = 0;
	cairo_matrix_init_identity(&o->local_matrix);
	cairo_matrix_init_identity(&o->global_matrix);

	if(luaL_testudata(L, 3, MT_IMAGE))
	{
		o->dtype = DOBJECT_TYPE_IMAGE;
		o->draw = dobject_draw_image;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_NINEPATCH))
	{
		o->dtype = DOBJECT_TYPE_NINEPATCH;
		o->draw = dobject_draw_ninepatch;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_SHAPE))
	{
		o->dtype = DOBJECT_TYPE_SHAPE;
		o->draw = dobject_draw_shape;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_TEXT))
	{
		o->dtype = DOBJECT_TYPE_TEXT;
		o->draw = dobject_draw_text;
		o->priv = lua_touserdata(L, 3);
	}
	else
	{
		o->dtype = DOBJECT_TYPE_CONTAINER;
		o->draw = dobject_draw_container;
		o->priv = NULL;
	}

	luaL_setmetatable(L, MT_DOBJECT);
	return 1;
}

static const luaL_Reg l_dobject[] = {
	{"new",	l_dobject_new},
	{NULL,	NULL}
};

static int m_dobject_gc(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->ctype == COLLIDER_TYPE_POLYGON)
	{
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			free(o->hit.polygon.points);
			o->hit.polygon.points = NULL;
			o->hit.polygon.length = 0;
		}
	}
	return 0;
}

static void dobject_mark_with_children(struct ldobject_t * o, int mark)
{
	struct ldobject_t * pos;

	if(o)
	{
		o->mflag |= mark;
		list_for_each_entry(pos, &o->children, entry)
		{
			dobject_mark_with_children(pos, mark);
		}
	}
}

static int m_add_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	if(c->parent != o)
	{
		c->parent = o;
		if(lua_toboolean(L, 3))
			list_add_tail(&c->entry, &o->children);
		else
			list_add(&c->entry, &o->children);
		dobject_mark_with_children(c, MFLAG_GLOBAL_MATRIX);
	}
	return 0;
}

static int m_remove_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	if(c->parent == o)
	{
		c->parent = NULL;
		list_del_init(&c->entry);
		dobject_mark_with_children(c, MFLAG_GLOBAL_MATRIX);
	}
	return 0;
}

static int m_to_front(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent)
		list_move(&o->entry, &o->parent->children);
	return 0;
}

static int m_to_back(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent)
		list_move_tail(&o->entry, &o->parent->children);
	return 0;
}

static int m_set_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->width = luaL_checknumber(L, 2);
	o->layout.width = NAN;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->width);
	return 1;
}

static int m_set_height(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->height = luaL_checknumber(L, 2);
	o->layout.height = NAN;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_height(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->height);
	return 1;
}

static int m_set_size(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->width = luaL_checknumber(L, 2);
	o->height = luaL_checknumber(L, 3);
	o->layout.width = NAN;
	o->layout.height = NAN;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_size(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->width);
	lua_pushnumber(L, o->height);
	return 2;
}

static int m_set_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->x = luaL_checknumber(L, 2);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->x);
	return 1;
}

static int m_set_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->y = luaL_checknumber(L, 2);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->y);
	return 1;
}

static int m_set_position(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->x = luaL_checknumber(L, 2);
	o->y = luaL_checknumber(L, 3);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_position(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->x);
	lua_pushnumber(L, o->y);
	return 2;
}

static int m_set_rotation(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->rotation = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if(o->rotation == 0.0)
		o->mflag &= ~MFLAG_ROTATE;
	else
		o->mflag |= MFLAG_ROTATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_rotation(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, fmod(o->rotation / (M_PI / 180.0), 360.0));
	return 1;
}

static int m_set_scale_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scalex = luaL_checknumber(L, 2);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scalex);
	return 1;
}

static int m_set_scale_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scaley = luaL_checknumber(L, 2);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scaley);
	return 1;
}

static int m_set_scale(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scalex = luaL_checknumber(L, 2);
	o->scaley = luaL_checknumber(L, 3);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scalex);
	lua_pushnumber(L, o->scaley);
	return 2;
}

static int m_set_skew_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewx);
	return 1;
}

static int m_set_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewy = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewy);
	return 1;
}

static int m_set_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	o->skewy = luaL_checknumber(L, 3) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, fmod(o->skewx / (M_PI / 180.0), 360.0));
	lua_pushnumber(L, fmod(o->skewy / (M_PI / 180.0), 360.0));
	return 2;
}

static int m_set_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->anchorx = luaL_checknumber(L, 2);
	o->anchory = luaL_checknumber(L, 3);
	if((o->anchorx == 0.0) && (o->anchory == 0.0))
		o->mflag &= ~MFLAG_ANCHOR;
	else
		o->mflag |= MFLAG_ANCHOR;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->anchorx);
	lua_pushnumber(L, o->anchory);
	return 2;
}

static int m_set_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->alpha = luaL_checknumber(L, 2);
	return 0;
}

static int m_get_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->alpha);
	return 1;
}

static int m_set_margin(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->margin.left = luaL_optnumber(L, 2, 0);
	o->margin.top = luaL_optnumber(L, 3, 0);
	o->margin.right = luaL_optnumber(L, 4, 0);
	o->margin.bottom = luaL_optnumber(L, 5, 0);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_margin(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->margin.left);
	lua_pushnumber(L, o->margin.top);
	lua_pushnumber(L, o->margin.right);
	lua_pushnumber(L, o->margin.bottom);
	return 4;
}

static int m_set_layout_direction(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "row");
	switch(shash(type))
	{
	case 0x0b88a69d: /* "row" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_ROW);
		break;
	case 0xf84b1686: /* "row-reverse" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_ROW_REVERSE);
		break;
	case 0xf6e39413: /* "column" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_COLUMN);
		break;
	case 0x839f19fc: /* "column-reverse" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_COLUMN_REVERSE);
		break;
	default:
		break;
	}
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_direction(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
		lua_pushstring(L, "row");
		break;
	case LAYOUT_DIRECTION_ROW_REVERSE:
		lua_pushstring(L, "row-reverse");
		break;
	case LAYOUT_DIRECTION_COLUMN:
		lua_pushstring(L, "column");
		break;
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		lua_pushstring(L, "column-reverse");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_justify(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "start");
	switch(shash(type))
	{
	case 0x106149d3: /* "start" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_CENTER);
		break;
	case 0x6f99fd6f: /* "between" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_BETWEEN);
		break;
	case 0xf271318e: /* "around" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_AROUND);
		break;
	case 0xfc089c58: /* "evenly" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_EVENLY);
		break;
	default:
		break;
	}
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_justify(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_justify(o))
	{
	case LAYOUT_JUSTIFY_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_JUSTIFY_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_JUSTIFY_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_JUSTIFY_BETWEEN:
		lua_pushstring(L, "between");
		break;
	case LAYOUT_JUSTIFY_AROUND:
		lua_pushstring(L, "around");
		break;
	case LAYOUT_JUSTIFY_EVENLY:
		lua_pushstring(L, "evenly");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_align(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "start");
	switch(shash(type))
	{
	case 0x106149d3: /* "start" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_CENTER);
		break;
	case 0xaf079762: /* "stretch" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_STRETCH);
		break;
	default:
		break;
	}
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_align(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_align(o))
	{
	case LAYOUT_ALIGN_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_ALIGN_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_ALIGN_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_ALIGN_STRETCH:
		lua_pushstring(L, "stretch");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_align_self(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "auto");
	switch(shash(type))
	{
	case 0x7c94415e: /* "auto" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_AUTO);
		break;
	case 0x106149d3: /* "start" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_CENTER);
		break;
	case 0xaf079762: /* "stretch" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_STRETCH);
		break;
	default:
		break;
	}
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_align_self(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_align_self(o))
	{
	case LAYOUT_ALIGN_SELF_AUTO:
		lua_pushstring(L, "auto");
		break;
	case LAYOUT_ALIGN_SELF_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_ALIGN_SELF_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_ALIGN_SELF_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_ALIGN_SELF_STRETCH:
		lua_pushstring(L, "stretch");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_grow(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.grow = luaL_checknumber(L, 2);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_grow(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.grow);
	return 1;
}

static int m_set_layout_shrink(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.shrink = luaL_checknumber(L, 2);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_shrink(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.shrink);
	return 1;
}

static int m_set_layout_basis(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.basis = luaL_checknumber(L, 2);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layout_basis(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.basis);
	return 1;
}

static int m_set_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->background.red = luaL_optnumber(L, 2, 1);
	o->background.green = luaL_optnumber(L, 3, 1);
	o->background.blue = luaL_optnumber(L, 4, 1);
	o->background.alpha = luaL_optnumber(L, 5, 1);
	if((o->background.alpha == 0.0) && (o->background.border_color_alpha == 0.0))
		o->background.enable = 0;
	else
		o->background.enable = 1;
	return 0;
}

static int m_get_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->background.red);
	lua_pushnumber(L, o->background.green);
	lua_pushnumber(L, o->background.blue);
	lua_pushnumber(L, o->background.alpha);
	return 4;
}

static int m_set_background_border_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->background.border_width = luaL_optnumber(L, 2, 0);
	return 0;
}

static int m_get_background_border_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->background.border_width);
	return 1;
}

static int m_set_background_border_radius(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->background.border_radius = luaL_optnumber(L, 2, 0);
	return 0;
}

static int m_get_background_border_radius(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->background.border_radius);
	return 1;
}

static int m_set_background_border_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->background.border_color_red = luaL_optnumber(L, 2, 0);
	o->background.border_color_green = luaL_optnumber(L, 3, 0);
	o->background.border_color_blue = luaL_optnumber(L, 4, 0);
	o->background.border_color_alpha = luaL_optnumber(L, 5, 1);
	if((o->background.alpha == 0.0) && (o->background.border_color_alpha == 0.0))
		o->background.enable = 0;
	else
		o->background.enable = 1;
	return 0;
}

static int m_get_background_border_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->background.border_color_red);
	lua_pushnumber(L, o->background.border_color_green);
	lua_pushnumber(L, o->background.border_color_blue);
	lua_pushnumber(L, o->background.border_color_alpha);
	return 4;
}

static int m_set_collider(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "");
	double * p = NULL;
	int i, n = 0;
	if(o->ctype == COLLIDER_TYPE_POLYGON)
	{
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			free(o->hit.polygon.points);
			o->hit.polygon.points = NULL;
			o->hit.polygon.length = 0;
		}
	}
	switch(shash(type))
	{
	case 0xf679fe97: /* "circle" */
		o->ctype = COLLIDER_TYPE_CIRCLE;
		o->hit.circle.x = luaL_optnumber(L, 3, o->width / 2);
		o->hit.circle.y = luaL_optnumber(L, 4, o->height / 2);
		o->hit.circle.radius = luaL_optnumber(L, 5, (o->width < o->height ? o->width : o->height) / 2);
		break;
	case 0x66448f53: /* "ellipse" */
		o->ctype = COLLIDER_TYPE_ELLIPSE;
		o->hit.ellipse.x = luaL_optnumber(L, 3, o->width / 2);
		o->hit.ellipse.y = luaL_optnumber(L, 4, o->height / 2);
		o->hit.ellipse.width = luaL_optnumber(L, 5, o->width / 2);
		o->hit.ellipse.height = luaL_optnumber(L, 6, o->height / 2);
		break;
	case 0xe1f5207a: /* "rectangle" */
		o->ctype = COLLIDER_TYPE_RECTANGLE;
		o->hit.rectangle.x = luaL_optnumber(L, 3, 0);
		o->hit.rectangle.y = luaL_optnumber(L, 4, 0);
		o->hit.rectangle.width = luaL_optnumber(L, 5, o->width);
		o->hit.rectangle.height = luaL_optnumber(L, 6, o->height);
		break;
	case 0x4b99d0b8: /* "rounded-rectangle" */
		o->ctype = COLLIDER_TYPE_ROUNDED_RECTANGLE;
		o->hit.rounded_rectangle.x = luaL_optnumber(L, 3, 0);
		o->hit.rounded_rectangle.y = luaL_optnumber(L, 4, 0);
		o->hit.rounded_rectangle.width = luaL_optnumber(L, 5, o->width);
		o->hit.rounded_rectangle.height = luaL_optnumber(L, 6, o->height);
		o->hit.rounded_rectangle.radius = luaL_optnumber(L, 7, (o->width < o->height ? o->width : o->height) / 5);
		break;
	case 0xbc0d44cd: /* "polygon" */
		o->ctype = COLLIDER_TYPE_POLYGON;
		if(lua_istable(L, 3))
		{
			n = lua_rawlen(L, 3) & ~0x1;
			if(n > 0)
			{
				p = malloc(sizeof(double) * n);
				for(i = 0; i < n; i++)
				{
					lua_rawgeti(L, 3, i + 1);
					p[i] = luaL_checknumber(L, -1);
					lua_pop(L, 1);
				}
			}
		}
		o->hit.polygon.points = p;
		o->hit.polygon.length = n;
		break;
	default:
		o->ctype = COLLIDER_TYPE_NONE;
		break;
	}
	return 0;
}

static int m_get_collider(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	int i;
	switch(o->ctype)
	{
	case COLLIDER_TYPE_NONE:
		lua_pushstring(L, "none");
		return 1;
	case COLLIDER_TYPE_CIRCLE:
		lua_pushstring(L, "circle");
		lua_pushnumber(L, o->hit.circle.x);
		lua_pushnumber(L, o->hit.circle.y);
		lua_pushnumber(L, o->hit.circle.radius);
		return 4;
	case COLLIDER_TYPE_ELLIPSE:
		lua_pushstring(L, "ellipse");
		lua_pushnumber(L, o->hit.ellipse.x);
		lua_pushnumber(L, o->hit.ellipse.y);
		lua_pushnumber(L, o->hit.ellipse.width);
		lua_pushnumber(L, o->hit.ellipse.height);
		return 5;
	case COLLIDER_TYPE_RECTANGLE:
		lua_pushstring(L, "rectangle");
		lua_pushnumber(L, o->hit.rectangle.x);
		lua_pushnumber(L, o->hit.rectangle.y);
		lua_pushnumber(L, o->hit.rectangle.width);
		lua_pushnumber(L, o->hit.rectangle.height);
		return 5;
	case COLLIDER_TYPE_ROUNDED_RECTANGLE:
		lua_pushstring(L, "rounded-rectangle");
		lua_pushnumber(L, o->hit.rounded_rectangle.x);
		lua_pushnumber(L, o->hit.rounded_rectangle.y);
		lua_pushnumber(L, o->hit.rounded_rectangle.width);
		lua_pushnumber(L, o->hit.rounded_rectangle.height);
		lua_pushnumber(L, o->hit.rounded_rectangle.radius);
		return 6;
	case COLLIDER_TYPE_POLYGON:
		lua_pushstring(L, "polygon");
		lua_newtable(L);
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			for(i = 0; i < o->hit.polygon.length; i++)
			{
				lua_pushnumber(L, o->hit.polygon.points[i]);
				lua_rawseti(L, -2, i + 1);
			}
		}
		return 2;
	default:
		break;
	}
	return 0;
}

static int m_set_visible(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->visible = lua_toboolean(L, 2);
	return 0;
}

static int m_get_visible(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, o->visible);
	return 1;
}

static int m_set_touchable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->touchable = lua_toboolean(L, 2);
	return 0;
}

static int m_get_touchable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, o->touchable);
	return 1;
}

static int m_set_layoutable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layoutable = lua_toboolean(L, 2);
	dobject_layout((o->parent && o->layoutable) ? o->parent : o);
	return 0;
}

static int m_get_layoutable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, o->layoutable);
	return 1;
}

static int m_global_to_local(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double nx, x = luaL_checknumber(L, 2);
	double ny, y = luaL_checknumber(L, 3);
	cairo_matrix_t * m = dobject_global_matrix(o);
	double id = 1.0 / (m->xx * m->yy - m->xy * m->yx);
	nx = ((x - m->x0) * m->yy + (m->y0 - y) * m->xy) * id;
	ny = ((y - m->y0) * m->xx + (m->x0 - x) * m->yx) * id;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static int m_local_to_global(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double nx, x = luaL_checknumber(L, 2);
	double ny, y = luaL_checknumber(L, 3);
	cairo_matrix_t * m = dobject_global_matrix(o);
	nx = m->xx * x + m->xy * y + m->x0;
	ny = m->yx * x + m->yy * y + m->y0;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static inline int circle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double r = o->hit.circle.radius;
	double r2, dx, dy;
	if(r > 0)
	{
		r2 = r * r;
		dx = o->hit.circle.x - x;
		dy = o->hit.circle.y - y;
		dx *= dx;
		dy *= dy;
		return (dx + dy <= r2) ? 1 : 0;
	}
	return 0;
}

static inline int ellipse_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.ellipse.width;
	double h = o->hit.ellipse.height;
	double normx, normy;
	if((w > 0) && (h > 0))
	{
		normx = (x - o->hit.ellipse.x) / w;
		normy = (y - o->hit.ellipse.y) / h;
		normx *= normx;
		normy *= normy;
		return (normx + normy <= 1) ? 1 : 0;
	}
	return 0;
}

static inline int rectangle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.rectangle.width;
	double h = o->hit.rectangle.height;
	double rx = o->hit.rectangle.x;
	double ry = o->hit.rectangle.y;
	if((w > 0) && (h > 0))
	{
		if((x >= rx) && (x < rx + w) && (y >= ry) && (y < ry + h))
			return 1;
	}
	return 0;
}

static inline int rounded_rectangle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.rounded_rectangle.width;
	double h = o->hit.rounded_rectangle.height;
	double rx = o->hit.rounded_rectangle.x;
	double ry = o->hit.rounded_rectangle.y;
	double r = o->hit.rounded_rectangle.radius;
	double r2, dx, dy;
	if((w > 0) && (h > 0))
	{
		if((x >= rx) && (x <= rx + w))
		{
			if((y >= ry) && (y <= ry + h))
			{
				if(((y >= ry + r) && (y <= ry + h - r)) || ((x >= rx + r) && (x <= rx + w - r)))
					return 1;
				dx = x - (rx + r);
				dy = y - (ry + r);
				r2 = r * r;
				if(dx * dx + dy * dy <= r2)
					return 1;
				dx = x - (rx + w - r);
				if(dx * dx + dy * dy <= r2)
					return 1;
				dy = y - (ry + h - r);
				if(dx * dx + dy * dy <= r2)
					return 1;
				dx = x - (rx + r);
				if(dx * dx + dy * dy <= r2)
					return 1;
			}
		}
	}
	return 0;
}

static inline int polygon_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double * p = o->hit.polygon.points;
	int n = o->hit.polygon.length / 2;
	int c = 0;
	int i, j;
	for(i = 0, j = n - 1; i < n; j = i++)
	{
		if(((p[(i << 1) + 1] > y) != (p[(j << 1) + 1] > y)) && (x < (p[j << 1] - p[i << 1]) * (y - p[(i << 1) + 1]) / (p[(j << 1) + 1] - p[(i << 1) + 1]) + p[i << 1]))
			c = !c;
	}
	return c;
}

static int m_hit_test_point(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	int hit = 0;
	if(o->visible && o->touchable)
	{
		double nx, x = luaL_checknumber(L, 2);
		double ny, y = luaL_checknumber(L, 3);
		cairo_matrix_t * m = dobject_global_matrix(o);
		double id = 1.0 / (m->xx * m->yy - m->xy * m->yx);
		nx = ((x - m->x0) * m->yy + (m->y0 - y) * m->xy) * id;
		ny = ((y - m->y0) * m->xx + (m->x0 - x) * m->yx) * id;
		switch(o->ctype)
		{
		case COLLIDER_TYPE_NONE:
			if((nx >= 0) && (nx < o->width) && (ny >= 0) && (ny < o->height))
				hit = 1;
			break;
		case COLLIDER_TYPE_CIRCLE:
			hit = circle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_ELLIPSE:
			hit = ellipse_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_RECTANGLE:
			hit = rectangle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_ROUNDED_RECTANGLE:
			hit = rounded_rectangle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_POLYGON:
			hit = polygon_hit_test_point(o, nx, ny);
			break;
		default:
			break;
		}
	}
	lua_pushboolean(L, hit);
	return 1;
}

static int m_bounds(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double x1 = 0;
	double y1 = 0;
	double x2 = o->width;
	double y2 = o->height;
	_cairo_matrix_transform_bounding_box(dobject_global_matrix(o), &x1, &y1, &x2, &y2, NULL);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2 - x1);
	lua_pushnumber(L, y2 - y1);
	return 4;
}

static int m_draw(lua_State * L)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->visible)
	{
		o->draw(L, o);
		if(disp->showobj)
		{
			cairo_t * cr = disp->cr;
			cairo_save(cr);
			cairo_set_matrix(cr, dobject_global_matrix(o));
			cairo_set_line_width(cr, 1);
			cairo_rectangle(cr, 0, 0, o->width, o->height);
			cairo_set_source_rgba(cr, 1, 0, 0, 0.6);
			cairo_stroke(cr);
			if((o->ctype != COLLIDER_TYPE_NONE) && o->touchable)
			{
				cairo_matrix_t m;
				double x, y;
				double w, h;
				double r;
				double * p;
				int n, i;

				switch(o->ctype)
				{
				case COLLIDER_TYPE_CIRCLE:
					cairo_new_sub_path(cr);
					cairo_move_to(cr, o->hit.circle.x + o->hit.circle.radius, o->hit.circle.y);
					cairo_arc(cr, o->hit.circle.x, o->hit.circle.y, o->hit.circle.radius, 0, M_PI * 2);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_ELLIPSE:
					x = o->hit.ellipse.x;
					y = o->hit.ellipse.y;
					w = o->hit.ellipse.width;
					h = o->hit.ellipse.height;
					cairo_get_matrix(cr, &m);
					cairo_translate(cr, x, y);
					cairo_scale(cr, 1, h / w);
					cairo_translate(cr, -x, -y);
					cairo_new_sub_path(cr);
					cairo_move_to(cr, x + w, y);
					cairo_arc(cr, x, y, w, 0, M_PI * 2);
					cairo_close_path(cr);
					cairo_set_matrix(cr, &m);
					break;
				case COLLIDER_TYPE_RECTANGLE:
					cairo_new_sub_path(cr);
					cairo_rectangle(cr, o->hit.rectangle.x, o->hit.rectangle.y, o->hit.rectangle.width, o->hit.rectangle.height);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_ROUNDED_RECTANGLE:
					x = o->hit.rounded_rectangle.x;
					y = o->hit.rounded_rectangle.y;
					w = o->hit.rounded_rectangle.width;
					h = o->hit.rounded_rectangle.height;
					r = o->hit.rounded_rectangle.radius;
					cairo_new_sub_path(cr);
					cairo_move_to(cr, x + r, y);
					cairo_line_to(cr, x + w - r, y);
					cairo_arc(cr, x + w - r, y + r, r, - M_PI / 2, 0);
					cairo_line_to(cr, x + w, y + h - r);
					cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
					cairo_line_to(cr, x + r, y + h);
					cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
					cairo_arc(cr, x + r, y + r, r, M_PI, M_PI + M_PI / 2);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_POLYGON:
					p = o->hit.polygon.points;
					n = o->hit.polygon.length / 2;
					if(n > 0)
					{
						cairo_new_sub_path(cr);
						cairo_move_to(cr, p[0], p[1]);
						for(i = 1; i < n; i++)
							cairo_line_to(cr, p[i << 1], p[(i << 1) + 1]);
						cairo_close_path(cr);
					}
					break;
				default:
					break;
				}
				cairo_set_source_rgba(cr, 1, 1, 0, 0.6);
				cairo_fill(cr);
			}
			cairo_restore(cr);
		}
	}
	return 0;
}

static const luaL_Reg m_dobject[] = {
	{"__gc",						m_dobject_gc},
	{"addChild",					m_add_child},
	{"removeChild",					m_remove_child},
	{"toFront",						m_to_front},
	{"toBack",						m_to_back},
	{"setWidth",					m_set_width},
	{"getWidth",					m_get_width},
	{"setHeight",					m_set_height},
	{"getHeight",					m_get_height},
	{"setSize",						m_set_size},
	{"getSize",						m_get_size},
	{"setX",						m_set_x},
	{"getX",						m_get_x},
	{"setY",						m_set_y},
	{"getY",						m_get_y},
	{"setPosition",					m_set_position},
	{"getPosition",					m_get_position},
	{"setRotation",					m_set_rotation},
	{"getRotation",					m_get_rotation},
	{"setScaleX",					m_set_scale_x},
	{"getScaleX",					m_get_scale_x},
	{"setScaleY",					m_set_scale_y},
	{"getScaleY",					m_get_scale_y},
	{"setScale",					m_set_scale},
	{"getScale",					m_get_scale},
	{"setSkewX",					m_set_skew_x},
	{"getSkewX",					m_get_skew_x},
	{"setSkewY",					m_set_skew_y},
	{"getSkewY",					m_get_skew_y},
	{"setSkew",						m_set_skew},
	{"getSkew",						m_get_skew},
	{"setAnchor",					m_set_archor},
	{"getAnchor",					m_get_archor},
	{"setAlpha",					m_set_alpha},
	{"getAlpha",					m_get_alpha},
	{"setMargin",					m_set_margin},
	{"getMargin",					m_get_margin},
	{"setLayoutDirection",			m_set_layout_direction},
	{"getLayoutDirection",			m_get_layout_direction},
	{"setLayoutJustify",			m_set_layout_justify},
	{"getLayoutJustify",			m_get_layout_justify},
	{"setLayoutAlign",				m_set_layout_align},
	{"getLayoutAlign",				m_get_layout_align},
	{"setLayoutAlignSelf",			m_set_layout_align_self},
	{"getLayoutAlignSelf",			m_get_layout_align_self},
	{"setLayoutGrow",				m_set_layout_grow},
	{"getLayoutGrow",				m_get_layout_grow},
	{"setLayoutShrink",				m_set_layout_shrink},
	{"getLayoutShrink",				m_get_layout_shrink},
	{"setLayoutBasis",				m_set_layout_basis},
	{"getLayoutBasis",				m_get_layout_basis},
	{"setBackgroundColor",			m_set_background_color},
	{"getBackgroundColor",			m_get_background_color},
	{"setBackgroundBorderWidth",	m_set_background_border_width},
	{"getBackgroundBorderWidth",	m_get_background_border_width},
	{"setBackgroundBorderRadius",	m_set_background_border_radius},
	{"getBackgroundBorderRadius",	m_get_background_border_radius},
	{"setBackgroundBorderColor",	m_set_background_border_color},
	{"getBackgroundBorderColor",	m_get_background_border_color},
	{"setCollider",					m_set_collider},
	{"getCollider",					m_get_collider},
	{"setVisible",					m_set_visible},
	{"getVisible",					m_get_visible},
	{"setTouchable",				m_set_touchable},
	{"getTouchable",				m_get_touchable},
	{"setLayoutable",				m_set_layoutable},
	{"getLayoutable",				m_get_layoutable},
	{"globalToLocal",				m_global_to_local},
	{"localToGlobal",				m_local_to_global},
	{"hitTestPoint",				m_hit_test_point},
	{"bounds",						m_bounds},
	{"draw",						m_draw},
	{NULL, NULL}
};

int luaopen_dobject(lua_State * L)
{
	luaL_newlib(L, l_dobject);
	luahelper_create_metatable(L, MT_DOBJECT, m_dobject);
	return 1;
}
