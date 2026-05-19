
//Euler Integration mechanics(the simplest mathematical method to calculate an object's new position and //velocity over time in a physics simulation)
#include <iostream>
#include <vector>
#include <string>

// ============================================================================
// 1. DATA LAYER: Pure Structure of Arrays (SoA) for extreme cache locality
// ============================================================================
struct PhysicalStorage {
    std::vector<float> posX, posY, posZ;  // Position components
    std::vector<float> velX, velY, velZ;  // Velocity components
    std::vector<float> accX, accY, accZ;  // Acceleration components
    std::vector<float> mass;              // Common physical metric
    std::vector<std::string> name;        // Metadata

    void reserve(size_t count) {
        posX.resize(count, 0.0f); posY.resize(count, 0.0f); posZ.resize(count, 0.0f);
        velX.resize(count, 0.0f); velY.resize(count, 0.0f); velZ.resize(count, 0.0f);
        accX.resize(count, 0.0f); accY.resize(count, 0.0f); accZ.resize(count, 0.0f);
        mass.resize(count, 1.0f);
        name.resize(count, "UnnamedEntity");
    }
};

// ============================================================================
// 2. PROXY LAYER: Lightweight object passing handles back to data arrays
// ============================================================================
class SimulationEntity {
private:
    size_t index;              // Position tracking inside the raw flat arrays
    PhysicalStorage* storage;  // Pointing back to central data pool

public:
    SimulationEntity(size_t id, PhysicalStorage* shared_pool)
        : index(id), storage(shared_pool) {}

    // Position API
    float getX() const { return storage->posX[index]; }
    void setX(float val) { storage->posX[index] = val; }
    
    float getY() const { return storage->posY[index]; }
    void setY(float val) { storage->posY[index] = val; }

    // Velocity API
    float getVelX() const { return storage->velX[index]; }
    void setVelX(float val) { storage->velX[index] = val; }

    // Acceleration API
    float getAccX() const { return storage->accX[index]; }
    void setAccX(float val) { storage->accX[index] = val; }

    // Mass / Physics properties
    float getMass() const { return storage->mass[index]; }
    void setMass(float val) { storage->mass[index] = val; }

    // Metadata
    std::string getName() const { return storage->name[index]; }
    void setName(const std::string& new_name) { storage->name[index] = new_name; }

    // Output utility
    void printStatus() const {
        std::cout << "[" << getName() << "] "
                  << "Pos: (" << getX() << ", " << getY() << ") | "
                  << "VelX: " << getVelX() << " | "
                  << "AccX: " << getAccX() << "\n";
    }
};

// ============================================================================
// 3. ENGINE LAYER: Loops raw components continuously without Proxy overhead
// ============================================================================
class PhysicsEngine {
private:
    PhysicalStorage storage;
    size_t entityCount = 0;

public:
    // Factory method returning proxy interface handles
    SimulationEntity createEntity(const std::string& entityName) {
        size_t newIndex = entityCount++;
        storage.reserve(entityCount);
        
        SimulationEntity proxy(newIndex, &storage);
        proxy.setName(entityName);
        return proxy;
    }

    // High performance sequential execution path
    void update(float dt) {
        for (size_t i = 0; i < entityCount; ++i) {
            // 1. Compute velocity changes using acceleration forces
            storage.velX[i] += storage.accX[i] * dt;
            storage.velY[i] += storage.accY[i] * dt;
            storage.velZ[i] += storage.accZ[i] * dt;

            // 2. Translate position changes via new velocity calculations
            storage.posX[i] += storage.velX[i] * dt;
            storage.posY[i] += storage.velY[i] * dt;
            storage.posZ[i] += storage.velZ[i] * dt;
        }
    }
};

// ============================================================================
// 4. RUNTIME EXECUTION ENTRY POINT
// ============================================================================
int main() {
    PhysicsEngine engine;

    // Construct entity handles via proxy architecture
    SimulationEntity car = engine.createEntity("RaceCar");
    SimulationEntity drone = engine.createEntity("Quadcopter");

    // Configure initial kinematic parameters
    car.setX(10.0f);
    car.setVelX(5.0f);
    car.setAccX(2.0f); // Fast acceleration line

    drone.setX(0.0f);
    drone.setVelX(20.0f);
    drone.setAccX(0.0f); // Uniform baseline motion

    std::cout << "--- Initial Frame Baseline ---\n";
    car.printStatus();
    drone.printStatus();

    // Step physics forward over a time constant delta
    float deltaTime = 1.0f; // 1-second interval execution chunk
    engine.update(deltaTime);

    std::cout << "\n--- Frame Update Simulation 1 (t = 1s) ---\n";
    car.printStatus();
    drone.printStatus();

    return 0;
}




