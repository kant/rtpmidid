/**
 * Real Time Protocol Music Industry Digital Interface Daemon
 * Copyright (C) 2019 David Moreno Montero <dmoreno@coralbits.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <map>
#include <functional>
#include <ctime>

namespace rtpmidid {
  /**
   * Simplified fd poller
   *
   * Internally uses epoll, it is level triggered, so data must be read or
   * will retrigger.
   */
  class poller_t{
    int epollfd;
    std::map<int, std::function<void(int)>> fd_events;
    std::vector<std::pair<std::time_t, std::function<void(void)>>> timer_events;
  public:
    poller_t();
    ~poller_t();

    // Call this function in X seconds
    void add_timer_event(std::time_t in_secs, std::function<void(void)> event_f);
    void add_fd_in(int fd, std::function<void(int)> event_f);
    void add_fd_out(int fd, std::function<void(int)> event_f);
    void add_fd_inout(int fd, std::function<void(int)> event_f);
    void remove_fd(int fd);

    void wait();

    void close();
    bool is_open(){
      return epollfd > 0;
    }
  };

  // Singleton for all events on the system.
  extern poller_t poller;
}
