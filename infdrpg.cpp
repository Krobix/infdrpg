#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>

const int ROOM_MAX = 5; //Amount of rooms to exist in memory at a time. should be left at around 5.
const int CHEAT_OFFSET = rand(); //Offset used to detect memory modifiers like cheat engine
const bool SHOW_DEBUG_MSGS = true; //If debug messages should be shown
const int BASE_ENEMY_HP = 15; //Base amount used to generate enemy hp numbers
const int BASE_ENEMY_DEFENSE = 1;
const int BASE_ENEMY_ATTACK = 2;
const int BASE_ENEMY_ADDED_DIFFICULTY = 0;
const int MAX_ENEMY_ADDED_DIFFICULTY = 10;
const int MAX_ENEMIES_PER_ROOM = 5;
const std::string enemy_names[] = {"Goblin", "Dark Knight", "Orc", "Barbarian"};
void patchDebugMenus();

int cheatEncrypt(int val) {
	return val + CHEAT_OFFSET;
}

int cheatDecrypt(int val) {
	return val - CHEAT_OFFSET;
}

void debug(std::string msg) {
	std::cout << "[DEBUG]" << msg;
}

class Player {
	
	int hp;
	int gender;
	int level;
	
public:
	std::string name;
	
	void setGender(int g) {
		debug("Setting gender");
		this->gender = g;
		if(this->gender == 5) {
			patchDebugMenus();
		}
	}
	
};

class Menu {
	
	std::vector<std::string> options;
	
public:
	int show() {
		std::string choice;
		int choice_i;
		std::cout << "Type the number of the option you choose.\n";
		for(int i=0; i < this->options.size(); i++) {
			std::cout << "[" << i << "]" << this->options[i] << "\n";
		}
		while(true) {
		    std::getline(std::cin, choice);
		    std::stringstream ts(choice);
		    if(ts >> choice_i) break;
		}
		return choice_i;
	}
	
	Menu(std::vector<std::string> opts) {
		this->options = opts;
	}
	
	void addOption(std::string option) {
		this->options.push_back(option);
	}
	
};

struct Weapon {
	
	int durability; //Only applies to player and party members
	int attack;
	int speed;
	std::string name;
	
	Weapon() {
		this->durability = 9999;
		this->attack = 9999;
		this->speed = 9999;
		this->name = "TEST WEAPON";
	}
	
	Weapon(std::string name, int durability, int attack, int speed) {
		this->durability = cheatEncrypt(durability);
		this->attack = cheatEncrypt(attack);
		this->speed = cheatEncrypt(speed);
		this->name = name;
	}
	
	Weapon(Weapon *to_copy) {
		this->durability = to_copy->durability;
		this->attack = to_copy->attack;
		this->speed = to_copy->speed;
		this->name = to_copy->name;
	}
	
};

namespace weapons {
	Weapon test;
	
	std::vector<Weapon> weak = {
		Weapon("stick", 5, 5, 5),
		Weapon("wooden sword", 7, 5, 6)
	};
	
	std::vector<Weapon> lessweak = {
		Weapon("bronze sword", 10, 25, 15)
	};
}

Weapon *genWeaponFromDifficulty(int difficulty) {
	if(difficulty < 30) {
		return new Weapon(weapons::weak[rand() % weapons::weak.size()]);
	}
	else if (difficulty < 60) {
		return new Weapon(weapons::lessweak[rand() % weapons::lessweak.size()]);
	}
}

class Enemy {
	
	int hp;
	std::string name;
	int defense;
	int added_difficulty = BASE_ENEMY_ADDED_DIFFICULTY;
	int attack;
	Weapon weapon;
	
	void init(int difficulty) {
		this->name = enemy_names[rand() % (sizeof(enemy_names) / sizeof(enemy_names[0]))];
		this->hp = cheatEncrypt(BASE_ENEMY_HP + difficulty + this->added_difficulty);
		this->defense = cheatEncrypt(BASE_ENEMY_DEFENSE + difficulty + this->added_difficulty);
		this->attack = cheatEncrypt(BASE_ENEMY_ATTACK + difficulty + this->added_difficulty);
		this->weapon = *genWeaponFromDifficulty(difficulty);
	}
	
public:
	
	Enemy() {
		this->init(1);
	}
	
	Enemy(int difficulty) {
		this->init(difficulty);
	}
	
	std::string getName() {
		return this->name;
	}
	
};

enum RoomType {
	Encounter,
	Shop,
	Empty,
	RewardEncounter,
	BossEncounter,
	Treasure,
	NewPartyMemberEncounter
};

enum EncounterResult {
	Victory,
	RewardVictory,
	BranchingPathVictory,
	Loss
};

class Room { 
	
	int difficulty;
	std::vector<Enemy> enemies;
	RoomType type;
	int id;
	
	void generateEnemies(int amount) {
		for(int i=0; amount > i; i++){
			this->enemies.push_back(Enemy(this->difficulty));
		}
	}
	
	void generateRoom(int difficulty) {
		int enemy_amount = rand() % MAX_ENEMIES_PER_ROOM;
		this->type = static_cast<RoomType>(rand() % 6);
		this->difficulty = this->id + (rand() % 5);
		switch(this->type) {
			default:
			case Encounter:
			    this->generateEnemies(enemy_amount);
			    break;
			//TODO: add other room types
		}
	}
	
	EncounterResult do_encounter() {
		std::cout << "You enter the room.\n";
		
		if((this->type == Encounter) || (this->type == RewardEncounter) || (this->type == NewPartyMemberEncounter)){ 
		for(int i=0;i<this->enemies.size();i++) {
			std::cout << (this->enemies[i].getName()) << " appeared in the room!\n";
		}
		}
	}
	
public:
	
	Room(Room *last_room) {
		this->id = last_room->id + 1;
		this->generateRoom(last_room->getDifficulty() + (rand() % 7));
	}
	
	Room() {
		this->id = 0;
		this->generateRoom(1);
	}
	
	int getDifficulty() {
		return this->difficulty;
	}

};

namespace menus {
	
	Menu gender({"Male", "Female"});
	Menu mainBattleMenu({"Attack", "Defend", "Heal"});
	
}

void patchDebugMenus() {
	menus::mainBattleMenu.addOption("DEBUG");
}

class Game {
	
	int difficulty;
	Player *player;
	Room rooms[ROOM_MAX];
	Room *newest_room;
	
public:
	Game() {
		std::cout << "Weclome. Please enter your name. \n";
		std::cin >> this->player->name;
		std::cout << "What is your gender? \n";
		this->player->setGender(menus::gender.show());
		for(int i=1;i<ROOM_MAX;i++) {
			this->rooms[i] = Room(&(this->rooms[i-1]));
		}
		std::cout << "You are standing outside a dangerous dungeon. There is a door in front; you decide that you will go in and open it.\n";	
	}
	
	Room *getLastRoom() {
		return this->newest_room;
	}
	
};

int main(){
	Game game;
	return 0;
}
