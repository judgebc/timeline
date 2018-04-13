#pragma once

#include <any>
#include <functional>
#include <memory>

class Timeline
{
public:
  Timeline();
  ~Timeline();

  /*!
   * Container for holding timeline events.
   */
  struct Event {
    int timestep;
    std::any value;
  };

  /*!
   * \brief Callback type for event listeners.
   *
   * Note that the event lifecycle ends after the callback returns.
   * So, a deep copy is necessary if the listener wishes to hold on to
   * the event.
   */
  using Callback = std::function<void(Event const &)>;

  /*!
   * \brief Token provided to event listeners to manage the lifecycle of the
   *        subscription.
   *
   * When the token goes out of scope, the listener is marked for removal in
   * the next simulation step.
   */
  using Token = std::shared_ptr<void>;

  /*!
   * Subscribe to timeline events.
   * \param callback function for receiving triggered events
   * \return token that unsubscribes the listener on destruction
   */
  Token subscribe(Callback);

  /*!
   * Inserts a deep-copy of the event in the queue.
  */
  void push(Event const &);

  /*!
   * \brief Takes ownership and moves the event into the queue.
   *
   * This override allows automatic variables to be safely placed in the
   * timeline by value.
   */
  void push(Event &&);

  /*!
   * Constructs an event in-place and sorts the queue.
   */
  void emplace(int timestep, std::any value);

  /*!
   * Advances the timeline a single timestep.
   */
  void step();

  /*!
   * Accessor for the current simulation timestep,
   */
  int getTimestep() const;

private:
  struct Private;
  std::unique_ptr<Private> pImpl;
};

/// operator overload for ordering timeline events in the priority queue
bool operator<(Timeline::Event const &lhs, Timeline::Event const &rhs);
