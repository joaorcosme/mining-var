#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include <atomic>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

class BarGraph
{
  public:
    BarGraph() = delete;
    BarGraph(const cv::Point& startPt, const double width,
                                       const double height,
                                       unsigned nTiles = 50)
    {
        const double totalSpacing = 0.2 * height;
        const double spacing = totalSpacing / (nTiles - 1);
        const double tileHeight = (height - totalSpacing)/nTiles;
        const cv::Size tileSize(width, tileHeight);
        cv::Point pt = startPt;

        m_txtOrg.x = startPt.x;
        m_txtOrg.y = startPt.y - 5;
        
        unsigned i = 0;
        while (i++ < nTiles)
        {
            cv::Rect rect(pt, tileSize);
            cv::Scalar color(0,255,0);
            m_tiles.emplace_back(rect, color); 
            pt.y += spacing + tileHeight;
        }
        
    }

    unsigned getNumberOfTiles() { return m_tiles.size(); }

    void draw(cv::Mat& frame, const double fraction)
    {
        assert(fraction >= 0 && fraction <= 1);
        const int nFilledTiles = fraction * getNumberOfTiles();
        int i = 0;
        for(const auto& tile : m_tiles)
        {
            // negative thickness yields a filled rectangle
            int thickness = i++ < nFilledTiles ? -1 : 1;
            cv::rectangle(frame, tile.first, tile.second, thickness); 
        }
        std::string txt = std::to_string(static_cast<int>(100*fraction));
        txt += "%";
        cv::putText(frame, txt, m_txtOrg, m_fontFace, 1, cv::Scalar(0,0,255), 2);
    }

  private:
    std::vector<std::pair<cv::Rect, cv::Scalar>> m_tiles;
    cv::Point m_txtOrg;
    static constexpr int m_fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
};


class SimulatedSensor
{
  public:
    SimulatedSensor() = delete;
    SimulatedSensor(int interval = 1000)
        : m_thread(&SimulatedSensor::updateFraction, this) 
        , m_msInterval(interval)
    {
        m_fraction.store(0.0);
    }

    ~SimulatedSensor()
    {
        m_leaving = true;
        m_thread.join();
    }

    double getFraction()
    {
        return m_fraction.load();
    }

  private:
    void updateFraction()
    {
        using namespace std;
        while(!m_leaving)
        {
            this_thread::sleep_for(chrono::milliseconds(m_msInterval));
            m_fraction.store(generateRandom(0.0, 1.0));
        }
    }

    static double generateRandom(double min, double max)
    {
        std::random_device rdev;
        std::mt19937 rgen(rdev());
        std::uniform_real_distribution<> iDist(min, max);
        return iDist(rgen);
    }
  
  private:
    std::thread m_thread;
    std::atomic<double> m_fraction;
    const int m_msInterval;
    bool m_leaving {false};
};


/**
 * --------------- main ----------------
 */
int main(int argc, char** argv )
{
    cv::Mat frame;
    cv::VideoCapture cap;

    // access built-in camera at index 0
    assert(cap.open(0));

    BarGraph bGraph1(cv::Point(50,50), 50 /* width */, 400 /* height */);
    BarGraph bGraph2(cv::Point(150,50), 50 /* width */, 400 /* height */);
    BarGraph bGraph3(cv::Point(250,50), 50 /* width */, 400 /* height */);

    SimulatedSensor sensor1(1500 /* interval in ms */);
    SimulatedSensor sensor2(1500 /* interval in ms */);
    SimulatedSensor sensor3(1500 /* interval in ms */);

    while(true)
    {
        cap >> frame;
        assert(!frame.empty());

        bGraph1.draw(frame, sensor1.getFraction());
        bGraph2.draw(frame, sensor2.getFraction());
        bGraph3.draw(frame, sensor3.getFraction());

        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0) break;
    }

    return 0;
}
