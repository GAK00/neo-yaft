/* See LICENSE for licence details. */
/* function for control character */
bool bs(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	if (term->mode & MODE_VWBS
		&& term->cursor.x - 1 >= 0
		&& term->cells[term->cursor.y][term->cursor.x - 1].width == NEXT_TO_WIDE)
		move_cursor(term, 0, -2);
	else
		move_cursor(term, 0, -1);
	return false;
}

bool tab(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	for (int i = term->cursor.x + 1; i < term->cols; i++) {
		if (term->tabstop[i]) {
			set_cursor(term, term->cursor.y, i);
			return false;
		}
	}
	set_cursor(term, term->cursor.y, term->cols - 1);
	return false;
}

bool nl(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		move_cursor(term, 1, 0);
	return false;
}

bool cr(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		set_cursor(term, term->cursor.y, 0);
	return false;
}

bool enter_esc(struct terminal_t *term, enum io_direction io_dir)
{
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;
	esc->state = STATE_ESC;
	return false;
}

/* function for escape sequence */
bool save_state(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
	{
		term->state.mode = term->mode & MODE_ORIGIN;
		term->state.cursor = term->cursor;
		term->state.attribute = term->attribute;
	}
	return false;
}

bool restore_state(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	/* restore state */
	if (term->state.mode & MODE_ORIGIN)
		term->mode |= MODE_ORIGIN;
	else
		term->mode &= ~MODE_ORIGIN;
	term->cursor    = term->state.cursor;
	term->attribute = term->state.attribute;
	return false;
}

bool crnl(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir != OUTPUT)
		return false;

	cr(term, io_dir);
	nl(term, io_dir);
	return false;
}

bool set_tabstop(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		term->tabstop[term->cursor.x] = true;
	return false;
}

bool reverse_nl(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		move_cursor(term, -1, 0);
	return false;
}

bool identify(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		ewrite(term->fd, "\033[?6c", 5); /* "I am a VT102" */
	return false;
}

bool enter_csi(struct terminal_t *term, enum io_direction io_dir)
{
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;
	esc->state = STATE_CSI;
	return false;
}

bool enter_osc(struct terminal_t *term, enum io_direction io_dir)
{
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;
	esc->state = STATE_OSC;
	return false;
}	

bool enter_dcs(struct terminal_t *term, enum io_direction io_dir)
{
	struct esc_t * esc = io_dir == OUTPUT ? &term->esc_out : &term->esc_in;
	esc->state = STATE_DCS;
	return false;
}

bool ris(struct terminal_t *term, enum io_direction io_dir)
{
	if(io_dir == OUTPUT)
		reset(term);
	return false;
}
