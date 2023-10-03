#define PATH "/dev/gpio"



#define PIN_E 	"15"
#define PIN_RS 	"18"
#define PIN_D4	"17"
#define PIN_D5	"4"
#define PIN_D6 	"3"
#define PIN_D7	"2"

#define CTL 	PATH "/ctl"
#define E 		PATH "/" PIN_E
#define RS 		PATH "/" PIN_RS
#define D4		PATH "/" PIN_D4
#define D5		PATH "/" PIN_D5
#define D6		PATH "/" PIN_D6
#define D7		PATH "/" PIN_D7

#define LOW 	"0"
#define HIGH 	"1"

enum {
	LEFT,
	RIGHT
};

enum {
	LOWER,
	UPPER
};

void ini(void);
void clean(void);
void home(void);
void mv(int dir);
void line(int n);
void cmd(char c);
void dat(char d);
void put(char *str);
void wr(int rs, char hex);
void set(int d7, int d6, int d5, int d4);
void snd(void);
void pin_wr(char *pin, int lvl);