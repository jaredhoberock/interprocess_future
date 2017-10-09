#include <thread>
#include <unistd.h>

#include "interprocess_future.hpp"

int main()
{
  // create a new pipe
  int in_and_out_file_descriptors[2];
  if(pipe(in_and_out_file_descriptors) == -1)
  {
    throw std::runtime_error("Error after pipe()");
  }

  file_descriptor_istream is(in_and_out_file_descriptors[0]);

  interprocess_future<int> future(is);

  std::thread([=]
  {
    file_descriptor_ostream os(in_and_out_file_descriptors[1]);

    interprocess_promise<int> promise(os);

    promise.set_value(13);

    close(in_and_out_file_descriptors[1]);
  }).detach();

  std::cout << "Received " << future.get() << " from producer thread" << std::endl;

  close(in_and_out_file_descriptors[0]);
}

