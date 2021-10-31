#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <fstream.h>
#include <unistd.h>

#define MILIARD 1000000000L;

int fd; // fd – uchwyt pliku (identyfikator otwartego pliku)
struct timespec start, stop;
struct timespec t1, t2;
double sekund;
double czas;

void dioda(void)
{
	static int stan = 0;

	//clock_gettime(CLOCK_GETTIME, &t1);
	if (stan==0)
	{
		stan = 1;
		write(fd,&stan,1);
		if(clock_gettime(CLOCK_REALTIME, &start) == -1){
				cout << "blad fukcji clock gettie" << endl;
				exit(-1);
		}
	}
	//clock_gettime(CLOCK_GETTIME, &t2);
	//czas = t2.tv_sec-t1.tv_sec;
	else if(stan==1)
	{
		stan = 0;
		write(fd,&stan,1);
		if(clock_gettime(CLOCK_REALTIME, &stop) == -1){
					cout << "blad fukcji clock gettie" << endl;
					exit(-1);
		}
		sekund = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec)/(double)MILIARD;
		czas+=sekund;
	}

}

void ustaw()
{

struct termios termio; // zmienna termio typu termios
int res;
int baud;
baud = 9600;
fd = open("/dev/ser1", O_RDWR); // otwórz "/dev/ser1" do odczytu i zapisu (O_RDWR)
res = tcgetattr(fd, &termio); // Pobranie parametrów terminala do zmiennej termio
termio.c_cflag &= ~(PARENB|PARODD|PARSTK|CSIZE|CSTOPB); // brak bitu parzystoœci
// 1 bit stopu
termio.c_cflag |= CS8; // Znak 8 bit
termio.c_cflag &= ~(IHFLOW|OHFLOW); // Brak kontroli przep³ywu
cfsetispeed(&termio, baud); // Szybkoœæ wejœciowa
cfsetospeed(&termio, baud); // Szybkoœæ wyjœciowa
res = tcsetattr(fd, TCSANOW, &termio); //Ustawienie parametrów terminala ze zmiennej termio
if(res == -1) {
perror("Blad"); // Wypisanie na stderr informacji o b³êdzie
exit(1);
}
std::cout <<"Parametry portu ustawione"<< std::endl;
}


uint8_t refVolt;
uint8_t val;
uint8_t ID;
double voltage;
double moc;
int liczba;
char buf[4];



void aktualny()
  {
    time_t time_of_day;
    char buffer[ 80 ];

    time_of_day = time( NULL );
    strftime( buffer, 80, "Czas z QNX  %H %M %S",
           localtime( &time_of_day ) );
    printf( "%s\n", buffer );


  }

void obsluga(){

	refVolt = buf[0];
	val = buf[1];
	val += 256*buf[2];
	ID = buf[3];



	voltage = (val/1023.0) * refVolt;

	moc = voltage*voltage/20.0;
	std::cout << "wolty " << voltage << std::endl;
	std::cout << "moc " << moc << std::endl;

}


void czytaj(void){


	liczba = 0;
	int znak = 0;
	int rd;

	int seriadanych = 1;

	ofstream plik;
	plik.open("plik.txt");
	//fd=open("/dev/ser1", O_RDWR); // otwórz "/dev/ser1" tylko do odczytu (O_RDONLY)
	do {
		rd = readcond(fd, &buf, 4, 4, 10, 50); // z pliku o uchwycie "fd" czytaj 1 bajt do zmiennej "c"
		if (rd==4)
		{
			//putchar(c); // pisz znak na standardowe wejœcie-wyjœcie

			dioda();
			obsluga();
			cout << "CZAS " << czas << endl;
			std::cout << std::endl;
			liczba++;
			flushall(); // wyczyœæ wszystkie bufory wejœcia-wyjœcia
			std::cout << buf[0] << " dupa " << std::endl;
			seriadanych++;
			plik << "__________________________________________" << endl;
			plik << "Seria danych numer: " << seriadanych << endl;
			plik << "Napiêcie wynosi: " << voltage << " V" << endl;
			plik << "Moc wynosi : " << moc << " W" << endl;
			//std::cout << "odbior : " << liczba << std::endl;
			if(liczba%2==1){
				znak = liczba*(-1);
			}else{
				znak=liczba;
			}
		//std::cout << "ze znakiem: " << znak << std::endl;
		} //if
	} while(1); // nieskoñczona pêtla
	close(fd); // zamkniêcie pliku o uchwycie "fd"
}


int main(int argc, char *argv[]) {



	ustaw();
	czytaj();




	//std::cout << "odbior " << liczba << std::endl;
	//std::cout << "volty " << voltage << std::endl;
	return EXIT_SUCCESS;
}
