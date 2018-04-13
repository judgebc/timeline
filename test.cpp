#include "timeline.h"

#include <cstdio>
#include <string>

/*
 * This is a simple test of the Timeline class. The expected output is as
 * follows:
 *
 *  step 1
 *      foo: test
 *  step 2
 *  step 3
 *      foo: skip 2
 *  step 4
 *      foo: two events
 *      foo: at the same time
 *  step 5
 *  step 6
 *  step 7
 *  step 8
 *      bar: this is a different type
 *  step 9
 *      bar: we will see this one
 *  step 10
 */
struct CustomEvent
{
  int type;
  std::string text;
};

void foo(Timeline::Event const &event)
{
  auto custom = std::any_cast<CustomEvent> (event.value);

  if(custom.type == 1)
    printf("\tfoo: %s\n", custom.text.c_str());
}

void bar(Timeline::Event const &event)
{
  auto custom = std::any_cast<CustomEvent> (event.value);

  if(custom.type == 2)
    printf("\tbar: %s\n", custom.text.c_str());
}

int main(int argc, char *argv[])
{
  Timeline timeline;

  auto t1 = timeline.subscribe(foo);
  auto t2 = timeline.subscribe(bar);

  timeline.emplace(1, std::any(CustomEvent{ 1, "test" }));
  timeline.emplace(3, std::any(CustomEvent{ 1, "skip 2" }));
  timeline.emplace(4, std::any(CustomEvent{ 1, "two events" }));
  timeline.emplace(4, std::any(CustomEvent{ 1, "at the same time" }));
  timeline.emplace(8, std::any(CustomEvent{ 2, "this is a different type" }));
  timeline.emplace(9, std::any(CustomEvent{ 1, "we won't see this one" }));
  timeline.emplace(9, std::any(CustomEvent{ 2, "we will see this one" }));

  for(int i = 0; i < 10; ++i) {
    printf("step %d\n", i + 1);
    if(timeline.getTimestep() == 5)
      t1.reset();  // unsubscribe foo
    timeline.step();
  }

  return 0;
}

