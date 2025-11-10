/* See LICENSE for licence details. */
/* function for csi sequence */
bool insert_blank(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir == INPUT)
		return false;
	int i, num = sum(parm);
	
	if (num <= 0)
		num = 1;

	for (i = term->cols - 1; term->cursor.x <= i; i--) {
		if (term->cursor.x <= (i - num))
			copy_cell(term, term->cursor.y, i, term->cursor.y, i - num);
		else
			erase_cell(term, term->cursor.y, i);
	}
	return false;
}

bool curs_up(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	int num = (parm->argc <=0) ? 1 : dec2num(parm->argv[0]);
	if(parm->argc == 2 && io_dir == INPUT)
	{
		if(dec2num(parm->argv[1]) == CTRL_MOD)
		{
			scroll_view(term, -1 * num);
			return true;
		}	
	}
	else if(io_dir == OUTPUT)
	{
		move_cursor(term, -num, 0);
	}
	return false;
}

bool curs_down(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	int num = (parm->argc <=0) ? 1 : dec2num(parm->argv[0]);
	if(parm->argc == 2 && io_dir == INPUT)
	{
		if(dec2num(parm->argv[1]) == CTRL_MOD)
		{
			scroll_view(term,  num);
			return true;
		}	
	}
	else if(io_dir == OUTPUT)
	{
		move_cursor(term, num, 0);
	}
	return false;
}

bool curs_forward(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num = sum(parm);

	if (num <= 0)
		num = 1;

	move_cursor(term, 0, num);
	return false;
}

bool curs_back(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num = sum(parm);

	if (num <= 0)
		num = 1;

	move_cursor(term, 0, -num);
	return false;
}

bool curs_nl(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num = sum(parm);

	if (num <= 0)
		num = 1;

	move_cursor(term, num, 0);
	cr(term, io_dir);
	return false;
}

bool curs_pl(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;
	
	int num = sum(parm);

	if (num <= 0)
		num = 1;

	move_cursor(term, -num, 0);
	cr(term, io_dir);
	return false;
}

bool curs_col(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num;

	num = (parm->argc <= 0) ? 0: dec2num(parm->argv[parm->argc - 1]) - 1;
	set_cursor(term, term->cursor.y, num);
	return false;
}

bool curs_pos(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int line, col;

	if (parm->argc <= 0) {
		line = col = 0;
	} else if (parm->argc == 2) {
		line = dec2num(parm->argv[0]) - 1;
		col  = dec2num(parm->argv[1]) - 1;
	} else {
		return false;
	}

	if (line < 0)
		line = 0;
	if (col < 0)
		col = 0;

	set_cursor(term, line, col);
	return false;
}

bool curs_line(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num;

	num = (parm->argc <= 0) ? 0: dec2num(parm->argv[parm->argc - 1]) - 1;
	set_cursor(term, num, term->cursor.x);
	return false;
}

bool erase_display(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, j, mode;

	mode = (parm->argc <= 0) ? 0: dec2num(parm->argv[parm->argc - 1]);

	if (mode < 0 || 2 < mode)
		return false;

	if (mode == 0) {
		for (i = term->cursor.y; i < term->lines; i++)
			for (j = 0; j < term->cols; j++)
				if (i > term->cursor.y || (i == term->cursor.y && j >= term->cursor.x))
					erase_cell(term, i, j);
	} else if (mode == 1) {
		for (i = 0; i <= term->cursor.y; i++)
			for (j = 0; j < term->cols; j++)
				if (i < term->cursor.y || (i == term->cursor.y && j <= term->cursor.x))
					erase_cell(term, i, j);
	} else if (mode == 2) {
		for (i = 0; i < term->lines; i++)
			for (j = 0; j < term->cols; j++)
				erase_cell(term, i, j);
	}
	return false;
}

bool erase_line(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, mode;

	mode = (parm->argc <= 0) ? 0: dec2num(parm->argv[parm->argc - 1]);

	if (mode < 0 || 2 < mode)
		return false;

	if (mode == 0) {
		for (i = term->cursor.x; i < term->cols; i++)
			erase_cell(term, term->cursor.y, i);
	} else if (mode == 1) {
		for (i = 0; i <= term->cursor.x; i++)
			erase_cell(term, term->cursor.y, i);
	} else if (mode == 2) {
		for (i = 0; i < term->cols; i++)
			erase_cell(term, term->cursor.y, i);
	}
	return false;
}

bool insert_line(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num = sum(parm);

	if (term->mode & MODE_ORIGIN) {
		if (term->cursor.y < term->scroll.top
			|| term->cursor.y > term->scroll.bottom)
			return false;
	}

	if (num <= 0)
		num = 1;

	scroll(term, term->cursor.y, term->scroll.bottom, -num);
	return false;
}

bool delete_line(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int num = sum(parm);

	if (term->mode & MODE_ORIGIN) {
		if (term->cursor.y < term->scroll.top
			|| term->cursor.y > term->scroll.bottom)
			return false;
	}

	if (num <= 0)
		num = 1;

	scroll(term, term->cursor.y, term->scroll.bottom, num);
	return false;
}

bool delete_char(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, num = sum(parm);

	if (num <= 0)
		num = 1;

	for (i = term->cursor.x; i < term->cols; i++) {
		if ((i + num) < term->cols)
			copy_cell(term, term->cursor.y, i, term->cursor.y, i + num);
		else
			erase_cell(term, term->cursor.y, i);
	}
	return false;
}

bool erase_char(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, num = sum(parm);

	if (num <= 0)
		num = 1;
	else if (num + term->cursor.x > term->cols)
		num = term->cols - term->cursor.x;

	for (i = term->cursor.x; i < term->cursor.x + num; i++)
		erase_cell(term, term->cursor.y, i);
	return false;
}

uint8_t rgb2index(uint8_t r, uint8_t g, uint8_t b)
{
	/* SGR: Set Graphic Rendition (special case)
	 * 	special color selection (from 256color index or as a 24bit color value)
	 *
	 *	ESC [ 38 ; 5 ; Ps m
	 *	ESC [ 48 ; 5 ; Ps m
	 *		select foreground/background color from 256 color index
	 *
	 *	ESC [ 38 ; 2 ; r ; g ; b m
	 *	ESC [ 48 ; 2 ; r ; g ; b m
	 *		select foreground/background color as a 24bit color value
	 *
	 * 	according to ITU T.416 (https://www.itu.int/rec/T-REC-T.416/en)
	 * 	this format is valid (but most of terminals don't support)
	 *	ESC [ 38 : 5 : Ps m
	 *	ESC [ 48 : 5 : Ps m
	 *
	 *	ESC [ 48 : 2 : r : g : b m
	 *	ESC [ 38 : 2 : r : g : b m
	 */
	int padding;
	uint8_t index;

	/* XXX: this calculation fails if color palette modified by OSC 4 */
	/* 256 color palette is defined in color.h */

	if (r == g && g == b) {
		/*
			search from grayscale
				index: 232 - 255
				value: 0x080808 - 0xEEEEEE
				inc  : 0x0A
		*/
		padding = (r - 0x08) / 0x0A;

		if (padding >= 24)
			/* index 231 (0xFFFFFF) is the last color of 6x6x6 cube */
			index = 231;
		else if (padding <= 0)
			index = 232;
		else
			index = 232 + padding;

	} else {
		/*
			search from 6x6x6 color cube
				index: 16 - 231
				each rgb takes 6 values: {0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF}
		*/
		const uint8_t values[] = {0x0, 0x5F, 0x87, 0xAF, 0xD7, 0xFF};
		const uint8_t rgb[] = {r, g, b};
		uint8_t closest_index[3];
		int small, big;

		for (int c = 0; c < 3; c++) {
			for (int i = 0; i < 5; i++) {
				if (values[i] <= rgb[c] && rgb[c] <= values[i + 1]) {
					small = abs(rgb[c] - values[i]);
					big   = abs(rgb[c] - values[i + 1]);
					if (small < big)
						closest_index[c] = i;
					else
						closest_index[c] = i + 1;
				}
			}
		}
		index = 16 + closest_index[0] * 36 + closest_index[1] * 6 + closest_index[2];
	}

	return index;
}

bool set_attr(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	/* SGR: Set Graphic Rendition
	 * 	ESC [ Pm m
	 * 	Pm:
	 *		0: reset all attribute
	 *
	 *		1: brighten foreground color (only work for color index 0-7)
	 *		4: underline
	 *		5: brighten background color (only work for color index 0-7)
	 *		7: reverse (swap foreground/background color)
	 *
	 *		21: reset brighten foreground color
	 *		24: reset underline
	 *		25: reset brighten background color
	 *		27: reset reverse
	 *
	 *		30-37: select foreground color (color index 0-7)
	 *		38: special foreground color selection: implemented in select_color_value()
	 *
	 *		40-47: select background color (color index 0-7)
	 *		48: special foreground color selection: implemented in select_color_value()
	 */
	int i, num;

	if (parm->argc <= 0) {
		term->attribute     = ATTR_RESET;
		term->color_pair.fg = DEFAULT_FG;
		term->color_pair.bg = DEFAULT_BG;
		return false ;
	}

	for (i = 0; i < parm->argc; i++) {
		num = dec2num(parm->argv[i]);
		//logging(DEBUG, "argc:%d num:%d\n", parm->argc, num);

		if (num == 0) {                        /* reset all attribute and color */
			term->attribute = ATTR_RESET;
			term->color_pair.fg = DEFAULT_FG;
			term->color_pair.bg = DEFAULT_BG;
		} else if (1 <= num && num <= 7) {     /* set attribute */
			term->attribute |= attr_mask[num];
		} else if (21 <= num && num <= 27) {   /* reset attribute */
			term->attribute &= ~attr_mask[num - 20];
		} else if (30 <= num && num <= 37) {   /* set foreground */
			term->color_pair.fg = (num - 30);
		} else if (num == 38) {                /* special foreground color selection */
			/* select foreground color from 256 color index */
			if ((i + 2) < parm->argc && dec2num(parm->argv[i + 1]) == 5) {
				term->color_pair.fg = dec2num(parm->argv[i + 2]);
				i += 2;
			/* select foreground color from specified rgb color */
			} else if ((i + 4) < parm->argc && dec2num(parm->argv[i + 1]) == 2) {
				term->color_pair.fg = rgb2index(dec2num(parm->argv[i + 2]),
					dec2num(parm->argv[i + 3]), dec2num(parm->argv[i + 4]));
				i += 4;
			}
		} else if (num == 39) {                /* reset foreground */
			term->color_pair.fg = DEFAULT_FG;
		} else if (40 <= num && num <= 47) {   /* set background */
			term->color_pair.bg = (num - 40);
		} else if (num == 48) {                /* special background  color selection */
			/* select background color from 256 color index */
			if ((i + 2) < parm->argc && dec2num(parm->argv[i + 1]) == 5) {
				term->color_pair.bg = dec2num(parm->argv[i + 2]);
				i += 2;
			/* select background color from specified rgb color */
			} else if ((i + 4) < parm->argc && dec2num(parm->argv[i + 1]) == 2) {
				term->color_pair.bg = rgb2index(dec2num(parm->argv[i + 2]),
					dec2num(parm->argv[i + 3]), dec2num(parm->argv[i + 4]));
				i += 4;
			}
		} else if (num == 49) {                /* reset background */
			term->color_pair.bg = DEFAULT_BG;
		} else if (90 <= num && num <= 97) {   /* set bright foreground */
			term->color_pair.fg = (num - 90) + BRIGHT_INC;
		} else if (100 <= num && num <= 107) { /* set bright background */
			term->color_pair.bg = (num - 100) + BRIGHT_INC;
		}
	}
	return false;
}

bool status_report(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, num;
	char buf[BUFSIZE];

	for (i = 0; i < parm->argc; i++) {
		num = dec2num(parm->argv[i]);
		if (num == 5) {         /* terminal response: ready */
			ewrite(term->fd, "\033[0n", 4);
		} else if (num == 6) {  /* cursor position report */
			snprintf(buf, BUFSIZE, "\033[%d;%dR", term->cursor.y + 1, term->cursor.x + 1);
			ewrite(term->fd, buf, strlen(buf));
		} else if (num == 15) { /* terminal response: printer not connected */
			ewrite(term->fd, "\033[?13n", 6);
		}
	}
	return false;
}

bool device_attribute(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	/* TODO: refer VT525 DA */
	(void) parm;
	ewrite(term->fd, "\033[?6c", 5); /* "I am a VT102" */
	return false;
}

bool set_mode(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, mode;
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;

	for (i = 0; i < parm->argc; i++) {
		mode = dec2num(parm->argv[i]);
		if (*(esc->buf + 1) != '?')
			continue; /* not supported */

		if (mode == 6) { /* private mode */
			term->mode |= MODE_ORIGIN;
			set_cursor(term, 0, 0);
		} else if (mode == 7) {
			term->mode |= MODE_AMRIGHT;
		} else if (mode == 25) {
			term->mode |= MODE_CURSOR;
		} else if (mode == 8901) {
			term->mode |= MODE_VWBS;
		}
	}
	return false;
}

bool reset_mode(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, mode;
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;

	for (i = 0; i < parm->argc; i++) {
		mode = dec2num(parm->argv[i]);
		if (*(esc->buf + 1) != '?')
			continue; /* not supported */

		if (mode == 6) { /* private mode */
			term->mode &= ~MODE_ORIGIN;
			set_cursor(term, 0, 0);
		} else if (mode == 7) {
			term->mode &= ~MODE_AMRIGHT;
			term->wrap_occurred = false;
		} else if (mode == 25) {
			term->mode &= ~MODE_CURSOR;
		} else if (mode == 8901) {
			term->mode &= ~MODE_VWBS;
		}
	}
	return false;
}

bool set_margin(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int top, bottom;

	if (parm->argc <= 0) {        /* CSI r */
		top    = 0;
		bottom = term->lines - 1;
	} else if (parm->argc == 2) { /* CSI ; r -> use default value */
		top    = (parm->argv[0] == NULL) ? 0: dec2num(parm->argv[0]) - 1;
		bottom = (parm->argv[1] == NULL) ? term->lines - 1: dec2num(parm->argv[1]) - 1;
	} else {
		return false;
	}

	if (top < 0 || top >= term->lines)
		top = 0;
	if (bottom < 0 || bottom >= term->lines)
		bottom = term->lines - 1;

	if (top >= bottom)
		return false;

	term->scroll.top = top;
	term->scroll.bottom = bottom;

	set_cursor(term, 0, 0); /* move cursor to home */
	return false;
}

bool clear_tabstop(struct terminal_t *term, struct parm_t *parm, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	int i, j, num;

	if (parm->argc <= 0) {
		term->tabstop[term->cursor.x] = false;
	} else {
		for (i = 0; i < parm->argc; i++) {
			num = dec2num(parm->argv[i]);
			if (num == 0) {
				term->tabstop[term->cursor.x] = false;
			} else if (num == 3) {
				for (j = 0; j < term->cols; j++)
					term->tabstop[j] = false;
				return false;
			}
		}
	}
	return false;
}
