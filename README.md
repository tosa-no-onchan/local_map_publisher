# local_map_publisher  
Ros2 Jazzy で、Static Map を、/local_map topic で、publish します。  

1. build  
   $ cd ~/colcon_ws-jazzy/src  
   $ git clone https://github.com/tosa-no-onchan/local_map_publisher.git  
   $ cd ..  
   $ colcon build --symlink-install --parallel-workers 1 --packages-select local_map_publisher  
   $ . install/setup.bash  

2. run  
   $ ros2 run local_map_publisher local_map_publisher /home/nishi/map/my_neiber4.yaml  

3. check  
   $ ros2 launch nav2_bringup rviz_launch.py  
   Rviz2 で、 map topic を /map -> /local_map  
   に変える。  
