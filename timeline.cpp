#include "timeline.h"

#include <queue>
#include <vector>

struct Timeline::Private
{
  Private() : consumers(), queue(), timestep(0) {}

  /// list of listeners to timeline events
  std::vector<std::weak_ptr<Callback>> consumers;

  /// event queue ordered by timestep
  std::priority_queue<Event> queue;

  /// current timestep of the simulation
  int timestep;
};

Timeline::Timeline() : pImpl(std::make_unique<Timeline::Private>())
{
}

Timeline::~Timeline()
{
}

Timeline::Token Timeline::subscribe(Callback callback)
{
  auto token = std::make_shared<Callback>(std::move(callback));
  pImpl->consumers.push_back(token);
  return token;
}

void Timeline::push(Event const &event)
{
  pImpl->queue.push(event);
}

void Timeline::push(Event &&event)
{
  pImpl->queue.push(std::move(event));
}

void Timeline::emplace(int timestep, std::any value)
{
  pImpl->queue.emplace(Event{ timestep, value });
}

void Timeline::step()
{
  pImpl->timestep += 1;

  // prune any subscribers that have deleted their tokens
  pImpl->consumers.erase(
    std::remove_if(pImpl->consumers.begin(), pImpl->consumers.end(),
      [](std::weak_ptr<Callback> wp)->bool { return !wp.lock(); }
    ),
    pImpl->consumers.end()
  );

  while(!pImpl->queue.empty())
  {
    auto event = pImpl->queue.top();

    // break if the next event is beyond the current timestep
    if(event.timestep > pImpl->timestep)
      break;

    // make a copy in case consumers is modified by listeners
    auto callbacks = pImpl->consumers;

    // dispatch the event to subscribers
    for(auto wp : callbacks) {
      if(auto sp = wp.lock()) {
        (*sp)(event);
      }
    }

    // pop the event from the queue
    pImpl->queue.pop();
  }
}

int Timeline::getTimestep() const
{
  return pImpl->timestep;
}

bool operator<(Timeline::Event const &lhs, Timeline::Event const &rhs)
{
  /*
   * The greater-than operator is used to reverse the default
   * std::priority_queue ordering.
   */
  return lhs.timestep > rhs.timestep;
}
