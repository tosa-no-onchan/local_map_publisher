/*------------
 1.build
 $ colcon build --symlink-install --parallel-workers 1 --packages-select local_map_publisher
 $ . install/setup.bash

 2.run
  $ ros2 run local_map_publisher local_map_publisher /home/nishi/map/my_map6.yaml

  $ ros2 run local_map_publisher local_map_publisher /home/nishi/map/my_neiber4.yaml

 3. Rviz2 で、
   map topic を /map -> /local_map
   に変える。
  $ ros2 launch nav2_bringup rviz_launch.py

-------------*/

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <yaml-cpp/yaml.h>
#include <opencv2/opencv.hpp>
#include <filesystem>
using namespace std;

int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    if(argc<2)
    {
        std::cout << "usage : local_map_publisher map.yaml\n";
        return 1;
    }

    auto node = std::make_shared<rclcpp::Node>("local_map_publisher");
    auto qos = rclcpp::QoS(1).transient_local().reliable();
    auto pub = node->create_publisher<nav_msgs::msg::OccupancyGrid>(
            "/local_map",
            qos);

    YAML::Node yaml = YAML::LoadFile(argv[1]);
    std::string image_file = yaml["image"].as<std::string>();

    double resolution = yaml["resolution"].as<double>();
    auto origin = yaml["origin"];

    std::filesystem::path yaml_path(argv[1]);

    std::filesystem::path image_path = yaml_path.parent_path() / image_file;

    cv::Mat img = cv::imread(image_path.string(), cv::IMREAD_GRAYSCALE);

    if(img.empty()){
        RCLCPP_ERROR(node->get_logger(),
                     "cannot open image");
        return 1;
    }

    //#define TEST1
    #if defined(TEST1)
        double minv, maxv;
        cv::minMaxLoc(img, &minv, &maxv);

        RCLCPP_INFO(node->get_logger(),
                    "image size=%dx%d  min=%g  max=%g",
                    img.cols, img.rows, minv, maxv);

        for (int i = 0; i < 20; i++){
            std::cout << (int)img.data[i] << " ";
        }
        std::cout << std::endl;

    #endif

    nav_msgs::msg::OccupancyGrid map;
    map.header.frame_id="map";

    auto now = node->get_clock()->now();
    map.header.stamp = now;
    map.info.map_load_time = now;

    map.info.resolution=resolution;
    map.info.width=img.cols;
    map.info.height=img.rows;

    map.info.origin.position.x = origin[0].as<double>();
    map.info.origin.position.y = origin[1].as<double>();
    map.info.origin.orientation.w=1.0;
    map.data.resize(img.cols*img.rows);

    // nav2_map_server (mode=trinary) と同じ変換 をする。
    auto negate = yaml["negate"].as<int>();
    auto occupied_thresh =yaml["occupied_thresh"].as<double>();
    auto free_thresh = yaml["free_thresh"].as<double>();

    assert(img.isContinuous());

    const int width = img.cols;
    const int height = img.rows;
    const uint8_t *src = img.data;
    int8_t *dst = map.data.data();
    
    for (int y = 0; y < height; ++y){
        // PGMは画像座標系なので、OccupancyGrid用に上下反転
        const uint8_t *row = src + (height - 1 - y) * width;
        int8_t *out = dst + y * width;
        for (int x = 0; x < width; ++x){
            uint8_t pixel = row[x];
            //double occ = (255.0 - pixel) / 255.0;
            double occ;
            if (0 == negate)
                occ = (255.0 - pixel) / 255.0;
            else
                occ = pixel / 255.0;
            if (occ > occupied_thresh)
                out[x] = 100;
            else if (occ < free_thresh)
                out[x] = 0;
            else
                out[x] = -1;
        }
    }

    #if defined(USE_OLD)
    uchar* p = img.data;
    int total = img.rows * img.cols;
    for(int i = 0; i < total; ++i) {
        uchar pixel = p[i];
        // 処理...
            double occ;
            //if (0 == yaml["negate"].as<int>())
            if (0 == negate)
                occ = (255.0 - pixel) / 255.0;
            else
                occ = pixel / 255.0;
            int8_t value;
            //if (occ > yaml["occupied_thresh"].as<double>())
            if (occ > occupied_thresh)
                value = 100;
            //else if (occ < yaml["free_thresh"].as<double>())
            else if (occ < free_thresh)
                value = 0;
            else
                value = -1;
            map.data[i] = value;
    }
    #endif

    pub->publish(map);
    RCLCPP_INFO(node->get_logger(),
                "map published");

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

