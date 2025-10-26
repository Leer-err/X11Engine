#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "EventBus.h"

using namespace Engine::Event;

struct TestEvent : public Event {};

struct TestCallable {
    MOCK_METHOD(void, call, (const TestEvent& event));
};

TEST(EventBusTest, SubscriptionTest) {
    auto callback = [](const TestEvent& event) {};

    EventBus::get().subscribe(EventCallback<TestEvent>(callback));
}

TEST(EventBusTest, CallbackTest) {
    TestCallable test_callable;

    auto callback = [&](const TestEvent& event) { test_callable.call(event); };
    EXPECT_CALL(test_callable, call).Times(1);

    auto subscription =
        EventBus::get().subscribe(EventCallback<TestEvent>(callback));
    EventBus::get().publish(TestEvent{});
    EventBus::get().flush();
}