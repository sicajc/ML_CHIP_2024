#ifndef FIFO_H
#define FIFO_H

// must update the Fifo such that it can support simultaneous read and write

#include <USER_DEFINED_PARAM.h>
#include <helperfunction.h>

template <typename T, size_t N>
class Fifo {
private:
    T data[N];
    size_t head;
    size_t tail;

public:
    Fifo() : head(0), tail(0) {}

    void enqueue(const T& item) {
        if (isFull()) {
            std::cout << "Queue is full. Cannot enqueue.\n";
            return;
        }
        data[tail] = item;
        tail = (tail + 1) % N;
    }

    T dequeue() {
        if (isEmpty()) {
            std::cout << "Queue is empty. Cannot dequeue.\n";
            return T();
        }
        T item = data[head];
        head = (head + 1) % N;
        return item;
    }

    T& front() {
        if (isEmpty()) {
            std::cout << "Queue is empty. Cannot access front.\n";
            return data[0];
        }
        return data[head];
    }

    bool isEmpty() const {
        return head == tail;
    }

    bool isFull() const {
        return (tail + 1) % N == head;
    }

    size_t getSize() const {
        return (tail >= head) ? (tail - head) : (N - head + tail);
    }

    void display() const {
        std::cout << "Current FIFO contents: \n";
        if (isEmpty()) {
            std::cout << "Empty";
        } else {
            size_t i = head;
            while (i != tail) {
                // std::cout << data[i] << " ";
                // modify to display the data in fifo
                T flit = data[i];
                sc_lv<32> flit_data = flit.range(31, 0);

                if(flit[33] == 1)
                {
                    std::cout << "Head flit: " << flit << std::endl;
                }
                else
                {
                    //display float value
                    std::cout << "Body flit: " << sc_lv_to_float(flit) << std::endl;

                }


                i = (i + 1) % N;
            }
        }
        std::cout << std::endl;
    }
};

#endif // FIFO_H
