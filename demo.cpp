#include <thread>
#include <unistd.h>

#include "interprocess_future.hpp"

int main()
{
  {
    // test normal case

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

    int result = future.get();
    std::cout << "Received " << result << " from producer thread" << std::endl;
    assert(result == 13);

    close(in_and_out_file_descriptors[0]);
  }

  {
    // test exceptional case

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

      promise.set_exception(interprocess_exception("exception"));

      close(in_and_out_file_descriptors[1]);
    }).detach();

    try
    {
      int result = future.get();
      assert(0);
    }
    catch(interprocess_exception e)
    {
      std::cout << "Received exception from producer thread: " << e.what() << std::endl;
    }
    catch(...)
    {
      assert(0);
    }

    close(in_and_out_file_descriptors[0]);
  }
}

