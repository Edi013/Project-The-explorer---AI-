#include "graphics.h"
#include <math.h>
#include <dos.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream> // for string cast #1 -- begin

using namespace std;

int matrice[40][60];
int scor = 0;	   // for string cast #2 - 3 
bool game_on = true;
bool hint_bool = false;


//declarare 
class Player { 
private:
	int x;
	int y;
	int perceptori[3];
public:
	Player(int _x = 200, int _y = 200);
	void first_time_player_drawing();
	void move_rotate_and_draw_player(const char m, const int ultima_pozitie_player_in_matrix);
	void move_player_forward(const int ultima_pozitie_player_in_matrix);
	void rotate_and_shoot_player(const char m,const int ultima_pozitie_player_in_matrix);
	int  delete_player();
	void move_player(const char m);
	void check_for_wall_move(const int ultima_pozitie_player_in_matrix);
	void perceptori_set_cu_vector(const int vector_perceptori[3]);
	void perceptori_set_cu_3_valori(const int v1, const int v2, const int v3);
	int* perceptori_get();
	void perceptori_afisare();
	void calculeaza_si_atribuie_perceptori();
	int perceptor_atingere(); 
	int perceptor_monstru(); 
	int perceptor_comoara();
	void check_treasure();
	void check_monster();
	void shoot_(const int ultima_pozitie_player_in_matrix);
};

void patrat(int x, int y, int culoare);
void grid();

void init_matrice();
// in matrice :
	// 0 = zid
	// 1 = drum  , cale , poteca (player-ul poate merge pe acolo)
	// 2 = monster 
	// 3 = treasure 
	// 6 w -> player headed top
	// 7 s -> player headed down
	// 8 a -> player headed left
	// 9 d -> player headed right
void afisare_matrice();

char* cast_scor_to_string(ostringstream& str1, string& s);
void delete_scor();
void afiseaza_scor();

char input_check(char m);

void hint_activation();

void menu();

void info_hint();

int main()
{ 
	Player player(20, 30);//  coordonate grid , in afisajul grafic , adica pe harta -> x - orizontala si y - verticala 
	
	init_matrice();
	initwindow(800, 600, "AXE", 200, 200);
	setbkcolor(1);
	cleardevice();

	setcolor(15);
	grid(); 
	outtextxy(50, 100, "SCOR");

	

	player.first_time_player_drawing();
	player.calculeaza_si_atribuie_perceptori();
	player.perceptori_afisare();
	afiseaza_scor();
	while (game_on) {

		char m;
		menu();
		cin >> m;
		m = input_check(m);

		player.move_player(m);
		player.calculeaza_si_atribuie_perceptori();
		cout << " --------------------------------------------------------- " << endl << endl;
		player.perceptori_afisare();
		afiseaza_scor();
		
		if (hint_bool) {
		   afisare_matrice();
		   info_hint();
		}
	}
	getchar();
	return 0;
}


Player::Player(int _x , int _y ) {
	this->x = _x;
	this->y = _y;
	matrice[y / 10][x / 10] = 9;
	for (int i = 0; i < 3; i++)
		perceptori[i] = 0;
}
void Player::first_time_player_drawing() {
	 // setup for the start | used before main loop (while)
	outtextxy(150 + x, 100 + y, ">");
	matrice[y / 10][x / 10] = 9;
	
}

void Player::move_player(char m) {
	//aceasta functie functioneaza dupa procedeul :
	// 1. sterge, indiferent de noile coordonate , sterge la cele vechi . 
	// 2. deseneaza din nou cu noi coordonate .
	// 3. in timpul lui deseneaza (2) , se face verificarea daca e zid 
	// functia DELETE_PLAYER returneaza valoarea veche din matrice asociata pozitiei playerului , 
	scor -= 10;
	if (m == 'p') {
		game_on = false;
	}
	else
		move_rotate_and_draw_player(m, delete_player());
}
int  Player::delete_player() {
	
	int nr_player_in_matrice ;
	if (x == 0 || x == 39 || y == 0 || y == 59) {			//sterge urma pe care o lasa in matricea asociata afisajului grafic playerul ,  
		nr_player_in_matrice = matrice[y / 10][x / 10];		//cum? // deseneaza un zid peste "  " si pe afisaj grafic si in matrice 
		matrice[y / 10][x / 10] = 0;
		outtextxy(150 + x, 100 + y, "#");

	}
	else {
		nr_player_in_matrice = matrice[y / 10][x / 10];
		matrice[y / 10][x / 10] = 1;//sterge vechea pozitie a playerului din matrice 
		outtextxy(150 + x, 100 + y, "  ");

		return nr_player_in_matrice;
	}
	return nr_player_in_matrice;
}
void Player::move_rotate_and_draw_player(const char m , int ultima_pozitie_player_in_matrix) { //CUM LEGAM MATRICEA DE AFISAJUL GRAFIC : Playerul vine cu x/y coordonate pentru grid  // move_and_draw_player
													   									  	   // Astefel , afisajul grafic : coordonatele de inceput al gridului + x/y de la player : 150 +x , 100 + 6
																							   // Iar , legarea matricii de afisajul grafic (m[40][60] pentru grid 600-Ox pe 400-Oy )
	//IN MATRICE VALORI :
	// 6 w -> player headed top
	// 7 s -> player headed down
	// 8 a -> player headed left
	// 9 d -> player headed right

	if (m == 'e') { // daca 'e' , atunci verifica cum e playerul indreptat si muta-l in directia respectiva (in if-uri avem miscarea normala de la w/a/s/d)
		move_player_forward(ultima_pozitie_player_in_matrix);
	}
	else rotate_and_shoot_player(m, ultima_pozitie_player_in_matrix);	
}
void Player::move_player_forward(int ultima_pozitie_player_in_matrix) {
	if (ultima_pozitie_player_in_matrix == 8) { //matrice[y/10][x/10]
		x -= 10;
		check_treasure();
		check_monster();
		check_for_wall_move( ultima_pozitie_player_in_matrix); // 
		outtextxy(150 + x, 100 + y, "<");
		matrice[y / 10][x / 10] = 8;
	}
	else if (ultima_pozitie_player_in_matrix == 9) {
		x += 10;
		check_treasure();
		check_monster();
		check_for_wall_move( ultima_pozitie_player_in_matrix);
		outtextxy(150 + x, 100 + y, ">");
		matrice[y / 10][x / 10] = 9;
	}
	else if (ultima_pozitie_player_in_matrix == 6) {
		y = y - 10;
		check_treasure();
		check_monster();
		check_for_wall_move( ultima_pozitie_player_in_matrix);
		outtextxy(150 + x, 100 + y, "^");
		matrice[y / 10][x / 10] = 6;
	}
	else if (ultima_pozitie_player_in_matrix == 7) {
		y = y + 10;
		check_treasure();
		check_monster();
		check_for_wall_move( ultima_pozitie_player_in_matrix);
		outtextxy(150 + x, 100 + y, "v");
		matrice[y / 10][x / 10] = 7;
	}
}
void Player::check_for_wall_move(const int ultima_pozitie_player_in_matrix) {
	if (matrice[y / 10][x / 10] == 0) {  // daca playerul s-a mutat intr-un perete , il muta inapoi si ii inapoiaza punctele
		cout << "-------------------------------------------" << endl;
		cout << "NU incerca sa treci prin pereti ! ! !" << endl;
		if (ultima_pozitie_player_in_matrix == 6) { // w
			y = y + 10;
			scor += 10;
		}
		else if (ultima_pozitie_player_in_matrix == 7) { // s
			y = y - 10;
			scor += 10;
		}
		else if (ultima_pozitie_player_in_matrix == 8) { // a
			scor += 10;
			x += 10;
		}
		else if (ultima_pozitie_player_in_matrix == 9) { // d
			scor += 10;
			x -= 10;
		}
	}
}
void Player::check_treasure() {
	if (matrice[y / 10][x / 10] == 3) {
		scor += 100;
		
		outtextxy(100 , 50 ,"Ai gasit o comoara (+ 100 points) !!!");
		delay(3000);
		outtextxy(100, 50, "                                                                ");
	}
	
}
void Player::check_monster() {
	if (matrice[y / 10][x / 10] == 2) {
		scor -= 200;
		
		outtextxy(100, 50, "O nu . . . ai intrat in celula unui monstru ! ! ! (you are dinner) ");
		delay(3000);
		outtextxy(100, 50, "                                                                                                                        ");
	}
}
void Player::rotate_and_shoot_player(char m , const int ultima_pozitie_player_in_matrix) {
	if (m == 'w') {
		outtextxy(150 + x, 100 + y, "^");
		matrice[y / 10][x / 10] = 6;
	}
	else if (m == 's') {
		outtextxy(150 + x, 100 + y, "v");
		matrice[y / 10][x / 10] = 7;
	}
	else if (m == 'a') {
		outtextxy(150 + x, 100 + y, "<");
		matrice[y / 10][x / 10] = 8;
	}
	else if (m == 'd') {
		outtextxy(150 + x, 100 + y, ">");
		matrice[y / 10][x / 10] = 9;
	}
	else if (m == 'h') { // hint 
		hint_activation();
		if (ultima_pozitie_player_in_matrix == 6) { // w
			outtextxy(150 + x, 100 + y, "^");
			matrice[y / 10][x / 10] = 6;
		}
		else if (ultima_pozitie_player_in_matrix == 7) { // s
			outtextxy(150 + x, 100 + y, "v");
			matrice[y / 10][x / 10] = 7;
		}
		else if (ultima_pozitie_player_in_matrix == 8) { // a
			outtextxy(150 + x, 100 + y, "<");
			matrice[y / 10][x / 10] = 8;
		}
		else if (ultima_pozitie_player_in_matrix == 9) { // d
			outtextxy(150 + x, 100 + y, ">");
			matrice[y / 10][x / 10] = 9;
		}
		
	}
	else if (m == 'q') { // shot
		scor -= 10;
		shoot_(ultima_pozitie_player_in_matrix);
	}
}
void Player::shoot_(const int ultima_pozitie_player_in_matrix) {
	if (ultima_pozitie_player_in_matrix == 6) { // w
		outtextxy(150 + x, 100 + y, "^");
		matrice[y / 10][x / 10] = 6;

		int copie_x = x, copie_y = y - 10;
		while (true) {
			if (matrice[copie_y / 10][copie_x / 10] == 2) {
				outtextxy(100, 50, "Ai impuscat un monstru ! ! ! (+50 points)");
				delay(3000);
				outtextxy(100, 50, "                                                                                        ");
				matrice[copie_y / 10][copie_x / 10] = 1;
				scor += 50;
				break;
			}
			else if (matrice[copie_y / 10][copie_x / 10] != 1) {
				outtextxy(100, 50, "Ai impuscat altceva , nu un  monstru ! ! ! (-25 points)(if not a wall -> object distructed)");
				delay(3000);
				outtextxy(100, 50, "                                                                                                                                                                   ");
				if (matrice[copie_y / 10][copie_x / 10] != 0)
					matrice[copie_y / 10][copie_x / 10] = 1;
				scor -= 25;
				break;
			}

			outtextxy(150 + copie_x, 100 + copie_y, ".");
			delay(500);

			outtextxy(150 + copie_x, 100 + copie_y, "  ");
			copie_y -= 10;
			outtextxy(150 + x, 100 + y, "^");
		}
	}
	else if (ultima_pozitie_player_in_matrix == 7) { // s
		outtextxy(150 + x, 100 + y, "v");
		matrice[y / 10][x / 10] = 7;

		int copie_x = x, copie_y = y + 10;
		while (true) {
			if (matrice[copie_y / 10][copie_x / 10] == 2) {
				outtextxy(100, 50, "Ai impuscat un monstru ! ! ! (+50 points)");
				delay(3000);
				outtextxy(100, 50, "                                                                                         ");
				matrice[copie_y / 10][copie_x / 10] = 1;
				scor += 50;
				break;
			}
			else if (matrice[copie_y / 10][copie_x / 10] != 1) {
				outtextxy(100, 50, "Ai impuscat altceva , nu un  monstru ! ! ! (-25 points)(if not a wall -> object distructed)");
				delay(3000);
				outtextxy(100, 50, "                                                                                                                                                             ");
				if (matrice[copie_y / 10][copie_x / 10] != 0)
					matrice[copie_y / 10][copie_x / 10] = 1;
				scor -= 25;
				break;
			}

			outtextxy(150 + copie_x, 100 + copie_y, ".");
			delay(500);
			outtextxy(150 + copie_x, 100 + copie_y, "  ");

			copie_y += 10;
			outtextxy(150 + x, 100 + y, "v");
		}

	}
	else if (ultima_pozitie_player_in_matrix == 8) { // a
		outtextxy(150 + x, 100 + y, "<");
		matrice[y / 10][x / 10] = 8;

		int copie_x = x - 10, copie_y = y;
		while (true) {
			if (matrice[copie_y / 10][copie_x / 10] == 2) {
				outtextxy(100, 50, "Ai impuscat un monstru ! ! ! (+50 points)");
				delay(3000);
				outtextxy(100, 50, "                                                                           ");
				matrice[copie_y / 10][copie_x / 10] = 1;
				scor += 50;
				break;
			}
			else if (matrice[copie_y / 10][copie_x / 10] == 3 || matrice[copie_y / 10][copie_x / 10] == 0) {
				outtextxy(100, 50, "Ai impuscat altceva , nu un  monstru ! ! ! (-25 points)(if not a wall -> object distructed)");
				delay(3000);
				outtextxy(100, 50, "                                                                                                                                                             ");
				if (matrice[copie_y / 10][copie_x / 10] != 0)
					matrice[copie_y / 10][copie_x / 10] = 1;
				scor -= 25;
				break;
			}

			outtextxy(150 + copie_x, 100 + copie_y, ".");
			delay(500);
			outtextxy(150 + copie_x, 100 + copie_y, "  ");
			copie_x = copie_x - 10;
			outtextxy(150 + x, 100 + y, "<");
		}

	}
	else if (ultima_pozitie_player_in_matrix == 9) { // d
		outtextxy(150 + x, 100 + y, ">");
		matrice[y / 10][x / 10] = 9;

		int copie_x = x + 10, copie_y = y;
		while (true) {
			if (matrice[copie_y / 10][copie_x / 10] == 2) {
				outtextxy(100, 50, "Ai impuscat un monstru ! ! ! (+50 points)");
				delay(3000);
				outtextxy(100, 50, "                                                                           ");
				matrice[copie_y / 10][copie_x / 10] = 1;
				scor += 50;
				break;
			}
			else if (matrice[copie_y / 10][copie_x / 10] != 1) {
				outtextxy(100, 50, "Ai impuscat altceva , nu un  monstru ! ! ! (-25 points)(if not a wall -> object distructed)");
				delay(3000);
				outtextxy(100, 50, "                                                                                                                                                                ");
				if (matrice[copie_y / 10][copie_x / 10] != 0)
					matrice[copie_y / 10][copie_x / 10] = 1;
				scor -= 25;
				break;
			}

			outtextxy(150 + copie_x, 100 + copie_y, ".");
			delay(500);
			outtextxy(150 + copie_x, 100 + copie_y, "  ");
			copie_x = copie_x + 10;
			outtextxy(150 + x, 100 + y, ">");
		}

	}
}

void Player::calculeaza_si_atribuie_perceptori() {
	//functie apelata in move_rotate_and_draw_player
	int v_aux[3] = { 0 , 0 , 0 };

	perceptori_set_cu_3_valori(perceptor_atingere(), perceptor_monstru(), perceptor_comoara());
}
int Player::perceptor_atingere() {
	if (matrice[y / 10][x / 10] == 8) {  // a 

		if (matrice[y / 10][(x - 10) / 10] != 1)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 9) { // d

		if (matrice[y / 10][(x + 10) / 10] != 1)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 6) { // w

		if (matrice[(y - 10) / 10][x / 10] != 1)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 7) { // s

		if (matrice[(y + 10) / 10][x  / 10] != 1)
			return 1;
		else
			return 0;
	}
	else {
		cout << "ERROR (in function 'perceptor_atingere')" << endl;
		return 666;
	}

}
int Player::perceptor_monstru() {
	if (matrice[y / 10][x / 10] == 8) {  // a 

		if (matrice[y / 10][(x - 10) / 10] == 2)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 9) { // d

		if (matrice[y / 10][(x + 10) / 10] == 2)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 6) { // w

		if (matrice[(y - 10) / 10][x / 10] == 2)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 7) { // s

		if (matrice[(y + 10) / 10][x  / 10] == 2)
			return 1;
		else
			return 0;
	}
	else {
		cout << "ERROR (in function 'perceptor_monstru()')" << endl;
		return 666;
	}
}
int Player::perceptor_comoara() {
	if (matrice[y / 10][x / 10] == 8) {  // a 

		if (matrice[y / 10][(x - 10) / 10] == 3)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 9) { // d

		if (matrice[y / 10][(x + 10) / 10] == 3)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 6) { // w

		if (matrice[(y - 10) / 10][x / 10] == 3)
			return 1;
		else
			return 0;
	}
	else if (matrice[y / 10][x / 10] == 7) { // s

		if (matrice[(y + 10) / 10][(x  / 10)] == 3)
			return 1;
		else
			return 0;
	}
	else {
		cout << "ERROR (in function 'perceptor_comoara()')" << endl;
		return 666;
	}
}
void Player::perceptori_set_cu_vector(const int  vector_perceptori[3]) {
	for (int i = 0; i < 3; i++)
		perceptori[i] = vector_perceptori[i];
}
void Player::perceptori_set_cu_3_valori(const int v1, const int v2, const int v3) {
	perceptori[0] = v1;
	perceptori[1] = v2;
	perceptori[2] = v3;
}
int* Player::perceptori_get() {
	return perceptori;
}
void Player::perceptori_afisare() {
	cout << "Senzor atingere: " << perceptori[0] <<endl;
	cout << "Senzor celula monstru: " << perceptori[1] << endl;
	cout << "Senzor celula comoara: " << perceptori[2] << endl;
}

void patrat(int x, int y, int culoare) {
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++) {

			putpixel(x + i, y + j, culoare);
			//15 = alb 
			//9 = mov
			//8 = gri
			//1 = mov inchis
		}
}
void grid() {
	for (int i = 0; i < 600; i += 10)
		for (int j = 0; j < 400; j += 10)
		{
			patrat(150 + i, 100 + j, 0);
			if (i == 0 || i == 590)
				outtextxy(150 + i, 100 + j, "#");
			if (j == 0 || j == 390)
				outtextxy(150 + i, 100 + j, "#");
		}
	// sa codez desenarea bordurii prima , pentru a suprapune imaginea penultimei linii cu # din bordura de jos, arata mai bine
}

void init_matrice() {
	// in matrice :
	// 0 = zid
	// 1 = nimic / deci se poate considera drum
	// 2 = monster 
	// 3 = treasure 
	// 4 = ...
	// 5 = ...
	// 6 w -> player headed top
	// 7 s -> player headed down
	// 8 a -> player headed left
	// 9 d -> player headed right

	for (int linii = 0; linii < 40; linii++) { // 1 - drumul accesibil de player
		if (linii != 0 && linii != 39) {
			for (int coloane = 1; coloane < 59; coloane++) {
				matrice[linii][coloane] = 1;
				
				//comorile 
					//comoara pentru testare la 1 1 
					// if (coloane == 1 && linii == 1)
					//	matrice[linii][coloane] = 3;

					if (linii % 2 == 1 && coloane % 2 == 1) {
					//	if (linii == 7 ||linii == 12 || linii == 18 || linii == 24 || linii == 28 || linii == 32   )
								matrice[linii][coloane] = 3;
					}

				//monstrii
					
					//monstrii pentru testare la 1 2 si 2 2 
					// if (coloane == 2 && linii == 1)
					//	matrice[linii][coloane] = 2;
					// if (coloane == 2 && linii == 2)
					//	 matrice[linii][coloane] = 2;
					 if (linii % 2 == 0 && coloane % 2 == 0) {
						// if (linii == 7+2 || linii == 12-2 || linii == 18+2 || linii == 24-2 || linii == 28-1 || linii == 32+1  )
							 matrice[linii][coloane] = 2;
					 }
			}
		}
		matrice[linii][0] = 0;
		matrice[linii][59] = 0;
		if (linii < 40) {
			matrice[0][linii] = 0;
			matrice[39][linii] = 0;
		}
	}
}
void afisare_matrice() {
	cout << "HINT start ----------------------------------------------------" << endl;
	// in matrice :
		// 0 = zid
		// 1 = drum  , cale , poteca (player-ul poate merge pe acolo)
		// 6 = monstru
		// 7 = comoara
		// 9 = player

	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 60; j++)
			cout << matrice[i][j] << " ";
		cout << endl;
	}
}

char* cast_scor_to_string(ostringstream &str1, string & s) {// for string cast #4	
	str1 << scor;// for string cast #5
	s = str1.str();// for string cast #6 -- end
	char* p_to_s = &s[0];
	return p_to_s;			
}
void delete_scor() {
	for (int i = 0; i < 60; i += 10) { // pentru a nu afisa un scor cu o cifra mai mult decat are : 1000 - 10 = 900 (0 - un 0 in plus)
		patrat(50 + i , 120 , 1);
		patrat(50 + i, 130, 1);
	}
}
void afiseaza_scor() {
	delete_scor();
	ostringstream str1; string s;
	outtextxy(50, 125 , cast_scor_to_string(str1, s));	
}

char input_check(char m) {
	if (m == 'a' || m == 'A')
		return 'a';
	else if (m == 'd' || m == 'D')
		return 'd';
	else if (m == 'w' || m == 'W')
		return 'w';
	else if (m == 's' || m == 'S')
		return 's';
	else if (m == 'e' || m == 'E')
		return 'e';
	else if (m == 'p' || m == 'P')
		return 'p';
	else if (m == 'h' || m == 'H')
		return 'h';
	else if (m == 'q' || m == 'Q')
		return 'q';
	else {
		do  {
			cout << "--------------------------------"<<endl; 
			cout << "Input incorect" << endl;
			menu();
			cin >> m;
		} while ((m != 'h') && (m != 'H') && (m != 'a') && (m != 'A') && (m != 'w') && (m != 'W') && (m != 'd') && (m != 'D') && (m != 's') && (m != 'S') && (m != 'p') && (m != 'P') && (m != 'e') && (m != 'E') && (m != 'q') && (m != 'Q'));
	}
	return m;
}

void hint_activation() {
	hint_bool = !hint_bool;
}

void menu() {
	cout << "walk forward : E " << endl;
	cout << "rotate : W/A/S/D" << endl;
	cout << "Q to shoot  " << endl;
	cout << "H for hint (you are not dinner)" << endl;
	cout << "P for quit" << endl;
}

void info_hint() {
	cout << " Matrix (hint) :" << endl;
	cout << "0 = wall " << endl;
	cout << "1 = empty space (road)" << endl;
	cout << "2 = monster " << endl;
	cout << "3 = treasure " << endl;
	cout << "6 =  w -> player headed top" << endl;
	cout << "7 = s -> player headed down" << endl;
	cout << "8 = a -> player headed left" << endl;
	cout << "9 = d -> player headed right" << endl;
	cout << "End hint -------------------------------------------------------" << endl;
}

//ERRORS 
// The game is ready ! 
// GAME ON <3 

