CC=5c
LD=5l
O=5

$O.lcdfs: lcdfs.$O lcd.$O
	$LD -o $O.lcdfs lcdfs.$O lcd.$O

lcdfs.$O: lcdfs.c lcd.h
	$CC -FVw lcdfs.c

lcd.$O: lcd.c lcd.h
	$CC -FVw lcd.c
