/* Logic Analyser (8 channels)
 * Xeon 2016
 *
 * gcc logicanalyser.c -o logicanalyser -lftdi -lncurses
 * 
 */

#include <stdio.h>
#include <string.h>
#include <ftdi.h>
#include <curses.h>

#define FTDI_VID 0x0403
#define FTDI_PID 0x6001

#define SAMPLERATE 50 /* samples per second  */

#define NOPINS 	0x00
#define PIN_TX(s) (s & 0x1)
#define PIN_RX(s) ((s >> 1) & 0x1)
#define PIN_RTS(s) ((s >> 2) & 0x1)
#define PIN_CTS(s) ((s >> 3) & 0x1)
#define PIN_DTR(s) ((s >> 4) & 0x1)
#define PIN_DSR(s) ((s >> 5) & 0x1)
#define PIN_DCD(s) ((s >> 6) & 0x1)
#define PIN_RI(s) ((s >> 7) & 0x1)
#define ALLPINS	0xFF
#define ON '-'
#define OFF '_'

struct ftdi_context ftdic;

void ftdi_check()
{
	printf("\nFTDI context: %p\n", &ftdic);
	printf("ftdi chip type:%d\n", ftdic.type);
	printf("baudrate: %d\n", ftdic.baudrate);
	printf("readbuffer: %d\n", *ftdic.readbuffer);
	printf("readbuffer offset: %d\n", ftdic.readbuffer_offset);
	printf("readbuffer chunksize: %d\n", ftdic.readbuffer_chunksize);
	printf("readbuffer remaining: %d\n", ftdic.readbuffer_remaining);
	printf("write buffer chunksize: %d\n", ftdic.writebuffer_chunksize);
	printf("max_packet_size: %d\n", ftdic.max_packet_size);
}

void ftdi_fatal (char *str)
{
	fprintf(stderr, "%s: %s\n", str, ftdi_get_error_string(&ftdic));
	//ftdi_usb_close(&ftdic);
	//ftdi_free(&ftdic);
	exit(-1);
}

int interface()
{
	int i;
	char graph0[50], graph1[50],graph2[50], graph3[50],
	graph4[50], graph5[50], graph6[50], graph7[50], s;

	printf("Running interface...\n");

	/* init graph */
	memset(graph0, ' ', sizeof(graph0));
	memset(graph1, ' ', sizeof(graph1));
	memset(graph2, ' ', sizeof(graph2));
	memset(graph3, ' ', sizeof(graph3));
	memset(graph4, ' ', sizeof(graph4));
	memset(graph5, ' ', sizeof(graph5));
	memset(graph6, ' ', sizeof(graph6));
	memset(graph7, ' ', sizeof(graph7));

	WINDOW * display;

	if((display = initscr()) == NULL)
	{
		fprintf(stderr, "Error initialising ncurses\n");
		exit(-1);
	}

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
	init_pair(6, COLOR_MAGENTA, COLOR_BLACK);

	mvprintw(0, 0, "LOGIC ANALYSER!");
	mvprintw(2, 0, "PINOUT:");
	attron(COLOR_PAIR(1));
	mvprintw(4, 0, "Channel 1  (PIN_TX)|");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	mvprintw(5, 0, "Channel 2  (PIN_RX)|");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(3));
	mvprintw(6, 0, "Channel 3 (PIN_RTS)|");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(4));
	mvprintw(7, 0, "Channel 4 (PIN_CTS)|");
	attroff(COLOR_PAIR(4));
	attron(COLOR_PAIR(5));
	mvprintw(8, 0, "Channel 5 (PIN_DTR)|");
	attroff(COLOR_PAIR(5));
	attron(COLOR_PAIR(6));
	mvprintw(9, 0, "Channel 6 (PIN_DSR)|");
	attroff(COLOR_PAIR(6));
	attron(COLOR_PAIR(1));
	mvprintw(10, 0, "Channel 7 (PIN_DCD)|");
	attron(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	mvprintw(11, 0, "Channel 8  (PIN_RI)|");
	attroff(COLOR_PAIR(2));

	while(1)
	{
		/* poll pins */
		if(ftdi_read_pins(&ftdic, &s) < 0)
			ftdi_fatal("Unable to read pins");

		/* display pin output */
		mvaddch(2, 8, PIN_TX(s) | 0x30);
		mvaddch(2, 9, PIN_RX(s) | 0x30);
		mvaddch(2, 10, PIN_RTS(s) | 0x30);
		mvaddch(2, 11, PIN_CTS(s) | 0x30);
		mvaddch(2, 12, PIN_DTR(s) | 0x30);
		mvaddch(2, 13, PIN_DSR(s) | 0x30);
		mvaddch(2, 14, PIN_DCD(s) | 0x30);
		mvaddch(2, 15, PIN_RI(s) | 0x30);


		/* upgrade graphs */
		for(i = 0; i < sizeof(graph0); i++)
			graph0[i] = graph0[i + 1];
		graph0[sizeof(graph0) - 1] = PIN_TX(s) ? ON : OFF;

		for(i = 0; i < sizeof(graph1); i++)
			graph1[i] = graph1[i + 1];
		graph1[sizeof(graph1) - 1] = PIN_RX(s) ? ON : OFF;

		for(i = 0; i < sizeof(graph2); i++)
			graph2[i] = graph2[i + 1];
		graph2[sizeof(graph2) - 1] = PIN_RTS(s) ? ON : OFF;

		for(i = 0; i < sizeof(graph3); i++)
			graph3[i] = graph3[i + 1];
		graph3[sizeof(graph3) - 1] = PIN_CTS(s) ? ON : OFF;

		for(i = 0; i < sizeof(graph4); i++)
			graph4[i] = graph4[i + 1];
		graph4[sizeof(graph4) - 1] = PIN_DTR(s) ? ON : OFF;
		
		for(i = 0; i < sizeof(graph5); i++)
			graph5[i] = graph5[i + 1];
		graph5[sizeof(graph5) - 1] = PIN_DSR(s) ? ON : OFF;
		
		for(i = 0; i < sizeof(graph6); i++)
			graph6[i] = graph6[i + 1];
		graph6[sizeof(graph6) - 1] = PIN_DCD(s) ? ON : OFF;

		for(i = 0; i < sizeof(graph7); i++)
			graph7[i] = graph7[i + 1];
		graph7[sizeof(graph7) - 1] = PIN_RI(s) ? ON : OFF;

		/* draw signal graph */
		attron(COLOR_PAIR(1));
		mvaddstr(4, 20, graph0);
		mvaddstr(4, 70, "|                  ");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		mvaddstr(5, 20, graph1);
		mvaddstr(5, 70, "|                  ");
		attroff(COLOR_PAIR(2));
		attron(COLOR_PAIR(3));
		mvaddstr(6, 20, graph2);
		mvaddstr(6, 70, "|                  ");
		attroff(COLOR_PAIR(3));
		attron(COLOR_PAIR(4));
		mvaddstr(7, 20, graph3);
		mvaddstr(7, 70, "|                  ");
		attroff(COLOR_PAIR(4));
		attron(COLOR_PAIR(5));
		mvaddstr(8, 20, graph4);
		mvaddstr(8, 70, "|                  ");
		attroff(COLOR_PAIR(5));
		attron(COLOR_PAIR(6));
		mvaddstr(9, 20, graph5);
		mvaddstr(9, 70, "|                  ");
		attroff(COLOR_PAIR(6));
		attron(COLOR_PAIR(1));
		mvaddstr(10, 20, graph6);
		mvaddstr(10, 70, "|                  ");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		mvaddstr(11, 20, graph7);
		mvaddstr(11, 70, "|                  ");
		attroff(COLOR_PAIR(2));

		refresh();
		usleep(1e6/(double)SAMPLERATE);
	}

	printf("Killing interface...\n");

	delwin(display);
	endwin();
	refresh();

	return 0;
}

int main(int argc, char const *argv[])
{
	/* Initialize context for subsequent function calls */
	printf("Initializing FTDI chip...");
	ftdi_init(&ftdic);
	printf("done.\n");

	/* Open FTDI device based on FT232R vendor & product IDs */
	if(ftdi_usb_open(&ftdic, FTDI_VID, FTDI_PID) < 0)
		ftdi_fatal("Can't open device");

	/* Enable bitbang mode */
	printf("Enabling bitbang mode.\n");
	if(ftdi_set_bitmode(&ftdic, NOPINS, BITMODE_BITBANG) < 0)
		ftdi_fatal("Can't enable bitbang mode");

	if(!ftdic.bitbang_enabled)
		ftdi_fatal("Bitbang mode not enabled");

	printf("Samplerate (sps): %d\n", SAMPLERATE);

//	ftdi_check();

	interface();

	ftdi_usb_close(&ftdic);

	ftdi_deinit(&ftdic);

//	printf("\nDestroying FTDI context: %p\n", &ftdic);
//	ftdi_free(&ftdic);

	return 0;
}
