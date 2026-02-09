#include "debug.h"
#include "threads.h"
#include "bb.h"


void kernelMain(void) {

    //simple bounding buffer get after put test
    auto buffer1 = new BB<int>(1);
    int add = 1;
    buffer1->put(add);
    int response1 = buffer1->get();
    Debug::printf("*** Response Received: %d\n", response1);

    //let's declare a buffer of buffer and perform a get after put test
    //tests that the bounded buffer can store any type (including its own type)
    //ensures that both calls to get are resolved in the order that values were put in the bounded buffer
    auto buffer3 = new BB<BB<int>*>(1);
    auto buffer3_1 = new BB<int>(1);
    int insert = 3;
    buffer3_1->put(insert);
    buffer3->put(buffer3_1);
    int response3 = buffer3->get()->get();
    Debug::printf("*** Buffer of buffer received response %d\n", response3);

    //buffer of buffer put after get test (forces context switch)
    //tests that we can create a bounded buffer inside a thread
    auto buffer4 = new BB<BB<int>*>(1);
    thread([buffer4] {
        auto buffer4_1 = new BB<int>(1);
        int insert = 4;
        buffer4_1->put(insert);
        buffer4->put(buffer4_1);
    });

    //checks that gets returns in order that values were placed so that we access the individual items in interior buffer
    Debug::printf("*** Buffer of buffer received response: %d\n", buffer4->get()->get());

    //now we add a larger buffer in the interior of the main buffer
    //tests that values are returned in the order they were placed in the bounded buffer so we print 0-9 in sequence
    auto buffer5 = new BB<BB<int>*>(1);
    thread([buffer5] {
        auto buffer5_1 = new BB<int>(10);
        for (int i = 0; i < 10; i++) {
            int insert = i;
            buffer5_1->put(insert);
        }
        buffer5->put(buffer5_1);
    });

    auto insideBuffer5 = buffer5->get();

    for (int i = 0; i < 10; i++)
        Debug::printf("*** Buffer of larger interior buffer received response: %d\n", insideBuffer5->get());

    //lastly, we construct a buffer of size 10 who contains 10 buffers of size 10.
    auto buffer6 = new BB<BB<int>*>(10);
    thread([buffer6] {
        auto buffer6_1 = new BB<int>(10);
        int counter = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                buffer6_1->put(counter);
                counter = counter + 1;
            }
            buffer6->put(buffer6_1);
        }
        
    });

    auto innerBuffer = buffer6->get();

    //rigorously tests that the values are returned in order they were placed in bounded buffer so we print 0-99 in sequence.
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            Debug::printf("*** Larger buffer of larger interior buffer received response: %d\n", innerBuffer->get());
        }
    }

    //this tests that we don't lose values that were added when the bounded buffer was full
    auto fullBuffer = new BB<int>(5);
    thread([fullBuffer] {
        Debug::printf("*** Context switch here should be forced\n");

        //this creates room for the value that was attempted to be added
        fullBuffer->get();
    });

    int addValue = 1;
    int fullValue = 2;

    fullBuffer->put(addValue);
    fullBuffer->put(addValue);
    fullBuffer->put(addValue);
    fullBuffer->put(addValue);
    fullBuffer->put(addValue);

    //this item was added when the buffer is full, forcing a context switch
    fullBuffer->put(fullValue);

    //now, we get all items from the buffer to make sure item added when full is there
    Debug::printf("*** Received value is %d\n", fullBuffer->get());
    Debug::printf("*** Received value is %d\n", fullBuffer->get());
    Debug::printf("*** Received value is %d\n", fullBuffer->get());
    Debug::printf("*** Received value is %d\n", fullBuffer->get());
    Debug::printf("*** Received value is %d\n", fullBuffer->get());

}

