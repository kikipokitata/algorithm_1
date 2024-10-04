#include <iostream>
#include <limits>
#include <fstream>
#include <ctime>

class Game {

private:
	int round = 1;
	int cur_num_of_people = 100;
	int	cur_num_of_wheat = 2800;
	int	cur_num_of_acres = 1000;

	int user_buy_acres = 0;
	int user_sell_acres = 0;
	int user_feed_people = 0;
	int user_sow_acres = 0;

	int collected_in_this_round = 0;
	int price_per_acre = 0;
	int amount_of_wheat_per_acre = 0;

	double coef_rat_eaten = 0;
	int rat_eaten = 0;

	int died = 0;
	bool plague = 0;
	int arrived = 0;

	int total_num_of_people = 100;
	int total_num_of_died = 0;

public:
	Game() {

		price_per_acre = random(17, 26);

		Start();
	};

	double random(double min, double max)
	{
		return (double)(rand()) / RAND_MAX * (max - min) + min;
	}

	void Start() {

		while (true) {
			ShowInfo();
			UserChoice();
			UpdatingValuesBeforeRound();  // назвать "вычисления"

			total_num_of_people += arrived;
			total_num_of_died += died;

			round += 1;
			if (round == 10) {
				break;
			}
		}

		GameRating();
	}

	void ShowInfo() {
		std::cout << "Раунд [" << round << "]" << std::endl;
		if (died > 0)
			std::cout << "Умерло с голоду " << died << " человек в этом году" << std::endl;
		if (arrived > 0)
			std::cout << "В город пришли " << arrived << " человек в этом году" << std::endl;
		if (plague)
			std::cout << "Чума уничтожила половину населения" << std::endl;
		std::cout << "В вашем распоряжении есть " << cur_num_of_people << " количества населения" << std::endl;
		if (collected_in_this_round > 0)
			std::cout << "Было собрано " << collected_in_this_round << " бушелей пшеницы, по " << amount_of_wheat_per_acre << " бушеля с каждого акра" << std::endl;
		if (coef_rat_eaten > 0)
			std::cout << "Крысы уничтожили " << rat_eaten << " количества пшеницы" << std::endl;
		std::cout << "В вашем распоряжении есть " << cur_num_of_wheat << " количества пшеницы" << std::endl;
		std::cout << "В вашем распоряжении есть " << cur_num_of_acres << " количества акров земли" << std::endl;
		std::cout << "Акр стоит " << price_per_acre << " количества пшеницы" << std::endl;

	}

	void UserChoice() {
		std::cout << "Сколько купить земли? ";
		InputChoice(&user_buy_acres);

		std::cout << "Сколько продать земли? ";
		InputChoice(&user_sell_acres);

		std::cout << "Сколько пшеницы съесть? ";
		InputChoice(&user_feed_people);

		std::cout << "Сколько засеять земли? ";
		InputChoice(&user_sow_acres);

		if (CheckChoice()) {
			std::cout << "Ваши дальшейшие действия?\n\n";
			DataFromRandom();
			Menu();
		}
	}

	void InputChoice(int* variable) { 
		std::cin >> *variable;
		while (std::cin.fail() || *variable < 0) {
			std::cin.clear();
		    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Введите значение повторно: ";
			std::cin >> *variable;
		}
	}

	bool CheckChoice() {
		if ((user_sell_acres > cur_num_of_acres) || 
		(cur_num_of_wheat < user_sell_acres * price_per_acre - user_buy_acres * price_per_acre + user_feed_people + user_sow_acres)) {
			std::cout << "Много ввели. Переделайте." << std::endl << std::endl;
			UserChoice();
		}
		return true;
	}

	void CountingPeople() {
		// Излишки пшеницы не возвращаются в банк пшеницы 
		died = cur_num_of_people - user_feed_people / 20;
		if (died * 100 / cur_num_of_people > 45) {
			std::cout << "Более 45% вашего населения умерло от голода(";
			exit(1);
		}
		cur_num_of_people -= died;

		if (plague) {
			cur_num_of_people /= 2;
		}

		arrived = died / 2 + (5 - amount_of_wheat_per_acre) * cur_num_of_wheat / 600 + 1;
		if (arrived < 0)
			arrived = 0;
		if (arrived > 50)
			arrived = 50;
		cur_num_of_people += arrived;
	}

	void CountingWheat() {
		cur_num_of_wheat -= (user_buy_acres * price_per_acre - user_sell_acres * price_per_acre + user_feed_people + user_sow_acres);

		// Излишки пшеницы возвращаются в банк пшеницы 
		int collected = user_sow_acres * 2 > cur_num_of_people * 10 ? cur_num_of_people * 10 : user_sow_acres * 2;
		int total_sowed = cur_num_of_acres > collected ? collected : cur_num_of_acres;
		collected_in_this_round = total_sowed * amount_of_wheat_per_acre;
		cur_num_of_wheat += (user_sow_acres * 2 - total_sowed) / 2 + collected_in_this_round;

		rat_eaten = coef_rat_eaten * cur_num_of_wheat;
		cur_num_of_wheat -= rat_eaten;
	}

	void DataFromRandom() {
		amount_of_wheat_per_acre = random(1, 6);
		coef_rat_eaten = random(0, 0.07);

		int probability_of_plague = random(1, 100);
		if (probability_of_plague <= 15) {
			plague = 1;
		}
	}

	void UpdatingValuesBeforeRound() {
		cur_num_of_acres += user_buy_acres - user_sell_acres;
		CountingWheat();
		CountingPeople();

		price_per_acre = random(17, 26);
	}

	void Menu() { 
		bool flag = true;
		int choice;
		while (flag) {
			std::cout << "1 - продолжить\n2 - сохранить\n3 - загрузаить сохранение\n4 - выйти\n";
			std::cin >> choice;
			switch (choice)
			{
			case(1):
				flag = false;
				break;
			case(2):
				Saving();
				flag = false;
				break;
			case(3):
				Reading();
				flag = false;
				break;
			case(4):
				std::cout << "До свидания";
				exit(1);
			default:
				std::cout << "Введите еще раз";
			}
		}
	}

	void GameRating() {
		int death_rate = (total_num_of_died * 100) / total_num_of_people;
		int P = 0; // Оценка  за среднегодовой процент умерших от голода
		if (death_rate > 10) {
			if (death_rate > 33)
				P = 2;
			else
				P = 3;
		}
		else {
			if (death_rate > 3)
				P = 4;
			else
				P = 5;
		}

		int acre_per_person = cur_num_of_acres / cur_num_of_people;
		int L = 0; // Оценка за количество акров земли на одного жителя 
		if (acre_per_person < 9) {
			if (acre_per_person < 7)
				L = 2;
			else
				L = 3;
		}
		else {
			if (acre_per_person < 10)
				L = 4;
			else
				L = 5;
		}

		int grade = (P + L) / 2;

		std::cout << "Оценка " << grade;

	}

	void Saving() {
		std::ofstream out;
		out.open("savegame.txt");
		if (out.is_open())
		{
			out << round << " " << cur_num_of_people << " " << cur_num_of_wheat << " " << cur_num_of_acres << " "
				<< user_buy_acres << " " << user_sell_acres << " " << user_feed_people << " " << user_sow_acres << " "
				<< price_per_acre << " " << amount_of_wheat_per_acre << " " << coef_rat_eaten << " " << plague << " "
				<< total_num_of_people << " " << total_num_of_died;
			std::cout << "Данные успешно сохранены.\n";
		}
		else{
			std::cout << "Ошибка сохранения.\n";
		}
		out.close();
	}

	void Reading() { 
		std::ifstream in;
		in.open("savegame.txt");
		if (in.is_open())
		{
			in >> round >> cur_num_of_people >> cur_num_of_wheat >> cur_num_of_acres >>
				user_buy_acres >> user_sell_acres >> user_feed_people >> user_sow_acres >>
				price_per_acre >> amount_of_wheat_per_acre >> coef_rat_eaten >> plague >>
				total_num_of_people >> total_num_of_died;

			std::cout << "Данные загружены!\n";
		}
		else{
			std::cout << "Нет сохраненной игры.\n";
		}
		in.close();
	}


};


int main() {
	setlocale(LC_ALL, "Russian");
	srand(time(0));

	Game game;

	return 0;
};