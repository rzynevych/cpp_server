#include <atomic>

int main()
{

    std::atomic_int val;
    val.store(1);
    int a = val;
    return 0;
}