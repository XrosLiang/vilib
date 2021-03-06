/*
 * Timer class for profiling algorithms on the GPU
 * timergpu.cpp
 *
 * Copyright (C) 2019-2020 Balazs Nagy,
 * Robotics and Perception Group, University of Zurich
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iomanip>
#include "vilib/cuda_common.h"
#include "vilib/timergpu.h"

namespace vilib {

TimerGPU::TimerGPU(const char * name, int indent) :
  name_(name),time_(0.0),indent_(indent),stat_("usec") {
  CUDA_API_CALL(cudaEventCreate(&start_event_));
  CUDA_API_CALL(cudaEventCreate(&stop_event_));
}

TimerGPU::TimerGPU(const std::string & name, int indent) :
  name_(name),time_(0.0),indent_(indent),stat_("usec") {
  CUDA_API_CALL(cudaEventCreate(&start_event_));
  CUDA_API_CALL(cudaEventCreate(&stop_event_));
}

TimerGPU::~TimerGPU(void) {
  CUDA_API_CALL(cudaEventDestroy(start_event_));
  CUDA_API_CALL(cudaEventDestroy(stop_event_));
}

void TimerGPU::start(cudaStream_t stream) {
  CUDA_API_CALL(cudaEventRecord(start_event_,stream));
}

void TimerGPU::stop(bool synchronize, cudaStream_t stream) {
  CUDA_API_CALL(cudaEventRecord(stop_event_,stream));
  if(synchronize) {
    sync();
  }
}

void TimerGPU::sync(void) {
  CUDA_API_CALL(cudaEventSynchronize(stop_event_));
  float time_ms;
  CUDA_API_CALL(cudaEventElapsedTime(&time_ms,start_event_,stop_event_));
  /*
   * Note to future self and others:
   * according to NVIDIA, the elapsed time is expressed in ms,
   * with 0.5usec resolution.
   */
  time_ = time_ms * 1.0e3; // go from ms to us
}

void TimerGPU::add_to_stat_n_reset(void) {
  stat_.add(time_);
  time_ = 0.0;
}

void TimerGPU::display_usec(void) const {
  for(int i=0;i<indent_;++i) {
    std::cout << " ";
  }
  std::cout << std::setw(name_size_characters_) << std::left << name_ << ": "
            << std::setw(time_size_characters_) << std::right << time_ << " usec" << std::endl;
}

void TimerGPU::display_stat_usec(void) const {
  for(int i=0;i<indent_;++i) {
    std::cout << " ";
  }
  std::cout << std::setw(name_size_characters_) << std::left  << name_ << ": ";
  stat_.display();
}

} // namespace vilib
