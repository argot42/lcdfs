#include <u.h>
#include <libc.h>

#include "lcd.h"

void
ini()
{
	int fd = open(CTL, OWRITE);
	if (fd < 0)
		sysfatal("open ctl: %r");

	char *func = "function out %s";

	fprint(fd, func, PIN_E);
	fprint(fd, func, PIN_RS);
	fprint(fd, func, PIN_D4);
	fprint(fd, func, PIN_D5);
	fprint(fd, func, PIN_D6);
	fprint(fd, func, PIN_D7);
	close(fd);

	sleep(100);

	pin_wr(RS, 0);
	set(0, 0, 1, 1);
	snd();
	sleep(.005);

	pin_wr(RS, 0);
	set(0, 0, 1, 1);
	snd();
	sleep(.1);

	pin_wr(RS, 0);
	set(0, 0, 1, 1);
	snd();
	sleep(.1);

	pin_wr(RS, 0);
	set(0, 0, 1, 0);
	snd();
	sleep(.1);

	cmd(0x28);
	sleep(.053);

	cmd(0x01);
	sleep(3);

	cmd(0x0C);
	sleep(.053);
	
	cmd(0x06);
	sleep(.053);
	
	cmd(0x80);
	sleep(.053);
}

void
clean()
{
	set(0, 0, 0, 0);
}

void
home()
{
	cmd(0x02);
	sleep(15.20);
}

void
mv(int dir)
{
	if (dir == RIGHT)
		cmd(0x14);
	else
		cmd(0x10);
	sleep(.37);
}

void
line(int n)
{
	int i;

	home();

	if (n == UPPER)
		return;

	for (i = 0; i < 40; i++)
		mv(RIGHT);
}

void
cmd(char c)
{
	wr(0, c);
}

void
dat(char d)
{
	wr(1, d);
	sleep(.37);
}

void
put(char *str)
{
	int i;

	for (i = 0; i < 16; i++) {
		if (str[i] == 0)
			break;
		dat(str[i]);
	}
}

void
wr(int rs, char hex)
{
	pin_wr(RS, rs);
	set(
        ((hex & 128) >> 7),
        ((hex & 64) >> 6),
        ((hex & 32) >> 5),
        ((hex & 16) >> 4)
    );
    snd();

    set(
        ((hex & 8) >> 3),
        ((hex & 4) >> 2),
        ((hex & 2) >> 1),
        ((hex & 1) >> 0)
    );
    snd();
}

void
set(int d7, int d6, int d5, int d4)
{
	pin_wr(D7, d7);
	pin_wr(D6, d6);
	pin_wr(D5, d5);
	pin_wr(D4, d4); 
}

void
snd()
{
	pin_wr(E, 1);
	pin_wr(E, 0);
}

void
pin_wr(char *pin, int lvl)
{
	int fd = open(pin, OWRITE);
	if (fd < 0)
		sysfatal("open: %r");

	switch(lvl) {
	case 1:
		if (write(fd, HIGH, 1) != 1)
			sysfatal("setting pin to high: %r");
		break;
	case 0:
		if (write(fd, LOW, 1) != 1)
			sysfatal("setting pin to low: %r");
		break;
	}

	close(fd);
}
