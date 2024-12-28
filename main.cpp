
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

// BaseAnt class with core attributes (Decorator Pattern)
class BaseAnt {
protected:
    string role;
    int power;
    int productivity;
    int restTimer = 0;

public:
    virtual void executeAction() = 0;
    string getRole() { return role; }
    int getPower() const { return power; }
    int getProductivity() const { return productivity; }
    void configureAttributes(int pwr, int prod) { power = pwr; productivity = prod; }

    void takeRest() {
        restTimer++;
        if (restTimer >= 10) {
            cout << role << " ant is recuperating and consuming resources.\n";
            restTimer = 0;
        }
    }

    virtual void mergeAttributes(const BaseAnt& other) {
        power += other.getPower();
        productivity += other.getProductivity();
    }
};

// Specialized Ant Types
class DroneAnt : public BaseAnt {
public:
    DroneAnt() { role = "Drone"; power = 6; productivity = 12; }
    void executeAction() override {
        cout << role << " ant is gathering supplies.\n";
        takeRest();
    }
};

class WarriorAnt : public BaseAnt {
public:
    WarriorAnt() { role = "Warrior"; power = 12; productivity = 4; }
    void executeAction() override {
        cout << role << " ant is defending the colony.\n";
        takeRest();
    }
};

class QueenAnt : public BaseAnt {
public:
    QueenAnt() { role = "Queen"; power = 20; productivity = 0; }
    void executeAction() override {
        cout << role << " ant is laying eggs.\n";
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
    virtual void reduceBuildTime(int workerCount) {
        if (buildProgress > 0) buildProgress = max(0, buildProgress - workerCount);
    }
    virtual int remainingBuildTime() const { return buildProgress; }
    virtual ~AntRoomBase() {}
};

class SpawnerRoom : public AntRoomBase {
public:
    SpawnerRoom() { buildProgress = 60; }
    void performRoomAction() override {
        cout << "SpawnerRoom is producing new ants.\n";
    }
};

class RelaxRoom : public AntRoomBase {
public:
    RelaxRoom() { buildProgress = 40; }
    void performRoomAction() override {
        cout << "RelaxRoom is providing rest to ants.\n";
    }
};

// Template-based Colony class
template <typename AntType>
class Colony {
private:
    vector<unique_ptr<AntRoomBase>> chambers;
    vector<unique_ptr<BaseAnt>> antSquad;
    string speciesName;
    int colonyID;
    int foodStock = 0;
    int waterStock = 0;
    bool hasQueen = true;
    int maxRoomCapacity;
    bool activeState = true;

    void applySpeciesAdvantage() {
        if (speciesName == "aggressive") {
            for (auto& ant : antSquad) {
                ant->configureAttributes(ant->getPower() + 3, ant->getProductivity());
            }
        } else if (speciesName == "productive") {
            for (auto& ant : antSquad) {
                ant->configureAttributes(ant->getPower(), ant->getProductivity() + 3);
            }
        }
    }

public:
    Colony(int id, const string& species, int capacity) : colonyID(id), speciesName(species), maxRoomCapacity(capacity) {}

    int getColonyID() const { return colonyID; }  // Getter for colonyID

    void deactivateColony() {
        activeState = false;
        antSquad.clear();
        foodStock = 0;
        waterStock = 0;
        hasQueen = false;
    }

    bool isColonyOperational() const { return activeState; }

    class Constructor {
    private:
        Colony* colInstance;

    public:
        Constructor(int id, const string& species, int capacity) {
            colInstance = new Colony(id, species, capacity);
        }

        Constructor& attachRoom(unique_ptr<AntRoomBase> room) {
            colInstance->chambers.push_back(move(room));
            return *this;
        }

        Colony* assemble() {
            colInstance->applySpeciesAdvantage();
            return colInstance;
        }
    };

    void enlistAnt(unique_ptr<BaseAnt> ant) {
        antSquad.push_back(move(ant));
    }

    void replenishResources(const string& resourceType, int amount) {
        if (resourceType == "food") foodStock += amount;
        else if (resourceType == "water") waterStock += amount;
        else cout << "Invalid resource type: " << resourceType << endl;
    }

    void manageTick() {
        random_device rndDevice;
        mt19937 generator(rndDevice());
        shuffle(antSquad.begin(), antSquad.end(), generator);

        int relaxSpace = 0;
        for (const auto& chamber : chambers) {
            if (dynamic_cast<RelaxRoom*>(chamber.get())) relaxSpace += 5;
        }

        int restingAnts = 0;
        for (auto& ant : antSquad) {
            if (restingAnts < relaxSpace) {
                ant->takeRest();
                restingAnts++;
            } else {
                break;
            }
        }

        for (auto& chamber : chambers) {
            chamber->performRoomAction();
        }
    }

    void engageInBattle(Colony<AntType>* opponent) {
        if (!activeState || !opponent->isColonyOperational()) {
            cout << "One or both colonies are inactive. No battle can occur.\n";
            return;
        }

        if (antSquad.empty() || opponent->antSquad.empty()) return;

        BaseAnt* myAnt = antSquad.back().get();
        BaseAnt* theirAnt = opponent->antSquad.back().get();

        cout << "Combat between " << speciesName << " and " << opponent->speciesName << " (ID " << opponent->getColonyID() << ")\n";

        if (myAnt->getPower() > theirAnt->getPower()) {
            cout << "Colony " << colonyID << " triumphs over Colony " << opponent->getColonyID() << "!\n";
            myAnt->mergeAttributes(*theirAnt);
            opponent->antSquad.pop_back();
            if (theirAnt->getRole() == "Queen") {
                mergeColonies(opponent);
            }
        } else {
            cout << "Colony " << opponent->getColonyID() << " triumphs over Colony " << colonyID << "!\n";
            theirAnt->mergeAttributes(*myAnt);
            antSquad.pop_back();
            if (myAnt->getRole() == "Queen") deactivateColony();
        }
    }

    void mergeColonies(Colony<AntType>* otherColony) {
        for (auto& ant : otherColony->antSquad) {
            enlistAnt(move(ant));
        }
        otherColony->deactivateColony();
    }

    void showStatus() const {
    if (!activeState) {
        cout << "Colony ID: " << colonyID << " is no longer active.\n";
        return;
    }
    cout << "Colony ID: " << colonyID << "\n"
         << "Species: " << speciesName << "\n"
         << "Ant Count: " << antSquad.size() << "\n"
         << "Food Stock: " << foodStock << "\n"
         << "Water Stock: " << waterStock << "\n";
}

};

// Singleton Habitat class
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

    void initiateTick() {
        for (auto& colony : colonies) {
            colony->manageTick();
        }

        int activeCount = count_if(colonies.begin(), colonies.end(), [](const unique_ptr<Colony<BaseAnt>>& col) {
            return col->isColonyOperational();
        });

        if (activeCount <= 1) cout << "Simulation ends: One or fewer active colonies remain.\n";
    }

    Colony<BaseAnt>* getColony(int id) {
    for (auto& colony : colonies) {
        if (colony->getColonyID() == id && colony->isColonyOperational()) {
            return colony.get();  // Return the colony with the matching ID
        }
    }
    return nullptr;  // Return nullptr if the colony with the specified ID is not found
}

};

Habitat* Habitat::instance = nullptr;

// Command Handling Functions
template <typename AntType>
void processSpawnCommand(int id, const string& species) {
    typename Colony<AntType>::Constructor builder(id, species, 10);
    builder.attachRoom(make_unique<SpawnerRoom>());
    builder.attachRoom(make_unique<RelaxRoom>());
    Colony<AntType>* colony = builder.assemble();

    Habitat::getInstance()->addColony(unique_ptr<Colony<AntType>>(colony));
    colony->enlistAnt(AntCreator::produceAnt("Queen"));

    cout << "New colony spawned with ID: " << id << "\n";
}

void processGiveCommand(int colonyID, const string& resource, int amount) {
    auto* colony = Habitat::getInstance()->getColony(colonyID);
    if (!colony) {
        cout << "Colony not found or inactive.\n";
        return;
    }

    if (resource == "food" || resource == "water") {
        colony->replenishResources(resource, amount);
    } else if (resource == "warrior") {
        for (int i = 0; i < amount; ++i) {
            colony->enlistAnt(AntCreator::produceAnt("Warrior"));
        }
    } else if (resource == "drone") {
        for (int i = 0; i < amount; ++i) {
            colony->enlistAnt(AntCreator::produceAnt("Drone"));
        }
    } else {
        cout << "Invalid resource type.\n";
    }
}

void processSummaryCommand(int colonyID) {
    auto* colony = Habitat::getInstance()->getColony(colonyID);
    if (colony) {
        colony->showStatus();
    } else {
        cout << "Colony not found or inactive.\n";
    }
}

// Main simulation
int main() {
    cout << "AntFarm Simulation Initiated. Enter commands (type 'exit' to terminate):\n";

    string command;
    int colonyCounter = 1;
    while (true) {
        cout << "> ";
        getline(cin, command);

        if (command == "exit") {
            cout << "Ending Simulation.\n";
            break;
        } else if (command.find("spawn") == 0) {
            processSpawnCommand<BaseAnt>(colonyCounter++, "generic");
        } else if (command.find("give") == 0) {
            stringstream ss(command);
            string cmd;
            int colonyID, amount;
            string resource;
            ss >> cmd >> colonyID >> resource >> amount;
            processGiveCommand(colonyID, resource, amount);
        } else if (command.find("summary") == 0) {
            stringstream ss(command);
            string cmd;
            int colonyID;
            ss >> cmd >> colonyID;
            processSummaryCommand(colonyID);
        } else if (command.find("tick") == 0) {
            Habitat::getInstance()->initiateTick();
        } else {
            cout << "Unknown command. Try again.\n";
        }
    }

    return 0;
}
