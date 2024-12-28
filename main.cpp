#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <map>
using namespace std;

// Forward declarations
class BaseAnt;
class DroneAnt;
class WarriorAnt;
class QueenAnt;
class AntRoomBase;
class SpawnerRoom;
class RelaxRoom;
template <typename AntType> class Colony;
class Habitat;

// BaseAnt class with core attributes
class BaseAnt {
protected:
    string role;
    int power;
    int productivity;
    int restTimer = 0;

public:
    virtual void executeAction() = 0;
    string getRole() const { return role; }
    int getPower() const { return power; }
    int getProductivity() const { return productivity; }
    void configureAttributes(int pwr, int prod) { power = pwr; productivity = prod; }

    void takeRest() {
        restTimer++;
        if (restTimer >= 10) {
            restTimer = 0;
        }
    }

    virtual void mergeAttributes(const BaseAnt& other) {
        power += other.getPower();
        productivity += other.getProductivity();
    }

    virtual ~BaseAnt() {}
};

// Specialized Ant Types
class DroneAnt : public BaseAnt {
public:
    DroneAnt() { role = "Drone"; power = 6; productivity = 12; }
    void executeAction() override {
        takeRest();
    }
};

class WarriorAnt : public BaseAnt {
public:
    WarriorAnt() { role = "Warrior"; power = 12; productivity = 4; }
    void executeAction() override {
        takeRest();
    }
};

class QueenAnt : public BaseAnt {
public:
    QueenAnt() { role = "Queen"; power = 20; productivity = 0; }
    void executeAction() override {
        takeRest();
    }
};

// Ant Factory (Creational Pattern)
class AntCreator {
public:
    static unique_ptr<BaseAnt> produceAnt(const string& type) {
        if (type == "Drone") return make_unique<DroneAnt>();
        if (type == "Warrior") return make_unique<WarriorAnt>();
        if (type == "Queen") return make_unique<QueenAnt>();
        return nullptr;
    }
};

// AntRoomBase class
class AntRoomBase {
protected:
    int buildProgress;

public:
    virtual void performRoomAction() = 0;
    virtual ~AntRoomBase() {}
};

class SpawnerRoom : public AntRoomBase {
public:
    void performRoomAction() override {}
};

class RelaxRoom : public AntRoomBase {
public:
    void performRoomAction() override {}
};

// Colony class with enhanced functionality
template <typename AntType>
class Colony {
private:
    vector<unique_ptr<BaseAnt>> antSquad;
    string speciesName;
    int colonyID;
    int foodStock = 0;
    int waterStock = 0;
    bool activeState = true;
    int workers = 0;
    int warriors = 0;
    int antKills = 0;
    int colonyKills = 0;
    int ticksAlive = 0;
    map<int, string> defeatedColonies;

public:
    Colony(int id, const string& species) : colonyID(id), speciesName(species) {}

    int getColonyID() const { return colonyID; }

    void enlistAnt(unique_ptr<BaseAnt> ant) {
        if (ant->getRole() == "Drone") workers++;
        else if (ant->getRole() == "Warrior") warriors++;
        antSquad.push_back(move(ant));
    }

    void manageTick() {
        if (!activeState) return;
        ticksAlive++;
        for (auto& ant : antSquad) {
            ant->executeAction();
        }
    }

    void engageInBattle(Colony<AntType>* opponent) {
        if (!activeState || !opponent->activeState) return;

        BaseAnt* myAnt = antSquad.back().get();
        BaseAnt* theirAnt = opponent->antSquad.back().get();

        if (myAnt->getPower() > theirAnt->getPower()) {
            myAnt->mergeAttributes(*theirAnt);
            opponent->antSquad.pop_back();
            antKills++;
            if (theirAnt->getRole() == "Queen") {
                defeatedColonies[opponent->colonyID] = opponent->speciesName;
                colonyKills++;
                opponent->activeState = false;
            }
        } else {
            theirAnt->mergeAttributes(*myAnt);
            antSquad.pop_back();
            if (myAnt->getRole() == "Queen") activeState = false;
        }
    }

    void replenishResources(const string& resourceType, int amount) {
        if (resourceType == "food") foodStock += amount;
        else if (resourceType == "water") waterStock += amount;
        else if (resourceType == "drone") {
            for (int i = 0; i < amount; ++i) enlistAnt(AntCreator::produceAnt("Drone"));
        } else if (resourceType == "warrior") {
            for (int i = 0; i < amount; ++i) enlistAnt(AntCreator::produceAnt("Warrior"));
        }
    }

    void showStatus() const {
        cout << "Species: " << speciesName << "\n";
        cout << "Workers: " << workers << "\n";
        cout << "Warriors: " << warriors << "\n";
        cout << "Ant Kills: " << antKills << "\n";
        cout << "Colony kills: " << colonyKills;
        if (!defeatedColonies.empty()) {
            cout << " (";
            for (const auto& [id, name] : defeatedColonies) {
                cout << id << ":" << name << " ";
            }
            cout << ")";
        }
        cout << "\nTicks alive: " << ticksAlive << "\n";
        cout << "Status: " << (activeState ? "Alive" : "Inactive") << "\n";
    }
};

// Habitat class (Singleton)
class Habitat {
private:
    static Habitat* instance;
    vector<unique_ptr<Colony<BaseAnt>>> colonies;

    Habitat() {}

public:
    static Habitat* getInstance() {
        if (!instance) instance = new Habitat();
        return instance;
    }

    void addColony(unique_ptr<Colony<BaseAnt>> colony) {
        colonies.push_back(move(colony));
    }

    Colony<BaseAnt>* getColony(int id) {
        for (auto& colony : colonies) {
            if (colony->getColonyID() == id) return colony.get();
        }
        return nullptr;
    }

    void initiateTick() {
        for (auto& colony : colonies) {
            colony->manageTick();
        }
    }
};

Habitat* Habitat::instance = nullptr;

// Command Handling
void processSpawnCommand(int x, int y, const string& species) {
    int id = x * 1000 + y;
    unique_ptr<Colony<BaseAnt>> colony = make_unique<Colony<BaseAnt>>(id, species);
    colony->enlistAnt(AntCreator::produceAnt("Queen"));
    Habitat::getInstance()->addColony(move(colony));
    cout << "New colony spawned at (" << x << "," << y << ") with ID: " << id << "\n";
}

void processGiveCommand(int colonyID, const string& resource, int amount) {
    auto* colony = Habitat::getInstance()->getColony(colonyID);
    if (colony) {
        colony->replenishResources(resource, amount);
    } else {
        cout << "Colony not found.\n";
    }
}

void processTickCommand(int ticks = 1) {
    auto* habitat = Habitat::getInstance();
    for (int i = 0; i < ticks; ++i) {
        habitat->initiateTick();
    }
}

void processSummaryCommand(int colonyID) {
    auto* colony = Habitat::getInstance()->getColony(colonyID);
    if (colony) {
        colony->showStatus();
    } else {
        cout << "Colony not found.\n";
    }
}

// Main Function
int main() {
    cout << "AntFarm Simulation Initiated. Enter commands (type 'exit' to terminate):\n";
    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);

        if (command == "exit") {
            cout << "Ending Simulation.\n";
            break;
        } else if (command.find("spawn") == 0) {
            stringstream ss(command);
            string cmd;
            int x, y;
            string species;
            ss >> cmd >> x >> y >> species;
            processSpawnCommand(x, y, species);
        } else if (command.find("give") == 0) {
            stringstream ss(command);
            string cmd;
            int colonyID, amount;
            string resource;
            ss >> cmd >> colonyID >> resource >> amount;
            processGiveCommand(colonyID, resource, amount);
        } else if (command.find("tick") == 0) {
            stringstream ss(command);
            string cmd;
            int ticks = 1;
            ss >> cmd >> ticks;
            processTickCommand(ticks);
        } else if (command.find("summary") == 0) {
            stringstream ss(command);
            string cmd;
            int colonyID;
            ss >> cmd >> colonyID;
            processSummaryCommand(colonyID);
        } else {
            cout << "Invalid command. Try again.\n";
        }
    }
    return 0;
}



















