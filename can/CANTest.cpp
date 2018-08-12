#include "BSFrameHandler.h"
#include "CANL2.h"
#include "CANUtils.h"
#include "CANproChannel.h"

#include <sys/poll.h>

#include <cerrno>
#include <cstring>

#include <chrono>
#include <experimental/optional>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>


#define DEBUG_INTERRUPTION(MSG)                                                \
    if (0)                                                                     \
    std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

static bool shouldTerminate(const std::future<void> &signal) {
    return signal.wait_for(std::chrono::system_clock::duration::zero()) ==
           std::future_status::ready;
}

namespace can {

static void interruption(CAN_HANDLE channel, backsense::RadarStateDB &stateDB,
                         std::future<void> futureSignal) {
    DEBUG_INTERRUPTION("Thread start");

    struct pollfd can_poll;

    backsense::FrameHandler frameHandler;

    while (!shouldTerminate(futureSignal)) {

        can_poll.fd = CANL2_handle_to_descriptor(channel);
        can_poll.events = POLLIN | POLLHUP;

        int ret = 0;

        DEBUG_INTERRUPTION("Poll section");
        // wait for event on file descriptor
        while (ret <= 0) {
            ret = poll(&can_poll, 1 /*nfds*/, -1 /*timeout*/);

            if (can_poll.revents & POLLHUP) {
                goto endthread;
            }
            if ((ret == -1) && (errno != EINTR)) {
                std::cerr << "#Error: poll() [" << std::strerror(errno) << "]";
            }
        }

        DEBUG_INTERRUPTION("Read section");
        // desciptor is ready to be read
        PARAM_STRUCT outParam;
        while ((ret = CANUtils::readBusEvent(channel, outParam))) {
            if (ret < 0 || shouldTerminate(futureSignal)) {
                goto endthread;
            }

            CANUtils::printReceivedData(ret, outParam);

            auto state = frameHandler.processRcvFrame(outParam);
            if (state) {
                std::ostringstream ss;
                state->dump(ss);
                std::cout << ss.str();
                stateDB.updateState(std::move(*state));
            }

        }
    }

endthread:
    DEBUG_INTERRUPTION("Thread end");
    return;
}

} // namespace can

int main(int argc, char **argv) {

    try {
        can::CANproChannel channel;
        can::backsense::RadarStateDB stateDB;
        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread interruptionHandler(can::interruption, channel.getHandle(),
                                        std::ref(stateDB),
                                        std::move(futureSignal));
        std::getchar();

        exitSignal.set_value();
        can::CANUtils::resetChip(channel.getHandle());

        interruptionHandler.join();

    } catch (std::runtime_error &ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
