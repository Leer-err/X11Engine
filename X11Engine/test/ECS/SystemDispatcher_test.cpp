#include "SystemDispatcher.h"

#include "ISystem.h"
#include "World.h"
#include "gmock/gmock.h"

// class MockSystem : public ISystem {
//    public:
//     MOCK_METHOD(bool, prepare, (), (override));
//     MOCK_METHOD(void, update, (World & world, float delta_time), (override));
// };

// class MockWrapper : public ISystem {
//    public:
//     MockWrapper(MockSystem& system) : system(system) {}

//     bool prepare(World& world) override { return system.prepare(World&
//     world); } void update(World& world, float delta_time) override {
//         system.update(world, delta_time);
//     }

//    private:
//     MockSystem& system;
// };

// TEST(SystemDispatcherTest, addTest) {
//     SystemDispatcher dispatcher;

//     MockSystem system;

//     dispatcher.add<MockWrapper>(system);

//     World world;
//     dispatcher.progress(world, 1);

//     ON_CALL(system, prepare).WillByDefault(testing::Return(true));

//     EXPECT_CALL(system, prepare);
//     EXPECT_CALL(system, update(testing::_, 1));
// }