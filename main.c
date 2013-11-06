#include <curses.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define min(x, y) x<y?x:y
#define max(x, y) x>y?x:y

int maxx, maxy;

void rectangle(int y1, int x1, int y2, int x2)
{
	attron(A_BOLD);				//Attribute ON
	mvhline(y1, x1, 0, x2-x1);
	mvhline(y2, x1, 0, x2-x1);
	mvvline(y1, x1, 0, y2-y1);
	mvvline(y1, x2, 0, y2-y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
	attroff(A_BOLD);			//Attribute OFF
}

int menu()
{
	int ch;
	do
	{
		werase(stdscr);			//clrscr()
		getmaxyx(stdscr, maxy, maxx);	//Macro implementation of getmaxy() & getmaxx() combined.
		rectangle(0, 0, maxy - 1, maxx - 1);
		mvprintw(maxy / 2 - 5, (maxx - 4)/ 2, "MENU");
		mvprintw(maxy / 2 - 3, (maxx - 6)/ 2, "1. FCFS");
		mvprintw(maxy / 2-2, (maxx - 6)/ 2, "2. SSTF");
		mvprintw(maxy / 2 - 1, (maxx - 6)/ 2, "3. SCAN1");
		mvprintw(maxy / 2, (maxx - 6)/ 2, "4. SCAN2");
		mvprintw(maxy / 2 + 1, (maxx - 6)/ 2, "5. Exit");
		mvprintw(maxy / 2 + 3, (maxx - 14)/ 2, "Enter choice: ");
		ch = getch() - 48;		//Yes. getch() works, and returns a value.
	}while(ch < 1 || ch > 5);
	return ch;
}

void fcfs(const int n, int start, const int qs, const int *queue)
{
	int num, step, total;
	float unit;
	maxx = getmaxx(stdscr);
	maxy = getmaxy(stdscr);
	step = n / 10;

	unit = (float)maxx / n;

	werase(stdscr);
	rectangle(1, 0, maxy - 1, maxx - 1);
	num = 1;
	total = 0;
	attron(A_BOLD);				//Enable bold
	mvprintw(0, (maxx - 38)/2, "First Come First Serve Disk Scheduling");
	mvprintw(2, 1, "%d", num);
	num = step;
	while(num<=n)
	{
		mvprintw(2, unit * num, "%d", num);		//Print all the cylinder head entries, at the top, like labelling a graph.
		num += step;
	}
	mvprintw(2, unit * n, "%d", n);
	mvhline(3, 1, 0, maxx - 2);

	attroff(A_BOLD);

	num = 0;
	step = 5;
	mvprintw(step - 1, start * unit, "%d", start);
	while(num < qs)
	{
		if(start < queue[num])				//If moving in the right direction...
		{
			mvhline(step, start * unit, 0, (queue[num] - start + 1) * unit);
			if(num)
			{
				mvaddch(step - 1, start * unit, ACS_VLINE);
				mvaddch(step, start * unit, ACS_LLCORNER);
			}
			mvaddch(step, queue[num]*unit, ACS_URCORNER);
			step++;
			if((num + 1) < qs)		//Print the cylinder numbers beside the lines.
				mvprintw(step, queue[num] * unit - log(queue[num]) + 2, "%d", queue[num]);
		}
		else						//If moving in the left direction...
		{
			mvaddch(step - 1, start * unit, ACS_VLINE);
			mvhline(step, queue[num] * unit, 0, (start - queue[num] + 2) * unit);
			if(num)
			{
				mvaddch(step - 1, start * unit, ACS_VLINE);
				mvaddch(step, start * unit, ACS_LRCORNER);
			}
			mvaddch(step, queue[num]*unit, ACS_ULCORNER);
			step++;

			if((num + 1) < qs)		//Print the cylinder numbers beside the lines.
				mvprintw(step, queue[num] * unit + 1, "%d", queue[num]);
		}
		
		total += abs(start - queue[num]);		//Total head movement
		
		step++;

		start = queue[num];
		num++;
	}
	if(start < queue[num])				//If moving in the right direction...
	{
		mvaddch(step - 2, queue[num - 1] * unit, ACS_LARROW);
		printw(" %d", queue[num - 1]);
	}
	else
	{
		mvaddch(step - 2, start * unit, ACS_RARROW);
		printw(" %d", queue[num - 1]);
	}

	attron(A_BOLD);
	mvprintw(maxy - 2, (maxx - 24)/2, "Total Head Movement: %d", total);
	attroff(A_BOLD);

	noecho();						//Output of the keypress will be supressed.
	getch();
	echo();
}

void sstf(const int n, int start, const int qs, const int *queue)
{
	int num, step, i, diff, selected, *temp, total;
	float unit;

	temp = (int *)malloc(sizeof(int) * qs);
	for(i = 0; i < qs; i++)
		temp[i] = queue[i];

	maxx = getmaxx(stdscr);
	maxy = getmaxy(stdscr);
	
	step = n / 10;

	unit = (float)maxx / n;

	werase(stdscr);
	rectangle(1, 0, maxy - 1, maxx - 1);
	num = 1;
	attron(A_BOLD);
	mvprintw(0, (maxx - 40)/2, "Shortest Seek Time First Disk Scheduling");
	mvprintw(2, 1, "%d", num);
	num = step;
	while(num <= n)
	{
		mvprintw(2, unit * num, "%d", num);
		num += step;
	}
	mvprintw(2, unit * n, "%d", n);
	mvhline(3, 1, 0, maxx - 2);

	attroff(A_BOLD);

	num = 0;
	total = 0;
	step = 5;
	while(num < qs)
	{
		for(i = 0, diff = n; i < qs; i++)
		{
			if(temp[i] == n+1)
				continue;
			if(abs(start - temp[i]) < diff)
			{
				diff = abs(start - temp[i]);
				selected = i;
			}
		}
		temp[selected] = n+1;
		if(start < queue[selected])
			mvhline(step, start * unit, 0, (queue[selected] - start + 1) * unit);
		else
			mvhline(step, queue[selected] * unit, 0, (start - queue[selected] + 1) * unit);
		if(start < queue[selected])
			mvprintw(step, queue[selected]*unit, ">");
		else
			mvprintw(step, queue[selected]*unit, "<");

		step++;
		mvprintw(step, start * unit, "%d", start);
		mvprintw(step, queue[selected] * unit, "%d", queue[selected]);
		step+=2;
		
		total += abs(start - queue[selected]);

		start = queue[selected];
		num++;
	}

	attron(A_BOLD);
	mvprintw(maxy - 2, (maxx - 24)/2, "Total Head Movement: %d", total);
	attroff(A_BOLD);

	noecho();
	getch();
	echo();
}

void scan1(const int n, int start, const int qs, const int *queue)
{
	int num, step, i, diff, sp, *temp, total;
	float unit;

	temp = (int *)malloc(sizeof(int) * qs);
	for(i = 0; i < qs; i++)
		temp[i] = queue[i];

	for(i = 0; i < qs; i++)
	{
		for(diff = i; diff < qs; diff++)
		{
			if(temp[i]>temp[diff])
			{
				step = temp[i];
				temp[i] = temp[diff];
				temp[diff] = step;
			}
		}
	}

	maxx = getmaxx(stdscr);
	maxy = getmaxy(stdscr);
	
	step = n / 10;

	unit = (float)maxx / n;

	werase(stdscr);
	rectangle(1, 0, maxy - 1, maxx - 1);
	num = 1;
	total = 0;
	attron(A_BOLD);
	mvprintw(0, (maxx - 40)/2, "Shortest Seek Time First Disk Scheduling");
	mvprintw(2, 1, "%d", num);
	num = step;
	while(num<=n)
	{
		mvprintw(2, unit * num, "%d", num);
		num += step;
	}
	mvprintw(2, unit * n, "%d", n);
	mvhline(3, 1, 0, maxx - 2);

	attroff(A_BOLD);

	num = 0;
	step = 5;

	for(i = 0; i < qs; i++)
	{
		if(temp[i] > start)
			break;
	}
	sp = i;

	for( i = sp ; i< qs; i++ , step+=2)
	{
		mvhline(step, start * unit, 0, (temp[i] - start + 1) * unit);
		mvprintw(step, temp[i]*unit, ">");
		total += temp[i] - start;

		step++;
		mvprintw(step, start * unit, "%d", start);
		mvprintw(step, temp[i] * unit, "%d", temp[i]);
		start = temp[i];
	}
	sp--;
	for( i = sp ; i >= 0; i-- , step+=2)
	{
		mvhline(step, temp[i] * unit, 0, (start - temp[i] + 1) * unit);
		mvprintw(step, temp[i]*unit, "<");
		total += start - temp[i];

		step++;
		mvprintw(step, start * unit, "%d", start);
		mvprintw(step, temp[i] * unit, "%d", temp[i]);

		start = temp[i];
	}


	attron(A_BOLD);
	mvprintw(maxy - 2, (maxx - 24)/2, "Total Head Movement: %d", total);
	attroff(A_BOLD);

	noecho();
	getch();
	echo();
}

void scan2(const int n, int start, const int qs, const int *queue)
{
	int num, step, i, diff, sp, *temp, total;
	float unit;

	temp = (int *)malloc(sizeof(int) * qs);
	for(i = 0; i < qs; i++)
		temp[i] = queue[i];

	for(i = 0; i < qs; i++)
	{
		for(diff = i; diff < qs; diff++)
		{
			if(temp[i]>temp[diff])
			{
				step = temp[i];
				temp[i] = temp[diff];
				temp[diff] = step;
			}
		}
	}

	maxx = getmaxx(stdscr);
	maxy = getmaxy(stdscr);
	
	step = n / 10;

	unit = (float)maxx / n;

	werase(stdscr);
	rectangle(1, 0, maxy - 1, maxx - 1);
	num = 1;
	total = 0;
	attron(A_BOLD);
	mvprintw(0, (maxx - 40)/2, "Shortest Seek Time First Disk Scheduling");
	mvprintw(2, 1, "%d", num);
	num = step;
	while(num<=n)
	{
		mvprintw(2, unit * num, "%d", num);
		num += step;
	}
	mvprintw(2, unit * n, "%d", n);
	mvhline(3, 1, 0, maxx - 2);

	attroff(A_BOLD);

	num = 0;
	step = 5;

	for(i = 0; i < qs; i++)
	{
		if(temp[i] > start)
			break;
	}
	sp = i - 1;
	
	for( i = sp ; i >= 0; i-- , step+=2)
	{
		mvhline(step, temp[i] * unit, 0, (start - temp[i] + 1) * unit);
		mvprintw(step, temp[i]*unit, "<");
		total += start - temp[i];

		step++;
		mvprintw(step, start * unit, "%d", start);
		mvprintw(step, temp[i] * unit, "%d", temp[i]);

		start = temp[i];
	}
	sp++;
	for( i = sp ; i< qs; i++ , step+=2)
	{
		mvhline(step, start * unit, 0, (temp[i] - start + 1) * unit);
		mvprintw(step, temp[i]*unit, ">");
		total += temp[i] - start;

		step++;
		mvprintw(step, start * unit, "%d", start);
		mvprintw(step, temp[i] * unit, "%d", temp[i]);
		start = temp[i];
	}


	attron(A_BOLD);
	mvprintw(maxy - 2, (maxx - 24)/2, "Total Head Movement: %d", total);
	attroff(A_BOLD);

	noecho();
	getch();
	echo();
}

int main(void)
{
	int n, start, *queue, qs, i;
	char ch, file[32];
	FILE *fp;
	initscr();				//Initialize curses mode.
	cbreak();
	getmaxyx(stdscr, maxy, maxx);
	do
	{
		werase(stdscr);
		rectangle(0, 0, maxy - 1, maxx - 1);
		mvprintw(maxy / 2, (maxx - 40) / 2, "Read data from file or manually? (f/m): ");
		scanw("%c", &ch);
	}while(ch != 'f' && ch != 'F' && ch != 'm' && ch != 'M');

	if(tolower(ch) == 'f')
	{
		mvprintw(maxy / 2 + 1, (maxx - 18)/2, "Enter file name: ");
		scanw("%s", file);
		fp = fopen(file, "r");
		if(fp == NULL)
		{
			mvprintw(maxy / 2 + 3, (maxx - 20)/2, "Error opening file!!!");
			noecho();
			getch();
			echo();
			endwin();		//End curses mode. Exit.
			return -1;
		}
		fscanf(fp, "%d", &n);
		fscanf(fp, "%d", &start);
		fscanf(fp, "%d", &qs);
		queue = (int *) malloc(sizeof(int) * qs);
		for( i = 0; i < qs; i++)
			fscanf(fp, "%d", &queue[i]);
		fclose(fp);
	}

	else
	{
		werase(stdscr);
		rectangle(0, 0, maxy - 1, maxx - 1);
		mvprintw(maxy / 2 - 5, (maxx - 40) / 2, "Enter the total number of cylinders: ");
		scanw("%d", &n);
		mvprintw(maxy / 2 - 4, (maxx - 40) / 2, "Enter the disk head position: ");
		scanw("%d", &start);
		mvprintw(maxy / 2 - 3, (maxx - 40) / 2, "Enter the number of requests: ");
		scanw("%d", &qs);
		queue = (int *) malloc(sizeof(int) * qs);
		mvprintw(maxy / 2 - 1, (maxx - 40) / 2, "Enter the cylinders of the requested I/O :");
		for(i =0 ; i < qs; i++)
			mvscanw(maxy / 2 + i, maxx / 2, "%d", &queue[i]);
	}


	while(1)
	{
		switch(menu())
		{
			case 1: fcfs(n, start, qs, queue);
					break;

			case 2: sstf(n, start, qs, queue);
					break;

			case 3: scan1(n, start, qs, queue);
					break;
			
			case 4: scan2(n, start, qs, queue);
					break;

			case 5:	endwin();
					return 0;
		}
	}
	return 0;
}
