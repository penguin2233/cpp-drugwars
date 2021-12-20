#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>

// Copyright (c) 2021 https://penguin2233.gq, no warranty is provided

#define VERSION 1
#define STARTING_LOAN 1000
#define INTEREST_RATE 0.005
#define DOWNTOWN_WEED_FACTOR 1.1
#define UPTOWN_COCAINE_FACTOR 1.6
#define GHETTO_SHROOMS_FACTOR 1.2

std::vector<int> gameState; // money, debt, cocainePocket, weedPocket, lsdPocket, shroomsPocket, days, location
std::vector<int> pricesForTheDay = { 0, 0, 0, 0 }; // cocaine, weed, lsd, shrooms
bool gaming;

void genPrices() {
	// set up random number generator device
	std::random_device rng_dev;
	std::mt19937 rng(rng_dev());
	// normal prices
	int cocaineMin = 300, cocaineMax = 600;
	int weedMin = 75, weedMax = 150;
	int lsdMin = 10, lsdMax = 50;
	int shroomsMin = 10, shroomsMax = 50;
	// economy factor
	int economyMin = -50, economyMax = 50;
	std::uniform_int_distribution<> economyRange(economyMin, economyMax);
	int economyFactorRaw = economyRange(rng);
	int economyFactor = 1;
	if (economyFactorRaw >= 30) economyFactor = 1.5;
	if (economyFactorRaw == 50) economyFactor = 2;
	if (economyFactorRaw <= -30) economyFactor = 0.5;
	if (economyFactorRaw == -50) economyFactor = 0.25;
	// generate
	std::uniform_int_distribution<> cocaineRange(cocaineMin * economyFactor, cocaineMax * economyFactor);
	pricesForTheDay[0] = cocaineRange(rng);
	std::uniform_int_distribution<> weedRange(weedMin * economyFactor, weedMax * economyFactor);
	pricesForTheDay[1] = weedRange(rng);
	std::uniform_int_distribution<> lsdRange(lsdMin * economyFactor, lsdMax * economyFactor);
	pricesForTheDay[2] = lsdRange(rng);
	std::uniform_int_distribution<> shroomsRange(shroomsMin * economyFactor, shroomsMax * economyFactor);
	pricesForTheDay[3] = shroomsRange(rng);
	// location factor, downtown = weed more expensive, uptown = cocaine more expensive, ghetto = shrooms more expensive
	if (gameState[7] == 0) pricesForTheDay[1] = pricesForTheDay[1] * DOWNTOWN_WEED_FACTOR;
	if (gameState[7] == 1) pricesForTheDay[0] = pricesForTheDay[0] * UPTOWN_COCAINE_FACTOR;
	if (gameState[7] == 2) pricesForTheDay[3] = pricesForTheDay[3] * GHETTO_SHROOMS_FACTOR;
	return;
}

void advance() {
	gameState[1] = gameState[1] + gameState[1] * INTEREST_RATE * gameState[6];
	gameState[6]++;
	std::cout << '\n';
	genPrices();
	return;
}

std::string drugString(int drugNumber) {
	switch (drugNumber) {
	case 0: return "Cocaine";
	case 1: return "Weed";
	case 2: return "LSD";
	case 3: return "Shrooms";
	default: std::cout << "Incorrect drug \n"; return "";
	}
}

std::string location(int currentLocation) {
	std::string currentLocationString;
	switch (currentLocation) {
	case 0: return "Downtown";
	case 1: return "Uptown";
	case 2: return "Ghetto";
	case 3: return "Island";
	case 4: return "City";
	default: std::cout << "Incorrect location \n"; return "";
	}
}

void saveGame(std::string saveFilePath) {
	if (saveFilePath == "") {
		std::cout << "Enter a path to save to: \n";
		std::cin >> saveFilePath;
	}
	std::ofstream saveFile(saveFilePath);
	if (!saveFile.good()) {
		std::cout << "Failed to save to that file \n";
		return;
	}
	std::ostringstream gameStateStringstream;
	for (int i = 0; i != gameState.size(); i++)
	{
		gameStateStringstream << gameState[i] << '|';
	}
	gameStateStringstream << "<|";
	saveFile << gameStateStringstream.str();
	saveFile.close();
	std::cout << "Save file written \n";
	return;
}

void buyDrug(int drug) {
	std::cout << "You have $" << gameState[0] << '\n';
	std::cout << "\nYou have " << gameState[drug + 2] << " bags of " << drugString(drug) << ". The price per bag is $" << pricesForTheDay[drug] << ". Amount to buy (0 to cancel): ";
	int buyAmount;
	std::cin >> buyAmount;
	if (buyAmount <= 0) return;
	int buyPrice = buyAmount * pricesForTheDay[drug];
	if (buyPrice > gameState[0]) {
		std::cout << "You don't have enough money. \n";
		return;
	}
	std::cout << "Buy " << buyAmount << " bags of " << drugString(drug) << " for $" << buyPrice << "? (y/n): ";
	char buySelection;
	std::cin >> buySelection;
	if (buySelection == 'y') {
		gameState[0] = gameState[0] - buyPrice;
		gameState[drug + 2] = gameState[drug + 2] + buyAmount;
		std::cout << "\nThe dealer thanks you for your purchase \n";
	}
	return;
}

void sellDrug(int drug) {
	std::cout << "\nYou have " << gameState[drug + 2] << " bags. The price per bag is $" << pricesForTheDay[drug] << ". Amount to sell (0 to cancel): ";
	int sellAmount;
	std::cin >> sellAmount;
	if (sellAmount <= 0) return;
	if (sellAmount > gameState[drug + 2]) {
		std::cout << "You don't have that many bags to sell \n"; 
		return;
	}
	int sellPrice = sellAmount * pricesForTheDay[drug];
	std::cout << "Sell " << sellAmount << " bags of " << drugString(drug) << " for $" << sellPrice << "? (y/n): ";
	char sellSelection;
	std::cin >> sellSelection;
	if (sellSelection == 'y') {
		gameState[0] = gameState[0] + sellPrice;
		gameState[drug + 2] = gameState[drug + 2] - sellAmount;
		std::cout << "\nThe buyer thanks you \n";
	}
	return;
}

void buyDrugsMenu() {
	bool buying = true;
	while (buying) {
		std::cout << "\nBuy drugs menu\n";
		std::cout << "Current street prices in " << location(gameState[7]) << '\n'
			<< '$' << pricesForTheDay[0] << "/bag of Cocaine \n"
			<< '$' << pricesForTheDay[1] << "/bag of Weed \n"
			<< '$' << pricesForTheDay[2] << "/bag of LSD \n"
			<< '$' << pricesForTheDay[3] << "/bag of Shrooms \n";
		std::cout << "(1) Buy Cocaine, (2) Buy Weed, (3) Buy LSD, (4) Buy Shrooms, (Q) Go back. \n";
		std::cout << "Enter your selection then press enter: ";
		char buyMenuSelection;
		std::cin >> buyMenuSelection;
		switch (buyMenuSelection) {
		case '1': buyDrug(0); break;
		case '2': buyDrug(1); break;
		case '3': buyDrug(2); break;
		case '4': buyDrug(3); break;
		case 'q': buying = false; break;
		default: std::cout << "Invalid selection \n"; break;
		}
	}
	return;
}

void sellDrugsMenu() {
	bool selling = true;
	while (selling) {
		std::cout << "\nSell drugs menu\n";
		std::cout << "Current street prices in " << location(gameState[7]) << ": \n"
			<< '$' << pricesForTheDay[0] << "/bag of Cocaine \n"
			<< '$' << pricesForTheDay[1] << "/bag of Weed \n"
			<< '$' << pricesForTheDay[2] << "/bag of LSD \n"
			<< '$' << pricesForTheDay[3] << "/bag of Shrooms \n";
		std::cout << "(1) Sell Cocaine, (2) Sell Weed, (3) Sell LSD, (4) Sell Shrooms, (Q) Go back. \n";
		std::cout << "Enter your selection then press enter: ";
		char sellMenuSelection;
		std::cin >> sellMenuSelection;
		switch (sellMenuSelection) {
		case '1': sellDrug(0); break;
		case '2': sellDrug(1); break;
		case '3': sellDrug(2); break;
		case '4': sellDrug(3); break;
		case 'q': selling = false; break;
		default: std::cout << "Invalid selection \n"; break;
		}
	}
	return;
}

void loan() {
	if (gameState[1] == 0) {
		std::cout << "You don't have a loan anymore! \n";
		return;
	}
	std::cout << "\nYour debt stands at $" << gameState[1] << ". You have $"<< gameState[0] << ". Make a payment to the loan shark? (y/n): ";
	char loanMenuSelection;
	std::cin >> loanMenuSelection;
	if (loanMenuSelection == 'y') {
		std::cout << "How much of your debt do you want to pay off? ";
			int payAmount;
		std::cin >> payAmount;
		if (payAmount > gameState[0]) {
			std::cout << "You don't have enough money. \n"; 
			return;
		}
		if (payAmount < gameState[1]) {
			std::cout << "Your debt isn't that large. \n";
			return;
		}
		std::cout << "Your debt will stand at $" << gameState[1] - payAmount << ". Pay the amount? (y/n): ";
		char payMenu;
		std::cin >> payMenu;
		if (payMenu == 'y') {
			gameState[0] = gameState[0] - payAmount;
			gameState[1] = gameState[1] - payAmount;
		}
		return;
	}
	return;
}

void travel() {
	std::cout << "\n1: Downtown, 2: Uptown, 3: Ghetto, 4: Island, 5: City, 6: Go back\n";
	std::cout << "Where do you want to travel to? (will advance 1 day): ";
	int travelSelection;
	std::cin >> travelSelection;
	if (travelSelection != 6 && travelSelection - 1 == gameState[7]) {
		std::cout << "You are already in " << location(gameState[7]) << '\n';
		return;
	}
	switch (travelSelection) {
	case 1: std::cout << "Travelling to " << location(0) << "...."; gameState[7] = 0; advance(); break;
	case 2: std::cout << "Travelling to " << location(1) << "...."; gameState[7] = 1; advance(); break;
	case 3: std::cout << "Travelling to " << location(2) << "...."; gameState[7] = 2; advance(); break;
	case 4: std::cout << "Travelling to " << location(3) << "...."; gameState[7] = 3; advance(); break;
	case 5: std::cout << "Travelling to " << location(4) << "...."; gameState[7] = 4; advance(); break;
	case 6: break;
	default: std::cout << "Invalid selection\n"; break;
	}
	return;
}

void gameLoop(std::string saveFilePath) {
	while (gaming) {
		std::cout << "\nWelcome to day " << gameState[6] << ". You are in " << location(gameState[7]) << '\n';
		std::cout << "You have $" << gameState[0] << '\n' << "Your debt stands at $" << gameState[1] << "\n\n";
		std::cout << "You have:\n"
			<< gameState[2] << " bags of Cocaine \n"
			<< gameState[3] << " bags of Weed \n"
			<< gameState[4] << " bags of LSD \n"
			<< gameState[5] << " bags of Shrooms \n";
		std::cout << "\n(Q) Buy, (W) Sell, (E) Advance 1 day, (R) Manage your loan, (T) Travel, (Y) Save, (U), Quit. \n";
		std::cout << "Enter your selection then press enter: ";
		char selection;
		std::cin >> selection;
		switch (selection) {
		case 'q': buyDrugsMenu(); break;
		case 'w': sellDrugsMenu(); break;
		case 'e': advance(); break;
		case 'r': loan(); break;
		case 't': travel(); break;
		case 'y': saveGame(saveFilePath); break;
		case 'u': std::cout << "Do you want to save before quitting (y/n)? "; char quitSelection; std::cin >> quitSelection; if (quitSelection == 'y') saveGame(saveFilePath); gaming = false; break;
		default: std::cout << "Not a valid selection \n"; break;
		}
	}
	std::cout << "Thank you for playing. \n";
	return;
}

void newGame() {
	gameState.push_back(STARTING_LOAN); // money
	gameState.push_back(STARTING_LOAN); // debt
	gameState.push_back(0); // cocaine in pocket
	gameState.push_back(0); // weed in pocket
	gameState.push_back(0); // lsd in pocket
	gameState.push_back(0); // shrooms in pocket
	gameState.push_back(0); // days 
	gameState.push_back(0); // location
	advance();
	gaming = true;
	return;
}

void loadGame() {
	std::cout << "\nEnter path to save file to load: \n";
	std::string saveFilePath;
	std::cin >> saveFilePath;
	std::ifstream saveFile(saveFilePath);
	if (!saveFile.good()) {
		std::cout << "Bad path/file \n";
		return;
	}
	std::vector<std::string> saveFileContents;
	std::string buffer;
	while (buffer != "<") {
		std::getline(saveFile, buffer, '|');
		saveFileContents.push_back(buffer);
	}
	for (int i = 0; i < saveFileContents.size() - 1; i++)
	{
		gameState.push_back(std::stoi(saveFileContents[i]));
	}
	gaming = true;
	advance();
	gameLoop(saveFilePath);
	return;
}

int main()
{
	std::cout << "cpp-drugwars, version " << VERSION << " built at " << __DATE__ << ' ' << __TIME__ << "\n\n";
	std::cout << "(N) New game, (L) Load a saved game, (Q) Quit \n";
	std::cout << "Make your selection then press enter: ";
	char mainMenuInput;
	std::cin >> mainMenuInput;
	switch (mainMenuInput) {
	case 'n':
		newGame();
		gameLoop("");
		break;
	case 'l':
		loadGame();
		break;
	case 'q':
		return 0;
	default:
		std::cout << "Not a valid option. \n";
		break;
	}
	return 0;
}
